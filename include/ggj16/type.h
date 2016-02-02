/**
 * @file include/ggj16/type.h
 *
 * Defines all types and associates their string with their type
 */
#ifndef __TYPE_H__
#define __TYPE_H__

#include <GFraMe/gfmError.h>

enum enItemType {
    T_CAULDRON = 0,
    T_RAT_TAIL,
    T_BAT_WING,
    T_EYE,
    T_WEB,
    T_PHOENIX_FEATHER,
    T_SKULL,
    T_BONE,
    T_MUSHROOM,
    T_ROTATE_CW,
    T_ROTATE_CCW,
    T_WAIT, /* Doesn't work, do not use */
    T_MOVE_VERTICAL,
    T_MOVE_HORIZONTAL,
    T_MAX,
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

