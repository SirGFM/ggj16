/**
 * @file src/recipeScroll.c
 *
 * Manages the recipe of the current level. It displays a scrolling list and 
 * keep track of the current "expected input".
 */
#include <base/game_ctx.h>

#include <GFraMe/gfmAssert.h>
#include <GFraMe/gfmError.h>
#include <GFraMe/gfmTilemap.h>

#include <ggj16/recipeScroll.h>

#include <stdlib.h>
#include <string.h>

#define gfmTilemap_loadfStatic(pTMap, pCtx, pFilename, pDictNames, pDictTypes, dictLen) \
    gfmTilemap_loadf(pTMap, pCtx, pFilename, sizeof(pFilename) - 1, pDictNames, pDictTypes, dictLen)
static char *dictStr[] = { "dummy" };
static int dictType[] = { 0 };
static int dictLen = sizeof(dictType) / sizeof(int);

struct stRecipeScroll {
    /** Tilemap used for rendering the current state */
    gfmTilemap *pRecipe;
    /** Mask that hides the incoming items */
    gfmTilemap *pMask;
    /** Recipe's vertical position (must be manually integrated) */
    double recipeY;
    /** Recipe's vertical speed */
    double recipeSpeed;
    /** Recipe's horizontal position */
    int recipeX;
    /** Number of items on the recipe */
    int numItems;
};

/**
 * Releases all memory
 *
 * @param  [ in]ppScroll The object to be released
 */
void recipeScroll_free(recipeScroll **ppScroll) {
    /* Avoid errors */
    if (!*ppScroll) {
        return;
    }

    /* Release the object and all of its attributes */
    if ((*ppScroll)->pRecipe) {
        gfmTilemap_free(&((*ppScroll)->pRecipe));
    }
    if ((*ppScroll)->pMask) {
        gfmTilemap_free(&((*ppScroll)->pMask));
    }
    free(*ppScroll);
    *ppScroll = 0;
}

/**
 * Alloc a new recipe scroller
 *
 * @param  [out]ppScroll The alloc'ed object
 * @return               GFraMe return value
 */
gfmRV recipeScroll_getNew(recipeScroll **ppScroll) {
    /** GFraMe return value */
    gfmRV rv;
    /** The new object */
    recipeScroll *pScroll;

    /* Alloc the object */
    pScroll = (recipeScroll*)malloc(sizeof(recipeScroll));
    ASSERT(pScroll, GFMRV_ALLOC_FAILED);
    memset(pScroll, 0x0, sizeof(recipeScroll));

    /* Initialize the object's data */
    rv = gfmTilemap_getNew(&(pScroll->pRecipe));
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmTilemap_init(pScroll->pRecipe, pGfx->pSset8x8, 1/*w*/, 1/*h*/,
            -1/*defTile*/); 
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmTilemap_getNew(&(pScroll->pMask));
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmTilemap_init(pScroll->pMask, pGfx->pSset8x8, 3/*w*/, 20/*h*/,
            -1/*defTile*/); 
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmTilemap_loadfStatic(pScroll->pMask, pGame->pCtx,
            "map/scrollMask.gfm", dictStr, dictType, dictLen);
    ASSERT(rv == GFMRV_OK, rv);

    rv = gfmTilemap_setPosition(pScroll->pMask, 15 * 8, 0);
    ASSERT(rv == GFMRV_OK, rv);

    pScroll->recipeX = 16 * 8 - 4;
    pScroll->recipeY = 8 * 8;
    pScroll->recipeSpeed = 4;

    *ppScroll = pScroll;
    rv = GFMRV_OK;
__ret:
    if (rv != GFMRV_OK && pScroll) {
        recipeScroll_free(&pScroll);
    }

    return rv;
}

/**
 * Loads a new recipe into the scroller
 *
 * @param  [ in]pScroll The object
 * @param  [ in]pItems  List of types in this recipe
 * @param  [ in]length  Number of entries in the types list
 * @param  [ in]speed   Recipe' scrolling speed (in pixels-per-second)
 * @return              GFraMe return value
 */
gfmRV recipeScroll_load(recipeScroll *pScroll, int *pItems, int length,
        double speed) {
    /* Recipe's tile data */
    int *pData;
    /** GFraMe return value */
    gfmRV rv;
    /** Iterate through the items */
    int i;

    /* Update the visible recipe */
    rv = gfmTilemap_init(pScroll->pRecipe, pGfx->pSset8x8, 2/*w*/,
            length * 2/*h*/, 352/*defTile*/); 
    rv = gfmTilemap_getData(&pData, pScroll->pRecipe);
    ASSERT(rv == GFMRV_OK, rv);
    pScroll->numItems = length;

    i = 0;
    while (i < length) {
        /** TODO Load the correct sprite and list */
        pData[i * 4 + 0] = 352;
        pData[i * 4 + 1] = -1;
        pData[i * 4 + 2] = -1;
        pData[i * 4 + 3] = -1;
        i++;
    }

    /* Reset the recipe's position */
    pScroll->recipeX = 16 * 8 - 4;
    pScroll->recipeY = 8 * 8;
    pScroll->recipeSpeed = speed;

    rv = GFMRV_OK;
__ret:
    return rv;
}

/**
 * Check if a item is the expected one. If it's not, an error flag will be set!
 *
 * @param  [ in]pScroll The object
 * @param  [ in]item    The checking item
 * @return              GFMRV_TRUE, GFMRV_FALSE
 */
gfmRV recipeScroll_isExpectedItem(recipeScroll *pScroll, int item) {
    return GFMRV_OK;
}

/**
 * Check if the scroller didn't detect any error (e.g., skipping an item).
 *
 * @param  [ in]pScroll The object
 * @return              GFMRV_TRUE, GFMRV_FALSE
 */
gfmRV recipeScroll_isValid(recipeScroll *pScroll) {
    return GFMRV_OK;
}

/**
 * Updates the state of the scroller
 *
 * @param  [ in]pScroll The object
 * @return              GFraMe return value
 */
gfmRV recipeScroll_update(recipeScroll *pScroll) {
    /* Recipe's tile data */
    int *pData;
    /** GFraMe return value */
    gfmRV rv;
    /** Iterate through the items */
    int i;

    /* Sanitize arguments */
    ASSERT(pScroll, GFMRV_ARGUMENTS_BAD);

    /* Integrate the recipe's position */
    pScroll->recipeY += pScroll->recipeSpeed *
            ((double)pGame->elapsed / 1000.0);
    rv = gfmTilemap_setPosition(pScroll->pRecipe, pScroll->recipeX,
            (int)pScroll->recipeY);
    rv = gfmTilemap_setPosition(pScroll->pRecipe, pScroll->recipeX, (int)pScroll->recipeY);
    ASSERT(rv == GFMRV_OK, rv);

    /* Clear the previous highlight in a lazy way */
    rv = gfmTilemap_getData(&pData, pScroll->pRecipe);
    ASSERT(rv == GFMRV_OK, rv);
    i = 0;
    while (i < pScroll->numItems) {
        pData[i * 4 + 0] &= 0xfffffffe;
        if (pData[i * 4 + 1] != -1) {
            pData[i * 4 + 1] &= 0xfffffffe;
        }
        i++;
    }
    /* Highlight the current item */
    if (pScroll->recipeY < 44) {
        /** Current active tile */
        int tile;
        /** Position within the valid area */
        int pos;

        tile = (int)(44 - pScroll->recipeY) / 16;
        pos  = (int)(44 - pScroll->recipeY) % 16;
        if (pos >= 4 && pos < 14 && tile < pScroll->numItems) {
            pData[tile *  4 + 0] |= 1;
            pData[tile *  4 + 1] |= 1;

            /* TODO Set the a item is expected */
        }
    }

    rv = gfmTilemap_update(pScroll->pRecipe, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmTilemap_update(pScroll->pMask, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);

    rv = GFMRV_OK;
__ret:
    return rv;
}

/**
 * Draw the scroller
 *
 * @param  [ in]pScroll The object
 * @return              GFraMe return value
 */
gfmRV recipeScroll_draw(recipeScroll *pScroll) {
    /** GFraMe return value */
    gfmRV rv;

    /* Draw the recipe bellow the mask, so it's partially hidden */
    rv = gfmTilemap_draw(pScroll->pRecipe, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmTilemap_draw(pScroll->pMask, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);

    rv = GFMRV_OK;
__ret:
    return rv;
}

