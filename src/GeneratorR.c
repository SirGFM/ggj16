#include <stdio.h>
#include <time.h>
#include "recipe.h"

#define   ARRAY_SIZE   120
#define   ARRAY_SIZE_NAME 255
#define   ARRAY_SIZE_ID 200

int generateRandom(char* filename,int* values);
Recipe* readRecipe(char* filename);
Template* readTemplate(char* filename,int type,int numT );
Item* readItem(char* filename);
int getvalues(Recipe* r, int* values);

void freeRecipe(Recipe *r);
void freeTemplate(Template*t);
void freeItem(Item* item);

int main()
{
    //Sample use;
    int tam=0;
    int* values=(int*)malloc(ARRAY_SIZE*sizeof(int));
    char* filename=(char*)malloc(ARRAY_SIZE_NAME*sizeof(char));
    filename="data/recipes/recipe_tutorial.txt";
    tam=generateRandom(filename,values);
    printf("%d\n",tam);

    for(int i=0;i<tam;i++){
        printf("%d ",values[i]);
    }
    //free memory
    free(values);
    free(filename);
    return 0;
}

int generateRandom(char* filename,int* values)
{
    Recipe* r=readRecipe(filename); //read files;
    int resul=getvalues(r, values);

    freeRecipe(r);
    return resul;// get list of int
}

Recipe* readRecipe(char* filename)
{
    FILE* file = fopen(filename,"r+");
    if(file == NULL) printf("Error al abrir archivo");

    //data
    char* idRecipe=(char*)malloc(sizeof(char)*ARRAY_SIZE_ID);
    int numTemplates=0;
    char** paths = NULL;
    Template** templates= NULL;

    //read
    fscanf(file, "%s", idRecipe);
    fscanf (file, "%d", &numTemplates);
    templates = (Template*)malloc(sizeof(Template*)* numTemplates);
    //memory
    paths=(char**)malloc(sizeof(char*)*ARRAY_SIZE_NAME);
    for(int i=0;i<numTemplates;i++){
        paths[i]=(char*)malloc(sizeof(char)*ARRAY_SIZE_NAME);
    }
    for(int i=0;i<numTemplates;i++){
       fscanf(file, "%s", paths[i]);
       int type=0;
       int numT=0;
       fscanf (file, "%d", &type);
       fscanf (file, "%d", &numT);
       templates[i]=readTemplate(paths[i],type,numT);
    }
    //fill
    Recipe* recipe=(Recipe*)malloc(sizeof(Recipe));
    recipe->id=idRecipe;
    recipe->numTemplates=numTemplates;
    recipe->templates=templates;

        //free
    for(int i=0;i<numTemplates;i++){
        free(paths[i]);
    }
    free(paths);

    fclose(file);
    return recipe;
}

Template* readTemplate(char* filename,int type,int numT )
{
    //get file
    char* newname=(char*)malloc(sizeof(char)*ARRAY_SIZE_NAME);
    int indice=0;
    srand((int)time(NULL));
    if(numT!=0)indice=rand() % numT;
    sprintf(newname, "%s_%d_%d.txt",filename,type, indice);
    FILE* file = fopen(newname,"r");
    if(file == NULL) printf("Error al abrir archivo");
    //data
    char* id=(char*)malloc(sizeof(char)*ARRAY_SIZE_ID);
    int numItems=0;
    int type2=0;
    char** itemPath=NULL;
    Item** items = NULL;
    //read
    fscanf(file, "%s", id);
    fscanf (file, "%d", &type2);
    fscanf (file, "%d", &numItems);
    //memory
    items=(Item**)malloc(sizeof(Item*)* numItems);
    itemPath=(char**)malloc(sizeof(char*)*ARRAY_SIZE_NAME);
    for(int i=0;i<numItems;i++){
        itemPath[i]=(char*)malloc(sizeof(char)*ARRAY_SIZE_NAME);
    }
    //read Items
    for(int i=0;i<numItems;i++){
       fscanf(file,"%s", itemPath[i]);
       items[i]=readItem(itemPath[i]);
    }
    //fill
    Template* templ=(Template*)malloc(sizeof(Template));
    templ->id=id;
    templ->type=type2;
    templ->numItems=numItems;
    templ->items=items;

    //free
    for(int i=0;i<numItems;i++){
        free(itemPath[i]);
    }
    free(itemPath);

    fclose(file);
    return templ;
}
Item* readItem(char* filename)
{
    FILE* file = fopen(filename,"r");
    if(file == NULL) printf("Error al abrir archivo");
    //data;
    char* id=(char*)malloc(sizeof(char)*ARRAY_SIZE_ID);
    int type=0;
    int numIngredients=0;
    int* ingredients = (int*)malloc(sizeof(int)*ARRAY_SIZE);;
    //read;
    fscanf(file, "%s", id);
    fscanf (file, "%d", &type);
    fscanf (file, "%d", &numIngredients);
    for(int i=0;i<numIngredients;i++){
       fscanf(file, "%d", &ingredients[i]);
    }
    //fill
    Item* item=(Item*)malloc(sizeof(Item));
    item->id=id;
    item->type=type;
    item->numIngredients=numIngredients;
    item->ingredients=ingredients;
    fclose(file);
    return item;
}

int getvalues(Recipe* r, int* values)
{

    int count=0;
    int numTemplates=r->numTemplates;
    for(int i= 0;i< numTemplates;i++){
        Template* t=r->templates[i];
        int numItems=t->numItems;
        for(int j=0;j<numItems;j++){
            Item* item= t->items[j];
            int* ingredient=item->ingredients;
            if(item->type==0){
               //get value of the item
               values[count++]=ingredient[0];
            }else{
                int numIngredients =item->numIngredients;
                srand((int)time(NULL));
                int indice=0;
                if(numIngredients!=0)indice=rand() % numIngredients;
                values[count++]=ingredient[indice];
                //get value of the item
            }
        }
    }
    return count;
}
/*
    free memory
*/
void freeRecipe(Recipe *r)
{
    for(int i=0;i<r->numTemplates;i++){
        freeTemplate(r->templates[i]);
    }
    free(r->id);
    free(r);
}
void freeTemplate(Template*t)
{
    for(int i=0;i<t->numItems;i++){
        freeItem(t->items[i]);
    }
    free(t->id);
    free(t);
}
void freeItem(Item*  item)
{
    free(item->id);
    free(item->ingredients);
    free(item);
}
