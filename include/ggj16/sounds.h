/**
 * @file include/ggj16/sounds.h
 *
 * Manages playing sfx and songs
 */
#ifndef __SOUNDS_H__
#define __SOUNDS_H__

#include <GFraMe/gfmError.h>

/**
 * Play the 'drip' sfx
 *
 * @return GFraMe return value
 */
gfmRV sound_drip();

/**
 * Play the 'bubble' sfx
 *
 * @return GFraMe return value
 */
gfmRV sound_bubble();

/**
 * Play the 'swipe' sfx
 *
 * @return GFraMe return value
 */
gfmRV sound_playSwipe();

/**
 * Play the sfx for items entering the central area
 *
 * @return GFraMe return value
 */
gfmRV sound_playEnterItem();

/**
 * Play the sfx for adding the wrong item
 *
 * @return GFraMe return value
 */
gfmRV sound_onWrongItem();

/**
 * Play the bg song (only once!)
 *
 * @return GFraMe return value
 */
gfmRV sound_playSong();

#endif /* __SOUNDS_H__ */

