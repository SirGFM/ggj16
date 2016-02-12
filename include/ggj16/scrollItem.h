/**
 * @file include/ggj16/scrollItem.h
 *
 * Objects used by the recipeScroll to render the current recipe
 */
#ifndef __SCROLLITEM_H__
#define __SCROLLITEM_H__

#include <GFraMe/gfmError.h>
#include <GFraMe/gfmSprite.h>

#include <ggj16/type.h>

enum enScrollItemStatus {
    /** Bit that flag that it's within the area */
    ITEM_INSIDE       = 0x0001,
    /** Bit that flag that it has just entered/left the area */
    ITEM_JUST         = 0x0010,
    /** Flag that the item may be recycled */
    ITEM_RECYCLE      = 0x0100,
    /** Flag that the item was correctly executed */
    ITEM_DONE         = 0x0200,

    /** Helper flags */
    ITEM_JUST_LEFT    = ITEM_JUST,
    ITEM_JUST_ENTERED = ITEM_INSIDE | ITEM_JUST
};
typedef enum enScrollItemStatus scrollItemStatus;

/** Scroll item object. It should only be accessed for reading! */
struct stScrollItem {
    /** The actual sprite */
    gfmSprite *pSelf;
    /** Cached vertical position */
    int y;
    /** The sprite type */
    itemType type;
    /** Whether the scroll item just got into the 'hitbox', or whatever */
    scrollItemStatus status;
};
typedef struct stScrollItem scrollItem;

/**
 * Release all memory alloc'ed by the item
 *
 * @param  [ in]ppItem The item
 */
void scrollItem_free(scrollItem **ppItem);

/**
 * Alloc a new item
 *
 * @param  [out]ppItem The item
 * @return             GFraMe return value
 */
gfmRV scrollItem_getNew(scrollItem **ppItem);

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
        int y);

/**
 * Update the current item
 *
 * @param  [ in]pItem The item
 * @return            GFraMe return value
 */
gfmRV scrollItem_update(scrollItem *pItem);

/**
 * Resets the item highlight (e.g., after being correctly executed)
 *
 * @param  [ in]pItem The item
 * @return            GFraMe return value
 */
gfmRV scrollItem_resetHighlight(scrollItem *pItem);

/**
 * Draw the current item
 *
 * @param  [ in]pItem The item
 * @return            GFraMe return value
 */
gfmRV scrollItem_draw(scrollItem *pItem);

#endif /* __SCROLLITEM_H__ */

