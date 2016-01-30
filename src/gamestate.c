/**
 * @file src/gamestate.c
 *
 * Main game state. Handles game logic, win/lose condition... pretty much
 * everything
 */
#include <base/game_const.h>
#include <base/game_ctx.h>

#include <GFraMe/gfmAssert.h>
#include <GFraMe/gfmError.h>

#include <ggj16/gamestate.h>

/**
 * Release everything alloc'ed on init
 */
void gs_free() {
}

/**
 * Initialize the game state (alloc anything needed, load first level and so on)
 */
gfmRV gs_init() {
    return GFMRV_OK;
}

/**
 * Update everything
 */
gfmRV gs_update() {
    return GFMRV_OK;
}

/**
 * Draws everything
 */
gfmRV gs_draw() {
    return GFMRV_OK;
}

