/* This code is copied directly from https://www.tutorialspoint.com/data_structures_algorithms/hash_table_program_in_c.htm
 * This is my first time coming across a hash table, so I just tried to find and already made one instead of making it one myself
 * This is because the main focus of this assignment is not to create a hash table, but to implement the grammar and make a symbol table
 * I made quite some changes to the original code though
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// This macro defines the size of the hash table, and also plays a role in the hash function
#define SIZE 31

// This is the actual structure of the item
typedef struct Symbol {
   char *ident;
   char *type;
   char *scope;
   int offset;
} Symbol;

// The first one just creates a hashArray of size SIZE, the rest are used as temp items
Symbol *dupItem;
Symbol *dummyItem;
Symbol *item;

Symbol **createTable();
void freeTable(Symbol **hashArray);
unsigned oat_hash(void *ident);
Symbol *lookupSymbol(Symbol **hashArray, char *ident);
int insertSymbol(Symbol **hashArray, char *ident, char *type, char *scope, int offset);
Symbol *deleteSymbol(Symbol **hashArray, Symbol *item);
void displayTable(Symbol **hashArray);
void displaySymbol(Symbol **hashArray, Symbol *item);


// This function creates, mallocs, and initializes a hash table
Symbol **createTable()
{
   	dummyItem = (Symbol *) malloc(sizeof(Symbol));
   	dummyItem->type = "\0";  
   	dummyItem->ident = "\0";
   	dummyItem->scope = "\0";
    dummyItem->offset = 0;

   	dupItem = (Symbol *) malloc(sizeof(Symbol));

	int i;
	Symbol **hashArray;
	hashArray = (Symbol **) malloc(sizeof(Symbol) * SIZE);

	for (i = 0; i < SIZE; i++)
		hashArray[i] = NULL;

	return hashArray;
}

// This function frees the memory of a table
void freeTable(Symbol **hashArray)
{
	int i;
	for (i = 0; i < SIZE; i++)
		free(hashArray[i]);

	free(dupItem);
	free(dummyItem);
	free(item);
}

/* This is the once-in-a-time has function I took from this website: http://eternallyconfuzzled.com/tuts/algorithms/jsw_tut_hashing.aspx
 * The hash value was pretty big, so I just made it return the modulo of the macro SIZE 
 */
unsigned oat_hash(void *ident)
{
    unsigned char *p = ident;
    unsigned h = 0;
    int i;
    int len = strlen(ident);

    for (i = 0; i < len; i++)
    {
        h += p[i];
        h += (h << 10);
        h ^= (h >> 6);
    }

    h += (h << 3);
    h ^= (h >> 11);
    h += (h << 15);

    return h % SIZE;
}

// This is the lookup function
Symbol *lookupSymbol(Symbol **hashArray, char *ident) 
{
   //get the hash 
   int hashIndex = oat_hash(ident);  
	
   //move in array until an empty 
   while(hashArray[hashIndex] != NULL) 
   {
   	  // Returns something if the ident is the same
      if( !strcmp(hashArray[hashIndex]->ident, ident) )
      {
         return hashArray[hashIndex];
      }

      //go to next cell
      ++hashIndex;
		
      //wrap around the table
      hashIndex %= SIZE;
   }        
	
   return NULL;        
}

// This is the insert function, need to include a lookup call to make sure the spot is not taken
int insertSymbol(Symbol **hashArray, char *ident, char *type, char *scope, int offset) 
{

   dupItem = lookupSymbol(hashArray, ident);

   if (dupItem != NULL)
      {
      	printf("Error: identifier [%s] of type [%s] in scope [%s] was already declared\n", ident, type, scope);
   	  	return -1;
      }

   //get the hash 
   int hashIndex = oat_hash(ident);

   Symbol *item = (Symbol *) malloc(sizeof(Symbol));
   item->type = type;  
   item->ident = ident;
   item->scope = scope;
   item->offset = offset;

   //move in array until an empty or deleted cell
   while(hashArray[hashIndex] != NULL && (strcmp(hashArray[hashIndex]->ident, "\0") != 0)) {
      //go to next cell
      ++hashIndex;
		
      //wrap around the table
      hashIndex %= SIZE;
   }
	
   hashArray[hashIndex] = item;
   return 0;
}

// This is the delete function, it just sets everything to -1
Symbol *deleteSymbol(Symbol **hashArray, Symbol *item) 
{
   char *ident = item->ident;

   //get the hash 
   int hashIndex = oat_hash(ident);

   //move in array until an empty
   while(hashArray[hashIndex] != NULL) {
	
      if(hashArray[hashIndex]->ident == ident) {
         Symbol *temp = hashArray[hashIndex]; 
			
         //assign a dummy item at deleted position
         hashArray[hashIndex] = dummyItem; 
         return temp;
      }
		
      //go to next cell
      ++hashIndex;
		
      //wrap around the table
      hashIndex %= SIZE;
   }      
	
   return NULL;        
}

// This function displays all the values in the table
void displayTable(Symbol **hashArray) 
{
   int i = 0;

   for(i = 0; i<SIZE; i++) {
	    printf("[%d] ", i);
      if(hashArray[i] != NULL)
         printf("Name: [%s], Type: [%s], Scope: [%s], Offset: [%d] Hash Value: [%d]\n",hashArray[i]->ident, hashArray[i]->type, hashArray[i]->scope, hashArray[i]->offset, oat_hash(hashArray[i]->ident));
      else
         printf(" ~~ \n");
   }
}

// This function displays a single value
void displaySymbol(Symbol **hashArray, Symbol *item)
{
	char *ident = item->ident;

	//get the hash 
    int hashIndex = oat_hash(ident);

    printf("Name: [%s], Type: [%s], Scope: [%s], Offset: [%d] is the content for %s\n",hashArray[hashIndex]->ident, hashArray[hashIndex]->type, hashArray[hashIndex]->scope, hashArray[hashIndex]->offset, hashArray[hashIndex]->ident);
}