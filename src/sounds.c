/**
 * @file src/sounds.c
 *
 * Manages playing sfx and songs
 */
#include <base/assets.h>
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
    if (pGame->loadedAssets < SFX_ON_SWIPE_IN_HND ||
            pGame->loadedAssets < SFX_ON_SWIPE_OUT_HND) {
        return GFMRV_OK;
    }
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
    if (pGame->loadedAssets < SFX_ON_ENTER_HND) {
        return GFMRV_OK;
    }
    return gfm_playAudio(0, pGame->pCtx, pAudio->sfx_onEnterItem, 0.4);
}

/**
 * Play the sfx for adding the wrong item
 *
 * @return GFraMe return value
 */
gfmRV sound_onWrongItem() {
    if (pGame->loadedAssets < SFX_ON_WRONG_HND) {
        return GFMRV_OK;
    }
    return gfm_playAudio(0, pGame->pCtx, pAudio->sfx_onWrongItem, 0.4);
}

/**
 * Play the bg song (only once!)
 *
 * @return GFraMe return value
 */
gfmRV sound_playSong() {
    if (pGame->loadedAssets < SONG_HND || pGlobal->pSong) {
        return GFMRV_OK;
    }
    return gfm_playAudio(&(pGlobal->pSong), pGame->pCtx, pAudio->song, 1.0);
}

