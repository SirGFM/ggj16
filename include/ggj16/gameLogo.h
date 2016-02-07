/**
 * @file include/ggj16/gameLogo.h
 *
 * The game's logo
 */
#ifndef __GAMELOGO_STRUCT__
#define __GAMELOGO_STRUCT__

/** 'Export' the game logo type */
typedef struct stGameLogo gameLogo;

#endif /* __GAMELOGO_STRUCT__ */

#ifndef __GAMELOGO_H__
#define __GAMELOGO_H__

#include <GFraMe/gfmError.h>

/**
 * Release all memory alloc'ed to the logo
 *
 * @param  [ in]ppLogo The logo
 */
void gameLogo_free(gameLogo **ppLogo);

/**
 * Alloc and initialize a new logo, it starts bellow the screen, on its center
 *
 * @param  [out]ppLogo The alloc'ed logo
 * @return             GFraMe return value
 */
gfmRV gameLogo_getNew(gameLogo **ppLogo);

/**
 * Centralize the logo on the screen
 *
 * @param  [ in]pLogo The logo
 * @return            GFraMe return value
 */
gfmRV gameLogo_centralize(gameLogo *pLogo);

/**
 * Check whether the logo has finished tweening
 *
 * @param  [ in]pLogo The logo
 * @return            GFMRV_TRUE, GFMRV_FALSE
 */
gfmRV gameLogo_didTweenFinish(gameLogo *pLogo);

/**
 * Update the logo during the intro state. It moves toward a wall and gets
 * reflected in a random direction
 *
 * @param  [ in]pLogo The logo
 * @return            GFraMe return value
 */
gfmRV gameLogo_updateIntro(gameLogo *pLogo);

/**
 * Tween the logo from it's current position to the center
 *
 * @param  [ in]pLogo The logo
 * @return            GFraMe return value
 */
gfmRV gameLogo_updateTween(gameLogo *pLogo);

/**
 * Moves the logo up and down
 *
 * @param  [ in]pLogo The logo
 * @return            GFraMe return value
 */
gfmRV gameLogo_update(gameLogo *pLogo);

/**
 * Render the logo
 *
 * @param  [ in]pLogo The logo
 * @return            GFraMe return value
 */
gfmRV gameLogo_draw(gameLogo *pLogo);

#endif /* __GAMELOGO_H__ */

