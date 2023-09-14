/** #####################################################################################################################
 * Date: 9.8.23
 * Author: Shay Linzberg 212341390
 * Purpose: preprocessor.c contains the implemntaion of the algorithm described preprocessor.h for the preprocess.
 * Assumptions: See preprocessor.h.
 ####################################################################################################################### */
/* ###################################################### Includes ##################################################### */
/* ##################################################################################################################### */
#include "preprocessor.h"
#include <string.h>

/* ##################################################################################################################### */
/* ################################################## Implementations ################################################## */
/* ##################################################################################################################### */
bool preprocessFile(FILE* src, FILE* dst, HashTable *macrosTable) {
    char line[MAX_LINE_LENGTH], *lineCopy;
    char *macroNamePtr, *endNamePtr, *stringToAdd;
	char macroName[MAX_MACRO_NAME_LENGTH]; /* represents the name of a macro. */
	int macroCheckRes, currentLineNumber = 0;
    HashNode* macro = NULL;
	MacroData macroData;
	int currentLineOffset = 0;
    bool insideMacro = FALSE, fail = FALSE;
	
    while (fgets(line, sizeof(line),src) != NULL)
    {
		currentLineNumber++;
        macroNamePtr = strstr(line, "mcro");
        endNamePtr = strstr(line, "endmcro");

		/* Create a line copy and clean it.*/
		lineCopy = (char *)strdup(line);
		removePrecedingSpaces(lineCopy);
		removeBackSpaces(lineCopy);

		/* Check if a line is a comment line. */
		if (line[0] == ';') {
			currentLineOffset = ftell(src);
			continue;
		}
		
		/* Check if line is empty line. */
		if (strlen(lineCopy) < 1) {
			continue;
		}
		
		/* Check if linecopy has only endmcro. */
		if (endNamePtr != NULL && strlen(lineCopy) > 7) {
			printf("ERROR: extra chars after endmcro.\n");
			printf("At line %d: %s.\n", currentLineNumber, line); 
			fail = TRUE;
			continue;
		}
		

		
		/* if a line contains the mcro defnition. */
		if (macroNamePtr != NULL && endNamePtr == NULL) {
			/* Extract the mcro name and set the macroData.start.*/
			extractMacroName(macroName, macroNamePtr);
			
			if (strlen(macroName) > MAX_MACRO_NAME_LENGTH) {
				printf("ERROR: mcro name too long (should be less than 32 chars.).\n");
			    printf("At line %d: %s.\n", currentLineNumber, line); 
				fail = TRUE;
				continue;
			}
			/* Check if macro name is legal. */	
			macroCheckRes = checkMacroName(macroName);
			if (macroCheckRes == -1) {
				printf("ERROR: mcro name illegal.\n");
			    printf("At line %d: %s.\n", currentLineNumber, line); 
				fail = TRUE;
				continue;
			}

        	/* Check for extra characters before "mcro". */
			/* Skip Macro */
			macroNamePtr += 4;
			removePrecedingSpaces(macroNamePtr);
			removeBackSpaces(macroNamePtr);
			removeBackSpaces(macroName);
			if (strcmp(macroNamePtr, macroName) != 0 || strncmp(lineCopy, "mcro", 4) != 0) {
				printf("ERROR: extra chars in macro defnition.\n");
			    printf("At line %d: %s.\n", currentLineNumber, lineCopy); 
				fail = TRUE;
				continue;
			} else {
				macroData.start = currentLineOffset; 
				insideMacro = TRUE;
				currentLineOffset = ftell(src);
				continue;
			}				
		}
		
		/* if a line contains the endmcro defnition, change inside macro flag, and add it to the table. */
        if (endNamePtr != NULL) {
			/* Add the macro to the table. */
			macroData.end = currentLineOffset; /* set the end line to the current index. */
			if (search(macrosTable,macroName) != NULL) {
				printf("ERROR: mcro Name already in use.\n");
			    printf("At line %d: %s.\n", currentLineNumber, lineCopy); 
				fail = TRUE;
			}
            addItem(macrosTable, macroName, MACRO, &macroData); /* adding the macro to the macro table. */
            
			insideMacro = FALSE;
			currentLineOffset = ftell(src);
			continue;
        }

		/* currently inside a macro defnition, skip it. */
		if (insideMacro) {
			currentLineOffset = ftell(src);
			continue;
		}
		
		/* If a line is not part of a macro defnition.*/
		if ((macroNamePtr == NULL && endNamePtr == NULL) && !insideMacro) {
			macro = search(macrosTable, lineCopy);
			currentLineOffset = ftell(src);
			
			/* If a line contains a macro name, parse the macro. */
			if (macro != NULL) {
				stringToAdd = getStringUntilWord(line, macro->key);
				parseMacro(src, dst, macro, currentLineOffset, lineCopy, stringToAdd);
				macro = NULL;
			} else {
				fputs(line, dst);
			}
		}
	
    }
	return fail;
}

void parseMacro(FILE* src, FILE* dst, HashNode *macro, int loopbackLine, char* key, char* stringToAdd) {
	/* Set the currentLine and endLine to macro.start and macro.end repsectivly. */    
	int currentLine = macro->value.macro.start; /* macro.start is the offset in bytes where the mcro is defined. */
	int endLine = macro->value.macro.end; /* macro.end is the offset in bytes where the endmcro is defined. */
    char line[MAX_LINE_LENGTH], *lineCopy; /* The current line a pointer to a copy of the line. */
	char *macroNamePtr;	/* A pointer to a line, is NULL if the line doesn't contain the word mcro. */
	
	/* Jump to the mcro start in the src file (.as). */
	fseek(src, currentLine, SEEK_SET);
	/* Loop over the file until there aren't any lines left and the current line offset is less than the end line. */
    while (currentLine < endLine && fgets(line, MAX_LINE_LENGTH, src) != NULL) {
		macroNamePtr = strstr(line, "mcro");
		removePrecedingSpaces(line); /* Removes all white spaces from the start of the line. */
		lineCopy = (char *)strdup(line); /* Copy the line to the line copy pointer. */
		removeBackSpaces(lineCopy); /* Removes all white spaces from the end of the line. */
		
		/* If a line does not contain the word mcro and the key matches the line, print the line.*/
		if (macroNamePtr == NULL && strcmp(lineCopy, key) != 0) {
			/* Put a prefix to each line if needed. */			
			if (stringToAdd != NULL) {
				fputs(stringToAdd, dst);
			}
			/* Print the rest of the line. */
			fputs(line, dst);
		}
		/* Advance the current line. */
		currentLine = ftell(src);
    }
	/* Free the line copy and begin loop to the last position of the file. */
	free(lineCopy);
	fseek(src, loopbackLine, SEEK_SET);
}
