/**
 * @file include/ggj16/recipeScroll.h
 *
 * Manages the recipe of the current level. It displays a scrolling list and 
 * keep track of the current "expected input".
 */
#ifndef __RECIPESCROLL_STRUCT__
#define __RECIPESCROLL_STRUCT__

typedef struct stRecipeScroll recipeScroll;

#endif /* __RECIPESCROLL_STRUCT__ */

#ifndef __RECIPESCROLL_H__
#define __RECIPESCROLL_H__

#include <GFraMe/gfmError.h>

#include <ggj16/type.h>

/**
 * Releases all memory
 *
 * @param  [ in]ppScroll The object to be released
 */
void recipeScroll_free(recipeScroll **ppScroll);

/**
 * Alloc a new recipe scroller
 *
 * @param  [out]ppScroll The alloc'ed object
 * @return               GFraMe return value
 */
gfmRV recipeScroll_getNew(recipeScroll **ppScroll);

/**
 * Loads a new recipe into the scroller
 *
 * @param  [ in]pScroll The object
 * @param  [ in]pTypes  List of items in this recipe
 * @param  [ in]length  Number of entries in the items list
 * @param  [ in]speed   Recipe' scrolling speed (in pixels-per-second)
 * @return              GFraMe return value
 */
gfmRV recipeScroll_load(recipeScroll *pScroll, itemType *pItems, int length,
        double speed);

/**
 * Check if a item is the expected one. If it's not, an error flag will be set!
 *
 * @param  [ in]pScroll The object
 * @param  [ in]item    The checking item
 * @return              GFMRV_TRUE, GFMRV_FALSE
 */
gfmRV recipeScroll_isExpectedItem(recipeScroll *pScroll, itemType item);

/**
 * Check if any input onto the scroller failed (e.g., skipping an item).
 *
 * @param  [ in]pScroll The object
 * @return              GFMRV_TRUE, GFMRV_FALSE
 */
gfmRV recipeScroll_didFail(recipeScroll *pScroll);

/**
 * Updates the state of the scroller
 *
 * @param  [ in]pScroll The object
 * @return              GFraMe return value
 */
gfmRV recipeScroll_update(recipeScroll *pScroll);

/**
 * Draw the scroller
 *
 * @param  [ in]pScroll The object
 * @return              GFraMe return value
 */
gfmRV recipeScroll_draw(recipeScroll *pScroll);

#endif /* __RECIPESCROLL_H__ */

