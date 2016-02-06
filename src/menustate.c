/**
 * @file src/menustate.c
 *
 * The menu state. Handles the intro animation (that plays while loading the
 * audio!!!), switching to states (options/game/...) etc
 */
#include <base/game_const.h>
#include <base/game_ctx.h>

#include <GFraMe/gfmAssert.h>
#include <GFraMe/gfmError.h>
#include <GFraMe/gfmText.h>

//#include <ggj16/gameLogo.h>
#include <ggj16/menustate.h>

#include <stdlib.h>
#include <string.h>

struct stMenustate {
    /** The game's logo */
    //gameLogo *pLogo;
    /** Text that displays while loading */
    gfmText *pLoadingTxt;
    /** Countdown for reseting the loading text */
    int loadingResetTime;
};
typedef struct stMenustate menustate;

/**
 * Release everything alloc'ed on init
 */
void ms_free() {
    /** The current state */
    menustate *pState;

    /* Retrieve the current state from the global one */
    pState = (menustate*)pGame->pState;

    /* Avoid errors if it hasn't been alloc'ed */
    if (!pState) {
        return;
    }

    /* Release everything else */
    gfmText_free(&(pState->pLoadingTxt));
}

/**
 * Initialize the menu state (alloc anything needed etc)
 */
gfmRV ms_init() {
    /** GFraMe return value */
    gfmRV rv;
    /** The new state */
    menustate *pState;

    /* Alloc the state and every of its components */
    pState = (menustate*)malloc(sizeof(menustate));
    ASSERT(pState, GFMRV_ALLOC_FAILED);
    memset(pState, 0x0, sizeof(menustate));

    rv = gfmText_getNew(&(pState->pLoadingTxt));
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmText_init(pState->pLoadingTxt, (V_WIDTH - 11 * 8) / 2 /* x */,
            V_HEIGHT - 16 /* y */, 11 /* maxWidth */, 1 /* maxLines */,
            110 /* delay */, 0 /* bind to screen */, pGfx->pSset8x8,
            0 /* first ASCII tile */);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmText_setTextStatic(pState->pLoadingTxt, "--LOADING--",
            1 /* doCopy */);
    ASSERT(rv == GFMRV_OK, rv);

    pState->loadingResetTime = 2000;

    /* Store the current state for later use */
    pGame->pState = (void*)pState;
    rv = GFMRV_OK;
__ret:
    if (rv != GFMRV_OK && pState) {
        /* Temporarily store the state so it can be properly cleared */
        pGame->pState = pState;
        ms_free();
        pGame->pState = 0;
    }

    return rv;
}

/**
 * Updates the intro state (in which a simple animation is played until
 * everything is ready)
 */
gfmRV ms_introUpdate() {
    /** GFraMe return value */
    gfmRV rv;
    /** The current state */
    menustate *pState;

    /* Retrieve the current state from the global one */
    pState = (menustate*)pGame->pState;

    if (gfmText_didFinish(pState->pLoadingTxt) == GFMRV_TRUE) {
        if (pState->loadingResetTime) {
            pState->loadingResetTime -= pGame->elapsed;
        }
        else {
            pState->loadingResetTime += 2000;
            /* Restart the loading animation, if it finished */
            rv = gfmText_setTextStatic(pState->pLoadingTxt, "--LOADING--",
                    1 /* doCopy */);
            ASSERT(rv == GFMRV_OK, rv);
        }
    }
    /* Update the text's animation */
    rv = gfmText_update(pState->pLoadingTxt, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);

    rv = GFMRV_OK;
__ret:
    return rv;
}

/**
 * Update everything
 */
gfmRV ms_update() {
    /** GFraMe return value */
    gfmRV rv;
    /** The current state */
    menustate *pState;

    /* Retrieve the current state from the global one */
    pState = (menustate*)pGame->pState;

    rv = GFMRV_OK;
__ret:
    return rv;
}

/**
 * Draws everything
 */
gfmRV ms_draw() {
    /** GFraMe return value */
    gfmRV rv;
    /** The current state */
    menustate *pState;

    /* Retrieve the current state from the global one */
    pState = (menustate*)pGame->pState;

    if (pGame->curState == ST_MENU_INTRO) {
        /* Only render the 'loading' text if on the first part */
        rv = gfmText_draw(pState->pLoadingTxt, pGame->pCtx);
        ASSERT(rv == GFMRV_OK, rv);
    }

    rv = GFMRV_OK;
__ret:
    return rv;
}

