/**
 * @file src/assets.c
 *
 * Handles loading assets and creating the required spritesets
 */
#include <base/assets.h>
#include <base/game_const.h>
#include <base/game_ctx.h>

#include <GFraMe/gfmAssert.h>
#include <GFraMe/gfmError.h>
#include <GFraMe/gframe.h>

static gfmAssetType pType[MAX_HND] = {
    ASSET_AUDIO,
    ASSET_AUDIO,
    ASSET_AUDIO,
    ASSET_AUDIO,
    ASSET_AUDIO
};
static char *ppPath[MAX_HND] = {
    "sfx/onEnterItem.wav",
    "sfx/onWrongItem.wav",
    "sfx/onSwipeIn.wav",
    "sfx/onSwipeOut.wav",
    "mml/song.mml"
};
static const int numAssets = MAX_HND;
static int *ppHandles[MAX_HND];

/**
 * Load all assets
 *
 * @return GFraMe return value
 */
gfmRV assets_load() {
    /** Return value */
    gfmRV rv;
    /** Iterate through all audio handles to set 'em before loading */
    bgAudioHandles i;

    /* Macros for loading stuff... */
#define GEN_SPRITESET(W, H, TEX) \
    rv = gfm_createSpritesetCached(&(pGfx->pSset##W##x##H), pGame->pCtx, TEX, \
            W, H); \
    ASSERT(rv == GFMRV_OK, rv);
#define LOAD_SFX(var, name) \
    rv = gfm_loadAudio(&(pAudio->var), pGame->pCtx, name, sizeof(name) - 1); \
    ASSERT(rv == GFMRV_OK, rv)

    /* Load the texture and its spritesets */
    rv = gfm_loadTextureStatic(&(pGfx->texHandle), pGame->pCtx, "gfx/atlas.bmp",
            COLORKEY);
    ASSERT(rv == GFMRV_OK, rv);
    GEN_SPRITESET(2, 2, pGfx->texHandle);
    GEN_SPRITESET(4, 4, pGfx->texHandle);
    GEN_SPRITESET(8, 8, pGfx->texHandle);
    GEN_SPRITESET(16, 16, pGfx->texHandle);
    GEN_SPRITESET(16, 32, pGfx->texHandle);
    GEN_SPRITESET(32, 32, pGfx->texHandle);
    GEN_SPRITESET(64, 64, pGfx->texHandle);
    GEN_SPRITESET(32, 128, pGfx->texHandle);
    GEN_SPRITESET(128, 32, pGfx->texHandle);
    GEN_SPRITESET(256, 128, pGfx->texHandle);

    /* Retrieve the audio's handle, so it can be passed to the loader thread */
    i = FIRST_HND;
    while (i < MAX_HND) {
        switch (i) {
            case SFX_ON_ENTER_HND: {
                ppHandles[i] = &(pAudio->sfx_onEnterItem);
            } break;
            case SFX_ON_WRONG_HND: {
                ppHandles[i] = &(pAudio->sfx_onWrongItem);
            } break;
            case SFX_ON_SWIPE_IN_HND: {
                ppHandles[i] = &(pAudio->sfx_onSwipeIn);
            } break;
            case SFX_ON_SWIPE_OUT_HND: {
                ppHandles[i] = &(pAudio->sfx_onSwipeOut);
            } break;
            case SONG_HND: {
                ppHandles[i] = &(pAudio->song);
            } break;
            default: {
                /** Shouldn't happen */
                ASSERT(0, GFMRV_INTERNAL_ERROR);
            }
        }
        i++;
    }

    rv = gfm_loadAssetsAsync(&(pGame->loadedAssets), pGame->pCtx, pType, ppPath,
            ppHandles, (int)numAssets);
    ASSERT(rv == GFMRV_OK, rv);

    rv = GFMRV_OK;
__ret:
    return rv;
}

