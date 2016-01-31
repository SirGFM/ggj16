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
#include <ggj16/type.h>


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
    /* Flag when errors happen */
    gfmRV error;
    /* Current expected type */
    itemType expected;
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

    pScroll->recipeX = 16 * 8;
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
gfmRV recipeScroll_load(recipeScroll *pScroll, itemType *pItems, int length,
        double speed) {
    /* Recipe's tile data */
    int *pData;
    /** GFraMe return value */
    gfmRV rv;
    /** Iterate through the items */
    int i;

    /* Update the visible recipe */
    rv = gfmTilemap_init(pScroll->pRecipe, pGfx->pSset8x8, 1/*w*/,
            length * 2/*h*/, 352/*defTile*/); 
    rv = gfmTilemap_getData(&pData, pScroll->pRecipe);
    ASSERT(rv == GFMRV_OK, rv);
    pScroll->numItems = length;

    i = 0;
    while (i < length) {
        /* All types were set sequentially on the tile set, with the first on
         * tile 352. Since each one spawns two tiles (the normal and a
         * highlighted version), retrieveing the tile is a simple matter of
         * calculating the correct index */
        pData[i * 2 + 0] = 352 + (pItems[i] - T_RAT_TAIL) * 2;
        pData[i * 2 + 1] = -1;
        i++;
    }

    /* Reset the recipe's position */
    pScroll->recipeX = 16 * 8;
    pScroll->recipeY = 8 * 8;
    pScroll->recipeSpeed = speed;

    pScroll->expected = T_NONE;
    pScroll->error = GFMRV_FALSE;

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
gfmRV recipeScroll_isExpectedItem(recipeScroll *pScroll, itemType item) {
    if (item == pScroll->expected) {
        return GFMRV_TRUE;
    }
    else {
        /* TODO Set error FLAG */
        return GFMRV_FALSE;
    }
}

/**
 * Check if any input onto the scroller failed (e.g., skipping an item).
 *
 * @param  [ in]pScroll The object
 * @return              GFMRV_TRUE, GFMRV_FALSE
 */
gfmRV recipeScroll_didFail(recipeScroll *pScroll) {
    return pScroll->error;
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
        pData[i * 2 + 0] &= 0xfffffffe;
        i++;
    }
    if (pScroll->recipeY < 44) {
        /** Current active tile */
        int tile;
        /** Position within the valid area */
        int pos;

        tile = (int)(44 - pScroll->recipeY) / 16;
        pos  = (int)(44 - pScroll->recipeY) % 16;

        /* Check if it's still a valid item */
        if (tile < pScroll->numItems) {
            if (pos == 0) {
                /* First frame when the item can be "done" */

                /* Set expected item */
                pScroll->expected  = (pData[tile * 2 + 0] - 352) / 2 +
                        T_RAT_TAIL;
                /* TODO Clear motion */
            }
            else if (pos >= 4 && pos < 14) {
                /* Highlight the current item */
                pData[tile *  2 + 0] |= 1;
            }
            else if (pos == 15) {
                /* TODO If item still hasn't been "done", the player failed */
                /* NOTE: If item is T_WAIT, the input must be NONE! */
                pScroll->expected  = T_NONE;
            }
        }
        else {
            /* TODO Set finished */
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

