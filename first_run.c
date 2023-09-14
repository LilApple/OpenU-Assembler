/** #####################################################################################################################
 * Date: -
 * Author: Shay Linzberg 212341390
 * Purpose: 
 * Assumptions: None.
 ####################################################################################################################### */
/* ###################################################### Includes ##################################################### */
/* ##################################################################################################################### */
#include <string.h>
#include "first_run.h"
#include "utils.h"


/* ##################################################################################################################### */
/* ################################################## Implementations ################################################## */
/* ##################################################################################################################### */

bool firstRunFile(FILE* src, HashTable *symbolTable, int *ic, int *dc, char *filename) {
	bool fail = FALSE; /* A flag that used for error checking, will be set  TRUE if error was encounterd.*/
    int currentLineIndex = 0, i, lineLength; /* The currentLineIndex is the number of the line, line length is the length of the line.*/
    char line[MAX_LINE_LENGTH], symbolName[MAX_SYMBOL_NAME_LENGTH]; /* The current line and symbol (if present). */
    char *typePtr; /* A pointer that is used to determine the data type of the line.*/
    SymbolData symbolData; /* Symbol data that will be added to the symbol table. */
    HashNode *symbolNode; /* A pointer to a symbol node used to check if item is already present in table. */
    dataType type; /* The type of data will be set to either: DATA_TYPE, STR_TYPE, ENTRY_TYPE, EXTERN_TYPE or ERROR_TYPE. */
    /* The type is set to ERROR_TYPE if no .extern/.entry/.data/.string are present. */

    while (fgets(line, sizeof(line),src) != NULL)
    {
		/* Determine the type of line, if a line has .data/.string/.extern/.entry it is a data line.*/
		currentLineIndex++;
        type = ERROR_TYPE;
        typePtr = strstr(line, ".data");

        if (typePtr != NULL && type == ERROR_TYPE) {
            type = DATA_TYPE;
        } else if (typePtr != NULL) {
			printf("ERROR: Extra Var declared when not needed.\n"); 
			printf("At line %d: %s.\n", currentLineIndex, line); 
			fail = TRUE;
			continue;
        }

        typePtr = strstr(line, ".string");
        if (typePtr != NULL && type == ERROR_TYPE) {
            type = STR_TYPE;
        } else if (typePtr != NULL) {
        	printf("ERROR: Extra Var declared when not needed.\n"); 
			printf("At line %d: %s.\n", currentLineIndex, line); 
			fail = TRUE;
			continue;
        }

		typePtr = strstr(line, ".extern");
        if (typePtr != NULL && type == ERROR_TYPE) {
            type = EXTERN_TYPE;
        } else if (typePtr != NULL) {
        	printf("ERROR: Extra Var declared when not needed.\n"); 
			printf("At line %d: %s.\n", currentLineIndex, line); 
			fail = TRUE;
			continue;
        }

		typePtr = strstr(line, ".entry");
        if (typePtr != NULL && type == ERROR_TYPE) {
            type = ENTRY_TYPE;
        } else if (typePtr != NULL) {
        	printf("ERROR: Extra Var declared when not needed.\n"); 
			printf("At line %d: %s.\n", currentLineIndex, line); 
			fail = TRUE;
			continue;
        }
        
        /* Clean the line by removing all white spaces from the start and the end.*/
        removePrecedingSpaces(line);
        removeBackSpaces(line);
        
        /* If it is an empty line just skip it. */
        if (strlen(line) < 1) {
			continue;
		}

        /* Check if a line contains a symbol and extract it if needed. */
        extractSymbolName(line, symbolName);
		/* Check that symbol name is legal. */
		if (isdigit(*symbolName) || strlen(symbolName) > MAX_SYMBOL_NAME_LENGTH) {
        	printf("ERROR: Illegal Symbol name.\n"); 
			printf("At line %d: %s.\n", currentLineIndex, line); 
			fail = TRUE;
			continue;
		}

        /* Check for label and add it to the symbol table if needed. */
        if (symbolName != NULL) {
            if (search(symbolTable, symbolName) != NULL) {
        		printf("ERROR: Symbol name already in use.\n"); 
				printf("At line %d: %s.\n", currentLineIndex, line); 
				fail = TRUE;
				continue;
            }

			/* Skip symbol. */
			lineLength = strlen(line);
            skipUntilChar(line, ':');

			/* Check for extra white space. */
			if (lineLength > strlen(line) && line[0] != ' ') {
        		printf("ERROR: symbol name missing whitespace.\n"); 
				printf("At line %d: %s.\n", currentLineIndex, line); 
				fail = TRUE;
				continue;
			}
			/* Remove extra white space */
			removePrecedingSpaces(line);
        	removeBackSpaces(line);

            /* If the line is empty print an error and terminate the current file creation process. */
            if (strlen(line) < 1) {
        		printf("ERROR: Empty line after symbol name.\n"); 
				printf("At line %d: %s.\n", currentLineIndex, line); 
				fail = TRUE;
				continue;
            }
        }

		/* If the line is a data line and it does not start with a . then its an error. */
		if (type != ERROR_TYPE && line[0] != '.') {
			printf("ERROR: data line after symbol does not start with a dot.\n"); 
			printf("At line %d: %s.\n", currentLineIndex, line); 
			fail = TRUE;
			continue;
		}
        
        /* If the line starts with . then it is a data line. */
        if (type != ERROR_TYPE) {
            if (strlen(symbolName) > 0 && (type == DATA_TYPE || type == STR_TYPE)) {
                symbolData.type = DATA;
                symbolData.value = IC_START_VALUE + *dc;
                addItem(symbolTable, symbolName, SYMBOL, &symbolData);
			    updateDC(line, src, dc, type, &fail, currentLineIndex); /* Encode the data line. */
            } else { 
                if (type == EXTERN_TYPE) {
                    symbolData.type = EXTERN;
                    symbolData.value = 0;
					/* Search for extern and skip it */
                    typePtr = strstr(line, ".extern");
                    typePtr += 7;
					/* Check for missing space. */
					if (*typePtr != ' ') {
       					printf("ERROR: missing space before extern.\n"); 
						printf("At line %d: %s.\n", currentLineIndex, line); 
						fail = TRUE;
						continue;
					}

					/* Clean spaces and check if symbol already in table. */
                    removePrecedingSpaces(typePtr);
                    removeBackSpaces(typePtr);
					if (search(symbolTable, typePtr) != NULL) {
        				printf("ERROR: Extern symbol already in use.\n"); 
						printf("At line %d: %s.\n", currentLineIndex, line); 
						fail = TRUE;
						continue;
					}
                    addItem(symbolTable, typePtr, SYMBOL, &symbolData);
                }
       		}
		} else {
            if (strlen(symbolName) > 0) {
                symbolData.type = CODE;
                symbolData.value = IC_START_VALUE + *ic;
                addItem(symbolTable, symbolName, SYMBOL, &symbolData);
            }
            /* Encode the command line. */
            updateIC(line, src, ic, &fail, currentLineIndex); 
		}
    }


    /* Adding the max ic to all the data symbols in the table, this inorder to seperate the data from code. */
    for (i = 0; i < HASH_TABLE_SIZE; i++) {
		symbolNode = symbolTable->rows[i];
		while (symbolNode != NULL) {
			if (symbolNode->value.symbol.value + *ic > IC_MAX_VALUE) {
        		printf("ERROR: IC + dc too big.\n"); 
				printf("At line %d: %s.\n", currentLineIndex, line); 
				fail = TRUE;
				continue;
			}
			if (symbolNode->value.symbol.type == DATA) {
                symbolNode->value.symbol.value += *ic;
			}
            symbolNode = symbolNode->next;
		}

	}
	/* Jump to the begining of the .am file. */
	fseek(src, 0, SEEK_SET);
	return fail;

}


void updateDC(char *line, FILE* src, int *dc, dataType type, bool *fail, int currentLineNumber) {
    char *paranPtr; /* A pointer that check if " is present*/
	char *lineCopy; /* A copy of the line. */
    bool neg = FALSE; /* Flag that checks if number is negative in .data, negative if TRUE. */
    int num; /* Current number used if .data*/
	
	lineCopy = (char *)strdup(line);
	removePrecedingSpaces(lineCopy);
    removeBackSpaces(lineCopy);

    /* Skip until . symbol. */
    while (*line != '.') {
        line++;
    }
    removeBackSpaces(line);

    switch (type) {
        case STR_TYPE:
            line += strlen(".string");
            removePrecedingSpaces(line);
			/* Check if string is empty after .string */
            if (strlen(line) < 1) {
                printf("ERROR: empty line after .string.\n"); 
				printf("At line %d: %s.\n", currentLineNumber, line); 
				*fail = TRUE;
				return;
            }
            /* Check and skip the first " */
            if (*line != '"') {
                printf("ERROR: Missing \" at start of string.\n"); 
				printf("At line %d: %s.\n", currentLineNumber, line); 
				*fail = TRUE;
				return;
            }
            line++; 

            paranPtr = strstr(line, "\"");
            if (paranPtr == NULL) {
                printf("ERROR: Missing \" at end of string.\n"); 
				printf("At line %d: %s.\n", currentLineNumber, line); 
				*fail = TRUE;
				return;
            }

            while (*line != '"') {
                line++;
                *dc += 1;
            }
            /* add null terminator to dc. */
            *dc += 1;            
            /* Skip second " */
            line++;
            /* Check for extra chars. */
            if (strlen(line) > 1) {
                printf("ERROR: Extra chars at the end of string.\n"); 
				printf("At line %d: %s.\n", currentLineNumber, line); 
				*fail = TRUE;
				return;
            }
            break;
        case DATA_TYPE:
            line += strlen(".data");
            removePrecedingSpaces(line);
			/* Check for empty line after .data */
            if (strlen(line) < 1) {
                printf("ERROR: Empty line after .data.\n"); 
				printf("At line %d: %s.\n", currentLineNumber, line); 
				*fail = TRUE;
				return;
            }
            /* Begin loop over the line. */
            while (*line !='\0') {
                removePrecedingSpaces(line);
                if (*line == '-') {
                    neg = TRUE;
                    line++;
                }

                if (*line == '+' && neg == FALSE) {
                    line++;
                }
				/* Check for plus after minus. */
                if (*line == '+' && neg == TRUE) {
                	printf("ERROR: plus after.\n"); 
					printf("At line %d: %s.\n", currentLineNumber, line); 
					*fail = TRUE;
					return;
                }

                if (!isdigit(*line)) {
                	printf("ERROR: char not digit.\n"); 
					printf("At line %d: %s.\n", currentLineNumber, line); 
					*fail = TRUE;
					return;
                } else {
                    /* Get the number and skip it. */
                    num = 0;
                    while (isdigit(*line)) {
                        num = num * 10 + (*line - '0');
                        line++;
                    }
                    if (neg == TRUE) {
                        num = -num;
                    }
					/* Check if number can be condesned to 12 bits. */
                    if (num > 2047 || num < -2048) {
                		printf("ERROR: Number after .data cannot be condensed to 12 bits.\n"); 
						printf("At line %d: %s.\n", currentLineNumber, line); 
						*fail = TRUE;
                        return;
                    }

                    /* Add dc and change neg. */
                    *dc += 1;
                    neg = FALSE;
                    
                    /* Skip comma if present and check for extra chars. */
                    removePrecedingSpaces(line);
                    if (*line == ',') {
                        line++;
                        if (strlen(line) < 1) {
                			printf("ERROR: Empty line after comma.\n"); 
							printf("At line %d: %s.\n", currentLineNumber, line); 
							*fail = TRUE;
                            return;
                        }
                    } else if (*line != ',' && strlen(line) > 1) { /* If comma is not present and there are extra chars. */
                		printf("ERROR: .data missing comma.\n"); 
						printf("At line %d: %s.\n", currentLineNumber, line); 
						*fail = TRUE;
                        return;
                    }
                }
            }
            break;

		case ENTRY_TYPE:
			break;
	
		case EXTERN_TYPE:
			break;

		case ERROR_TYPE:
			break;
    }


}


void updateIC(char *line, FILE* src, int *ic, bool *fail, int currentLineNumber) {
	addrType srcOp = EMPTY_ADDR, dstOp = EMPTY_ADDR; /* src and dst op for the head command.*/
    int opIndex, srcRegValue, dstRegValue, immediateSrc, immediateDst; /* Explantiation below: */
    int i = 0, lineLength; /* Line legnth that will is used for various checks. */
	char *lineCopy; /*  A copy of the line. */
    /**
     * opIndex: The index of the current operation, will be -1 if error is encountered.
     * srcRegValue/dstRegValue: src and dst registers value, not always used.
     * immediateSrc/immediateDst: address value or number, 10 bits.
     */

	/* Make a copy of the line, used for error printing.*/
	lineCopy = (char *)strdup(line);
	removePrecedingSpaces(lineCopy);
    removeBackSpaces(lineCopy);


    /* Set the op index. */
    opIndex = getOpIndex(line);
    /* If op index is unkown then the command is terminated. */
	if (opIndex == -1) {
        printf("ERROR: Command is of unknown op.\n"); 
		printf("At line %d: %s.\n", currentLineNumber, line); 
		*fail = TRUE;
        return;
	}

	/* Skip the operation name and clean the line. */
	lineLength = strlen(line);
    skipOperation(line, opIndex);
	/* Check that the line has at least one white space.*/
	if (lineLength > strlen(line) && line[0] != ' ' && strlen(line) != 0) {
        printf("ERROR: Missing  white space.\n"); 
		printf("At line %d: %s.\n", currentLineNumber, line); 
		*fail = TRUE;
		return;
	}

	
	removePrecedingSpaces(line);
    removeBackSpaces(line);

    
	/* If command is of type 0.*/
    if (opIndex == 15 || opIndex == 14) {
        /* Check for extra chars after type 0 command. */
		if (strlen(line) > 0) {
        	printf("ERROR: Extra chars at type 0 command.\n"); 
			printf("At line %d: %s.\n", currentLineNumber, line); 
			*fail = TRUE;
            return;
		} else {
			*ic += 1;
		}
		return;
    }

    /* If the line is empty after the first op skip, then no opperand is given which is an error. */
    if (strlen(line) < 1) {
        printf("ERROR: missing first operand after command.\n"); 
		printf("At line %d: %s.\n", currentLineNumber, line); 
		*fail = TRUE;
        return;
    }

    
    if (line[0] == '@') { /* If line starts with @ it is a register. */
		/* if the second char is not r then the register is not legal. */
		if (line[1] != 'r') {
        	printf("ERROR: Unknown register name.\n"); 
			printf("At line %d: %s.\n", currentLineNumber, line); 
			*fail = TRUE;
            return;
		}
        /* Check the register number and add it to both srcRegValue and dstRegValue. */
        srcRegValue = dstRegValue = getRegisterValue(line);
        if (srcRegValue == -1) {
        	printf("ERROR: Unknown register name.\n"); 
			printf("At line %d: %s.\n", currentLineNumber, line); 
			*fail = TRUE;
            return;
        }
        /* REMEMBER: if command is of type 2 dstOp will be changed. */
		srcOp = REGISTER_ADDR;
        dstOp = REGISTER_ADDR;
	} else if (isdigit(line[0]) || line[0] == '-' || line[0] == '+') {
        /* Check the number and add it to both immediateSrc and immediateDst. */
        immediateSrc = immediateDst = getNumber(line);
        if (immediateSrc < -1024 || immediateSrc > 1023) {
        	printf("ERROR: Number is inccorect.\n"); 
			printf("At line %d: %s.\n", currentLineNumber, line); 
			*fail = TRUE;
            return;
        }
        /* REMEMBER: if command is of type 2 dstOp will be changed. */
		srcOp = dstOp = IMMIDEIATE_ADDR;
    } else { /* We are not checking the symbol name here only on the second run. */
        /* REMEMBER: if command is of type 2 dstOp will be changed. */
        srcOp = dstOp = DIRECT_ADDR;
    }
	
	/* Skip until the end of the file/until the comma. */
    while (line[i] != '\0' && line[i] != ',' && !isspace(line[i])) {
        i++;
    }
    memmove(line, line + i, strlen(line + i) + 1);
   
    removePrecedingSpaces(line);
    removeBackSpaces(line);

	/* Handle command type 1. */	
	if (opIndex >= 4 && opIndex <= 13 && opIndex != 6) {
        if (strlen(line) > 0) { /* If command is of type one line should be empty after the skip. */
        	printf("ERROR: Extra chars after type 1 command.\n"); 
			printf("At line %d: %s.\n", currentLineNumber, line); 
			*fail = TRUE;
            return;
        }
		/* prn can get a dst addressing of any type. */
		/* while every other command of type 1 cannot get immediate value */
		if (opIndex != 12 && dstOp == IMMIDEIATE_ADDR) {
        	printf("ERROR: Type one command cannot get a num in dst.\n"); 
			printf("At line %d: %s.\n", currentLineNumber, line); 
			*fail = TRUE;
            return;
		}
        /* If we arrived here there are no errors. */
        *ic += 2;
        return;
    }

    /* If we are past this line it means have type 2 command. */
    /* Checking for a comma and cleaning the line.*/
    if (line[0] != ',') {
        printf("ERROR: Type two command missing comma.\n"); 
		printf("At line %d: %s.\n", currentLineNumber, line); 
		*fail = TRUE;
        return;
    }

    /* Skip the first char and clean the line.*/
    memmove(line, line +1, strlen(line));   
    removePrecedingSpaces(line);
    removeBackSpaces(line);
    
    /* If the line is empty then we have a missing opperand. */
    if (strlen(line) < 1) {
        printf("ERROR: Type two command missing second operrand.\n"); 
		printf("At line %d: %s.\n", currentLineNumber, line); 
		*fail = TRUE;
        return;
    }

    if (line[0] == '@') { /* If line starts with @ it is a register. */
		/* if the second char is not r then the register is not legal. */
		if (line[1] != 'r') {
        	printf("ERROR: Unknown register name.\n"); 
			printf("At line %d: %s.\n", currentLineNumber, line); 
			*fail = TRUE;
            return;
		}
        /* Check the register number and add it to both srcRegValue and dstRegValue. */
        dstRegValue = getRegisterValue(line);
        if (dstRegValue == -1) {
        	printf("ERROR: Unknown register name.\n"); 
			printf("At line %d: %s.\n", currentLineNumber, line); 
			*fail = TRUE;
            return;
        }
        /* REMEMBER: if command is of type 2 dstOp will be changed. */
        dstOp = REGISTER_ADDR;
	} else if (isdigit(line[0]) || line[0] == '-' || line[0] == '+') {
        /* Check the number and add it to both immediateSrc and immediateDst. */
        immediateDst = getNumber(line);
        if (immediateDst < -1024 || immediateDst > 1023) {
        	printf("ERROR: Number is inccorect.\n"); 
			printf("At line %d: %s.\n", currentLineNumber, line); 
			*fail = TRUE;
        }
        /* REMEMBER: if command is of type 2 dstOp will be changed. */
        dstOp = IMMIDEIATE_ADDR;
    } else { /* We are not checking the symbol name here only on the second run. */
        /* REMEMBER: if command is of type 2 dstOp will be changed. */
        dstOp = DIRECT_ADDR;       
    }

    if ((opIndex >= 0 && opIndex <= 3) || opIndex == 6) {
        /* Error checking. */
        /* If the command is lea the src operand can be of type 3 only and dst is 3,5*/
        if ((opIndex == 6) && (srcOp != DIRECT_ADDR || dstOp == IMMIDEIATE_ADDR)) {
        	printf("ERROR: Lea inccoret use.\n"); 
			printf("At line %d: %s.\n", currentLineNumber, line); 
			*fail = TRUE;
            return;
        }
        /* If the command is not cmp or lea and the dstOp is a number. */
        if ((opIndex != 1 && opIndex != 6) && dstOp == IMMIDEIATE_ADDR) {
        	printf("ERROR: Type 2 command inccoret use.\n"); 
			printf("At line %d: %s.\n", currentLineNumber, line); 
			*fail = TRUE;
            return;
        }

        /* If both are register we add just one line instead of 3. */
        if (srcOp == REGISTER_ADDR && dstOp == REGISTER_ADDR) {
            *ic += 2;
            return;

        } else { /* Two more prints are needed*/
            *ic += 3;
        }
    }
        return;
}
