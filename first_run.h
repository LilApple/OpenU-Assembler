/** #####################################################################################################################
 * Date: 9.8.13
 * Author: Shay Linzberg 212341390
 * Purpose: The first run of the files only fills the symbol table which is the hash table.
 * It also check for errors, the errors are described in the book as well as the code.
 * The main function called is firstRunFile and then the following algorithm is executed:
 * 1. It loop over all the lines in the .am file.
 *      1.1 If there is a symbol skip it and add it to the hash table.
 *	1.2 If the line does not have a symbol and is empty skip it.
 * 	1.3 If the line containes .data or .string call updateDC and update the dc.
 * 		1.3.1 Encode the line as a date line according to the encoding logic described in the book.
 *		1.3.2 Update dc according to logic.
 *		1.3.3 return to 1.
 *	1.4 If the line is .extern add it to the symbol table with value 0.
 *	1.5 If the line is .entry skip it and retrun to 1..
 *	1.6 Otherwise call updateIC.
 *		1.6.1 Encode the line as a command line according to the encoding logic described in the book.
 *		1.6.2 Update ic according to logic.
 *		1.6.3 return to 1.
 * 2. For every symbol of type DATA in the symbol table update the value to be current Value + ic.
 * Note: if an error is dicoverd .ob,.ext,.ent files that created on the second phase will not be created.
 * Assumptions: None.
 ####################################################################################################################### */
/* ###################################################### Includes ##################################################### */
/* ##################################################################################################################### */
#ifndef FIRST_RUN_H
#define FIRST_RUN_H
#include "lib.h"
#include "constants.h"
#include "table.h"
/* ##################################################################################################################### */
/* ##################################################### Functions ##################################################### */
/* ##################################################################################################################### */

/**
 * @brief The first file run gets a .am file and an empty symbol table and fills it with symbols that have a name and a value.
 * There are 3 symbols type in the table: 1. CODE type code is 0. , 2. DATA type code is 1. 3. EXTERN type code is 2.
 * The value of a CODE symbol is ic, the value of a DATA symbol is dc + max_ic, and the value of an EXTERN is 0.
 * The values of ic and dc are updated by the functions declared below.
 * @param src .am file.
 * @param symbolTable Empty symbol Table.
 * @param ic current IC initaliy set to 0.
 * @param dc current DC initaliy set to 0.
 * @param filename The name of the current file used for error prints.
 */
bool firstRunFile(FILE* src, HashTable *symbolTable, int *ic, int *dc, char *filename);

/**
 * @brief If the line was discoverd to contain a .data or .string it is a Line of type DATA and dc is updated accordingly.
 * If it is a string then: we skip the .string and loop check for the first " and then begin looping over the string until ".
 * We incremente the dc by 1 for every char in the string and then increment another time for the null terminator.
 * If it is a data then: we skip the .data and lopp check every number and a comma, we increment dc by one.
 * @param line The current line.
 * @param src The source file.
 * @param dc Current dc that will be updated.
 * @param type The type of the line.
 * @param fail A flag that will be set to TRUE if an error was found on the line. 
 * @param currentLineIndex The current line index is the number of the current line, used for error prints.
 */
void updateDC(char *line, FILE* src, int *dc, dataType type, bool *fail, int currentLineIndex);

/**
 * @brief If the line does not contain a .data or .string it is a line of type CODE and ic is updated accordingly.
 * @param line The current line
 * @param src The source file.
 * @param ic Current ic that will be updated.
 * @param fail A flag that will be set to TRUE if an error was found on the line. 
 * @param currentLineIndex The current line index is the number of the current line, used for error prints.
 */
void updateIC(char *line, FILE* src, int *ic, bool *fail, int currentLineIndex);

#endif
