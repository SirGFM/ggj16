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
    "cauldron",
    "rat_tail",
    "bat_wing",
    "eye",
    "web",
    "phoenix_feather",
    "monkey_ear",
    "bone",
    "mushroom",
    "rotate_cw",
    "rotate_ccw",
    "wait",
    "move_horizontal",
    "move_vertical"
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

