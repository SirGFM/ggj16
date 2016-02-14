/**
 * @file src/menustate.c
 *
 * The menu state. Handles the intro animation (that plays while loading the
 * audio!!!), switching to states (options/game/...) etc
 */
#include <base/assets.h>
#include <base/game_const.h>
#include <base/game_ctx.h>

#include <GFraMe/gfmAssert.h>
#include <GFraMe/gfmError.h>
#include <GFraMe/gfmSprite.h>
#include <GFraMe/gfmText.h>

#include <ggj16/gesture.h>
#include <ggj16/gameLogo.h>
#include <ggj16/menustate.h>

#include <stdlib.h>
#include <string.h>

struct stMenustate {
    /** The game's logo */
    gameLogo *pLogo;
    /** Face icon that takes you to the credits */
    gfmSprite *pFaceIcon;
    /** Text that displays while loading */
    gfmText *pLoadingTxt;
    /** Text that renders 'a game by' */
    gfmText *pGameBy;
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
    gfmSprite_free(&(pState->pFaceIcon));
    gameLogo_free(&(pState->pLogo));
    gfmText_free(&(pState->pGameBy));
    gfmText_free(&(pState->pLoadingTxt));

    free(pState);
    pGame->pState = 0;
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

    /* Initialize the logo */
    rv = gameLogo_getNew(&(pState->pLogo));
    ASSERT(rv == GFMRV_OK, rv);

    /* Initialize the '--LOADING--' text */
    rv = gfmText_getNew(&(pState->pLoadingTxt));
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmText_init(pState->pLoadingTxt, (V_WIDTH - 15 * 8) / 2 /* x */,
            V_HEIGHT - 48 /* y */, 15 /* maxWidth */, 1 /* maxLines */,
            110 /* delay */, 0 /* bind to screen */, pGfx->pSset8x8,
            0 /* first ASCII tile */);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmText_setTextStatic(pState->pLoadingTxt, "  --LOADING--  ",
            1 /* doCopy */);
    ASSERT(rv == GFMRV_OK, rv);

    rv = gfmText_getNew(&(pState->pGameBy));
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmText_init(pState->pGameBy, (V_WIDTH - 9 * 8) - 32 /* x */,
            V_HEIGHT - 16 /* y */, 10 /* maxWidth */, 2 /* maxLines */,
            110 /* delay */, 0 /* bind to screen */, pGfx->pSset8x8,
            0 /* first ASCII tile */);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmText_setTextStatic(pState->pGameBy, "A GAME BY\n"" @SIRGFM",
            1 /* doCopy */);
    ASSERT(rv == GFMRV_OK, rv);

    rv = gfmSprite_getNew(&(pState->pFaceIcon));
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_init(pState->pFaceIcon, V_WIDTH - 32/* x */,
            V_HEIGHT - 32 /* y */, 32 /* w */, 32 /* h */, pGfx->pSset32x32,
            0 /* off x */, 0 /* off y */, 0 /* pChild */, 0 /* type */);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_setFrame(pState->pFaceIcon, 63);
    ASSERT(rv == GFMRV_OK, rv);

    pState->loadingResetTime = 2000;

    gesture_reset(pGlobal->pGesture);

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
            rv = gfmText_setTextStatic(pState->pLoadingTxt, "  --LOADING--  ",
                    1 /* doCopy */);
            ASSERT(rv == GFMRV_OK, rv);
        }
    }
    /* Update the text's animation */
    rv = gfmText_update(pState->pLoadingTxt, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);

    rv = gameLogo_updateIntro(pState->pLogo);
    ASSERT(rv == GFMRV_OK, rv);

    if (pGame->loadedAssets >= MAX_HND) {
        pGame->curState = ST_MENU_TWEEN;
        rv = gfmText_setTextStatic(pState->pLoadingTxt, "  GAME LOADED  ",
                1 /* doCopy */);
    }

    rv = GFMRV_OK;
__ret:
    return rv;
}

/**
 * Tweens the logo to the proper position
 */
gfmRV ms_tweenUpdate() {
    /** GFraMe return value */
    gfmRV rv;
    /** The current state */
    menustate *pState;

    /* Retrieve the current state from the global one */
    pState = (menustate*)pGame->pState;

    rv = gfmText_update(pState->pLoadingTxt, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gameLogo_updateTween(pState->pLogo);
    ASSERT(rv == GFMRV_OK, rv);

    if (gameLogo_didTweenFinish(pState->pLogo) == GFMRV_TRUE) {
        pGame->curState = ST_MENU;
        rv = gfmText_setTextStatic(pState->pLoadingTxt, "TOUCH TO START ",
                1 /* doCopy */);
        ASSERT(rv == GFMRV_OK, rv);
    }

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

    rv = gameLogo_update(pState->pLogo);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmText_update(pState->pLoadingTxt, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmText_update(pState->pGameBy, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);

    if ((pButton->click.state & gfmInput_justPressed) ==
            gfmInput_justPressed) {
        /** Framework's input context */
        gfmInput *pInput;
        /** Mouse position, in screen space */
        int mouseX, mouseY;

        /* Retrieve the current mouse position */
        rv = gfm_getInput(&pInput, pGame->pCtx);
        ASSERT(rv == GFMRV_OK, rv);
        rv = gfmInput_getPointerPosition(&mouseX, &mouseY, pInput);
        ASSERT(rv == GFMRV_OK, rv);
        /* Check if should switch to credits */
        rv = gfmSprite_isPointInside(pState->pFaceIcon, mouseX, mouseY);
        ASSERT(rv == GFMRV_TRUE || rv == GFMRV_FALSE, rv);
        if (rv == GFMRV_TRUE) {
            pGame->nextState = ST_CREDITS;
        }
    }
    else if ((pButton->click.state & gfmInput_justReleased) ==
            gfmInput_justReleased) {
        pGame->nextState = ST_GAME;
    }

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

    rv = gameLogo_draw(pState->pLogo);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmText_draw(pState->pLoadingTxt, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmText_draw(pState->pGameBy, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);
    if (pGame->curState == ST_MENU) {
        rv = gfmSprite_draw(pState->pFaceIcon, pGame->pCtx);
        ASSERT(rv == GFMRV_OK, rv);
    }

    rv = GFMRV_OK;
__ret:
    return rv;
}

