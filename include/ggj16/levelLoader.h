/**
 * @file include/ggj16/levelLoader.h
 *
 * Generates levels with pseudo-random elements. Those elements are read from
 * the files listed on "assets/levels/tmpl_list.gfm". Levels are listed on
 * "assets/levels/level_list.gfm".
 *
 * Each element has a difficulty and a list of ingredients and actions. If the
 * special value 'random' is assigned, the key's value will be resolved in
 * run-time, whenever it's used to generate a new level.
 *
 * Levels are built from "static" and "dynamics" templates. A static one has a
 * filename as its value, and that actual template is used. Dynamics ones,
 * whoever simply defined that a template of a given type should be used.
 *
 * Because of how the parser works, every value must have a key associated with
 * it. Therefore, this is how the files are arranged:
 *
 * assets/levels/level_list.gfm:
 *  attr
 *  [ file, level_filename_001 ] 
 *  [ file, level_filename_002 ] 
 *  [ file, level_filename_003 ] 
 *  ...
 *
 * assets/levels/tmpl_list.gfm:
 *  attr
 *  [ file, tmpl_filename_001 ]
 *  [ file, tmpl_filename_002 ]
 *  [ file, tmpl_filename_003 ]
 *  ...
 *
 * assets/levels/level_filename_001:
 *  attr
 *  [ static, tmpl_filename_001 ]
 *  [ dynamic, easy ]
 *  [ static, tmpl_filename_002 ]
 *  [ dynamic, easy ]
 *  [ dynamic, normal ]
 *  ...
 *
 * assets/levels/tmpl_filename_001:
 *  attr
 *  [ difficulty, easy ]
 *  [ ingredient, rat_tail ]
 *  [ ingredient, random ]
 *  [ action, random ]
 *  ...
 *
 * And so on...
 */ 
#ifndef __LEVELLOADER_STRUCT__
#define __LEVELLOADER_STRUCT__

typedef struct stLevelLoader levelLoader;

#endif /* __LEVELLOADER_STRUCT__ */

#ifndef __LEVELLOADER_H__
#define __LEVELLOADER_H__

#include <GFraMe/gfmError.h>

#include <ggj16/type.h>

/**
 * Alloc the level loader and initialize it with every template
 *
 * @return GFraMe return value
 */
gfmRV levelLoader_init();

/**
 * Release all memory releated to a loaded/initialized level loader
 */
void levelLoader_free();

/**
 * Load (and procedurally generate it, as necessary) a level and return it on an
 * array
 *
 * @param  [out]ppData The loaded level
 * @param  [out]pLen   The number of items in the level
 * @param  [ in]level  The current level
 * @return             GFraMe return value
 */
gfmRV levelLoader_generateLevel(itemType **ppData, int *pLen, int level);

#endif /* __LEVELLOADER_H__ */

