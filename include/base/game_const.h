/**
 * @file include/base/game_const.h
 *
 * Defines all constants
 */
#ifndef __GAME_CONST_H__
#define __GAME_CONST_H__

/* == Game's basic configurations =========================================== */

/** Game's "organization" */
#define ORG         "com.gfmgamecorner"
/** Game's title */
#define TITLE       "witchs_spell"
/** Config file name */
#define CONF        "config.sav"
/** Virtual window's width */
#define V_WIDTH     160
/** Virtual window's height */
#define V_HEIGHT    120
/** Virtual window's horizontal center */
#define V_CENTER_X  (V_WIDTH / 2)
/** Virtual window's vertical center */
#define V_CENTER_Y  (V_HEIGHT / 2)
/** Whether the user can resize the game window */
#define CAN_RESIZE  1
/** Initial background color in 0xAARRGGBB format */
#define BG_COLOR    0xFF000000
/** Spriteset used by the FPS counter. It's ignored, if NULL */
#define FPS_SSET    pGfx->pSset8x8
/** First ASCII tile in FPS_SSET */
#define FPS_INIT    0
/** FPS counter position. It must be enabled through a 'gfm_showFPSCounter',
 * which is called by default on debug mode */
#define FPS_X       0
#define FPS_Y       0

/* == Config file IDs ======================================================= */

#define CONF_ID_INIT        "init"
#define CONF_ID_FLAGS       "flags"
#define CONF_ID_RES         "res"
#define CONF_ID_WIDTH       "width"
#define CONF_ID_HEIGHT      "height"
#define CONF_ID_FPS         "fps"
#define CONF_ID_AUDIOQ      "audio"
#define CONF_ID_LAST_FLAGS  "lFlags"
#define CONF_ID_LAST_RES    "lRes"
#define CONF_ID_LAST_WIDTH  "lWidth"
#define CONF_ID_LAST_HEIGHT "lHeight"
#define CONF_ID_LAST_FPS    "lFps"
#define CONF_ID_LAST_AUDIOQ "lAudio"

/* == Default configuration ================================================= */

#define CONF_FLAGS  (CFG_VSYNC)
#define CONF_RES    0
#define CONF_WIDTH  640
#define CONF_HEIGHT 480
#define CONF_FPS    60
#define CONF_AUDIOQ gfmAudio_defQuality

/* == ... =================================================================== */

/** The logo's dimensions */
#define LOGO_WIDTH          96
#define LOGO_HEIGHT         32
/** The logo's sprite offset */
#define LOGO_OFFX           -6
/** The logo's speed */
#define LOGO_SPEED          64.0
/** Prime module used when calculating the logo's speed */
#define LOGO_MODULE         13463

/** Texture's transparent color */
#define COLORKEY            0xFF00FF
/** Quadtree position */
#define QT_X                -8
#define QT_Y                -8
/** Quadtree dimensions */
#define QT_WIDTH            V_WIDTH + 16
#define QT_HEIGHT           V_HEIGHT + 16
/** Maximum number of times the quadtree will subdivide */
#define QT_MAX_DEPTH        6
/** Number of nodes required for the quadtree to subdivide */
#define QT_MAX_NODES        10

/** First item tile in a 8x8 tileset */
#define FIRST_ITEM_TILE     64
/** Maximum number of items on the recipe scroller; The current limit (8)
 * was choosen considering there will never be sprites overlaping each other;
 * Also, items should have at least a 8 pixels of distance */
#define MAX_SCROLL_SPR      8
/** Define the area were items should be executed */
#define SCROLL_AREA_POS     38
#define SCROLL_AREA_HEIGHT  11
/** Items above this vertical position are completelly hidden */
#define SCROLL_HIDDEN_Y     8

#endif /* __GAME_CONST_H__ */

