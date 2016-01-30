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

#include <stdlib.h>
#include <string.h>

static int isAnyObjectDragging = GFMRV_FALSE;

enum enObjectType
{
	object_ingredient,
	object_cauldron
};typedef enum enObjectType objectType;


struct stObject {
    /** The object's sprite */
    gfmSprite *pSelf;
	objectType type;
	int initialPosX;
	int initialPosY;
	int isDragging;
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
    int tile, type;

    /** Get the object's position, dimensions and type */
    rv = gfmParser_getPos(&x, &y, pParser);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmParser_getDimensions(&width, &height, pParser);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmParser_getIngameType(&pName, pParser);
    ASSERT(rv == GFMRV_OK, rv);
    /** Adjust the vertical position to the sprite's top */
    y -= height;
    /* Set the tile and spriteset according to the type */
    if (strcmp(pName, "cauldron") == 0) {
        width = 20;
        height = 18;
        tile = 24;
        pSset = pGfx->pSset32x32;
		pObj->type = object_cauldron;
    }
    else if (strcmp(pName, "rat_tail") == 0) {
        tile = 352;
        pSset = pGfx->pSset8x8;
		pObj->type = object_ingredient;
    }
    type = 0;
	
	pObj->initialPosX = x;
	pObj->initialPosY = y;

    /** Initialize the sprite */
    rv = gfmSprite_init(pObj->pSelf, x, y, width, height, pSset, 0 /* offx */,
            0 /* offy */, pObj, type);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_setFrame(pObj->pSelf, tile);
    ASSERT(rv == GFMRV_OK, rv);

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
	object_handleDragNDrop(pObj);
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

/**
 * Handles drag and drop behaviour
 *
 * @param  [ in]pObj    The parsed object
 * @return            GFraMe return value
 */
gfmRV object_handleDragNDrop(object *pObj) {
	gfmRV result = GFMRV_OK;
	
	gfmInput *pInput;
	gfm_getInput(&pInput, pGame->pCtx);	
	
	if (pObj->isDragging) {
			result = object_updateDrag(pInput, pObj);
	}

	return result;
}

gfmRV object_updateDrag(gfmInput *pInput, object *pObj) {
	int posX;
	int posY;
	gfmRV result = gfmInput_getPointerPosition(&posX, &posY, pInput);
	
	gfmSprite_setPosition(pObj->pSelf, posX, posY);
	
	return result;
}

gfmRV object_initDrag(object *pObj) {	
	pObj->isDragging = GFMRV_TRUE;
	isAnyObjectDragging = GFMRV_TRUE;
	
	return GFMRV_OK;
}

gfmRV object_isPointInside(object *pObj, int x, int y) {
	gfmRV result = 	(pObj->type == object_ingredient) ?
							gfmSprite_isPointInside(pObj->pSelf, x, y) :
							GFMRV_FALSE;
							
	return result;		
}