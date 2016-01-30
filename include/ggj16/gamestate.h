/**
 * @file include/ggj16/gamestate.h
 *
 * Main game state. Handles game logic, win/lose condition... pretty much
 * everything
 */
#ifndef __GAMESTATE_H__
#define __GAMESTATE_H__

#include <GFraMe/gfmError.h>

/**
 * Release everything alloc'ed on init
 */
void gs_free();

/**
 * Initialize the game state (alloc anything needed, load first level and so on)
 */
gfmRV gs_init();

/**
 * Update everything
 */
gfmRV gs_update();

/**
 * Draws everything
 */
gfmRV gs_draw();

#endif  /* __GAMESTATE_H__ */

