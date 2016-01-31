/**
 * @file src/gesture.c
 *
 * Recognizes gestures from the mouse
 */
#include <base/game_const.h>
#include <base/game_ctx.h>

#include <GFraMe/gfmAssert.h>
#include <GFraMe/gfmError.h>
#include <GFraMe/gfmInput.h>

#include <ggj16/gesture.h>
#include <ggj16/type.h>

#include <math.h>
#include <stdlib.h>
#include <string.h>

#define PI 3.1415926

#if defined(DEBUG) && !(defined(__WIN32) || defined(__WIN32__))
#  include <signal.h>
#endif

enum enMoveState {
    MOVE_LEFT  = 0x0001,
    MOVE_RIGHT = 0x0002,
    MOVE_UP    = 0x0004,
    MOVE_DOWN  = 0x0008
};
typedef enum enMoveState moveState;

struct stGesture {
    /** Last angle */
    double lastAng;
    /** Delta angle */
    double dAng;
    /** Delta horizontal movement */
    int dX;
    /** Delta vertical movement */
    int dY;
    /** Last horizontal position in the screen */
    int lastX;
    /** Last vertical position in the screen */
    int lastY;
    /** Flag that signals when the struct has been just reset, so it's properly
     * initialized */
    int justReset;
    /** Allow a few frames (urg) of error when spinning */
    int angErr;
    /** Allow a few frames (urg) of error when moving */
    int xErr;
    int yErr;
    /** Check the movement state */
    moveState move;
};

/**
 * Release the struct
 *
 * @param  [ in]ppCtx The recognizer
 */
void gesture_free(gesture **ppCtx) {
    if (!ppCtx) {
        return;
    }

    free(*ppCtx);
    *ppCtx = 0;
}

/**
 * Alloc and initialize a new recognizer
 *
 * @param  [out]ppCtx The alloc'ed recognizer
 * @return            GFraMe return value
 */
gfmRV gesture_getNew(gesture **ppCtx) {
    /** The alloc'ed gesture */
    gesture *pCtx;
    /** GFraMe return value */
    gfmRV rv;

    /* Sanitize arguments */
    ASSERT(ppCtx, GFMRV_ARGUMENTS_BAD);

    pCtx = (gesture*)malloc(sizeof(gesture));
    ASSERT(pCtx, GFMRV_ALLOC_FAILED);
    gesture_reset(pCtx);

    *ppCtx = pCtx;
    rv = GFMRV_OK;
__ret:
    return rv;
}

/**
 * Reset the gesture recognizer so no gestures are incorrectly detected
 *
 * @param  [ in]pCtx The recognizer
 */
void gesture_reset(gesture *pCtx) {
    memset(pCtx, 0x0, sizeof(gesture));
    pCtx->justReset = 1;
}

/**
 * Update the recognizer
 *
 * @param  [ in]pCtx The recognizer
 * @return           GFraMe return value
 */
gfmRV gesture_update(gesture *pCtx) {
    /** Angle moved since the last frame (range: [0, 2*pi] )*/
    double curAng;
    /** Framework's input context */
    gfmInput *pInput;
    /** GFraMe return value */
    gfmRV rv;
    /** Distance of the mouse from the center of the screen */
    int distCX, distCY;
    /** Current mouse position on the screen */
    int mouseX, mouseY;

    /* Retrieve the current mouse position */
    rv = gfm_getInput(&pInput, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmInput_getPointerPosition(&mouseX, &mouseY, pInput);
    ASSERT(rv == GFMRV_OK, rv);

    /* Calculate the current angle (NOTE: The vertical distance must be
     * inverted since the origin is on the top left corner) */
    distCX = mouseX - V_CENTER_X;
    distCY = -(mouseY - V_CENTER_Y);
    /* Avoid corner cases */
    if (distCX == 0) {
        if (distCY > 0) {
            curAng = PI / 2;
        }
        else {
            curAng = 3 * PI / 2;
        }
    }
    else if (distCY == 0) {
        if (distCX > 0) {
            curAng = 0.0;
        }
        else {
            curAng = PI;
        }
    }
    else {
        /* Get the angle in [0, PI/2] */
        curAng = atan(((double)distCX) / (double)distCY);
        if (curAng < 0.0) {
            curAng = -curAng;
        }

        /* Convert it to [0, 2*PI] */
        if (distCX >= 0 && distCY >= 0) {
            /* Q1 [0, PI/2) */
            curAng = PI / 2.0 - curAng;
        }
        else if (distCX < 0 && distCY >= 0) {
            /* Q2 [PI/2, PI) */
            curAng = curAng + PI / 2.0;
        }
        else if (distCX < 0 && distCY < 0) {
            /* Q3 [PI, 3*PI/2) */
            curAng = 3 * PI / 2.0 - curAng;
        }
        else if (distCX >= 0 && distCY < 0) {
            /* Q4 [3*PI/2, 2*PI) */
            curAng = curAng + 3 * PI / 2.0;
        }
    }

    /** Only update the state if we know the previous state */
    if (!pCtx->justReset) {
        double deltaAng;
        int dX, dY;

        /* Update the movement state */
        dX = mouseX - pCtx->lastX;
        dY = mouseY - pCtx->lastY;
        if (dX != 0) {
            /* Few frames of leeway */
            if ((pCtx->dX >= 0 && dX < 0) || (pCtx->dX <= 0 && dX > 0)) {
                pCtx->xErr++;
                if (pCtx->xErr > 3) {
                    pCtx->dX = 0;
                }
            }
            else {
                pCtx->xErr = 0;
            }

            /* Update movement */
            pCtx->dX += dX;
            if (pCtx->dX > 40) {
                pCtx->move |= MOVE_RIGHT;
            }
            else if (pCtx->dX < -40) {
                pCtx->move |= MOVE_LEFT;
            }
        }
        if (dY != 0) {
            /* Few frames of leeway */
            if ((pCtx->dY >= 0 && dY < 0) || (pCtx->dY <= 0 && dY > 0)) {
                pCtx->yErr++;
                if (pCtx->yErr > 3) {
                    pCtx->dY = 0;
                }
            }
            else {
                pCtx->yErr = 0;
            }

            /* Update movemente */
            pCtx->dY += dY;
            if (pCtx->dY > 40) {
                pCtx->move |= MOVE_DOWN;
            }
            else if (pCtx->dY < -40) {
                pCtx->move |= MOVE_UP;
            }
        }

        /* Update the angular state */
        deltaAng = curAng - pCtx->lastAng;
        if ((deltaAng >= 0.0 && pCtx->dAng >= 0.0)) {
            pCtx->dAng += deltaAng;
            pCtx->angErr = 0;
        }
        else if (curAng < PI / 2 &&
                pCtx->lastAng > 3 * PI / 2 &&
                2 * PI + deltaAng >= 0.0 &&
                pCtx->dAng >= 0.0) {
            /* Just went over 2*PI (corner case) */
            pCtx->dAng += 2 * PI + deltaAng;
            pCtx->angErr = 0;
        }
        else if (deltaAng <= 0.0 && pCtx->dAng <= 0.0) {
            pCtx->dAng += deltaAng;
            pCtx->angErr = 0;
        }
        else if (curAng > 3 * PI / 2 &&
                pCtx->lastAng < PI / 2 &&
                deltaAng - 2 * PI <= 0.0 &&
                pCtx->dAng <= 0.0) {
            /* Just went over 0 (corner case) */
            pCtx->dAng += deltaAng - 2 * PI;
            pCtx->angErr = 0;
        }
        else if (deltaAng != 0.0) {
            /* Mouse on the opposite direction, reset */
            pCtx->angErr++;
            if (pCtx->angErr > 3) {
                pCtx->dAng = 0.0;
                pCtx->angErr = 0;
            }
        }
    }

    /* Store the current state to compare on the next frame */
    pCtx->lastX = mouseX;
    pCtx->lastY = mouseY;
    pCtx->lastAng = curAng;
    pCtx->justReset = 0;

    rv = GFMRV_OK;
__ret:
    return rv;
}

/**
 * Mostly for debug, draw random stuff
 *
 * @param  [ in]pCtx The recognizer
 */
void gesture_draw(gesture *pGesture) {
#if defined(DEBUG)
    /* Debug dX */
    gfm_drawTile(pGame->pCtx, pGfx->pSset8x8, 0/*x*/, 120-24/*y*/, 'Y' - '!',
            0/*flip*/);
    gfm_drawNumber(pGame->pCtx, pGfx->pSset8x8, 8/*x*/, 120-24/*y*/,
            (int)pGesture->dY, 3/*numDigits*/, 0/*first ascii tile*/);

    /* Debug dY */
    gfm_drawTile(pGame->pCtx, pGfx->pSset8x8, 0/*x*/, 120-16/*y*/, 'X' - '!',
            0/*flip*/);
    gfm_drawNumber(pGame->pCtx, pGfx->pSset8x8, 8/*x*/, 120-16/*y*/,
            (int)pGesture->dX, 3/*numDigits*/, 0/*first ascii tile*/);

    /* Debug dAng */
    gfm_drawTile(pGame->pCtx, pGfx->pSset8x8, 0/*x*/, 120-8/*y*/, 'A' - '!',
            0/*flip*/);
    gfm_drawNumber(pGame->pCtx, pGfx->pSset8x8, 8/*x*/, 120-8/*y*/,
            (int)pGesture->dAng, 3/*numDigits*/, 0/*first ascii tile*/);
    gfm_drawNumber(pGame->pCtx, pGfx->pSset8x8, 40/*x*/, 120-8/*y*/,
            (int)(pGesture->dAng * 100) - ((int)pGesture->dAng) * 100,
            3/*numDigits*/, 0/*first ascii tile*/);
#endif
}

/**
 * Retrieve the current gesture (if any)
 *
 * @param  [out]pItem The current gestures (must have 4 positions)
 * @param  [ in]pCtx  The recognizer
 * @return            GFraMe return value
 */
gfmRV gesture_getCurrentGesture(itemType *pItem, gesture *pCtx) {
    /** GFraMe return value */
    gfmRV rv;
    /** Iterate through items */
    int i;

    /* Sanitize arguments */
    ASSERT(pItem, GFMRV_ARGUMENTS_BAD);
    ASSERT(pCtx, GFMRV_ARGUMENTS_BAD);

    /* Set all available actions */
    i = 0;
    while (i < 4) {
        pItem[i] = T_NONE;
        switch (i) {
            case 0: {
                if (pCtx->dAng > 2 * PI) {
                    pItem[i] = T_ROTATE_CCW;
                }
            } break;
            case 1: {
                if (pCtx->dAng < -2 * PI) {
                    pItem[i] = T_ROTATE_CW;
                }
            } break;
            case 2: {
                if ((pCtx->move & MOVE_LEFT) && (pCtx->move & MOVE_RIGHT)) {
                    pItem[i] = T_MOVE_HORIZONTAL;
                }
            } break;
            case 3: {
                if ((pCtx->move & MOVE_LEFT) && (pCtx->move & MOVE_RIGHT)) {
                    pItem[i] = T_MOVE_VERTICAL;
                }
            } break;
        }
        i++;
    }

    rv = GFMRV_OK;
__ret:
    return rv;
}

