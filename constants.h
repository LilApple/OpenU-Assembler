/**
 * Date: 7.8.23
 * Author: Shay Linzberg 212341390
 * Purpose: The constants.h contians all the constants defnetions of the program.
 * The constans are documented here.
 * Assumptions: (all are from the book).
 * 1. MAX ic + dc + start_value needs to be less then 1023.
 * 2. Max macro/symbol name length is 32.
 * 3. Max line length is 80.
 * 4. Start value is 100.
 */
#ifndef _CONSTANTS_H
#define _CONSTANTS_H

/* The start value of the IC as defined in the instruction book. */
#define IC_START_VALUE 100

/* The max value of the IC as defined in the instruction book. */
#define IC_MAX_VALUE 1023

/* The maximum line length. */
#define MAX_LINE_LENGTH 80

/* The maximum macro name length. */
#define MAX_MACRO_NAME_LENGTH 32

/* The maximum macro name length. */
#define MAX_SYMBOL_NAME_LENGTH 32

/* The current size of the hash table. */
#define HASH_TABLE_SIZE 16

/* The error number value. */
#define ERROR_NUM_VALUE -9999

/* A simple boolean defnition. */
typedef enum boolean {
	FALSE = 0,
	TRUE = 1
} bool;

/* Encoding types which represent the first 0-1 bits of a command, as defined in the instruction book. */
typedef enum encodeType {
	/* Default use for commands. */
	A = 0,
	/* External encode, used to note that the value is external. */
	E = 1,
	/* Relocatable encode, used to note that the value is relocatable inside the current file.*/
	R = 2
} encodeType;

/* OP code of the commands which represent bits 5-8 of a command, as defined in the instruction book. */
typedef enum opcodes {
	/* First Group of commands, take 2 opperands. */
	MOV_OP = 0,
	CMP_OP = 1,
	ADD_OP = 2,
	SUB_OP = 3,
	LEA_OP = 6,
	
	/* Second Group of commands, take 1 opperand. */
	NOT_OP = 4,
	CLR_OP = 5,
	INC_OP = 7,
	DEC_OP = 8,
	JMP_OP = 9,
	BNE_OP = 10,
	RED_OP = 11,
	PRN_OP = 12,
	JSR_OP = 13,
	
	/* 3rd Group of commands, take no operand. */
	RTS_OP = 14,
	STOP_OP = 15,
	
	/* OP used for handling unknown ops. */
	ERROR_OP = -1
} opcode;

/* Used for quick searching. */
typedef struct opElement 
{
	char *key;
	opcode op;
}op;

/* Addressing types which represent the both bits 2-4 and bits 9-11 of a command, as defined in the instruction book. */
typedef enum addressingType {
	/* Empty addressing, where addresing is not needed. */
	EMPTY_ADDR = 0,
	
	/* Immediate addressing, where the operand is signed integer. */
	IMMIDEIATE_ADDR = 1,
	
	/* Direct addressing, where the operand is a label, that is present in the file. */
	DIRECT_ADDR = 3,

	/* Direct Register addressing, where the operand is a register with @ in front of it. */
	REGISTER_ADDR = 5,
	
	/* Error addressing, for handling unknown addressing types. */
	ERROR_ADDR = -1
} addrType;

/* Registers 0 to 8.*/
typedef enum registers {
	R0 = 0,
	R1,
	R2,
	R3,
	R4,
	R5,
	R6,
	R7,
	/* Error handling register, for unknown register.*/
	ERROR_REG = -1
} reg;

/* Represents a single command in base 64. */
typedef struct baseCommand {
	unsigned int firstChar: 6;
	unsigned int secondChar: 6;
} baseCommand;

/* Representes a 12 bit command with Format: ARE:2 bits, dst Addressing: 3 bits, opcode: 4 bits, src Addressing: 4 bits.*/
typedef struct headCommand {
	unsigned int ARE: 2;
	unsigned int dst: 3;
	unsigned int opcode: 4;
	unsigned int src: 3;

} headCommand;

/* Represents a 12 bit register command, with Format: ARE:2 bits, dst Register: 5 bits, src Register: 5 bits. */
typedef struct registerCommand {
	unsigned int ARE: 2;
	unsigned int dstReg: 5;
	unsigned int srcReg: 5;
} regCommand;

/* Represents a 12 bit immediate command, the immediate can be a value or a label address. */
typedef struct immediateCommand {
	unsigned int ARE: 2;
	unsigned int immediate: 10;
} immCommand;

/* Represents a symbol type which can be either code or data symbol.*/
typedef enum symbolType {
	CODE,
	DATA,
	EXTERN,
	ENTRY
} symbolType;

/* */
typedef enum commandType {
	/* Head command is the first line of every the format of head is: 2 bits of ARE, 3 bits of dstAddr, 4 bits of opcode, 3 bits of srcAddr */
	HEAD_COM = 1,
	/* Register command is an extra command with the format: 2 bits of ARE, 5 bits of dst Register, 5 bits src register.*/
	REGISTER_COM,
	/* Immediate command is an extra command with the format: 2 bits of ARE, 10 bits of num or address.*/
	NUM_COM,
	/* ERROR types used for initial value or errors. */
	ERROR_COM = -1
} commandType;

/* Represents a 12 bit data command, which can hold a string or a signed number. */
typedef struct dataCommand {
	int dataValue: 12;
} dataCommand;

/* Rpresents anything a command can be, which can be either head command/register command/immediate command/data command.*/
/* The base command is used inorder to print the value stored in base64. */
typedef union command {
	headCommand head;
	regCommand reg;
	immCommand imm;
	dataCommand data;
	baseCommand base;
} command;

/* Represents a single data line type which can be data, str, entry or extern.*/
typedef enum dataType {
	/* line contians .data. */
	DATA_TYPE = 1,
	/* line contians .str. */
	STR_TYPE,
	/* line contians .entry. */
	ENTRY_TYPE,
	/* line contians .extern. */
	EXTERN_TYPE,
	/* line does not contians data instruction or it cotians two types of data instructions or more. */
	ERROR_TYPE = -1
} dataType;

#endif
