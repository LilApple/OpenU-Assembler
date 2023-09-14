/** #####################################################################################################################
 * Date: 9.8.23
 * Author: Shay Linzberg 212341390
 * Purpose: The second run of prints the data.
 * It also checks for errors, the errors are described in the book as well as the code.
 * The main function called is firstRunFile and then the following algorithm is executed:
 * 0. Create .ext .ent and .ob file and print the ic dc to the top of the .ob file.
 * 1. Loop over all the lines in the .am file.
 *      1.1 If there is a symbol skip it.
 * 	1.3 If the line containes .data or .string call encodeDataLine and update the dc.
 * 		1.3.1 Encode the line as a date line according to the encoding logic described in the book.
 *		1.3.2 Call printBase64Value to print the value to the .ob file.
 *		1.3.2 Update dc according to logic.
 *		1.3.3 return to 1.
 *	1.4 If the line is .extern skip it.
 *	1.5 If the line is .entry add it to .ent file (With a check that it is in the symbol Table).
 *	1.6 Otherwise call encodeCommandLine. (see assumptions below).
 *		1.6.1 Get the opcode of the command, if it is illegal return to 1. .
 *		1.6.2 If Command type 0:
 * 			1.6.2.0 Check if its legal otherwise return to 1. .
 *			1.6.2.1	Call printBase64Value and print it to the .ob file.
 *		1.6.3 Check for extra space, return to 1. if not extra space found.
 *		1.6.4 Determine Src/Dst addressing type, search symbol Table if needed.
 * 		1.6.3 If Command type 1:
 * 			1.6.4.0 Check if its legal otherwise return to 1. 
 *			1.6.4.1	Call printBase64Value and print the head command to the .ob file.
 *			1.6.4.2	Call printBase64Value and print the dst command to the .ob file.
 *			1.6.4.3 If addressing type of one of the commands was EXTERN print it to .ext file.
 *		1.6.4 Determine Dst addressing type, search symbol Table if needed.
 * 		1.6.5 If Command type 2:
 * 			1.6.4.0 Check if its legal otherwise return to 1. 
 *			1.6.4.1	Call printBase64Value and print the head command to the .ob file.
 *			1.6.4.2	Call printBase64Value and print the src command to the .ob file.
 *			1.6.4.3	Call printBase64Value and print the dst command to the .ob file.
 *			1.6.4.4 If addressing type of one of the commands was EXTERN print it to .ext file.
 *		1.6.6 Update ic according to logic described in the book.
 *		1.6.7 return to 1.
 * 2. If .ent or .ext files are not needed remove them.
 * 3. If any error was discoverd remove all the files created (either here or in the first run).
 * Note: if an error is dicoverd .ob,.ext,.ent files that created on the second phase will not be created
 * Assumptions: 
 * 1. The command format is:
 * Command type 0 is: OPTINAL: stop or OPTINAL: rts
 * Command type 1 is: OPTINAL: jmp DST (note that a space is required between the command name and dst).
 * Command type 2 is: OPTINAL: mov SRC, DST (note that one space is required between the command name and src).
 * 2. The .ent file does not need to be sorted by value according to the book.
 * 3. Only if .extern symbol is used it is added to the .ext file.
 * 4. 
 ####################################################################################################################### */
/* ###################################################### Includes ##################################################### */
/* ##################################################################################################################### */
#ifndef SECOND_RUN_H
#define SECOND_RUN_H
#include "lib.h"
#include "constants.h"
#include "table.h"
/* ##################################################################################################################### */
/* ##################################################### Functions ##################################################### */
/* ##################################################################################################################### */
/**
 * @brief The second run of the file, where .ob, .ext, .ent are created.
 * @param src .am file.
 * @param currentFileName the current name of the file inorder to create .ob, .ext and .ent. 
 * @param symbolTable The symbol table that continas all the symbols and their addresses.
 * @param ic current ic.
 * @param dc current dc.
 */
void secondRunFile(FILE* src, char *currentFileName, HashTable *symbolTable, int *ic, int *dc, bool *fail);


/** 
 * @brief Gets a 12 bit command and prints the 2 base64 encoded chars to the .obj file.
 * @param dst the .obj file.
 * @param com the 12 bit command can be head/imm/reg/data command.
 */
void printBase64Value(FILE *dst, command com);

/**
 * @brief Encode the data line into 12 bit command and print it via printBase64Value.
 * @param line the line to encode.
 * @param src .am file.
 * @param dst .ob file.
 * @param dc current dc which will be updated.
 * @param type The type of the command which can be either STR or DATA.
 * @param fail The fail bool used for finding errors, will be set to TRUE if errors were found.
 * @param currentLineIndex the number of the current line in .am file, used for errors.
 */
void encodeDataLine(char *line, FILE* src, FILE* dst, int *dc, dataType type, bool *fail, int currentLineIndex);

/**
 * @brief Encode the command line into 12 bit command and print it via printBase64Value.
 * @param line the line to encode.
 * @param src .am file.
 * @param dst .ob file.
 * @param ic current ic which will be updated.
 * @param fail The fail bool used for finding errors, will be set to TRUE if errors were found.
 * @param symbolTable The symbol table for searching symbol names, used for error checking.
 * @param ext The ext file which will be used for external symbol printing.
 * @param extFound A boolean which will be set to TRUE if we have found a external symbol.
 * @param currentLineNumber the number of the current line in .am file, used for errors.
 */
void encodeCommandLine(char *line, FILE* src, FILE* dst, int *ic, bool *fail, HashTable *symbolTable, FILE* ext, bool *extFound, int currentLineNumber);

#endif
