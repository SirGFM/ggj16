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

    /* Update the 'shadows' positions */
    rv = gfmSprite_getPosition(&x, &y, pLogo->pShadow1);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_setPosition(pLogo->pShadow2, x, y);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_getPosition(&x, &y, pLogo->pMainLogo);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_setPosition(pLogo->pShadow1, x, y);
    ASSERT(rv == GFMRV_OK, rv);

    /* Update the main sprite position, randomly reflecting on walls */
    if (x < 0) {
        int signal;
        double vx, vy;

        /* Get the vertical speed to maintain the direction */
        rv = gfmSprite_getVerticalVelocity(&vy, pLogo->pMainLogo);
        ASSERT(rv == GFMRV_OK, rv);
        signal = (vy > 0) * 2 - 1;

        /* Calculate the normal speed */
        vy = (rand() % LOGO_MODULE) / (double)LOGO_MODULE;
        vx = sqrt(1.0 - vy * vy);
        /* Set its speed pixels per second */
        vx *= LOGO_SPEED;
        vy *= signal * LOGO_SPEED;
        rv = gfmSprite_setVelocity(pLogo->pMainLogo, vx, vy);
        ASSERT(rv == GFMRV_OK, rv);
    }
    else if (x + LOGO_WIDTH > V_WIDTH) {
        int signal;
        double vx, vy;

        /* Get the vertical speed to maintain the direction */
        rv = gfmSprite_getVerticalVelocity(&vy, pLogo->pMainLogo);
        ASSERT(rv == GFMRV_OK, rv);
        signal = (vy > 0) * 2 - 1;

        /* Calculate the normal speed */
        vy = (rand() % LOGO_MODULE) / (double)LOGO_MODULE;
        vx = -sqrt(1.0 - vy * vy);
        /* Set its speed pixels per second */
        vx *= LOGO_SPEED;
        vy *= signal * LOGO_SPEED;
        rv = gfmSprite_setVelocity(pLogo->pMainLogo, vx, vy);
        ASSERT(rv == GFMRV_OK, rv);
    }
    else if (y < 0) {
        int signal;
        double vx, vy;

        /* Get the horizontal speed to maintain the direction */
        rv = gfmSprite_getHorizontalVelocity(&vy, pLogo->pMainLogo);
        ASSERT(rv == GFMRV_OK, rv);
        signal = (vy > 0) * 2 - 1;

        /* Calculate the normal speed */
        vx = (rand() % LOGO_MODULE) / (double)LOGO_MODULE;
        vy = sqrt(1.0 - vx * vx);
        /* Set its speed pixels per second */
        vx *= signal * LOGO_SPEED;
        vy *= LOGO_SPEED;
        rv = gfmSprite_setVelocity(pLogo->pMainLogo, vx, vy);
        ASSERT(rv == GFMRV_OK, rv);
    }
    else if (y + LOGO_HEIGHT > V_HEIGHT) {
        int signal;
        double vx, vy;

        /* Get the horizontal speed to maintain the direction */
        rv = gfmSprite_getHorizontalVelocity(&vy, pLogo->pMainLogo);
        ASSERT(rv == GFMRV_OK, rv);
        signal = (vy > 0) * 2 - 1;

        /* Calculate the normal speed */
        vx = (rand() % LOGO_MODULE) / (double)LOGO_MODULE;
        vy = -sqrt(1.0 - vx * vx);
        /* Set its speed pixels per second */
        vx *= signal * LOGO_SPEED;
        vy *= LOGO_SPEED;
        rv = gfmSprite_setVelocity(pLogo->pMainLogo, vx, vy);
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
gfmRV gameLogo_updateTween(gameLogo *pLogo);

/**
 * Moves the logo up and down
 *
 * @param  [ in]pLogo The logo
 * @return            GFraMe return value
 */
gfmRV gameLogo_update(gameLogo *pLogo);

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

