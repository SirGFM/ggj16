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
#include <ggj16/scrollItem.h>
#include <ggj16/sounds.h>
#include <ggj16/type.h>

#include <stdlib.h>
#include <string.h>

struct stRecipeScroll {
    /** Sprites (used in a round-robin fashion) for the "recipe map" */
    scrollItem *ppItems[MAX_SCROLL_SPR];
    /** Current highlighted sprite */
    scrollItem *pCurScrollItem;
    /** List of items on the current recipe */
    itemType *pRecipe;
    /** Recipe's vertical speed */
    double recipeSpeed;
    /** Number of items on the recipe */
    int numItems;
    /** Length of the recipe buffer */
    int recipeLen;
    /** If the item was sucessfully added */
    int done;
    /* Flag when errors happen */
    gfmRV error;
    /** Index to the next item to be loaded into view */
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
        if ((*ppScroll)->ppItems[i]) {
            scrollItem_free(&((*ppScroll)->ppItems[i]));
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
        rv = scrollItem_getNew(&(pScroll->ppItems[i]));
        ASSERT(rv == GFMRV_OK, rv);

        i++;
    }

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
 * Load the next item into view
 *
 * @param  [ in]pScroll The recipe scroller
 * @param  [ in]pItem   The recycled scroll item
 * @return              GFraMe return value
 */
static gfmRV _recipeScroll_loadNextItem(recipeScroll *pScroll,
        scrollItem *pItem) {
    /** GFraMe return value */
    gfmRV rv;
    /** Iterate through all items */
    int i;
    /** Next sprite position */
    int x, y;

    /* Check if the recipe did finish */
    if (pScroll->curItem >= pScroll->numItems) {
        return GFMRV_OK;
    }

    /** Retrieve the lowest vertical position */
    i = 0;
    y = 0;
    while (i < pScroll->numItems && i < MAX_SCROLL_SPR) {
        if ((pScroll->ppItems[i]->status & ITEM_RECYCLE) != ITEM_RECYCLE &&
                pScroll->ppItems[i]->y > y) {
            y = pScroll->ppItems[i]->y;
        }
        i++;
    }

    /* Recycle the next item */
    x = 16 * 8;
    y += 16;
    rv = scrollItem_init(pItem, pScroll->recipeSpeed,
            pScroll->pRecipe[pScroll->curItem], x, y);
    ASSERT(rv == GFMRV_OK, rv);

    pScroll->curItem++;

    rv = GFMRV_OK;
__ret:
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
    int i;
    /** Next sprite position */
    int x, y;

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
    pScroll->recipeSpeed = speed;
    pScroll->curItem = 0;

    pScroll->expected = T_NONE;
    pScroll->error = GFMRV_FALSE;

    /* Position all sprites */
    i = 0;
    x = 16 * 8;
    y = 8 * 8;
    while (i < length && i < MAX_SCROLL_SPR) {
        /** Load the items manually */
        rv = scrollItem_init(pScroll->ppItems[i], speed, pItems[i], x, y);
        ASSERT(rv == GFMRV_OK, rv);

        y += 16;
        i++;
    }
    pScroll->curItem = i;

    rv = GFMRV_OK;
__ret:
    return rv;
}

/**
 * Retrieve the expected type
 *
 * @param  [out]pItem   The checking item
 * @param  [ in]pScroll The object
 * @return              GFraMe return value
 */
gfmRV recipeScroll_getExpectedType(itemType *pItem, recipeScroll *pScroll) {
    if (!pScroll->done) {
        *pItem = pScroll->expected;
    }
    else {
        *pItem = T_NONE;
    }

    return GFMRV_OK;
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
        /* Stop highlighting the current item */
        scrollItem_resetHighlight(pScroll->pCurScrollItem);
        pScroll->done = 1;
        return GFMRV_TRUE;
    }
    else {
        /* Set error FLAG */
        pScroll->error = GFMRV_TRUE;
        sound_onWrongItem();
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
    gfmRV rv;

    rv = pScroll->error;
    /* Clear the error flag, so it won't be issued again next frame */
    pScroll->error = GFMRV_FALSE;

    return rv;
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
        rv = scrollItem_update(pScroll->ppItems[i]);
        ASSERT(rv == GFMRV_OK, rv);

#define curStatus (pScroll->ppItems[i]->status)

        if ((curStatus & ITEM_RECYCLE) == ITEM_RECYCLE) {
            /* The item just went invisible, it may be recycled */
            rv = _recipeScroll_loadNextItem(pScroll, pScroll->ppItems[i]);
            ASSERT(rv == GFMRV_OK, rv);
        }
        else if ((curStatus & ITEM_JUST_ENTERED) == ITEM_JUST_ENTERED) {
            /* The item just entered the area, set it as the expected */
            gesture_reset(pGlobal->pGesture);
            pScroll->pCurScrollItem = pScroll->ppItems[i];
            pScroll->expected  = pScroll->ppItems[i]->type;
            pScroll->done = 0;
        }
        else if (curStatus == ITEM_JUST_LEFT) {
            /* The item just left, check that it was done */
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

        i++;
    }

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
        rv = scrollItem_draw(pScroll->ppItems[i]);
        ASSERT(rv == GFMRV_OK, rv);
        i++;
    }

    rv = GFMRV_OK;
__ret:
    return rv;
}

