/**
 * @file src/sounds.c
 *
 * Manages playing sfx and songs
 */
#include <base/game_ctx.h>

#include <GFraMe/gfmAssert.h>
#include <GFraMe/gfmError.h>

#include <ggj16/sounds.h>

/**
 * Play the 'swipe' sfx
 *
 * @return GFraMe return value
 */
gfmRV sound_playSwipe() {
    if (pGlobal->sfx_lastSwipeWasIn) {
        pGlobal->sfx_lastSwipeWasIn = 0;
        return gfm_playAudio(0, pGame->pCtx, pAudio->sfx_onSwipeOut, 0.4);
    }
    else {
        pGlobal->sfx_lastSwipeWasIn = 1;
        return gfm_playAudio(0, pGame->pCtx, pAudio->sfx_onSwipeIn, 0.4);
    }
}

/**
 * Play the sfx for items entering the central area
 *
 * @return GFraMe return value
 */
gfmRV sound_playEnterItem() {
    return gfm_playAudio(0, pGame->pCtx, pAudio->sfx_onEnterItem, 0.4);
}

/**
 * Play the sfx for adding the wrong item
 *
 * @return GFraMe return value
 */
gfmRV sound_onWrongItem() {
    return gfm_playAudio(0, pGame->pCtx, pAudio->sfx_onWrongItem, 0.4);
}

