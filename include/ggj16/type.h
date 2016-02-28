/**
 * @file include/ggj16/type.h
 *
 * Defines all types and associates their string with their type
 */
#ifndef __TYPE_H__
#define __TYPE_H__

#include <GFraMe/gfmError.h>

#define FIRST_INGREDIENT    T_RAT_TAIL
#define FIRST_ACTION        T_ROTATE_CW
#define NUM_INGREDIENT      (T_WAIT - T_RAT_TAIL)
#define NUM_ACTION          (T_CAULDRON - T_ROTATE_CW)

enum enItemType {
    T_RAT_TAIL = 0,
    T_BAT_WING,
    T_EYE,
    T_STAR,
    T_PHOENIX_FEATHER,
    T_SKULL,
    T_HORN,
    T_MUSHROOM,
    T_WAIT, /* Doesn't work, do not use */
    T_ROTATE_CW,
    T_ROTATE_CCW,
    T_MOVE_VERTICAL,
    T_MOVE_HORIZONTAL,
    T_CAULDRON,
    T_MAX,
    T_RANDOM_INGREDIENT,
    T_RANDOM_ACTION,
    T_NONE
};
typedef enum enItemType itemType;

/**
 * Search for the type of a given string
 *
 * @param  [out]pType The type
 * @param  [ in]pName The name to be checked
 * @return            GFraMe return value
 */
gfmRV type_getHandle(itemType *pType, char *pName);

#endif /* __TYPE_H__ */

