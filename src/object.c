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

#include <ggj16/object.h>

#include <stdlib.h>
#include <string.h>

struct stObject {
    /** The object's sprite */
    gfmSprite *pSelf;
};

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
    return GFMRV_OK;
}

/**
 * Update the object
 *
 * @param  [ in]pObj    The parsed object
 * @return            GFraMe return value
 */
gfmRV object_update(object *pObj) {
    return GFMRV_OK;
}

/**
 * Draw the object
 *
 * @param  [ in]pObj    The parsed object
 * @return            GFraMe return value
 */
gfmRV object_draw(object *pObj) {
    return GFMRV_OK;
}

