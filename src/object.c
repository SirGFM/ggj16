/**
 * @file src/object.c
 *
 * Parser for objects. Also implements Drag 'n' Drop.
 */
#include <base/game_ctx.h>

#include <GFraMe/gfmAssert.h>
#include <GFraMe/gfmError.h>
#include <GFraMe/gfmParser.h>
#include <GFraMe/gfmSprite.h>
#include <GFraMe/gfmSpriteset.h>

#include <ggj16/object.h>
#include <ggj16/type.h>

#include <stdlib.h>
#include <string.h>

struct stObject {
    /** The object's sprite */
    gfmSprite *pSelf;
};
static int pCauldronAnim[] = {9, 10, 11, 12};

/**
 * Release all alloc'ed memory
 *
 * @param  [ in]ppObj The object to be release
 * @return            GFraMe return value
 */
void object_free(object **ppObj) {
    /** Avoid errors */
    if (!ppObj) {
        return;
    }

    /** Release its sprite */
    if ((*ppObj)->pSelf) {
        gfmSprite_free(&((*ppObj)->pSelf));
    }

    /** Release the object */
    free(ppObj);
    *ppObj = 0;
}

/**
 * Alloc a new object
 *
 * @param  [out]ppObj The alloc'ed object
 * @return            GFraMe return value
 */
gfmRV object_getNew(object **ppObj) {
    /** GFraMe return value */
    gfmRV rv;
    /** The new object */
    object *pObj;

    ASSERT(ppObj, GFMRV_ARGUMENTS_BAD);

    /** Alloc the object and every reference within it */
    pObj = (object*)malloc(sizeof(object));
    ASSERT(pObj, GFMRV_ALLOC_FAILED);
    memset(pObj, 0x0, sizeof(object));
    rv = gfmSprite_getNew(&(pObj->pSelf));
    ASSERT(rv == GFMRV_OK, rv);

    *ppObj = pObj;
    rv = GFMRV_OK;
__ret:
    if (rv != GFMRV_OK && pObj) {
        object_free(&pObj);
    }

    return rv;
}

/**
 * Parse the object
 *
 * @param  [ in]pObj    The parsed object
 * @param  [ in]pParser The parser
 * @return            GFraMe return value
 */
gfmRV object_init(object *pObj, gfmParser *pParser) {
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

    /* Get the object's position, dimensions and type */
    rv = gfmParser_getPos(&x, &y, pParser);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmParser_getDimensions(&width, &height, pParser);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmParser_getIngameType(&pName, pParser);
    ASSERT(rv == GFMRV_OK, rv);
    /* Convert the type to its internal type */
    rv = type_getHandle(&type, pName);
    ASSERT(rv == GFMRV_OK, rv);
    /** Adjust the vertical position to the sprite's top */
    y -= height;
    /* Set the tile and spriteset according to the type */
    if (type == T_CAULDRON) {
        width = 20;
        height = 18;
        tile = 8;
        pSset = pGfx->pSset64x64;
        offx = -22;
        offy = -19;
    }
    else {
        /* All types were set sequentially on the tile set, with the first on
         * tile 352. Since each one spawns two tiles (the normal and a
         * highlighted version), retrieveing the tile is a simple matter of
         * calculating the correct index */
        tile = 352 + (type - T_RAT_TAIL) * 2;
        pSset = pGfx->pSset8x8;
    }

    /** Initialize the sprite */
    rv = gfmSprite_init(pObj->pSelf, x, y, width, height, pSset, offx, offy,
            pObj, type);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_setFrame(pObj->pSelf, tile);
    ASSERT(rv == GFMRV_OK, rv);
    if (type == T_CAULDRON) {
        int index;

        rv = gfmSprite_addAnimation(&index, pObj->pSelf, pCauldronAnim,
                sizeof(pCauldronAnim) / sizeof(int) /* len  */, 16 /* fps */,
                0 /* loop */);
        ASSERT(rv == GFMRV_OK, rv);
    }

    rv = GFMRV_OK;
__ret:
    return GFMRV_OK;
}

/**
 * Update the object
 *
 * @param  [ in]pObj    The parsed object
 * @return            GFraMe return value
 */
gfmRV object_update(object *pObj) {
    return gfmSprite_update(pObj->pSelf, pGame->pCtx);
}

/**
 * Draw the object
 *
 * @param  [ in]pObj    The parsed object
 * @return            GFraMe return value
 */
gfmRV object_draw(object *pObj) {
    return gfmSprite_draw(pObj->pSelf, pGame->pCtx);
}

