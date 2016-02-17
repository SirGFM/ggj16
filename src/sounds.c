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

#include <stdlib.h>

#define SFX_DRIP_VOL    0.25
#define SFX_BUBBLE_VOL  0.25
#define SFX_SWIPE_VOL   0.25
#define SFX_ITEM_VOL    0.5
#define SONG_VOL        0.8

/**
 * Play the 'drip' sfx
 *
 * @return GFraMe return value
 */
gfmRV sound_drip() {
    if (pGame->loadedAssets < SFX_DRIP) {
        return GFMRV_OK;
    }
    return gfm_playAudio(0, pGame->pCtx, pAudio->sfx_drip, SFX_DRIP_VOL);
}

/**
 * Play the 'bubble' sfx
 *
 * @return GFraMe return value
 */
gfmRV sound_bubble() {
    if (pGame->loadedAssets < SFX_BUBBLE_1 ||
            pGame->loadedAssets < SFX_BUBBLE_2) {
        return GFMRV_OK;
    }
    if ((rand() % 17) % 2 == 0) {
        return gfm_playAudio(0, pGame->pCtx, pAudio->sfx_bubble1,
                SFX_BUBBLE_VOL);
    }
    else {
        return gfm_playAudio(0, pGame->pCtx, pAudio->sfx_bubble2,
                SFX_BUBBLE_VOL);
    }
}

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
        return gfm_playAudio(0, pGame->pCtx, pAudio->sfx_onSwipeOut,
                SFX_SWIPE_VOL);
    }
    else {
        pGlobal->sfx_lastSwipeWasIn = 1;
        return gfm_playAudio(0, pGame->pCtx, pAudio->sfx_onSwipeIn,
                SFX_SWIPE_VOL);
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
    return gfm_playAudio(0, pGame->pCtx, pAudio->sfx_onEnterItem, SFX_ITEM_VOL);
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
    return gfm_playAudio(0, pGame->pCtx, pAudio->sfx_onWrongItem, SFX_ITEM_VOL);
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
    return gfm_playAudio(&(pGlobal->pSong), pGame->pCtx, pAudio->song,
            SONG_VOL);
}

