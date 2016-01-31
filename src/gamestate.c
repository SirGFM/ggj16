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
#include <GFraMe/gfmParser.h>
#include <GFraMe/gfmTilemap.h>

#include <ggj16/gamestate.h>
#include <ggj16/object.h>
#include <ggj16/recipeScroll.h>
#include <ggj16/type.h>

#include <stdlib.h>
#include <string.h>

gfmGenArr_define(object);

#define gfmTilemap_loadfStatic(pTMap, pCtx, pFilename, pDictNames, pDictTypes, dictLen) \
    gfmTilemap_loadf(pTMap, pCtx, pFilename, sizeof(pFilename) - 1, pDictNames, pDictTypes, dictLen)

struct stGamestate {
    /** The background */
    gfmTilemap *pBackground;
    /** List of objects */
    gfmGenArr_var(object, pObjects);
    /** Current recipe */
    recipeScroll *pRecipe;
};
typedef struct stGamestate gamestate;

static char *dictStr[] = { "dummy" };
static int dictType[] = { 0 };
static int dictLen = sizeof(dictType) / sizeof(int);

/**
 * Release everything alloc'ed on init
 */
void gs_free() {
    /** The new state */
    gamestate *pState;

    /* Retrieve the current state from the global one */
    pState = (gamestate*)pGame->pState;

    gfmTilemap_free(&(pState->pBackground));
    recipeScroll_free(&(pState->pRecipe));
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

    /* Check that the state is correct and there's none loaded */
    ASSERT(pGame->nextState == ST_GAME, GFMRV_INTERNAL_ERROR);
    ASSERT(pGame->pState == 0, GFMRV_INTERNAL_ERROR);

    pState = (gamestate*)malloc(sizeof(gamestate));
    ASSERT(pState, GFMRV_ALLOC_FAILED);
    memset(pState, 0x0, sizeof(gamestate));

    /* Load the background */
    rv = gfmTilemap_getNew(&(pState->pBackground));
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmTilemap_init(pState->pBackground, pGfx->pSset8x8, 1, 1, -1);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmTilemap_loadfStatic(pState->pBackground, pGame->pCtx,
            "map/map_map.gfm", dictStr, dictType, dictLen);
    ASSERT(rv == GFMRV_OK, rv);

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
            object *pObj;

            /* Parse and spawn the object */
            gfmGenArr_getNextRef(object, pState->pObjects, 1/* inc */, pObj,
                    object_getNew);
            rv = object_init(pObj, pParser);
            ASSERT(rv == GFMRV_OK, rv);
            gfmGenArr_push(pState->pObjects);
        }
        else {
            /* Something weird happened */
            ASSERT(0, GFMRV_INTERNAL_ERROR);
        }
    } /* while(1) parser */

    /* Initialize the recipe */
    rv = recipeScroll_getNew(&(pState->pRecipe));
    ASSERT(rv == GFMRV_OK, rv);
    /* TODO Load level from generator */
    do {
        itemType pData[T_MAX];
        int i;

        i = 0;
        while (i < T_MAX) {
            pData[i] = i;
            i++;
        }
        rv = recipeScroll_load(pState->pRecipe, pData, sizeof(pData) / sizeof(int), -48);
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

    /* Check that the state is correct and retrieve it*/
    ASSERT(pGame->curState == ST_GAME, GFMRV_INTERNAL_ERROR);
    ASSERT(pGame->pState != 0, GFMRV_INTERNAL_ERROR);
    pState = (gamestate*)pGame->pState;

    /* Initialize the quadtree */
    rv = gfmQuadtree_initRoot(pGlobal->pQt, QT_X, QT_Y, QT_WIDTH, QT_HEIGHT,
            QT_MAX_DEPTH, QT_MAX_NODES);
    ASSERT(rv == GFMRV_OK, rv);

    /* Update the scroller */
    rv = recipeScroll_update(pState->pRecipe);
    ASSERT(rv == GFMRV_OK, rv);
    /* Update the tilemap (e.g., if it's animated) */
    rv = gfmTilemap_update(pState->pBackground, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);
    /* Update all objects */
    gfmGenArr_callAll(pState->pObjects, object_update);

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
    rv = gfmTilemap_draw(pState->pBackground, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);
    /* Draw the scroll */
    rv = recipeScroll_draw(pState->pRecipe);
    ASSERT(rv == GFMRV_OK, rv);

    /* Draw all objects */
    gfmGenArr_callAll(pState->pObjects, object_draw);

    rv = GFMRV_OK;
__ret:
    return GFMRV_OK;
}

