/**
 * @file src/gamestate.c
 *
 * Main game state. Handles game logic, win/lose condition... pretty much
 * everything
 */
#include <base/game_const.h>
#include <base/game_ctx.h>

#include <GFraMe/gfmAssert.h>
#include <GFraMe/gfmError.h>
#include <GFraMe/gfmGenericArray.h>
#include <GFraMe/gfmGroup.h>
#include <GFraMe/gfmParser.h>
#include <GFraMe/gfmTilemap.h>

#include <ggj16/cauldron.h>
#include <ggj16/gesture.h>
#include <ggj16/gamestate.h>
#include <ggj16/object.h>
#include <ggj16/type.h>

#include <stdlib.h>
#include <string.h>

gfmGenArr_define(object);

struct stGamestate {
    /** List of objects */
    gfmGenArr_var(object, pObjects);
    /** Fire particles */
    gfmGroup *pFire;
    /** Iterator for spawn fire particles */
    int curFire;
};
typedef struct stGamestate gamestate;

/**
 * Release everything alloc'ed on init
 */
void gs_free() {
    /** The new state */
    gamestate *pState;

    /* Retrieve the current state from the global one */
    pState = (gamestate*)pGame->pState;

    cauldron_free(&(pGlobal->pCauldron));
    gfmGroup_free(&(pState->pFire));
    gfmGenArr_clean(pState->pObjects, object_free);
    recipeScroll_free(&(pGlobal->pRecipe));
}

/**
 * Initialize the game state (alloc anything needed, load first level and so on)
 */
gfmRV gs_init() {
    /** Parse the objects in the map */
    gfmParser *pParser;
    /** GFraMe return value */
    gfmRV rv;
    /** The new state */
    gamestate *pState;

    pParser = 0;
    pState = 0;

    /* Check that the state is correct and there's none loaded */
    ASSERT(pGame->nextState == ST_GAME, GFMRV_INTERNAL_ERROR);
    ASSERT(pGame->pState == 0, GFMRV_INTERNAL_ERROR);

    pState = (gamestate*)malloc(sizeof(gamestate));
    ASSERT(pState, GFMRV_ALLOC_FAILED);
    memset(pState, 0x0, sizeof(gamestate));

    /* Load all objects */
    rv = gfmParser_getNew(&pParser);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmParser_initStatic(pParser, pGame->pCtx, "map/map_obj.gfm");
    ASSERT(rv == GFMRV_OK, rv);

    /* Parse every object */
    while (1) {
        gfmParserType type;

        /* Start parsing the next object */
        rv = gfmParser_parseNext(pParser);
        if (rv == GFMRV_PARSER_FINISHED) {
            break;
        }
        rv = gfmParser_getType(&type, pParser);
        ASSERT(rv == GFMRV_OK, rv);

        if (type == gfmParserType_area) {
            /* There should be no areas */
            ASSERT(0, GFMRV_INTERNAL_ERROR);
        }
        else if (type == gfmParserType_object) {
            /** In game type (as string) */
            char *pName;
            /** Type of the current item */
            itemType type;

            /* Retrieve the type */
            rv = gfmParser_getIngameType(&pName, pParser);
            ASSERT(rv == GFMRV_OK, rv);
            rv = type_getHandle(&type, pName);
            ASSERT(rv == GFMRV_OK, rv);

            if (type == T_CAULDRON) {
                rv = cauldron_getNew(&(pGlobal->pCauldron));
                ASSERT(rv == GFMRV_OK, rv);
                rv = cauldron_init(pGlobal->pCauldron, pParser);
                ASSERT(rv == GFMRV_OK, rv);
            }
            else {
                /** Alloc'ed object */
                object *pObj;

                /* Parse and spawn the object */
                gfmGenArr_getNextRef(object, pState->pObjects, 1/* inc */, pObj,
                        object_getNew);
                rv = object_init(pObj, pParser);
                ASSERT(rv == GFMRV_OK, rv);
                gfmGenArr_push(pState->pObjects);
            }
        }
        else {
            /* Something weird happened */
            ASSERT(0, GFMRV_INTERNAL_ERROR);
        }
    } /* while(1) parser */

    /* Initialize the fire particles */
    rv = gfmGroup_getNew(&(pState->pFire));
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_setDefSpriteset(pState->pFire, pGfx->pSset2x2);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_setDefDimensions(pState->pFire, 2 /* width */, 2 /* height */,
            0 /* offX */, 0 /* offY */);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_setDefVelocity(pState->pFire, 0 /* vx */, -25 /* vy */);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_setDefAcceleration(pState->pFire, 0 /* ax */, 6 /* ay */);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_setDeathOnLeave(pState->pFire, 1 /* doDie */);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_preCache(pState->pFire, 1024, 0/* infinite */);
    ASSERT(rv == GFMRV_OK, rv);

    /* Initialize the recipe */
    rv = recipeScroll_getNew(&(pGlobal->pRecipe));
    ASSERT(rv == GFMRV_OK, rv);
    /* TODO Load level from generator */
    do {
        itemType pData[T_MAX];
        int i;

        i = 0;
        while (i < T_MAX) {
            pData[i] = i;
            pData[i] = T_MOVE_VERTICAL;
            i++;
        }
        rv = recipeScroll_load(pGlobal->pRecipe, pData, sizeof(pData) / sizeof(int), -8);
        ASSERT(rv == GFMRV_OK, rv);
    } while(0);

    pGame->pState = pState;
    rv = GFMRV_OK;
__ret:
    if (rv != GFMRV_OK && pState) {
        /* Make sure to clean everything on error */
        pGame->pState = pState;
        gs_free();
        pGame->pState = 0;
    }

    return rv;
}

/**
 * Update everything
 */
gfmRV gs_update() {
    /** GFraMe return value */
    gfmRV rv;
    /** The new state */
    gamestate *pState;
    /** Count how many particles were spawned */
    int i;

    /* Check that the state is correct and retrieve it*/
    ASSERT(pGame->curState == ST_GAME, GFMRV_INTERNAL_ERROR);
    ASSERT(pGame->pState != 0, GFMRV_INTERNAL_ERROR);
    pState = (gamestate*)pGame->pState;

    /* Update the gesture recognizer */
    rv = gesture_update(pGlobal->pGesture);
    ASSERT(rv == GFMRV_OK, rv);

    /* Initialize the quadtree */
    rv = gfmQuadtree_initRoot(pGlobal->pQt, QT_X, QT_Y, QT_WIDTH, QT_HEIGHT,
            QT_MAX_DEPTH, QT_MAX_NODES);
    ASSERT(rv == GFMRV_OK, rv);

    /* Update the scroller */
    rv = recipeScroll_update(pGlobal->pRecipe);
    ASSERT(rv == GFMRV_OK, rv);
    /* Update all objects */
    gfmGenArr_callAll(pState->pObjects, object_update);

    /* Update the cauldron */
    rv = cauldron_update(pGlobal->pCauldron);
    ASSERT(rv == GFMRV_OK, rv);
    i = 0;
    while (i < 5) {
        gfmSprite *pSpr;

        /* Spawn some new particles */
        rv = gfmGroup_setDeathOnTime(pState->pFire,
                800 + (pState->curFire % 16 + 1) * 25 /* time to live */);
        ASSERT(rv == GFMRV_OK, rv);
        rv = gfmGroup_recycle(&pSpr, pState->pFire);
        ASSERT(rv == GFMRV_OK, rv);
        rv = gfmGroup_setPosition(pState->pFire, 79 /* x */, 104 /* y */);
        ASSERT(rv == GFMRV_OK, rv);
        rv = gfmGroup_setFrame(pState->pFire, 
                12 + ((19 * pState->curFire) % 4) /* tile */);
        ASSERT(rv == GFMRV_OK, rv);
        rv = gfmGroup_setVelocity(pState->pFire,
                -10 + (1 + pState->curFire % 23) /* vx */,
                -2 - (pState->curFire % 7) /* vy */);
        ASSERT(rv == GFMRV_OK, rv);

        /* Update the "RNG" */
        pState->curFire += 17;
        pState->curFire %= 32;

        i++;
    }
    rv = gfmGroup_update(pState->pFire, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);

    /* Everything was updated, check if the user failed */
    rv = recipeScroll_didFail(pGlobal->pRecipe);
    ASSERT(rv == GFMRV_TRUE || rv == GFMRV_FALSE, rv);
    if (rv == GFMRV_TRUE) {
        cauldron_doExplode(pGlobal->pCauldron);
    }

    rv = GFMRV_OK;
__ret:
    return GFMRV_OK;
}

/**
 * Draws everything
 */
gfmRV gs_draw() {
    /** GFraMe return value */
    gfmRV rv;
    /** The new state */
    gamestate *pState;

    /* Check that the state is correct and retrieve it*/
    ASSERT(pGame->curState == ST_GAME, GFMRV_INTERNAL_ERROR);
    ASSERT(pGame->pState != 0, GFMRV_INTERNAL_ERROR);
    pState = (gamestate*)pGame->pState;

    /* Draw the background */
    rv = gfm_drawTile(pGame->pCtx, pGfx->pSset256x128, 0 /* x */, 0 /* y */,
            1 /* tile */, 0 /* flip */);
    /* Draw the scroll */
    rv = recipeScroll_draw(pGlobal->pRecipe);
    ASSERT(rv == GFMRV_OK, rv);

    /* Draw the cauldron */
    rv = cauldron_draw(pGlobal->pCauldron);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_draw(pState->pFire, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);

    /* Draw all objects */
    gfmGenArr_callAll(pState->pObjects, object_draw);

    /* Draw the mask over the map */
    rv = gfm_drawTile(pGame->pCtx, pGfx->pSset32x128, 120 /* x */, 0 /* y */,
            13 /* tile */, 0 /* flip */);

    /* Draw info about the gesture */
    gesture_draw(pGlobal->pGesture);

    rv = GFMRV_OK;
__ret:
    return GFMRV_OK;
}

