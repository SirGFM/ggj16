/**
 * @file src/levelLoader.c
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
#include <base/game_const.h>
#include <base/game_ctx.h>

#include <GFraMe/gfmAssert.h>
#include <GFraMe/gfmError.h>
#include <GFraMe/gfmParser.h>
#include <GFraMe/gfmString.h>

#include <ggj16/levelLoader.h>
#include <ggj16/type.h>

#include <stdlib.h>
#include <string.h>

typedef enum enRecipeDifficulty recipeDifficulty;
typedef struct stRecipeTemplate recipeTemplate;

/** Possible difficulty levels */
enum enRecipeDifficulty {
    RD_NONE = 0,
    RD_EASY,
    RD_MEDIUM,
    RD_HARD,
    RD_MAXDIFF
};

/** Dictionary (i.e., map) of difficulties */
static const char *diffDict[RD_MAXDIFF] = {
    "none",
    "easy",
    "medium",
    "hard"
};

/** A recipe template */
struct stRecipeTemplate {
    /** The items on the template */
    itemType *pData;
    /** Next template of this same type (yay! Linked list!) */
    recipeTemplate *pNext;
    /** Template's filename (so it may be searched) */
    char *pPath;
    /** The number of items in thius template */
    int len;
    /** The recipe's difficulty */
    recipeDifficulty difficulty;
};

/** Global struct with templates and stuff */
struct stLevelLoader {
    /** String used to retrieve the actual path to a file */
    gfmString *pString;
    /** List with all parsed templates */
    recipeTemplate *pTemplate;
    /** Linked-list of easy templates */
    recipeTemplate *pEasyTemplates;
    /** Linked-list of medium templates */
    recipeTemplate *pMediumTemplates;
    /** Linked-list of hard templates */
    recipeTemplate *pHardTemplates;
    /** The current level data */
    itemType *pData;
    /** Number of easy templates */
    int numEasy;
    /** Number of medium templates */
    int numMedium;
    /** Number of hard templates */
    int numHard;
    /** Total number of templates */
    int numTemplates;
    /** Total length of the level data */
    int dataLen;
};

/**
 * Load a file into a template
 *
 * @param  [ in]pTemplate The template
 * @param  [ in]pString   gfmString used to prepend "levels/" to the filename
 * @param  [ in]pFilename Relative path to the file (only its name+extension).
 *                        The file must be located on assets/levels/
 * @return                GFraMe return value
 */
static gfmRV levelLoader_parseTemplate(recipeTemplate *pTemplate,
        gfmString *pString, char *pFilename) {
    /** The actual path to the file */
    char *pPath;
    /** The parser */
    gfmParser *pParser;
    /** The parsed type, must be 'gfmParserType_attributes' */
    gfmParserType type;
    /** GFraMe return value */
    gfmRV rv;
    /** Iterate through the items */
    int i;
    /** Current position on the item array */
    int curItem;
    /** String's length */
    int len;

    pParser = 0;
    pString = 0;

    /* Prepend 'level/' to the file path */
    rv = gfmString_initStatic(pString, "levels/", 1);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmString_concat(pString, pFilename, strlen(pFilename));
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmString_getString(&pPath, pString);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmString_getLength(&len, pString);
    ASSERT(rv == GFMRV_OK, rv);

    /* Open the template file */
    rv = gfmParser_getNew(&pParser);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmParser_init(pParser, pGame->pCtx, pPath, len);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmParser_parseNext(pParser);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmParser_getType(&type, pParser);
    ASSERT(rv == GFMRV_OK, rv);
    ASSERT(type == gfmParserType_attributes, GFMRV_INTERNAL_ERROR);

    /* Retrieve the number of items in the template */
    rv = gfmParser_getNumProperties(&(pTemplate->len), pParser);
    ASSERT(rv == GFMRV_OK, rv);

    /* Alloc a big enough data buffer */
    pTemplate->pData = (itemType*)malloc(sizeof(itemType) * pTemplate->len);
    ASSERT(pTemplate->pData, GFMRV_ALLOC_FAILED);
    memset(pTemplate->pData, 0x0, sizeof(itemType) * pTemplate->len);

    /* Parse the template */
    i = 0;
    curItem = 0;
    while (i < pTemplate->len) {
        /** The current property */
        char *pKey, *pValue;

        rv = gfmParser_getProperty(&pKey, &pValue, pParser, i);
        ASSERT(rv == GFMRV_OK, rv);

        if (strcmp(pKey, "difficulty") == 0) {
            recipeDifficulty diff;

            /* Retrieve the difficulty as an integer */
            diff = 0;
            while (diff < RD_MAXDIFF && strcmp(pValue, diffDict[diff]) == 0) {
                diff++;
            }
            ASSERT(diff > RD_NONE, GFMRV_PARSER_BAD_TOKEN);
            ASSERT(diff < RD_MAXDIFF, GFMRV_PARSER_BAD_TOKEN);

            /* Store it */
            pTemplate->difficulty = diff;
        } /* if (strcmp(pKey, "difficulty") == 0) */
        else if (strcmp(pKey, "ingredient") == 0 ||
                strcmp(pKey, "action") == 0) {
            itemType item;

            if (strcmp(pValue, "random") == 0) {
                if (strcmp(pKey, "ingredient") == 0) {
                    item = T_RANDOM_INGREDIENT;
                }
                else if (strcmp(pKey, "action") == 0) {
                    item = T_RANDOM_ACTION;
                }
            }
            else {
                rv = type_getHandle(&item, pValue);
                ASSERT(rv == GFMRV_OK, rv);
            }

            /* Store the retrieved item */
            pTemplate->pData[curItem] = item;
            curItem++;
        } /* else if (strcmp(pKey, "ingredient") == 0 || [...] */
        else {
            ASSERT(0, GFMRV_PARSER_BAD_TOKEN);
        }

        i++;
    } /* while (i < pTemplate->len) */

    /* Check that all items were valid (and that there was exactly one
     * difficulty) */
    ASSERT(curItem == pTemplate->len - 1, GFMRV_PARSER_BAD_TOKEN);

    /* Remove the difficulty from the length */
    pTemplate->len--;

    /* Store the template's path */
    pTemplate->pPath = (char*)malloc(sizeof(char) * (strlen(pFilename) + 1));

    rv = GFMRV_OK;
__ret:
    gfmParser_free(&pParser);

    return rv;
}

/**
 * Alloc the level loader and initialize it with every template
 *
 * @return GFraMe return value
 */
gfmRV levelLoader_init() {
    /** The parser */
    gfmParser *pParser;
    /** The parsed type, must be 'gfmParserType_attributes' */
    gfmParserType type;
    /** GFraMe return value */
    gfmRV rv;
    /** Iterate through the file names */
    int i;

    pParser = 0;

    /* Make sure the pointers are valid (and the singleton hasn't been
     * instantiated, yet) */
    ASSERT(pGlobal, GFMRV_INTERNAL_ERROR);
    ASSERT(!pGlobal->pLevelLoader, GFMRV_INTERNAL_ERROR);

    /* Alloc the level loader */
    pGlobal->pLevelLoader = (levelLoader*)malloc(sizeof(levelLoader));
    ASSERT(pGlobal->pLevelLoader, GFMRV_ALLOC_FAILED);
    memset(pGlobal->pLevelLoader, 0x0, sizeof(levelLoader));

    /* Alloc the string used to retrieve file paths */
    rv = gfmString_getNew(&(pGlobal->pLevelLoader->pString));
    ASSERT(rv == GFMRV_OK, rv);

    /* Load the template list and all templates */
    rv = gfmParser_getNew(&pParser);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmParser_initStatic(pParser, pGame->pCtx, LEVELLOADER_TEMPLATE_FILE);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmParser_parseNext(pParser);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmParser_getType(&type, pParser);
    ASSERT(rv == GFMRV_OK, rv);
    ASSERT(type == gfmParserType_attributes, GFMRV_INTERNAL_ERROR);

    /* Get the number of templates and parse them all */
    rv = gfmParser_getNumProperties(&(pGlobal->pLevelLoader->numTemplates),
            pParser);
    ASSERT(rv == GFMRV_OK, rv);
    pGlobal->pLevelLoader->pTemplate = (recipeTemplate*)malloc(
            sizeof(recipeTemplate) * pGlobal->pLevelLoader->numTemplates);
    ASSERT(pGlobal->pLevelLoader->pTemplate, GFMRV_ALLOC_FAILED);
    memset(pGlobal->pLevelLoader->pTemplate, 0x0,
            sizeof(recipeTemplate) * pGlobal->pLevelLoader->numTemplates);
    i = 0;
    while (i < pGlobal->pLevelLoader->numTemplates) {
        /** Strings read from the latest property */
        char *pKey, *pPath;

        /* Retrieve the current template path */
        rv = gfmParser_getProperty(&pKey, &pPath, pParser, i);
        ASSERT(rv == GFMRV_OK, rv);
        ASSERT(strcmp(pKey, "file") == 0, GFMRV_INTERNAL_ERROR);

        /* Parse the file into the current template */
        rv = levelLoader_parseTemplate(&(pGlobal->pLevelLoader->pTemplate[i]),
                pGlobal->pLevelLoader->pString, pPath);
        ASSERT(rv == GFMRV_OK, rv);

        /* Add it to a list based on its difficulty */
        do {
            /** Position were stuff will be added */
            recipeTemplate **ppListTop;

            /* Retrieve the current top */
            switch (pGlobal->pLevelLoader->pTemplate[i].difficulty) {
                case RD_EASY: {
                    ppListTop = &(pGlobal->pLevelLoader->pEasyTemplates);
                    pGlobal->pLevelLoader->numEasy++;
                }; break;
                case RD_MEDIUM: {
                    ppListTop = &(pGlobal->pLevelLoader->pMediumTemplates);
                    pGlobal->pLevelLoader->numMedium++;
                }; break;
                case RD_HARD: {
                    ppListTop = &(pGlobal->pLevelLoader->pHardTemplates);
                    pGlobal->pLevelLoader->numHard++;
                }; break;
                default: {
                    ASSERT(0, GFMRV_INTERNAL_ERROR);
                }
            }

            /* Make this last template point to the list top */
            pGlobal->pLevelLoader->pTemplate[i].pNext = (*ppListTop);
            /* Update the top with this latest template */
            *ppListTop = &(pGlobal->pLevelLoader->pTemplate[i]);
        } while (0);

        /* Go to the next one */
        i++;
    } /* while (i < pGlobal->pLevelLoader->numTemplates) */

    rv = GFMRV_OK;
__ret:
    if (rv != GFMRV_OK) {
        levelLoader_free();
    }
    gfmParser_free(&pParser);

    return rv;
}

/**
 * Release all memory releated to a loaded/initialized level loader
 */
void levelLoader_free() {
    /** Iterate through all templates */
    int i;

    /* Make sure it has been previously alloc'ed */
    if (!pGlobal || !pGlobal->pLevelLoader) {
        return;
    }

    /* Release all templates */
    i = 0;
    while (pGlobal->pLevelLoader->pTemplate &&
            i < pGlobal->pLevelLoader->numTemplates) {
        if (pGlobal->pLevelLoader->pTemplate[i].pData) {
            free(pGlobal->pLevelLoader->pTemplate[i].pData);
        }
        if (pGlobal->pLevelLoader->pTemplate[i].pPath) {
            free(pGlobal->pLevelLoader->pTemplate[i].pPath);
        }
        i++;
    }
    if (pGlobal->pLevelLoader->pTemplate) {
        free(pGlobal->pLevelLoader->pTemplate);
    }

    gfmString_free(&(pGlobal->pLevelLoader->pString));
    if (pGlobal->pLevelLoader->pData) {
        free(pGlobal->pLevelLoader->pData);
    }

    /* Finally, release the level loader itself */
    free(pGlobal->pLevelLoader);
    pGlobal->pLevelLoader = 0;
}

/**
 * Load (and procedurally generate it, as necessary) a level and return it on an
 * array
 *
 * @param  [out]ppData The loaded level
 * @param  [out]pLen   The number of items in the level
 * @param  [ in]level  The current level
 * @return             GFraMe return value
 */
gfmRV levelLoader_generateLevel(itemType **ppData, int *pLen, int level) {
    /** File's actual path */
    char *pKey, *pPath;
    /** The parser */
    gfmParser *pParser;
    /** The parsed type, must be 'gfmParserType_attributes' */
    gfmParserType type;
    /** GFraMe return value */
    gfmRV rv;
    /** Iterate through the templates */
    int i;
    /** Length of the string */
    int len;
    /** Length of the level */
    int levelLen;
    /** Maximum number of levels */
    int max;

    /* Load the levels list and the desired level */
    rv = gfmParser_getNew(&pParser);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmParser_initStatic(pParser, pGame->pCtx, LEVELLOADER_LEVELS_FILE);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmParser_parseNext(pParser);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmParser_getType(&type, pParser);
    ASSERT(rv == GFMRV_OK, rv);
    ASSERT(type == gfmParserType_attributes, GFMRV_INTERNAL_ERROR);

    /* Check that the requested level is valid */
    rv = gfmParser_getNumProperties(&max, pParser);
    ASSERT(rv == GFMRV_OK, rv);
    ASSERT(level < max, GFMRV_INVALID_INDEX);

    /* Retrieve the level's filename */
    rv = gfmParser_getProperty(&pKey, &pPath, pParser, level);
    ASSERT(rv == GFMRV_OK, rv);
    ASSERT(strcmp(pKey, "file") == 0, GFMRV_INTERNAL_ERROR);

    /* Prepend 'level/' to the file path */
    rv = gfmString_initStatic(pGlobal->pLevelLoader->pString, "levels/", 1);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmString_concat(pGlobal->pLevelLoader->pString, pPath, strlen(pPath));
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmString_getString(&pPath, pGlobal->pLevelLoader->pString);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmString_getLength(&len, pGlobal->pLevelLoader->pString);
    ASSERT(rv == GFMRV_OK, rv);

    /* Open the level file for parsing */
    rv = gfmParser_reset(pParser);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmParser_init(pParser, pGame->pCtx, pPath, len);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmParser_parseNext(pParser);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmParser_getType(&type, pParser);
    ASSERT(rv == GFMRV_OK, rv);
    ASSERT(type == gfmParserType_attributes, GFMRV_INTERNAL_ERROR);
    rv = gfmParser_getNumProperties(&len, pParser);
    ASSERT(rv == GFMRV_OK, rv);

    /* Actually load the level */
    i = 0;
    levelLen = 0;
    while (i < len) {
        /** The current property */
        char *pKey, *pVal;
        /** The template being loaded */
        recipeTemplate *pTemplate;
        /** Iterate through the templates items */
        int j;

        rv = gfmParser_getProperty(&pKey, &pVal, pParser, i);
        ASSERT(rv == GFMRV_OK, rv);
        if (strcmp(pKey, "static") == 0) {
            /** Iterate through all the template searching for the name */
            int j;

            j = 0;
            while (j < pGlobal->pLevelLoader->numTemplates && strcmp(pVal,
                    pGlobal->pLevelLoader->pTemplate[j].pPath) != 0) {
                j++;
            }
            ASSERT(j < pGlobal->pLevelLoader->numTemplates,
                    GFMRV_PARSER_BAD_TOKEN);

            pTemplate = &(pGlobal->pLevelLoader->pTemplate[j]);
        } /* if (strcmp(pKey, "static") == 0) */
        else if (strcmp(pKey, "dynamic") == 0) {
            /** Difficulty of the requested random template */
            recipeDifficulty diff;
            /** 'Index' of the selected template */
            int index;
            /** Maximum 'index' of the selected template */
            int max;

            /* Retrieve the difficulty as an integer */
            diff = 0;
            while (diff < RD_MAXDIFF && strcmp(pVal, diffDict[diff]) == 0) {
                diff++;
            }
            ASSERT(diff > RD_NONE, GFMRV_PARSER_BAD_TOKEN);
            ASSERT(diff < RD_MAXDIFF, GFMRV_PARSER_BAD_TOKEN);

            /* Select a template of the given type */
            switch (diff) {
                case RD_EASY: {
                    pTemplate = pGlobal->pLevelLoader->pEasyTemplates;
                    max = pGlobal->pLevelLoader->numEasy;
                } break;
                case RD_MEDIUM: {
                    pTemplate = pGlobal->pLevelLoader->pMediumTemplates;
                    max = pGlobal->pLevelLoader->numMedium;
                } break;
                case RD_HARD: {
                    pTemplate = pGlobal->pLevelLoader->pHardTemplates;
                    max = pGlobal->pLevelLoader->numHard;
                } break;
                default: {
                    /* Shouldn't happen... */
                    ASSERT(0, GFMRV_INTERNAL_ERROR);
                }
            } /* switch (diff) */

            /* Select a template at random and use it */
            index = (rand() % 127) % max;
            while (index > 0) {
                ASSERT(pTemplate->pNext, GFMRV_INTERNAL_ERROR);
                index--;
            }
        } /* else if (strcmp(pKey, "dynamic") == 0) */
        else {
            ASSERT(0, GFMRV_PARSER_BAD_TOKEN);
        }

        /* Expand the buffer as necessary */
        if (levelLen + pTemplate->len >= pGlobal->pLevelLoader->dataLen) {
            pGlobal->pLevelLoader->pData = (itemType*)realloc(
                    pGlobal->pLevelLoader->pData,
                    sizeof(itemType) * (levelLen + pTemplate->len));
            ASSERT(pGlobal->pLevelLoader->pData, GFMRV_ALLOC_FAILED);
            pGlobal->pLevelLoader->dataLen = levelLen + pTemplate->len;
        }

        /* Insert the template data into the level */
        j = 0;
        while (j < pTemplate->len) {
            /** The current item */
            itemType item;

            item = pTemplate->pData[j];
            if (item == T_RANDOM_INGREDIENT) {
                /* Select a random ingredient */
                item = FIRST_INGREDIENT + ((rand() % 127) % NUM_INGREDIENT);
            }
            else if (item == T_RANDOM_ACTION) {
                /* Select a random action */
                item = FIRST_ACTION + ((rand() % 127) % NUM_ACTION);
            }

            pGlobal->pLevelLoader->pData[levelLen] = item;
            j++;
            levelLen++;
        }

        i++;
    } /* while (i < len) */

    /* Retrieve the loaded level */
    *ppData = pGlobal->pLevelLoader->pData;
    *pLen = levelLen;
    rv = GFMRV_OK;
__ret:
    gfmParser_free(&pParser);

    return rv;
}

