/**
 * @file include/base/assets.h
 *
 * Handles loading assets and creating the required spritesets
 */
#ifndef __ASSETS_STRUCT__
#define __ASSETS_STRUCT__

typedef enum enBgAudioHandles bgAudioHandles;

#endif /* __ASSETS_STRUCT__ */

#ifndef __ASSETS_H__
#define __ASSETS_H__

#include <GFraMe/gfmError.h>

enum enBgAudioHandles {
    SFX_ON_ENTER_HND = 0,
    SFX_ON_WRONG_HND,
    SFX_ON_SWIPE_IN_HND,
    SFX_ON_SWIPE_OUT_HND,
    SFX_BUBBLE_1,
    SFX_BUBBLE_2,
    SFX_DRIP,
    SONG_HND,
    MAX_HND,
    FIRST_HND = 0
};

/**
 * Load all assets
 *
 * @return GFraMe return value
 */
gfmRV assets_load();

#endif /* __ASSETS_H__ */

