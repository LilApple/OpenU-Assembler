/** #####################################################################################################################
 * Date: 6.8.23
 * Author: Shay Linzberg 212341390
 * Purpose: The assembler.c, is the main entry of the program.
 * The program is an assembler written for an imaginary assembly language.
 * For more information, you can view the read.md file.
 * Command syntax: assembler filename (can be more than 1 file), PLEASE NOTE THAT File is at the same folder as assembler.
 * Output: filename.am, filename.ob, filename.ent, filename.ext (.ext and .ent only if needed) 
 * Assumptions: See other files.
 * IMPORTANT: Please use 4 Tab width to view this code with gedit, unfortunetly I could not fix the look of the code in Tab width 8.
 * Also errors are printed to the screen as well as a message that is used to denote that that .ob and other files are created.
 ####################################################################################################################### */
/* ###################################################### Includes ##################################################### */
/* ##################################################################################################################### */
#include <string.h>
#include "lib.h"
#include "constants.h"
#include "table.h"
#include "preprocessor.h"
#include "first_run.h"
#include "second_run.h"
/* ##################################################################################################################### */
/* ##################################################### Defnitions #################################################### */
/* ##################################################################################################################### */

/**
 * Takes an .asm filename and outputs the following files: .am .object .extern .entry
 * May not output .extern and .entry files, only if needed. 
 * Fails in case the fail is inacsasabile. 
 */
static void process_file(char *filename);

/**
 * Program main entry point, Program syntax is described in the readme file and at the begginng of this file.
 */
int main(int argc, char *argv[]) {
	int i; /* Current file index. */

	for(i = 1; i < argc; ++i) {
		process_file(argv[i]);
	}
	return 0;
}

static void process_file(char *filename) {
	FILE *src, *dst; /* Source file dst file.*/
	HashTable *macroTable, *symbolTable; /* macros and symbol table. macro table used in preprocssor phase, symbol in other phases.*/
	int ic = 0, dc = 0; /* IC and DC of the file set to 0. */
	char *currentFile; /* The current name of the file. */
	bool fail = FALSE; /* Bool used to represent if any errors were encounterd. */
	
	/* Duplicate the filename, since we don't want to modify it. */
	currentFile = (char *)strdup(filename);
	/* Try to open the file, skip in case it is inacsseable. */
	strcat(currentFile, ".as");
	src = fopen(currentFile, "r");
	if (src == NULL) {
		printf("Error: couldn't access %s, skipping it.\n", filename);
		return;
	}
	
	/* Begin preprocessor phase. */
	
	/* Initilaize the macro table.*/	
	macroTable = createHashTable();

	/* Create the .am file. */
	currentFile[strlen(currentFile) - 1] = 'm'; /* Change filename to .am instead of .as. */
	dst = fopen(currentFile, "w+");
	if (dst == NULL) {
		printf("Error: couldn't open %s, skipping it.\n", filename);
		return;
	}
	
	/* Preprocess the file. */
	fail = preprocessFile(src, dst, macroTable);
	/* Uncheck the following line if you want to see the macro table. */
	/* printHashTable(macroTable, MACRO); */
	
	fclose(dst);
	fclose(src);

	if (fail) {
	    printf("ERROR: preprocessor found errors skipping %s (.am not created).\n", filename);
		freeHashTable(macroTable);
		remove(currentFile);
		return;
	}
	/* Begin first run. */
	symbolTable = createHashTable();
	src = fopen(currentFile, "r");	
	fail = firstRunFile(src, symbolTable, &ic, &dc, filename);
	/* Uncheck the following line if you want to see the symbol table. */
	/* printHashTable(symbolTable, SYMBOL); */
	/* begin second run. */
	secondRunFile(src, filename, symbolTable, &ic, &dc, &fail);

	if (fail) {
	    printf("ERROR: first or second phase found errors skipping %s (.ob/.ent/.ext not created).\n", filename);
		freeHashTable(macroTable);
		freeHashTable(symbolTable);
		return;
	}
	printf("File: %s is good, .ob and .ext and .ent are created (if needed).\n", filename);
	/* Memory freeing phase. */
	freeHashTable(macroTable);
	freeHashTable(symbolTable);
	fclose(src);
	free(currentFile);
}

