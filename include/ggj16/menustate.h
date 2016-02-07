/**
 * @file include/ggj16/menustate.h
 *
 * The menu state. Handles the intro animation (that plays while loading the
 * audio!!!), switching to states (options/game/...) etc
 */
#ifndef __MENUSTATE_H__
#define __MENUSTATE_H__

#include <GFraMe/gfmError.h>

/**
 * Release everything alloc'ed on init
 */
void ms_free();

/**
 * Initialize the menu state (alloc anything needed etc)
 */
gfmRV ms_init();

/**
 * Updates the intro state (in which a simple animation is played until
 * everything is ready)
 */
gfmRV ms_introUpdate();

/**
 * Tweens the logo to the proper position
 */
gfmRV ms_tweenUpdate();

/**
 * Update everything
 */
gfmRV ms_update();

/**
 * Draws everything
 */
gfmRV ms_draw();

#endif  /* __MENUSTATE_H__ */

