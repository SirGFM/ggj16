/**
 * @file src/scrollItem.c
 *
 * Objects used by the recipeScroll to render the current recipe
 */
#include <base/game_const.h>
#include <base/game_ctx.h>

#include <GFraMe/gfmAssert.h>
#include <GFraMe/gfmError.h>
#include <GFraMe/gfmSprite.h>

#include <ggj16/type.h>
#include <ggj16/scrollItem.h>
#include <ggj16/sounds.h>

#include <stdlib.h>
#include <string.h>

/**
 * Release all memory alloc'ed by the item
 *
 * @param  [ in]ppItem The item
 */
void scrollItem_free(scrollItem **ppItem) {
    if (!ppItem || !(*ppItem)) {
        return;
    }

    gfmSprite_free(&((*ppItem)->pSelf));
    free(*ppItem);
    *ppItem = 0;
}

/**
 * Alloc a new item
 *
 * @param  [out]ppItem The item
 * @return             GFraMe return value
 */
gfmRV scrollItem_getNew(scrollItem **ppItem) {
    /** GFraMe return value */
    gfmRV rv;
    /** The alloc'ed item */
    scrollItem *pItem;

    /* Alloc the item and its attributes */
    pItem = (scrollItem*)malloc(sizeof(scrollItem));
    ASSERT(pItem, GFMRV_ALLOC_FAILED);
    memset(pItem, 0x0, sizeof(scrollItem));

    rv = gfmSprite_getNew(&(pItem->pSelf));
    ASSERT(rv == GFMRV_OK, rv);

    *ppItem = pItem;
    rv = GFMRV_OK;
__ret:
    if (rv != GFMRV_OK && pItem) {
        scrollItem_free(&pItem);
    }

    return rv;
}

/**
 * (Re)Initialize a item
 *
 * @param  [ in]pItem The item
 * @param  [ in]vy    The vertical speed
 * @param  [ in]type  The item's type
 * @param  [ in]x     The horizontal position
 * @param  [ in]y     The vertical position
 * @return            GFraMe return value
 */
gfmRV scrollItem_init(scrollItem *pItem, double vy, itemType type, int x,
        int y) {
    /** GFraMe return value */
    gfmRV rv;

    /* Initialize the sprite and set its tile */
    rv = gfmSprite_init(pItem->pSelf, x, y, 8 /* w */, 8 /* h */,
            pGfx->pSset8x8, 0 /* offx */, 0 /* offy */, 0 /* child */,
            0 /* type */);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_setFrame(pItem->pSelf, type * 2 + FIRST_ITEM_TILE);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_setVerticalVelocity(pItem->pSelf, vy);
    ASSERT(rv == GFMRV_OK, rv);

    /* Cache everything for ease of access */
    pItem->y = y;
    pItem->type = type;
    pItem->status = 0;

    rv = GFMRV_OK;
__ret:
    return rv;
}

/**
 * Resets the item highlight (e.g., after being correctly executed)
 *
 * @param  [ in]pItem The item
 * @return            GFraMe return value
 */
gfmRV scrollItem_resetHighlight(scrollItem *pItem) {
    /** GFraMe return value */
    gfmRV rv;
    /** Sprite's new tile */
    int tile;

    /* Remove the highlight (i.e., set the default tile) */
    tile = pItem->type * 2 + FIRST_ITEM_TILE;
    rv = gfmSprite_setFrame(pItem->pSelf, tile);
    ASSERT(rv == GFMRV_OK, rv);
    /* Set its status so it won't be highlighted again */
    pItem->status = ITEM_DONE;

    rv = GFMRV_OK;
__ret:
    return rv;
}

/**
 * Update the current item
 *
 * @param  [ in]pItem The item
 * @return            GFraMe return value
 */
gfmRV scrollItem_update(scrollItem *pItem) {
    /** GFraMe return value */
    gfmRV rv;
    /** Sprite's new tile */
    int tile;

    /* Update the sprite's position and cache it */
    rv = gfmSprite_update(pItem->pSelf, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_getVerticalPosition(&(pItem->y), pItem->pSelf);
    ASSERT(rv == GFMRV_OK, rv);

    /* Calculate the default (i.e., non-highlighted) tile */
    tile = pItem->type * 2 + FIRST_ITEM_TILE;

    /* Clear the 'just moved in/out' flag */
    pItem->status &= ~ITEM_JUST;

    /* Check whether it's inside the area */
    if (pItem->y - SCROLL_AREA_POS > -2 &&
            pItem->y - SCROLL_AREA_POS + 2 <= SCROLL_AREA_HEIGHT &&
            !(pItem->status & ITEM_DONE)) {
        tile |= 1;
        if (!(pItem->status & ITEM_INSIDE)) {
            /* If the item wasn't inside the area, flag it as having just
             * entered */
            pItem->status |= ITEM_JUST;
            rv = sound_playEnterItem();
            ASSERT(rv == GFMRV_OK, rv);
        }
        /* Set it as being inside the area */
        pItem->status |= ITEM_INSIDE;
    }
    else if (pItem->y < SCROLL_HIDDEN_Y) {
        pItem->status = ITEM_RECYCLE;
    }
    else {
        if (pItem->status & ITEM_INSIDE) {
            /* If it was inside, flag it as having just left */
            pItem->status |= ITEM_JUST;
        }
        /* Set it as being outside the area */
        pItem->status &= ~ITEM_INSIDE;
    }

    /* Update its tile */
    rv = gfmSprite_setFrame(pItem->pSelf, tile);
    ASSERT(rv == GFMRV_OK, rv);

    rv = GFMRV_OK;
__ret:
    return rv;
}

/**
 * Draw the current item
 *
 * @param  [ in]pItem The item
 * @return            GFraMe return value
 */
gfmRV scrollItem_draw(scrollItem *pItem) {
    return gfmSprite_draw(pItem->pSelf, pGame->pCtx);
}

