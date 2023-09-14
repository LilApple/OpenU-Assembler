/** #####################################################################################################################
 * Date: 9.8.23
 * Author: Shay Linzberg 212341390
 * Purpose: The preprocessor.h file defines the preprocess phase of a file, after this phase the .am file is created.
 * The preprocessor processes one file at a time, and checks the following: 
 * 1. The macro name is legal.
 * 2. There are no extra chars in the mcro and endmcro lines.
 * 3. There are no two mcro defnition with the same name.
 * All of checks 1-3 are done in the fillMacroTableFromFile funtion.
 * If the macro name or defnition are illegal .am creation is aborted.
 * Assumptions: The following is assumed when we preprocess a file, which come from the fourm and the book:
 * 1. There is no macro inside a macro.
 * 2. Every mcro defnition has an endmcro.
 * 3. The macro definition will always be before the macro call.
 * 4. The macro defintion doesn't contain any errors.
 * 5. The macro call doesn't contain a label (according to Ester from the fourm its ok to assume that).
 * 6. The mcro defintion doesn't have a a label in any of its rows (according to the fourm). 
 * 7. macro call doesn't contain extra chars ex: mov m1 (when m1 is mcro) will not show up.
 * 7 is according to the fourm as well (I think it was michal who said its ok to assume that).
 * If 7 does show up it will be reported as an error inside the first run.
 ####################################################################################################################### */
/* ###################################################### Includes ##################################################### */
/* ##################################################################################################################### */
#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H
#include "constants.h"
#include "table.h"


/* ##################################################################################################################### */
/* ##################################################### Functions ##################################################### */
/* ##################################################################################################################### */

/**
 * @brief Parses the all the macros using data stored in the macros table and the src file. Uses the macro table to get 
 * the number of line in which the macros starts and its name, and uses the source file for the line text lines.
 * The data is parsed to the destination file.
 * @param macrosTable The macros table which contains all the macros data.
 * @param src The source file which contains the macros body defnitions (.as).
 * @param dst The destenation file (.am).
 */
bool preprocessFile(FILE* src, FILE* dst, HashTable *macrosTable);

/**
 * @brief Parses a single macro to the destination file, using data from the macro node and the source file.
 * The information is parsed to the destination file.
 * @param macro The macro information which cotains the line start of the macro defination and the line end.
 * @param src 
 * @param dst 
 */
void parseMacro(FILE* src, FILE* dst, HashNode *macro, int loopbackLine, char* key, char* stringToAdd);
#endif
