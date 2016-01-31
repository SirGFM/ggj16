#ifndef RECIPE_H_INCLUDED
#define RECIPE_H_INCLUDED

struct Recipe
{
    char* id;
    int numTemplates;
    struct Template** templates;
};
typedef struct Recipe Recipe;

struct Template
{
    char* id;
    int type;
    int numItems;
    struct Item** items;
};
typedef struct Template Template;

struct Item
{
    char* id;
    int type;
    int numIngredients;
    int* ingredients;
};
typedef struct Item Item;
#endif // RECIPE_H_INCLUDED
