/** #####################################################################################################################
 * Date: 9.8.23
 * Author: Shay Linzberg 212341390
 * Purpose: The second_run.c containes the implemntiations of all the functions in second_run.h
 * Assumptions: None
 ####################################################################################################################### */
/* ###################################################### Includes ##################################################### */
/* ##################################################################################################################### */
#include <string.h>
#include "second_run.h"
/* ##################################################################################################################### */
/* ################################################## Implementations ################################################## */
/* ##################################################################################################################### */
void secondRunFile(FILE* src, char *currentFileName, HashTable *symbolTable, int *ic, int *dc, bool *fail) {
    int currentLineIndex = 0; /* The number of the current line used for error prints.*/
    char line[MAX_LINE_LENGTH], symbolName[MAX_SYMBOL_NAME_LENGTH]; /* String that contains the line and the symbolName respectively. */
    char *typePtr, *objFileName, *extFileName, *entFileName; /* objfile names and typePtr which is used to scan for data. */
    FILE *objFile, *ext, *ent; /* File pointers that will be created and deleted accordingly. */
    bool entFound = FALSE, extFound = FALSE; /* Flags used for deleting ent/ext files repsectily. */
    HashNode *symbolNode; /* symbol node use for searching if we .entry types. */
    dataType type; /* The current type of data, set to error every line. */
    
    /* Open the object file. */
    objFileName = (char *)strdup(currentFileName);
    strcat(objFileName, ".ob");
    objFile = fopen(objFileName, "w+");
    if (objFile == NULL) {
		printf("Error: couldn't open %s, skipping it.\n", objFileName);
		free(objFile);
        return;
	}
    
	/* Open the ext file. */
    extFileName = (char *)strdup(currentFileName);
    strcat(extFileName, ".ext");
    ext = fopen(extFileName, "w+");
    if (ext == NULL) {
		printf("Error: couldn't open %s, skipping it.\n", extFileName);
		free(objFile);
        free(ext);
        return;
	}

	/* Open the ent file. */
    entFileName = (char *)strdup(currentFileName);
    strcat(entFileName, ".ent");
    ent = fopen(entFileName, "w+");
    if (ent == NULL) {
		printf("Error: couldn't open %s, skipping it.\n", entFileName);
        free(objFile);
        free(ext);
		free(ent);
        return;
	}
	
	/* Print ic and dc and reset them. */
	fprintf(objFile, "%d %d\n", *ic, *dc);
	*ic = 0;
	*dc = 0;

    /* Adding missing lines && adding extern and ent if needed. */
    while (fgets(line, sizeof(line),src) != NULL)
    {
		currentLineIndex++;
        /* Determine the type of line, if a line has .data/.string/.extern/.entry it is a data line.*/
        type = ERROR_TYPE;
        typePtr = strstr(line, ".data");

        if (typePtr != NULL && type == ERROR_TYPE) {
            type = DATA_TYPE;
        } else if (typePtr != NULL) {
            continue;
        }

        typePtr = strstr(line, ".string");
        if (typePtr != NULL && type == ERROR_TYPE) {
            type = STR_TYPE;
        } else if (typePtr != NULL) {
           	continue;
        }

		typePtr = strstr(line, ".extern");
        if (typePtr != NULL && type == ERROR_TYPE) {
            type = EXTERN_TYPE;
        } else if (typePtr != NULL) {
            continue;
        }

		typePtr = strstr(line, ".entry");
        if (typePtr != NULL && type == ERROR_TYPE) {
            type = ENTRY_TYPE;
        } else if (typePtr != NULL) {
            continue;
        }

        /* Clean the line by removing all white spaces from the start and the end.*/
        removePrecedingSpaces(line);
        removeBackSpaces(line);

        /* If an optional symbol is present extract and skip it. */
        extractSymbolName(line, symbolName);
        if (symbolName != NULL) {
            skipUntilChar(line, ':');
			removePrecedingSpaces(line);
        	removeBackSpaces(line);
        }

        switch (type)
        {
        case ERROR_TYPE:
            encodeCommandLine(line, src, objFile, ic, fail, symbolTable, ext, &extFound, currentLineIndex);
            break;
        case DATA_TYPE:
            encodeDataLine(line, src, objFile, dc, type, fail, currentLineIndex);
            break;
        case STR_TYPE:
            encodeDataLine(line, src, objFile, dc, type, fail, currentLineIndex);
            break;
        case EXTERN_TYPE:
            /*Skip extern declration.*/
            break;
        case ENTRY_TYPE:
            /*skip .entry and spaces.*/
            entFound = TRUE;
        	typePtr = strstr(line, ".entry");
            typePtr += 6;
			/* Check for missing space. */
			if (*typePtr != ' ') {
       			printf("ERROR: missing space before extern.\n"); 
				printf("At line %d: %s.\n", currentLineIndex, line); 
				*fail = TRUE;
				continue;
			}
			/* Clean the line. */
            removePrecedingSpaces(typePtr);
            removeBackSpaces(typePtr);
			symbolNode = search(symbolTable, typePtr);
			if (symbolNode == NULL) {
				printf("ERROR: unkown entry");
				printf("At line %d: %s.\n", currentLineIndex, line); 
				*fail = TRUE;
				break;
			} else {
            	/* Print it to the file. */
            	fprintf(ent, "%s %d\n", symbolNode->key, symbolNode->value.symbol.value);
            	break;
			}
        }
    }

    /* Close the files. */
    fclose(objFile);
    fclose(ext);
    fclose(ent);

    /* Remove fails on fail.*/
    if (*fail) {
        printf("Found ERROR on one run on or run two removing objfile, ext and ent. (if needed).\n");
        remove(objFileName);
        remove(extFileName);
        remove(entFileName);
		return;
    }
	
	/* Remove .ent if needed. */
	if (!entFound) {
		remove(entFileName);
	}
	
	/* Remove .ext if needed. */
	if (!extFound) {
		remove(extFileName);
	}

}

void printBase64Value(FILE *dst, command com) {
	/* A base 64 lookup table. */
    static const char base64Chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    if (com.base.secondChar >= 0 && com.base.secondChar < 64) {
        fputc(base64Chars[com.base.secondChar], dst);
    }

    if (com.base.firstChar >= 0 && com.base.firstChar < 64) {
        fputc(base64Chars[com.base.firstChar], dst);
    }

	fputc('\n', dst);
}

void encodeDataLine(char *line, FILE* src, FILE* dst, int *dc, dataType type, bool *fail, int currentLineIndex) {
    int asciiVal, num; /* An asciiVal and a number both used if type is of .data*/
    char *paranPtr; /* A pointer that check if " is present*/
    bool neg = FALSE; /* Flag that checks if number is negative in .data, negative if TRUE. */
    dataCommand dataToPrint; /* Used to store the value of .data number/.string char 12 bits.*/
	command comToPrint; /* Command to print will make the above command into 6bits and 6bits via baseCommand. */
    
    /* Skip until . symbol. */
    while (*line != '.') {
        line++;
    }

    removeBackSpaces(line);
    switch (type) {
        case STR_TYPE:
            line += strlen(".string");
            removePrecedingSpaces(line);
            if (strlen(line) < 1) {
                return;
            }
            /* Check and skip the first " */
            if (*line != '"') {
                return;
            }
            line++; 

            paranPtr = strstr(line, "\"");
            if (paranPtr == NULL) {
                return;
            }

            while (*line != '"') {
                asciiVal = (int)*line;
                dataToPrint.dataValue = asciiVal & 0xFFF;
				comToPrint.data = dataToPrint;
				printBase64Value(dst, comToPrint);
                line++;
                *dc += 1;
            }
            /* Print null terminator. */
            *dc += 1;
			comToPrint.data.dataValue = 0;
			printBase64Value(dst, comToPrint);
            
            /* Skip second " */
            line++;
            /* Check for extra chars. */
            if (strlen(line) > 1) {
                return;
            }
            break;
        case DATA_TYPE:
            line += strlen(".data");
            removePrecedingSpaces(line);
            if (strlen(line) < 1) {
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

                if (*line == '+' && neg == TRUE) {
                    return;
                }

                if (!isdigit(*line)) {
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
                    if (num > 2047 || num < -2048) {
                        return;
                    }
                    /* Print the number */
                    *dc += 1;
                    dataToPrint.dataValue = num & 0xFFF;
					comToPrint.data = dataToPrint;
					printBase64Value(dst, comToPrint);
                    neg = FALSE;
                    
                    /* Skip comma if present and check for extra chars. */
                    removePrecedingSpaces(line);
                    if (*line == ',') {
                        line++;
                        if (strlen(line) < 1) {
                            return;
                        }
                    } else if (*line != ',' && strlen(line) > 1) { /* If comma is not present and there are extra chars. */
                        return;
                    }

                
                }
            }
            break;
		case EXTERN_TYPE:
			break;
		case ENTRY_TYPE:
			break;
		case ERROR_TYPE:
			break;
    }


}

void encodeCommandLine(char *line, FILE* src, FILE* dst, int *ic, bool *fail, HashTable *symbolTable, FILE* ext, bool *extFound, int currentLineNumber) {
    headCommand com; /* A command that represent the Head command.*/
    regCommand regCom; /* A command that represent the Reg command.*/
	immCommand immSrcCom, immDstCom; /* Two commands that represetns the immediate command. */
    /* Immidiate command can hold a number or a adress, but address is used in the seconnd run. */
    /* There are two commands because one represents the src and the other represent the dst opperand.*/
    command comToPrint; /* The command to print can be either head/reg/imm. */
	addrType srcOp = EMPTY_ADDR, dstOp = EMPTY_ADDR; /* src and dst op for the head command.*/
    HashNode *srcNode, *dstNode; /* The dst and src nodes are used for searching the symbol and getting its value. */
    int opIndex, srcRegValue, dstRegValue, immediateSrc, immediateDst; /* Explantiation below: */
    char *lineCopy, *symbolName; /* A line copy and symbol name. */
    int i = 0, symbolLength = 0 /* index i and symbol length, used to calculate the index length. */;
    /**
     * opIndex: The index of the current operation, will be -1 if error is encountered.
     * srcRegValue/dstRegValue: src and dst registers value, not always used.
     * immediateSrc/immediateDst: address value or number, 10 bits.
     */
    /* Set the op index. */
    opIndex = getOpIndex(line);
    /* If op index is unkown then the command is terminated. */
	if (opIndex == -1) {
        return;
	}

    /* Set the initial known values, some of which won't be used.*/
    com.opcode = opIndex;
    com.ARE = A;
    regCom.ARE = A;
    immSrcCom.ARE = A;
    immDstCom.ARE = A;

	/* Skip the operation name and clean the line. */
    skipOperation(line, opIndex);	
	removePrecedingSpaces(line);
    removeBackSpaces(line);

	/* If command is of type 0.*/
    if (opIndex == 15 || opIndex == 14) {
        /* Check for extra chars after type 0 command. */
		if (strlen(line) > 0) {
            return;
		} else {
			*ic += 1;
        	com.src = srcOp;
        	com.dst = dstOp;
            /* Print. */
            comToPrint.head = com;
			printBase64Value(dst, comToPrint);
		}
		return;
    }

    /* If the line is empty after the first op skip, then no opperand is given which is an error. */
    if (strlen(line) < 1) {
        return;
    }
    
	/* Determine addressing type of dst/src and skip until , or whitespace.*/
    if (line[0] == '@') { /* If line starts with @ it is a register. */
		/* if the second char is not r then the register is not legal. */
		if (line[1] != 'r') {
            return;
		}
        /* Check the register number and add it to both srcRegValue and dstRegValue. */
        srcRegValue = dstRegValue = getRegisterValue(line);
        if (srcRegValue == -1) {
            return;
        }
		/* skip legal register and clean white sapces. */
		line += 3;
		removePrecedingSpaces(line);
        /* REMEMBER: if command is of type 2 dstOp will be changed. */
        srcOp = dstOp = REGISTER_ADDR;
	} else if (isdigit(line[0]) || line[0] == '-' || line[0] == '+') {
        /* Check the number and add it to both immediateSrc and immediateDst. */
		if (*line == '+') {
			line++;
		}
		/* Get the number, REMEMBER: if command is of type 2 immediateDst can be changed.*/
        immediateSrc = immediateDst = getNumber(line);
		if (*line == '-') {
			line++;
		}
		/* Check if num is legal. */
        if (immediateSrc < -1024 || immediateSrc > 1023) {
            return;
        }

		/* skip legal num. and remove white spaces.*/
		while(isdigit(*line)) {
			line++;
		}
		removePrecedingSpaces(line);

        /* REMEMBER: if command is of type 2 dstOp will be changed. */
		srcOp = IMMIDEIATE_ADDR;
        dstOp = IMMIDEIATE_ADDR;
    } else { /* We are not checking the symbol name here only on the second run. */
        /* REMEMBER: if command is of type 2 dstOp will be changed. */
        lineCopy = (char *)strdup(line);
		removePrecedingSpaces(lineCopy);
        removeBackSpaces(lineCopy);

		/* Calculate the length of the symbol. */
		if (!isalpha(*lineCopy)) {
            printf("ERROR: Illegal symbol name.\n");
			printf("At line %d: %s.\n", currentLineNumber, line); 
			*fail = TRUE;
			return;
		}

		/* Calculate the length until digit. */
		while (isalpha(*lineCopy) || isdigit(*lineCopy)) {
			symbolLength++;
			lineCopy++;
		}

		/* Check if symbol name not too big. */
		if (symbolLength > MAX_SYMBOL_NAME_LENGTH) {
            printf("ERROR: Symbol Name too long.\n");
			printf("At line %d: %s.\n", currentLineNumber, line); 
			*fail = TRUE;
			return;
		}
		/* Malloc (+1 for null terminator). */
		symbolName = (char *)malloc(symbolLength + 1);
		lineCopy = line;
		i = 0;
		/* append the symbol to symbol name. */
    	while (isalpha(*lineCopy) || isdigit(*lineCopy) ) {
        	symbolName[i] = *lineCopy;
        	i++;
        	lineCopy++;
    	}
		symbolName[i] = '\0';

		/* Skip symbol and clean the line.*/
		line += symbolLength;
		removePrecedingSpaces(line);
        removeBackSpaces(line);

		/* Search the name. */
        srcNode = search(symbolTable, symbolName);
        if (srcNode == NULL) {
            printf("ERROR: unknown first symbol.\n");
			printf("At line %d: %s.\n", currentLineNumber, line); 
			*fail = TRUE;
            return;
        }
		/* free symbolName. */
		free(symbolName);

        immSrcCom.ARE = immDstCom.ARE = R;
		/* Set extern if needed.*/
        if (srcNode ->value.symbol.type == EXTERN) {
            immSrcCom.ARE = immDstCom.ARE = E;
        }
        immediateSrc = immediateDst = srcNode->value.symbol.value;
        srcOp = DIRECT_ADDR;
        dstOp = DIRECT_ADDR;
    }

	/* Clean the line from white spaces. */
    removePrecedingSpaces(line);
    removeBackSpaces(line);

	/* Handle command type 1. */	
	if (opIndex >= 4 && opIndex <= 13 && opIndex != 6) {
        if (strlen(line) > 0) { /* If command is of type one line should be empty after the skip. */
            return;
        }
		/* prn can get a dst addressing of any type. */
		/* while every other command of type 1 cannot get immediate value */
		if (opIndex != 12 && dstOp == IMMIDEIATE_ADDR) {
            return;
		}

        /* If we arrived here there are no errors. */
        *ic += 2;
		com.src = EMPTY_ADDR;
        com.dst = dstOp;
        
        /* Print the head. */
        comToPrint.head = com;
        printBase64Value(dst, comToPrint);

        /* Print the second line. */
        switch (dstOp)
        {
        case IMMIDEIATE_ADDR:
            immDstCom.immediate = immediateDst; /* Add immediate. */
            /* Print dstCom.*/
            comToPrint.imm = immDstCom;
			printBase64Value(dst, comToPrint);
            break;
        
        case DIRECT_ADDR:
            immDstCom.immediate = immediateDst;
            /* Print dstCom.*/
            comToPrint.imm = immDstCom;
			printBase64Value(dst, comToPrint);
            /* If extern print to ext file.*/
            if (immSrcCom.ARE == E) {
                fprintf(ext, "%s %d\n", srcNode->key, (IC_START_VALUE - 1 + *ic));
                *extFound = TRUE;
            }  
            break;

        case REGISTER_ADDR:
            regCom.srcReg = 0;
            regCom.dstReg = dstRegValue;
            /* Print the reg. */
            comToPrint.reg = regCom;
			printBase64Value(dst, comToPrint);
            break;
		case ERROR_ADDR:
			break;
		case EMPTY_ADDR:
			break;
        }
        return;
    } 
    /* If we are past this line it means have type 2 command. */
    /* Checking for a comma and cleaning the line.*/
    if (line[0] != ',') {
        return;
    } else {
		line++;
	}

    /* Skip the first char and clean the line.*/
    removePrecedingSpaces(line);
    removeBackSpaces(line);

    /* If the line is empty then we have a missing opperand. */
    if (strlen(line) < 1) {
        return;
    }

    if (line[0] == '@') { /* If line starts with @ it is a register. */
		/* if the second char is not r then the register is not legal. */
		if (line[1] != 'r') {
            return;
		}
        /* Check the register number and add it to both srcRegValue and dstRegValue. */
        dstRegValue = getRegisterValue(line);
        if (srcRegValue == -1) {
            return;
        }
        /* REMEMBER: if command is of type 2 dstOp will be changed. */
        dstOp = REGISTER_ADDR;
	} else if (isdigit(line[0]) || line[0] == '-' || line[0] == '+') {
        /* Check the number and add it to both immediateSrc and immediateDst. */
        immediateDst = getNumber(line);
        if (immediateDst < -1024 || immediateDst > 1023) {
            return;
        }
        /* REMEMBER: if command is of type 2 dstOp will be changed. */
        dstOp = IMMIDEIATE_ADDR;
    } else { /* We are not checking the symbol name here only on the second run. */
        lineCopy = (char *)strdup(line);
		removePrecedingSpaces(lineCopy);
        removeBackSpaces(lineCopy);
        dstNode = search(symbolTable, lineCopy);
        /* If symbol not present in the table then its an error. */
        if (dstNode == NULL) {
            printf("ERROR: unknown second symbol.\n");
			printf("At line %d: %s.\n", currentLineNumber, line); 
			*fail = TRUE;
            return;
        }
        immDstCom.ARE = R;
        /* If symbol the is present in the table and is of type EXTERN then ARE is E. */
        if (dstNode ->value.symbol.type == EXTERN) {
            immDstCom.ARE = E;
        }
        immediateDst = dstNode->value.symbol.value;
        dstOp = DIRECT_ADDR;    
    }

    if ((opIndex >= 0 && opIndex <= 3) || opIndex == 6) {
        /* Error checking. */
        /* If the command is lea the src operand can be of type 3 only and dst is 3,5*/
        if ((opIndex == 6) && (srcOp != DIRECT_ADDR || dstOp == IMMIDEIATE_ADDR)) {
            return;
        }
        /* If the command is not cmp or lea and the dstOp is a number. */
        if ((opIndex != 1 && opIndex != 6) && dstOp == IMMIDEIATE_ADDR) {
            return;
        }

        if (strlen(line) < 1) {
            return;
        }

        /* If we arrived here no there are no errors. */
        com.src = srcOp;
        com.dst = dstOp;

        /* Print the head. */
        comToPrint.head = com;
		printBase64Value(dst, comToPrint);

        /* If both are register we add just one line instead of 3. */
        if (com.src == REGISTER_ADDR && com.dst == REGISTER_ADDR) {
            *ic += 2;
            regCom.srcReg = srcRegValue;
            regCom.dstReg = dstRegValue;

            /* Print reg.*/
            comToPrint.reg = regCom;
			printBase64Value(dst, comToPrint);
            return;

        } else { /* Two more prints are needed*/
            *ic += 3;
            /* Print the first line by type.*/
            switch (srcOp)
            {
            case IMMIDEIATE_ADDR:
                immSrcCom.immediate = immediateSrc;
                comToPrint.imm = immSrcCom;
                /* Print num. */
				printBase64Value(dst, comToPrint);
                break;
            case DIRECT_ADDR:
                immSrcCom.immediate = immediateSrc;
                /* Print srcCom.*/
                comToPrint.imm = immSrcCom;
				printBase64Value(dst, comToPrint);

                /* If extern print to ext file.*/
                if (immSrcCom.ARE == E) {
                    fprintf(ext, "%s %d\n", srcNode->key, (IC_START_VALUE - 2 + *ic));
                    *extFound = TRUE;
                }
                break;
            case REGISTER_ADDR:
                regCom.srcReg = srcRegValue;
                regCom.dstReg = 0;

                /* Print reg. */
                comToPrint.reg = regCom;
				printBase64Value(dst, comToPrint);
                break;
			case ERROR_ADDR:
				break;
			case EMPTY_ADDR:
				break;
            }
            /* Print the second line by type.*/
            switch (dstOp)
            {
            case IMMIDEIATE_ADDR:
                immDstCom.immediate = immediateDst;
                comToPrint.imm = immDstCom;
                /* Print num. */
				printBase64Value(dst, comToPrint);
                break;
            case DIRECT_ADDR:
                immDstCom.immediate = immediateDst;
                /* Print address.*/
                comToPrint.imm = immDstCom;
				printBase64Value(dst, comToPrint);
                /* If extern print to ext file.*/
                if (immDstCom.ARE == E) {
                    fprintf(ext, "%s %d\n", dstNode->key, (IC_START_VALUE - 1 + *ic));
                    *extFound = TRUE;
                }      
                break;
            case REGISTER_ADDR:
                regCom.srcReg = 0;
                regCom.dstReg = dstRegValue;
                 /* Print reg. */
                comToPrint.reg = regCom;
				printBase64Value(dst, comToPrint);
                break;
			case ERROR_ADDR:
				break;
			case EMPTY_ADDR:
				break;
            }
        }
        return;
    }   

}
