/**
 * @file src/assets.c
 *
 * Handles loading assets and creating the required spritesets
 */
#include <base/game_const.h>
#include <base/game_ctx.h>

#include <GFraMe/gfmAssert.h>
#include <GFraMe/gfmError.h>
#include <GFraMe/gframe.h>

enum {
    SONG_HND,
    MAX_HND
};

static gfmAssetType pType[MAX_HND] = {
    ASSET_AUDIO
};
static char *ppPath[MAX_HND] = {
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
    ppHandles[0] = &(pAudio->song);

    rv = gfm_loadAssetsAsync(&(pGame->loadedAssets), pGame->pCtx, pType, ppPath,
            ppHandles, (int)numAssets);
    ASSERT(rv == GFMRV_OK, rv);

    rv = GFMRV_OK;
__ret:
    return rv;
}

