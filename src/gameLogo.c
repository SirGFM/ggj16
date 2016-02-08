/**
 * @file src/gameLogo.c
 *
 * The game's logo
 */
#include <base/game_const.h>
#include <base/game_ctx.h>

#include <GFraMe/gfmAssert.h>
#include <GFraMe/gfmError.h>
#include <GFraMe/gfmSprite.h>

#include <ggj16/gameLogo.h>

#include <math.h>
#include <stdlib.h>
#include <string.h>

/** The game logo type */
struct stGameLogo {
    /** The main sprite that gets to move around */
    gfmSprite *pMainLogo;
    /** Sprite rendered at the main logo previous position */
    gfmSprite *pShadow1;
    /** Sprite rendered at the first shadow previous position */
    gfmSprite *pShadow2;
    /** Whether the logo has started tweening */
    int didStartTween;
    /** Whether the logo has finished tweening */
    int didFinishTween;
};

/**
 * Release all memory alloc'ed to the logo
 *
 * @param  [ in]ppLogo The logo
 */
void gameLogo_free(gameLogo **ppLogo) {
    /* Avoid freen'ing memory not alloc'ed */
    if (!ppLogo || !(*ppLogo)) {
        return;
    }

    /* Release all of its components and its memory */
    gfmSprite_free(&((*ppLogo)->pMainLogo));
    gfmSprite_free(&((*ppLogo)->pShadow1));
    gfmSprite_free(&((*ppLogo)->pShadow2));
    free(*ppLogo);
    *ppLogo = 0;
}

/**
 * Alloc and initialize a new logo, it starts bellow the screen, on its center
 *
 * @param  [out]ppLogo The alloc'ed logo
 * @return             GFraMe return value
 */
gfmRV gameLogo_getNew(gameLogo **ppLogo) {
    /** GFraMe return value */
    gfmRV rv;
    /** The alloc'ed logo */
    gameLogo *pLogo;

    pLogo = 0;

    ASSERT(ppLogo, GFMRV_ARGUMENTS_BAD);

    /* Alloc the logo and its sub-components */
    pLogo = (gameLogo*)malloc(sizeof(gameLogo));
    ASSERT(pLogo, GFMRV_ALLOC_FAILED);
    memset(pLogo, 0x0, sizeof(gameLogo));

    rv = gfmSprite_getNew(&(pLogo->pMainLogo));
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_getNew(&(pLogo->pShadow1));
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_getNew(&(pLogo->pShadow2));
    ASSERT(rv == GFMRV_OK, rv);

    /* Initialize everything on the proper position and with the proper sprite */
    rv = gfmSprite_init(pLogo->pMainLogo, (V_WIDTH - LOGO_WIDTH) / 2 /* x */,
            V_HEIGHT /* y */, LOGO_WIDTH, LOGO_HEIGHT, pGfx->pSset128x32,
            LOGO_OFFX, 0 /* off y */, 0 /* pChild */, 0 /* type */);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_init(pLogo->pShadow1, (V_WIDTH - LOGO_WIDTH) / 2 /* x */,
            V_HEIGHT /* y */, LOGO_WIDTH, LOGO_HEIGHT, pGfx->pSset128x32,
            LOGO_OFFX, 0 /* off y */, 0 /* pChild */, 0 /* type */);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_init(pLogo->pShadow2, (V_WIDTH - LOGO_WIDTH) / 2 /* x */,
            V_HEIGHT /* y */, LOGO_WIDTH, LOGO_HEIGHT, pGfx->pSset128x32,
            LOGO_OFFX, 0 /* off y */, 0 /* pChild */, 0 /* type */);
    ASSERT(rv == GFMRV_OK, rv);

    rv = gfmSprite_setVerticalVelocity(pLogo->pMainLogo, -LOGO_SPEED);
    ASSERT(rv == GFMRV_OK, rv);

    rv = gfmSprite_setFrame(pLogo->pMainLogo, 3);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_setFrame(pLogo->pShadow1, 5);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_setFrame(pLogo->pShadow2, 7);
    ASSERT(rv == GFMRV_OK, rv);

    *ppLogo = pLogo;
    rv = GFMRV_OK;
__ret:
    if (rv != GFMRV_OK && pLogo) {
        gameLogo_free(&pLogo);
    }
    return rv;
}

/**
 * Centralize the logo on the screen
 *
 * @param  [ in]pLogo The logo
 * @return            GFraMe return value
 */
gfmRV gameLogo_centralize(gameLogo *pLogo);

/**
 * Check whether the logo has finished tweening
 *
 * @param  [ in]pLogo The logo
 * @return            GFMRV_TRUE, GFMRV_FALSE
 */
gfmRV gameLogo_didTweenFinish(gameLogo *pLogo) {
    if (pLogo->didFinishTween) {
        return GFMRV_TRUE;
    }
    return GFMRV_FALSE;
}

/**
 * Set the logo's "shadows" position
 *
 * @param  [ in]pLogo The logo
 * @return            GFraMe return value
 */
static gfmRV _gameLogo_setShadowPosition(gameLogo *pLogo) {
    /** GFraMe return value */
    gfmRV rv;
    /** Last position of the logo */
    int x, y;

    /* Update the 'shadows' positions */
    rv = gfmSprite_getPosition(&x, &y, pLogo->pShadow1);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_setPosition(pLogo->pShadow2, x, y);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_getPosition(&x, &y, pLogo->pMainLogo);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_setPosition(pLogo->pShadow1, x, y);
    ASSERT(rv == GFMRV_OK, rv);

    rv = GFMRV_OK;
__ret:
    return rv;
}

/**
 * Calculate new random velocities
 *
 * @param  [out]pConst Component that must keep its direction. Must be passed
 *                     with its current value
 * @param  [out]pVar   The 'variable' component. It's value is returned in
 *                     absolute, so it may be later be inverted
 */
static void _gameLog_getNewVelocity(double *pConst, double *pVar) {
    /** Current signal */
    int signal;

    /* Retrieve the direction of the 'constant' part */
    signal = ((*pConst) > 0) * 2 - 1;
    /* Calculate the normal speed */
    *pConst = (rand() % LOGO_MODULE) / (double)LOGO_MODULE;
    *pVar = sqrt(1.0 - (*pConst) * (*pConst));
    /* Set its speed pixels per second */
    *pVar *= LOGO_SPEED;
    *pConst *= signal * LOGO_SPEED;
}

/**
 * Update the logo during the intro state. It moves toward a wall and gets
 * reflected in a random direction
 *
 * @param  [ in]pLogo The logo
 * @return            GFraMe return value
 */
gfmRV gameLogo_updateIntro(gameLogo *pLogo) {
    /** GFraMe return value */
    gfmRV rv;
    /** Last position of the logo */
    int x, y;

    rv = _gameLogo_setShadowPosition(pLogo);
    ASSERT(rv == GFMRV_OK, rv);

    rv = gfmSprite_getPosition(&x, &y, pLogo->pMainLogo);
    ASSERT(rv == GFMRV_OK, rv);

    /* Update the main sprite position, randomly reflecting on walls */
    if (x < 0) {
        /** Newly calculated velocities */
        double vx, vy;

        /* Get the vertical speed to maintain the direction */
        rv = gfmSprite_getVerticalVelocity(&vy, pLogo->pMainLogo);
        ASSERT(rv == GFMRV_OK, rv);

        /* Calculate and set a new random velocity */
        _gameLog_getNewVelocity(&vy, &vx);
        rv = gfmSprite_setVelocity(pLogo->pMainLogo, vx, vy);
        ASSERT(rv == GFMRV_OK, rv);
    }
    else if (x + LOGO_WIDTH > V_WIDTH) {
        /** Newly calculated velocities */
        double vx, vy;

        /* Get the vertical speed to maintain the direction */
        rv = gfmSprite_getVerticalVelocity(&vy, pLogo->pMainLogo);
        ASSERT(rv == GFMRV_OK, rv);

        /* Calculate and set a new random velocity */
        _gameLog_getNewVelocity(&vy, &vx);
        rv = gfmSprite_setVelocity(pLogo->pMainLogo, -vx, vy);
        ASSERT(rv == GFMRV_OK, rv);
    }
    else if (y < 0) {
        /** Newly calculated velocities */
        double vx, vy;

        /* Get the horizontal speed to maintain the direction */
        rv = gfmSprite_getHorizontalVelocity(&vy, pLogo->pMainLogo);
        ASSERT(rv == GFMRV_OK, rv);

        /* Calculate and set a new random velocity */
        _gameLog_getNewVelocity(&vx, &vy);
        /* Set its speed pixels per second */
        rv = gfmSprite_setVelocity(pLogo->pMainLogo, vx, vy);
        ASSERT(rv == GFMRV_OK, rv);
    }
    else if (y + LOGO_HEIGHT > V_HEIGHT) {
        /** Newly calculated velocities */
        double vx, vy;

        /* Get the horizontal speed to maintain the direction */
        rv = gfmSprite_getHorizontalVelocity(&vy, pLogo->pMainLogo);
        ASSERT(rv == GFMRV_OK, rv);

        /* Calculate and set a new random velocity */
        _gameLog_getNewVelocity(&vx, &vy);
        rv = gfmSprite_setVelocity(pLogo->pMainLogo, vx, -vy);
        ASSERT(rv == GFMRV_OK, rv);
    }

    /* Update the position of the logo */
    rv = gfmSprite_update(pLogo->pMainLogo, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);

    rv = GFMRV_OK;
__ret:
    return rv;
}

/**
 * Tween the logo from it's current position to the center
 *
 * @param  [ in]pLogo The logo
 * @return            GFraMe return value
 */
gfmRV gameLogo_updateTween(gameLogo *pLogo) {
    /** GFraMe return value */
    gfmRV rv;
    /** Distance to the desired position */
    int dx, dy;
    /** Current position of the logo */
    int x, y;

    if (!pLogo->didStartTween) {
        /** Calculate velocities */
        double div, vx, vy;

        /* If the logo just started tweening, calculate its velocity */

        rv = gfmSprite_getPosition(&x, &y, pLogo->pMainLogo);
        ASSERT(rv == GFMRV_OK, rv);

        /* Calculate the velocity and normalize it to LOGO_SPEED */
        vx = LOGO_X - x;
        vy = LOGO_Y - y;
        div = 1 / sqrt(vx * vx + vy * vy);
        vx *= div * LOGO_SPEED;
        vy *= div * LOGO_SPEED;

        rv = gfmSprite_setVelocity(pLogo->pMainLogo, vx, vy);
        ASSERT(rv == GFMRV_OK, rv);

        pLogo->didStartTween = 1;
    }

    /* Update the position of the logo */
    rv = _gameLogo_setShadowPosition(pLogo);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_update(pLogo->pMainLogo, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);

    /* Check if it has reached it destination and stop */
    rv = gfmSprite_getPosition(&x, &y, pLogo->pMainLogo);
    ASSERT(rv == GFMRV_OK, rv);
    dx = LOGO_X - x;
    dy = LOGO_Y - y;
    if (dx * dx < 4) {
        rv = gfmSprite_setHorizontalVelocity(pLogo->pMainLogo, 0.0);
        ASSERT(rv == GFMRV_OK, rv);
        rv = gfmSprite_setHorizontalPosition(pLogo->pMainLogo, LOGO_X);
        ASSERT(rv == GFMRV_OK, rv);
    }
    if (dy * dy < 4) {
        rv = gfmSprite_setVerticalVelocity(pLogo->pMainLogo, 0.0);
        ASSERT(rv == GFMRV_OK, rv);
        rv = gfmSprite_setVerticalPosition(pLogo->pMainLogo, LOGO_Y);
        ASSERT(rv == GFMRV_OK, rv);
    }

    /* If it finished tweening, add a small vertical speed (so it will animate
     * properly) */
    if (dx * dx + dy * dy == 0) {
        pLogo->didFinishTween = 1;
        rv = gfmSprite_setVerticalVelocity(pLogo->pMainLogo, LOGO_MAX_SPEED);
        ASSERT(rv == GFMRV_OK, rv);
    }

    rv = GFMRV_OK;
__ret:
    return rv;
}

/**
 * Moves the logo up and down
 *
 * @param  [ in]pLogo The logo
 * @return            GFraMe return value
 */
gfmRV gameLogo_update(gameLogo *pLogo) {
    /** Current speed, so it may be clamped */
    double vy;
    /** GFraMe return value */
    gfmRV rv;
    /** Distance to the desired position */
    int dy;
    /** Current position of the logo */
    int  y;

    /* Check if it should switch directions */
    rv = gfmSprite_getVerticalPosition(&y, pLogo->pMainLogo);
    ASSERT(rv == GFMRV_OK, rv);
    dy = LOGO_Y - y;
    if (dy > LOGO_MAX_DIST) {
        rv = gfmSprite_setVerticalAcceleration(pLogo->pMainLogo, LOGO_ACC);
        ASSERT(rv == GFMRV_OK, rv);
    }
    else if (dy < -LOGO_MAX_DIST) {
        rv = gfmSprite_setVerticalAcceleration(pLogo->pMainLogo, -LOGO_ACC);
        ASSERT(rv == GFMRV_OK, rv);
    }

    /* Clamp the logo's velocity */
    rv = gfmSprite_getVerticalVelocity(&vy, pLogo->pMainLogo);
    ASSERT(rv == GFMRV_OK, rv);
    if (vy > LOGO_MAX_SPEED) {
        rv = gfmSprite_setVerticalVelocity(pLogo->pMainLogo, LOGO_MAX_SPEED);
        ASSERT(rv == GFMRV_OK, rv);
    }
    if (vy < -LOGO_MAX_SPEED) {
        rv = gfmSprite_setVerticalVelocity(pLogo->pMainLogo, -LOGO_MAX_SPEED);
        ASSERT(rv == GFMRV_OK, rv);
    }

    /* Update the position of the logo */
    rv = _gameLogo_setShadowPosition(pLogo);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_update(pLogo->pMainLogo, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);

    rv = GFMRV_OK;
__ret:
    return rv;
}

/**
 * Render the logo
 *
 * @param  [ in]pLogo The logo
 * @return            GFraMe return value
 */
gfmRV gameLogo_draw(gameLogo *pLogo) {
    /** GFraMe return value */
    gfmRV rv;

    rv = gfmSprite_draw(pLogo->pShadow2, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_draw(pLogo->pShadow1, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_draw(pLogo->pMainLogo, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);

    rv = GFMRV_OK;
__ret:
    return rv;
}

