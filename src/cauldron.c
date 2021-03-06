/**
 * @file src/cauldron.c
 *
 * Parser for cauldrons. Also implements Drag 'n' Drop.
 */
#include <base/game_ctx.h>

#include <GFraMe/gfmAssert.h>
#include <GFraMe/gfmError.h>
#include <GFraMe/gfmParser.h>
#include <GFraMe/gfmSprite.h>
#include <GFraMe/gfmSpriteset.h>

#include <ggj16/cauldron.h>
#include <ggj16/type.h>

#include <stdlib.h>
#include <string.h>

static int pCauldronAnim[] = {9, 10, 11, 12};

/**
 * Release all alloc'ed memory
 *
 * @param  [ in]ppCal The cauldron to be release
 * @return            GFraMe return value
 */
void cauldron_free(cauldron **ppCal) {
    /** Avoid errors */
    if (!ppCal) {
        return;
    }

    /** Release its sprite */
    if ((*ppCal)->pSelf) {
        gfmSprite_free(&((*ppCal)->pSelf));
    }

    /** Release the cauldron */
    free(ppCal);
    *ppCal = 0;
}

/**
 * Alloc a new cauldron
 *
 * @param  [out]ppCal The alloc'ed cauldron
 * @return            GFraMe return value
 */
gfmRV cauldron_getNew(cauldron **ppCal) {
    /** GFraMe return value */
    gfmRV rv;
    /** The new cauldron */
    cauldron *pCal;

    ASSERT(ppCal, GFMRV_ARGUMENTS_BAD);

    /** Alloc the cauldron and every reference within it */
    pCal = (cauldron*)malloc(sizeof(cauldron));
    ASSERT(pCal, GFMRV_ALLOC_FAILED);
    memset(pCal, 0x0, sizeof(cauldron));
    rv = gfmSprite_getNew(&(pCal->pSelf));
    ASSERT(rv == GFMRV_OK, rv);

    *ppCal = pCal;
    rv = GFMRV_OK;
__ret:
    if (rv != GFMRV_OK && pCal) {
        cauldron_free(&pCal);
    }

    return rv;
}

/**
 * Parse the cauldron
 *
 * @param  [ in]pCal    The parsed cauldron
 * @param  [ in]pParser The parser
 * @return            GFraMe return value
 */
gfmRV cauldron_init(cauldron *pCal, gfmParser *pParser) {
    /** Sprite's type */
    char *pName;
    /** GFraMe return value */
    gfmRV rv;
    /** Sprite's spriteset */
    gfmSpriteset *pSset;
    /** Sprite's position */
    int x, y;
    /** Sprite's dimensions */
    int height, width;
    /** Sprite's offset from the origin */
    int offx, offy;
    /** Sprite's tile */
    int tile;
    /** Type of the current item */
    itemType type;

    ASSERT(pCal, GFMRV_ARGUMENTS_BAD);
    ASSERT(pParser, GFMRV_ARGUMENTS_BAD);

    /* Get the cauldron's position, dimensions and type */
    rv = gfmParser_getPos(&x, &y, pParser);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmParser_getDimensions(&width, &height, pParser);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmParser_getIngameType(&pName, pParser);
    ASSERT(rv == GFMRV_OK, rv);
    /* Convert the type to its internal type */
    rv = type_getHandle(&type, pName);
    ASSERT(rv == GFMRV_OK, rv);
    ASSERT(type == T_CAULDRON, GFMRV_ARGUMENTS_BAD);

    /** Adjust the vertical position to the sprite's top */
    y -= height;
    /* Set the tile and spriteset according to the type */
    width = 20;
    height = 18;
    tile = 8;
    pSset = pGfx->pSset64x64;
    offx = -22;
    offy = -19;

    /** Initialize the sprite */
    rv = gfmSprite_init(pCal->pSelf, x, y, width, height, pSset, offx, offy,
            pCal, type);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_setFrame(pCal->pSelf, tile);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_addAnimation(&(pCal->anim), pCal->pSelf, pCauldronAnim,
            sizeof(pCauldronAnim) / sizeof(int) /* len  */, 16 /* fps */,
            0 /* loop */);
    ASSERT(rv == GFMRV_OK, rv);

    rv = GFMRV_OK;
__ret:
    return GFMRV_OK;
}

/**
 * Explode the cauldron
 *
 * @param  [ in]pCal The parsed cauldron
 * @return           GFraMe return value
 */
gfmRV cauldron_doExplode(cauldron *pCal) {
    /** GFraMe return value */
    gfmRV rv;

    rv = gfmSprite_playAnimation(pCal->pSelf, pCal->anim);
    pCal->anim = -1;

    return rv;
}

/**
 * Update the cauldron
 *
 * @param  [ in]pCal The cauldron
 * @return           GFraMe return value
 */
gfmRV cauldron_update(cauldron *pCal) {
    return gfmSprite_update(pCal->pSelf, pGame->pCtx);
}

/**
 * Draw the cauldron
 *
 * @param  [ in]pCal The cauldron
 * @return           GFraMe return value
 */
gfmRV cauldron_draw(cauldron *pCal) {
    return gfmSprite_draw(pCal->pSelf, pGame->pCtx);
}

/**
 * Check if a sprite is overlapping the cauldron
 *
 * @param  [ in]pCal The cauldron
 * @param  [ in]pSpr The other sprite
 * @return           GFraMe return value
 */
gfmRV cauldron_isOverlapping(cauldron *pCal, gfmSprite *pSpr) {
    return gfmSprite_isOverlaping(pCal->pSelf, pSpr);
}

