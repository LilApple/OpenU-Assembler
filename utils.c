/** #####################################################################################################################
 * Date: 6.8.23
 * Author: Shay Linzberg 212341390
 * Purpose: utils.c contains all the implemntiations of utils.h .
 * Assumptions: None.
 ####################################################################################################################### */
/* ###################################################### Includes ##################################################### */
/* ##################################################################################################################### */
#include "utils.h"

/* ##################################################################################################################### */
/* ################################################## Implementations ################################################## */
/* ##################################################################################################################### */

/* Represents the opTable used in getOpIndex function and skipOperation function. */
/* Op is declared in constants.h and represents a single line in the opTable.*/
static op opTable[] = {
	{"mov", MOV_OP},
	{"cmp", CMP_OP},
	{"add", ADD_OP},
	{"sub", SUB_OP},
	{"not", NOT_OP},
	{"clr", CLR_OP},
	{"lea", LEA_OP},
	{"inc", INC_OP},
	{"dec", DEC_OP},
	{"jmp", JMP_OP},
	{"bne", BNE_OP},
	{"red", RED_OP},
	{"prn", PRN_OP},
	{"jsr", JSR_OP},
	{"rts", RTS_OP},
	{"stop", STOP_OP},
	{NULL, ERROR_OP}
};


char *strdup(const char *s) {
    char *d = malloc(strlen(s) + 1);
    if (d) {
        strcpy(d, s);
    }
    return d;
}

void removePrecedingSpaces(char *line) {
    char *src = line;
    char *dst = line;
    int leadingSpaces = 0;

    /* Find the number of leading spaces */
    while (isspace(*src)) {
        ++src;
        ++leadingSpaces;
    }

    /* Remove the leading spaces. */
    while (*src) {
        *dst++ = *src++;
    }

    /* Null terminate the new line. */
    *dst = '\0';
}

void removeBackSpaces(char *line) {
	int len, end;    
	if (line == NULL || *line == '\0') {
        return;
    }

    len = strlen(line);
    end = len - 1;

    while (end >= 0 && isspace(line[end])) {
        end--;
    }

    line[end + 1] = '\0';
}

char* getStringUntilWord(char* str, char* word) {
    char* result = NULL;
    char* ptr = strstr(str, word);
	int length;
    if (ptr != NULL) {
        length = ptr - str;
        result = (char*)malloc(length + 1);
        strncpy(result, str, length);
        result[length] = '\0';
    }
    return result;
}

void extractMacroName(char *macroName, char *macroNamePtr) {
    int i = 0;
    macroNamePtr += 4; /* Move to the first character after mcro. */
    /* Skip leading white spaces. */
    while (isspace(*macroNamePtr))
        macroNamePtr++;

    /* Copy the macro name. */
    while (*macroNamePtr && !isspace(*macroNamePtr) && i < MAX_MACRO_NAME_LENGTH - 1) {
        macroName[i++] = *macroNamePtr++;
    }
    macroName[i] = '\0'; /* Null-terminate the macro name */
}

int checkMacroName(char *macroName) {
    int i;
	/* Check that the first char is not a digit. */
	if (isdigit(*macroName)) {
		return -1;
	}
	/* Loop over and check that mcro name is not a command name. */
	for (i = 0; opTable[i].key != NULL; i++) {
		if (strncmp(macroName, opTable[i].key, strlen(opTable[i].key)) == 0 && (strlen(opTable[i].key) == strlen(macroName))) {
			return -1;
		}
	}
	return 0;
}

void extractSymbolName(char *line, char *symbolName) {
    int length = strlen(line);
    int i = 0;

    /* Calculate the symbol length. */
    while (i < length && line[i] != ':')
 	{
        ++i;
    }


    /* Copy the symbol from the line if found. */
    if (line[i] == ':') {
        int symbolLength = i;
        strncpy(symbolName, line, symbolLength);
        symbolName[symbolLength] = '\0';
    } else {
        /* No symbol found, set symbolName to NULL. */
        *symbolName = '\0';
    }

}

void skipUntilChar(char *line, char c) {
    int length = strlen(line);
    int i = 0;
    
    while (i < length)
    {
        if (line[i] == c) {
            ++i;
            memmove(line, line + i, length - i + 1);
            break;
        }
        ++i;
    }
}

int getOpIndex(char *line) {
    int i;
    for (i = 0; opTable[i].key != NULL; i++) {
        if (strncmp(line, opTable[i].key, strlen(opTable[i].key)) == 0) {
            return i;
        }
    }
	return -1;
}

void skipOperation(char *line, int opIndex) {
	int opLength = strlen(opTable[opIndex].key);
	memmove(line, line + opLength, strlen(line) - opLength + 1);
}

int getRegisterValue(char *line) {
    if (line[3] == '\0' || line[3] == ',' || line[3] == ' ' || line[3] == '\t' || line[3] == '\n') {
        switch (line[2]) {
            case '0': return R0;
            case '1': return R1;
            case '2': return R2;
            case '3': return R3;
            case '4': return R4;
            case '5': return R5;
            case '6': return R6;
            case '7': return R7;
        }
    }
    return ERROR_REG;
}

int getNumber(char *line) {
    bool neg = FALSE;
	int num = 0;
	if (*line == '-') {
		neg = TRUE;
		line++;
	}

	while (isdigit(*line)) {
		num = num * 10 + (*line - '0');
        line++;
	}

	if (neg == TRUE) {
     		num = -num;
	}


	if (*line == '\0' || *line == ',' || *line == ' ' || *line == '\t' || *line == '\n') {
		return num;
	}
	return ERROR_NUM_VALUE;
}

