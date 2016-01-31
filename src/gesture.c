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
            curAng = -PI / 2;
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
    }

    /* Store the current state to compare on the next frame */
    pCtx->lastX = mouseX;
    pCtx->lastY = mouseY;
    pCtx->lastAng = curAng;

    rv = GFMRV_OK;
__ret:
    return rv;
}

/**
 * Retrieve the current gesture (if any)
 *
 * @param  [out]pItem The current gesture
 * @param  [ in]pCtx  The recognizer
 * @return            GFraMe return value
 */
gfmRV gesture_getCurrentGesture(itemType *pItem, gesture *pCtx) {
    /** GFraMe return value */
    gfmRV rv;

    /* Sanitize arguments */
    ASSERT(pItem, GFMRV_ARGUMENTS_BAD);
    ASSERT(pCtx, GFMRV_ARGUMENTS_BAD);

    /* TODO */

    rv = GFMRV_OK;
__ret:
    return rv;
}

