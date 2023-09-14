/** #####################################################################################################################
 * Date: 8.9.23
 * Author: Shay Linzberg 212341390
 * Purpose: table.c contians all the implemntaions of the table.h functions.
 * Assumptions: Please read the preprocessor.h, firstrun.h and secoandrun.h for assumtions.
 ####################################################################################################################### */
/* ###################################################### Includes ##################################################### */
/* ##################################################################################################################### */
#include <string.h>
#include "table.h"
/* ##################################################################################################################### */
/* ################################################## Implementations ################################################## */
/* ##################################################################################################################### */
HashTable* createHashTable() {
    int i;
    HashTable* table = (HashTable*)malloc(sizeof(HashTable));
    /* Handle memory allocation failure. */
    if (table == NULL) {
        printf("ERROR: couldn't allocate memory for hashtable");
        return NULL;
    } 
    
    /* Initalize the table rows. */
    for (i = 0; i < HASH_TABLE_SIZE; i++) {
        table->rows[i] = NULL;
    }
    return table;
}

unsigned int hash(char* key) {
    unsigned int hashValue = 5381; /* A prime number used for better distribution. */
    
    /* The main of the hash function, will end when key will point to NULL. */
    while (*key) {
        hashValue = ((hashValue << 5) + hashValue) + *key++;
    }
    
    /* Return the hash value as a remainder of the table size. */
    return hashValue % HASH_TABLE_SIZE; 
}

HashNode* search(HashTable *table ,char* key) {
    unsigned int itemIndex = hash(key);
    HashNode *current = table->rows[itemIndex];
    while (current != NULL) {
        if(strcmp(current->key, key) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void addItem(HashTable *table, char *key, HashItemType type, void *data) {
    unsigned int itemIndex = hash(key);
    HashNode *new = (HashNode *)malloc(sizeof(HashNode));

    /*Checking in case the memory allocation failed. */
    if(new == NULL) {
        printf("ERROR: couldn't allocate memory for node. \n");
        return;
    }

    /* Duplicating the string.*/
    new->key = (char *)strdup(key);

    /* Adding new data based on the data type. */
    switch (type)
    {
        case NONE:
            break;
        case MACRO:
            new->value.macro = *(MacroData *)data;
            break;

        case SYMBOL:
            new->value.symbol = *(SymbolData *)data;
            break;
    }

    /* Updating the table. */
    new->next = table->rows[itemIndex];
    table->rows[itemIndex] = new;
}

void printHashTable(HashTable *table, HashItemType type) {
    int i;
    for (i = 0; i < HASH_TABLE_SIZE; i++) {
        HashNode* current = table->rows[i];
        printf("Row %d: ", i);
        while (current != NULL)
        {

            /* Printing the data by type. */
            switch (type)
            {
            case NONE:
                break;

            case MACRO:
                printf("key: %s start: %d, end: %d -> ", current->key, current->value.macro.start, current->value.macro.end);
                break;

            case SYMBOL:
                printf("key: %s type: %d, value: %d ->", current->key, current->value.symbol.type, current->value.symbol.value);
                break;
            }
            current = current->next;
        }
		printf("...\n");
    }
}

void freeHashTable(HashTable *table) {
    int i;
    for (i = 0; i < HASH_TABLE_SIZE; i++) {
        HashNode* current = table->rows[i];
        while (current != NULL)
        {
            HashNode* temp = current; /* Create a temporary node.*/
            current = current->next; /* Move current forward.*/
            
            /* Free the data pointed by temp.*/
            free(temp->key); 
            free(temp);
        }
    }
}
