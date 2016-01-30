/**
 * @file include/ggj16/object.h
 *
 * Parser for objects. Also implements Drag 'n' Drop.
 */
#ifndef __OBJECT_STRUCT_H__
#define __OBJECT_STRUCT_H__

/** Export the object 'class' */
typedef struct stObject object;

#endif  /* __OBJECT_STRUCT_H__ */

#ifndef __OBJECT_H__
#define __OBJECT_H__

#include <GFraMe/gfmError.h>
#include <GFraMe/gfmParser.h>

/**
 * Release all alloc'ed memory
 *
 * @param  [ in]ppObj The object to be release
 * @return            GFraMe return value
 */
void object_free(object **ppObj);

/**
 * Alloc a new object
 *
 * @param  [out]ppObj The alloc'ed object
 * @return            GFraMe return value
 */
gfmRV object_getNew(object **ppObj);

/**
 * Parse the object
 *
 * @param  [ in]pObj    The parsed object
 * @param  [ in]pParser The parser
 * @return            GFraMe return value
 */
gfmRV object_init(object *pObj, gfmParser *pParser);

/**
 * Update the object
 *
 * @param  [ in]pObj    The parsed object
 * @return            GFraMe return value
 */
gfmRV object_update(object *pObj);

/**
 * Draw the object
 *
 * @param  [ in]pObj    The parsed object
 * @return            GFraMe return value
 */
gfmRV object_draw(object *pObj);
 
gfmRV object_updateDrag(object *pObj);
 
gfmRV object_initDrag(object *pObj);

gfmRV object_isPointInside(object *pObj, int x, int y);

gfmRV object_drop(object *pObj);

#endif /* __OBJECT_H__ */

