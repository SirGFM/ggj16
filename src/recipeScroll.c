/**
 * @file src/recipeScroll.c
 *
 * Manages the recipe of the current level. It displays a scrolling list and 
 * keep track of the current "expected input".
 */
#include <base/game_const.h>
#include <base/game_ctx.h>

#include <GFraMe/gfmAssert.h>
#include <GFraMe/gfmError.h>
#include <GFraMe/gfmSprite.h>
#include <GFraMe/gfmTilemap.h>

#include <ggj16/gesture.h>
#include <ggj16/recipeScroll.h>
#include <ggj16/type.h>


#include <stdlib.h>
#include <string.h>

struct stRecipeScroll {
    /** Sprites (used in a round-robin fashion) for the "recipe map" */
    gfmSprite *ppSpr[MAX_SCROLL_SPR];
    /** List of items on the current recipe */
    itemType *pRecipe;
    /** Recipe's vertical position (must be manually integrated) */
    double recipeY;
    /** Recipe's vertical speed */
    double recipeSpeed;
    /** Recipe's horizontal position */
    int recipeX;
    /** Number of items on the recipe */
    int numItems;
    /** Length of the recipe buffer */
    int recipeLen;
    /** If the item was sucessfully added */
    int done;
    /* Flag when errors happen */
    gfmRV error;
    /** Index to the current expected item */
    int curItem;
    /* Current expected type */
    itemType expected;
};

/**
 * Releases all memory
 *
 * @param  [ in]ppScroll The object to be released
 */
void recipeScroll_free(recipeScroll **ppScroll) {
    /** Loop through all sprites */
    int i;

    /* Avoid errors */
    if (!*ppScroll) {
        return;
    }

    /* Release the object and all of its attributes */
    if ((*ppScroll)->pRecipe) {
        free((*ppScroll)->pRecipe);
    }
    i = 0;
    while (i < MAX_SCROLL_SPR) {
        if ((*ppScroll)->ppSpr[i]) {
            gfmSprite_free(&((*ppScroll)->ppSpr[i]));
        }
        i++;
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
    /** Loop through all sprites */
    int i;

    /* Alloc the object */
    pScroll = (recipeScroll*)malloc(sizeof(recipeScroll));
    ASSERT(pScroll, GFMRV_ALLOC_FAILED);
    memset(pScroll, 0x0, sizeof(recipeScroll));

    i = 0;
    while (i < MAX_SCROLL_SPR) {
        rv = gfmSprite_getNew(&(pScroll->ppSpr[i]));
        ASSERT(rv == GFMRV_OK, rv);

        i++;
    }
    /** TODO Alloc the maximum number of objects on the recipe? */

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
    /** GFraMe return value */
    gfmRV rv;
    /** Iterate through the items */
    int i, y;

    /* Expand the buffer as necessary */
    if (pScroll->recipeLen < length) {
        pScroll->pRecipe = (itemType*)realloc(pScroll->pRecipe,
                sizeof(itemType) * length);
        ASSERT(pScroll->pRecipe, GFMRV_ALLOC_FAILED);
        pScroll->recipeLen = length;
    }
    /* Clean the previous recipe */
    memset(pScroll->pRecipe, 0x0, sizeof(itemType) * length);
    pScroll->numItems = length;

    /* Copy the list */
    memcpy(pScroll->pRecipe, pItems, sizeof(itemType) * length);

    /* Reset the recipe's position */
    pScroll->recipeX = 16 * 8;
    pScroll->recipeY = 8 * 8;
    pScroll->recipeSpeed = speed;
    pScroll->curItem = 0;

    pScroll->expected = T_NONE;
    pScroll->error = GFMRV_FALSE;

    /* Position all sprites */
    i = 0;
    y = pScroll->recipeY;
    while (i < length && i < MAX_SCROLL_SPR) {
        rv = gfmSprite_init(pScroll->ppSpr[i], pScroll->recipeX, y,
                8 /* w */, 8 /* h */, pGfx->pSset8x8, 0 /* offx */,
                0 /* offy */, 0 /* child */, 0 /* type */);
        ASSERT(rv == GFMRV_OK, rv);
        rv = gfmSprite_setFrame(pScroll->ppSpr[i], pItems[i] * 2 + FIRST_ITEM_TILE);
        ASSERT(rv == GFMRV_OK, rv);
        rv = gfmSprite_setVerticalVelocity(pScroll->ppSpr[i],
                pScroll->recipeSpeed);
        ASSERT(rv == GFMRV_OK, rv);

        y += 16;
        i++;
    }

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
    if (pScroll->done == 0 && item == pScroll->expected) {
        pScroll->done = 1;
        return GFMRV_TRUE;
    }
    else {
        /* Set error FLAG */
        pScroll->error = GFMRV_TRUE;
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
    /** GFraMe return value */
    gfmRV rv;
    /** Iterate through the items */
    int i;

    /* Sanitize arguments */
    ASSERT(pScroll, GFMRV_ARGUMENTS_BAD);

    /* Integrate the recipe's position */
    i = 0;
    while (i < pScroll->numItems && i < MAX_SCROLL_SPR) {
        int tile, y;

        rv = gfmSprite_update(pScroll->ppSpr[i], pGame->pCtx);
        ASSERT(rv == GFMRV_OK, rv);

        /* Clear (or set) the highlight, according to vertical position */
        rv = gfmSprite_getVerticalPosition(&y, pScroll->ppSpr[i]);
        ASSERT(rv == GFMRV_OK, rv);
        rv = gfmSprite_getFrame(&tile, pScroll->ppSpr[i]);
        ASSERT(rv == GFMRV_OK, rv);

        /* Check if the sprite's lower half bellow the upper limit and its upper
         * half is above the lower one */
        if (y - SCROLL_AREA_POS > -2 && y - SCROLL_AREA_POS + 2 <= SCROLL_AREA_HEIGHT) {
            tile |= 1;
        }
        else {
            tile &= 0xfffffffe;
            /* TODO Check if it's a new item that should be added */
        }
        rv = gfmSprite_setFrame(pScroll->ppSpr[i], tile);
        ASSERT(rv == GFMRV_OK, rv);

        /* TODO Check if it just went outside the visible area and should be recycled */

        i++;
    }

#if 0
    if ((int)pScroll->recipeY < 52) {
        /** Current active tile */
        int tile;
        /** Position within the valid area */
        int pos;

#define MAX_H_BLA 16
        tile = (int)(52 - pScroll->recipeY) / MAX_H_BLA;
        pos  = (int)(52 - pScroll->recipeY) % MAX_H_BLA;

        /* Check if it's still a valid item */
        if ((pos == 0 || (pScroll->expected >= T_RAT_TAIL &&
                pScroll->expected < T_MAX)) && tile < pScroll->numItems) {
            if (pos == 0) {
                /* First frame when the item can be "done" */
                pScroll->done = 0;

                /* Set expected item */
                pScroll->expected  = (pData[tile * 2 + 0] - FIRST_ITEM_TILE) / 2;
                /* Clear motion */
                gesture_reset(pGlobal->pGesture);
            }
            else if (pos >= 1 && pos < MAX_H_BLA - 1) {
                /* Highlight the current item */
                pData[tile *  2 + 0] |= 1;
            }
            else if (pos == MAX_H_BLA - 1) {
                if (!pScroll->done) {
                    /** All possibles actions states */
                    itemType pActions[4];
                    /** Iterate through actions */
                    int i;

                    /* If an action was expected, check it now! */
                    rv = gesture_getCurrentGesture(pActions, pGlobal->pGesture);
                    ASSERT(rv == GFMRV_OK, rv);

                    i = 0;
                    while (i < 4) {
                        if (pActions[i] == pScroll->expected) {
                            pScroll->done = 1;
                            break;
                        }
                        i++;
                    }
                    if (i == 4) {
                        /* If no action was found, either the expected was
                         * T_WAIT or an error happened */
                        if (pScroll->expected == T_WAIT) {
                            pScroll->done = 1;
                        }
                        else {
                            pScroll->error = GFMRV_TRUE;
                        }
                    }

                    /* Clean the state */
                    pScroll->expected = T_NONE;
                }
            }
        }
        else {
            /* TODO Set finished */
        }
    }

    rv = gfmTilemap_update(pScroll->pRecipe, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);
#endif

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
    /** Iterate through the items */
    int i;

    /* Draw the recipe */
    i = 0;
    while (i < pScroll->numItems && i < MAX_SCROLL_SPR) {
        rv = gfmSprite_draw(pScroll->ppSpr[i], pGame->pCtx);
        ASSERT(rv == GFMRV_OK, rv);
        i++;
    }

    rv = GFMRV_OK;
__ret:
    return rv;
}

