/**
 * @file src/type.c
 *
 * Defines all types and associates their string with their type
 */
#include <GFraMe/gfmAssert.h>
#include <GFraMe/gfmError.h>

#include <ggj16/type.h>

#include <string.h>

static char *pTypeStr[T_MAX] = {
    "rat_tail",
    "bat_wing",
    "eye",
    "star",
    "phoenix_feather",
    "skull",
    "horn",
    "mushroom",
    "wait",
    "rotate_cw",
    "rotate_ccw",
    "move_horizontal",
    "move_vertical",
    "cauldron",
};

/**
 * Search for the type of a given string
 *
 * @param  [out]pType The type
 * @param  [ in]pName The name to be checked
 * @return            GFraMe return value
 */
gfmRV type_getHandle(itemType *pType, char *pName) {
    /** GFraMe return value */
    gfmRV rv;
    /** Iterate through the types */
    itemType i;

    ASSERT(pName, GFMRV_ARGUMENTS_BAD);

    /** Check every type on the dictionary */
    i = 0;
    while (i < T_MAX) {
        if (strcmp(pName, pTypeStr[i]) == 0) {
            break;
        }
        i++;
    }
    ASSERT(i < T_MAX, GFMRV_INVALID_INDEX);

    *pType = i;
    rv = GFMRV_OK;
__ret:
    return rv;
}

