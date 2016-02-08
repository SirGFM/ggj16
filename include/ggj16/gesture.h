/**
 * @file include/ggj16/gesture.h
 *
 * Recognizes gestures from the mouse
 */
#ifndef __GESTURE_STRUCT__
#define __GESTURE_STRUCT__

typedef enum enMoveState moveState;
typedef struct stGesture gesture;

#endif /* __GESTURE_STRUCT__ */

#ifndef __GESTURE_H__
#define __GESTURE_H__

#include <GFraMe/gfmError.h>

#include <ggj16/type.h>

enum enMoveState {
    MOVE_LEFT  = 0x0001,
    MOVE_RIGHT = 0x0002,
    MOVE_UP    = 0x0004,
    MOVE_DOWN  = 0x0008
};

/**
 * Release the struct
 *
 * @param  [ in]ppCtx The recognizer
 */
void gesture_free(gesture **ppCtx);

/**
 * Alloc and initialize a new recognizer
 *
 * @param  [out]ppCtx The alloc'ed recognizer
 * @return            GFraMe return value
 */
gfmRV gesture_getNew(gesture **ppCtx);

/**
 * Reset the gesture recognizer so no gestures are incorrectly detected
 *
 * @param  [ in]pCtx The recognizer
 */
void gesture_reset(gesture *pCtx);

/**
 * Update the recognizer
 *
 * @param  [ in]pCtx The recognizer
 * @return           GFraMe return value
 */
gfmRV gesture_update(gesture *pCtx);

/**
 * Mostly for debug, draw random stuff
 *
 * @param  [ in]pCtx The recognizer
 */
void gesture_draw(gesture *pGesture);

/**
 * Retrieve the current gesture (if any)
 *
 * @param  [out]pItem The current gestures (must have 4 positions)
 * @param  [ in]pCtx  The recognizer
 * @return            GFraMe return value
 */
gfmRV gesture_getCurrentGesture(itemType *pItem, gesture *pCtx);

#endif /* __GESTURE_H__ */

