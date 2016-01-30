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
};
typedef struct stGamestate gamestate;

static char *dictStr[] = {
    "dummy"
};
static int dictType[] = {
    0
};
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

    /* Draw the tilemap */
    rv = gfmTilemap_draw(pState->pBackground, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);
    /* Draw all objects */
    gfmGenArr_callAll(pState->pObjects, object_draw);

    rv = GFMRV_OK;
__ret:
    return GFMRV_OK;
}

