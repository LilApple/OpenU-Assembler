/** #####################################################################################################################
 * Date: 9.8.23
 * Author: Shay Linzberg 212341390
 * Purpose: Utils.h continas defnition for helper functions that are used throughout the program.
 * You can see the the documention of them below.
 * Assumptions: None.
 ####################################################################################################################### */
/* ###################################################### Includes ##################################################### */
/* ##################################################################################################################### */
#ifndef UTILS_H
#define UTILS_H
#include "lib.h"
#include "constants.h"

/* ##################################################################################################################### */
/* ##################################################### Functions ##################################################### */
/* ##################################################################################################################### */

/**
 * @brief An helper function used to duplicate a string.
 * @param The string to copy.
 */
char *strdup(const char *s);

/**
 * @brief An helper function that removes all the whitespaces from the start of the string.
 * @param line The string to remove the first white spaces of.
 */
void removePrecedingSpaces(char *line);

/**
 * @brief An helper function that removes all the whitespaces from the end of the string.
 * @param line The string to remove the last white spaces of.
 */
void removeBackSpaces(char *line);

/**
 * @brief An helper function that gets a string until a word.
 * Example: str = "hello world", word = "world" returns "hello "
 * @param str The string to get the word of.
 * @param word The word.
 * @return char*  The str until the
 */
char* getStringUntilWord(char* str, char* word);

/**
 * @brief Helper function used to extract a macro name from a given line. 
 * @param macroName Variable used to store the extracted name.
 * @param macroNamePtr The line to extract the macro name from.
 */
void extractMacroName(char *macroName, char *macroNamePtr);

/**
 * @brief Check that the mcro name does not start with a digit or is a command name.
 * @param macroName the macroName to check.
 * @return int return -1 if macro name is illegal, 0 otherwise.
 */
int checkMacroName(char *macroName);

/**
 * @brief Helper function used to extract a symbol name from a given line. 
 * @param line The line to extract the symbol name from.
 * @param symbolName Variable used to store the extracted name.
 */
void extractSymbolName(char *line, char *symbolName);

/**
 * @brief Helper function that skips the line until first apperance of char c.
 * Example: if line is "hello \tbye" and char is '\t' line will be 'bye'.
 * @param line The line to change.
 * @param c The char to find the first apperance of.
 */
void skipUntilChar(char *line, char c);

/**
 * @brief Helper function that gets the Op Index from a given line
 * Example line is: "mov @r1, LENGTH" will return 0.
 * @param line .am Line file without symbol and precceding white spaces.
 * @return int The opcode of the functions
 */
int getOpIndex(char *line);

/**
 * @brief Helper function that gets a line with an operation peresnt and skips the op.
 * Example: line is mov @r1,@r2, line after skip op: @r1,@r2.
 * @param line The line to skip the operation of.
 * @param opIndex The index of the operation.
 */
void skipOperation(char *line, int opIndex);

/**
 * @brief Retruns the value of the register if line is legal.
 * @param line The register value to return
 * @return int -1 if name is illegal, registerIndex otherwise.
 */
int getRegisterValue(char *line);

/**
 * @brief Helper function that gets a number from the line, the line should start with a number.
 * Example line is: -5 returns -5.
 * @param line The line that starts with a number.
 * @return int -9999 if illegal number, the number otherwise.
 */
int getNumber(char *line);

#endif 
