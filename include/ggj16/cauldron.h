/**
 * @file include/ggj16/cauldron.h
 *
 * Parser for cauldrons. Also implements Drag 'n' Drop.
 */
#ifndef __CAULDRON_STRUCT_H__
#define __CAULDRON_STRUCT_H__

/** Export the cauldron 'class' */
typedef struct stCauldron cauldron;

#endif  /* __CAULDRON_STRUCT_H__ */

#ifndef __CAULDRON_H__
#define __CAULDRON_H__

#include <GFraMe/gfmError.h>
#include <GFraMe/gfmParser.h>

/* >____< */
struct stCauldron {
    /** The cauldron's sprite */
    gfmSprite *pSelf;
    /** The heat trail over the cauldron */
    gfmSprite *pHeatTrail;
    /** Cauldron's only animation */
    int anim;
    /** Whether the cauldron exploded */
    int didExplode;
};

/**
 * Release all alloc'ed memory
 *
 * @param  [ in]ppObj The cauldron to be release
 * @return            GFraMe return value
 */
void cauldron_free(cauldron **ppObj);

/**
 * Alloc a new cauldron
 *
 * @param  [out]ppObj The alloc'ed cauldron
 * @return            GFraMe return value
 */
gfmRV cauldron_getNew(cauldron **ppObj);

/**
 * Parse the cauldron
 *
 * @param  [ in]pObj    The parsed cauldron
 * @param  [ in]pParser The parser
 * @return            GFraMe return value
 */
gfmRV cauldron_init(cauldron *pObj, gfmParser *pParser);

/**
 * Explode the cauldron
 *
 * @param  [ in]pCal The parsed cauldron
 * @return           GFraMe return value
 */
gfmRV cauldron_doExplode(cauldron *pCal);

/**
 * Update the cauldron
 *
 * @param  [ in]pObj    The parsed cauldron
 * @return            GFraMe return value
 */
gfmRV cauldron_update(cauldron *pObj);

/**
 * Draw the cauldron
 *
 * @param  [ in]pObj    The parsed cauldron
 * @return            GFraMe return value
 */
gfmRV cauldron_draw(cauldron *pObj);

/**
 * Check if a sprite is overlapping the cauldron
 *
 * @param  [ in]pCal The cauldron
 * @param  [ in]pSpr The other sprite
 * @return           GFraMe return value
 */
gfmRV cauldron_isOverlapping(cauldron *pCal, gfmSprite *pSpr);

#endif /* __CAULDRON_H__ */

