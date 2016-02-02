/**
 * @file src/object.c
 *
 * Parser for objects. Also implements Drag 'n' Drop.
 */
#include <base/game_ctx.h>

#include <GFraMe/gfmAssert.h>
#include <GFraMe/gfmError.h>
#include <GFraMe/gfmInput.h>
#include <GFraMe/gfmParser.h>
#include <GFraMe/gfmSprite.h>
#include <GFraMe/gfmSpriteset.h>

#include <ggj16/object.h>
#include <ggj16/type.h>

#if defined(DEBUG) && !(defined(__WIN32) || defined(__WIN32__))
#  include <signal.h>
#endif
#include <stdlib.h>
#include <string.h>

struct stObject {
    /** The object's sprite */
    gfmSprite *pSelf;
    /** Original horizontal position */
    int originX;
    /** Original vertical position */
    int originY;
    /** Offset from the mouse */
    int offX;
    /** Offset from the mouse */
    int offY;
    /** Cached type of the item (for ease of access) */
    itemType type;
};

/**
 * Release all alloc'ed memory
 *
 * @param  [ in]ppObj The object to be release
 * @return            GFraMe return value
 */
void object_free(object **ppObj) {
    /** Avoid errors */
    if (!ppObj || !(*ppObj)) {
        return;
    }

    /** Release its sprite */
    if ((*ppObj)->pSelf) {
        gfmSprite_free(&((*ppObj)->pSelf));
    }

    /** Release the object */
    free(*ppObj);
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
    /* Set the tile and spriteset according to the type.
     * All types were set sequentially on the tile set, with the first on
     * tile 352. Since each one spawns two tiles (the normal and a
     * highlighted version), retrieveing the tile is a simple matter of
     * calculating the correct index */
    tile = 352 + (type - T_RAT_TAIL) * 2;
    pSset = pGfx->pSset8x8;

    /** Initialize the sprite */
    rv = gfmSprite_init(pObj->pSelf, x, y, width, height, pSset,  0 /* offx */,
            0 /* offy */, pObj, type);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_setFrame(pObj->pSelf, tile);
    ASSERT(rv == GFMRV_OK, rv);

    /* Set the position the object will be returned */
    pObj->originX = x;
    pObj->originY = y;
    /* Cache its type */
    pObj->type = type;

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
    /** Framework's input context */
    gfmInput *pInput;
    /** GFraMe return value */
    gfmRV rv;
    /** Current mouse position on the screen */
    int mouseX, mouseY;
    /** The sprite's current tile */
    int tile;

    /* Retrieve the current mouse position */
    rv = gfm_getInput(&pInput, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmInput_getPointerPosition(&mouseX, &mouseY, pInput);
    ASSERT(rv == GFMRV_OK, rv);

    /* Retrieve the current tile */
    rv = gfmSprite_getFrame(&tile, pObj->pSelf);
    ASSERT(rv == GFMRV_OK, rv);
    /* Clear highlight */
    rv = gfmSprite_setFrame(pObj->pSelf, tile & 0xfffffffe);
    ASSERT(rv == GFMRV_OK, rv);

    /* Check if it's inside */
    if (!pGlobal->isDragging) {
        rv = gfmSprite_isPointInside(pObj->pSelf, mouseX, mouseY);
        ASSERT(rv == GFMRV_TRUE || rv == GFMRV_FALSE, rv);
        if (rv == GFMRV_TRUE) {
            /* Highlight it */
            rv = gfmSprite_setFrame(pObj->pSelf, tile | 1);
            ASSERT(rv == GFMRV_OK, rv);
            /* Check if it should be dragged */
            if ((pButton->click.state & gfmInput_justPressed) ==
                    gfmInput_justPressed) {
                /** The sprite's position */
                int x, y;

                /* Drag it */
                pGlobal->isDragging = 1;
                pGlobal->pDragging = pObj;
                /* Set its offset to the mouse */
                rv = gfmSprite_getPosition(&x, &y, pObj->pSelf);
                ASSERT(rv == GFMRV_OK, rv);
                pObj->offX = x - mouseX;
                pObj->offY = y - mouseY;
            }
        }
    }

    /* Check if the object being dragged it this */
    if (pGlobal->pDragging == pObj) {
        /* Check if it should be released */
        if ((pButton->click.state & gfmInput_justReleased) ==
                gfmInput_justReleased) {
                pGlobal->isDragging = 0;
                pGlobal->pDragging = 0;

                /*  Check if it's over the cauldron */
                rv = cauldron_isOverlapping(pGlobal->pCauldron, pObj->pSelf);
                ASSERT(rv == GFMRV_TRUE || rv == GFMRV_FALSE, rv);
                if (rv == GFMRV_TRUE) {
                    /* Check if it was the expected type */
                    recipeScroll_isExpectedItem(pGlobal->pRecipe, pObj->type);
                }

                rv = gfmSprite_setPosition(pObj->pSelf, pObj->originX,
                        pObj->originY);
                ASSERT(rv == GFMRV_OK, rv);
        }
        else {
            /* Update its position */
            rv = gfmSprite_setPosition(pObj->pSelf, pObj->offX + mouseX,
                    pObj->offY + mouseY);
                ASSERT(rv == GFMRV_OK, rv);
        }
    }

    rv = gfmSprite_update(pObj->pSelf, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);

    rv = GFMRV_OK;
__ret:
    return rv;
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

