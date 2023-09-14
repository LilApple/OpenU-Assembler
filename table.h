/** #####################################################################################################################
 * Date: 3.6.23
 * Author: Shay Linzberg 212341390
 * Purpose: table.h contains the hashTable implementation used to store macros and symbols.
 * Assumptions: None.
 ####################################################################################################################### */
/* ###################################################### Includes ##################################################### */
/* ##################################################################################################################### */
#ifndef TABLE_H
#define TABLE_H
#include "constants.h"
#include "utils.h"

/* ##################################################################################################################### */
/* ##################################################### Defnitions #################################################### */
/* ##################################################################################################################### */

/**
 * @brief HashItemType defines the two item types that the hash table can hold which are MACRO and SYMBOL.
 * You can find more information in the HashTable/HashNode structs.
 */
typedef enum HashItemType {
	NONE,
	MACRO,
	SYMBOL
} HashItemType;

/**
 * @brief MacroData holds relevant information for a macro, in which line it starts and in which it ends.
 */
typedef struct MacroData {
	int start; /* Macro start line, the line where mcro is defined. */
	int end; /* Macro end line, the line where mcroend is defined. */
} MacroData;

/**
 * @brief SymbolData holds relevant information for a label, meaning in which line is it at.
 */
typedef struct SymbolData{
	unsigned int value;
	symbolType type;
} SymbolData;

/**
 * @brief The following struct defines a row (node) of a Hash Table, The Node can hold both macros and labels (only one at a time).
 * TBD.
 */
typedef struct HashNode {
	char *key; /* Name of the macro/label. */
	union /* Data of the macro/label. */
	{
		SymbolData symbol;
		MacroData macro;
	} value;
	struct HashNode *next; /* Pointer to the next macro. */
} HashNode;

/**
 * @brief The following defines the hash table proper which can hold either macros/labels.
 * The size of the Hash Table is defined in the constants.h file.
 */
typedef struct HashTable {
	HashNode* rows[HASH_TABLE_SIZE]; /* A hash table with HASH_TABLE_SIZE Rows.*/
} HashTable;

/* ##################################################################################################################### */
/* ##################################################### Functions ##################################################### */
/* ##################################################################################################################### */

/**
 * @brief Create a Hash Table, with all rows set to NULL.
 * @return Pointer to the Hash Table.
 */
HashTable* createHashTable();

/**
 * @brief Hashes a key
 * @param key 
 * @return unsigned int which represents the row number (0 to Table Size). 
 */
unsigned int hash(char* key);

/**
 * @brief Adds a Macro/Symbol to the hash table, the row number is decided by the hash function.
 * @param table The Table to add the item to.
 * @param key The name of the item.
 * @param type The item type which can be MACRO or SYMBOL.
 * @param data A void pointer which should point to MacroData or SymbolData.
 */
void addItem(HashTable *table, char *key, HashItemType type, void *data);

/**
 * @brief Search the table for a key and returns the Node coressponding to the key.
 * @param table The Table to search.
 * @param key The name of the Macro/Symbol to search.
 * @return HashNode* which represents the node.
 */
HashNode* search(HashTable *table, char *key);

/**
 * @brief Prints a hash table. Used for debug propuses only.
 * @param table The Table to print. 
 * @param type The item type which can be MACRO or SYMBOL.
 */
void printHashTable(HashTable *table, HashItemType type);

/**
 * @brief Frees the memory of the hash table.
 * @param table The Table to free.
 */
void freeHashTable(HashTable *table);

#endif

