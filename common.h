#ifndef COMMON_H
#define COMMON_H

/*------------------- Libraries-------------------*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

/*------------------- Constant Variables -------------------*/

#define IC_FIRST_ADDRESS       	100
#define DC_FIRST_ADDRESS       	0
#define MAX_DATA_SIZE          	1000
#define MAX_BUF                	81
#define MAX_FILENAME           	33
#define MAX_LABEL_LENGTH       	31
#define WORD_LENGTH           	15
#define MAX_REGISTER_DIGIT     	7
#define BYTE_SIZE	       	8
#define COMMANDS_NUMBER		15
#define ZERO_SIZE		1
#define UNUSED_SIZE		3
#define GROUP_SIZE		2
#define OPCODE_SIZE		4
#define OPERAND_SIZE		2
#define ERA_SIZE		2



/*------------------- String constants -------------------*/

#define SOURCE_FILE ".as"
#define OBJ_FILE ".ob"
#define EXT_FILE ".ext"
#define ENT_FILE ".ent"

#define DATA_WORD ".data"
#define STRING_WORD ".string"
#define ENTRY_WORD ".entry"
#define EXTERN_WORD ".extern"

#define STRING_END "\0"

#define COMMENT_SIGN ';'
#define STRING_SIGN '"'
#define NEWLINE '\n'
#define LABEL_SIGN ':'
#define STR_FLAG '"'
#define IMD_FLAG '#'
#define REG_FLAG 'r'
#define COMMA ','
#define SPACE ' '
#define PLUS '+'
#define MINUS '-'
#define LEFT_BRACKET '['
#define RIGHT_BRACKET ']'


/*------------------- Global Variables -------------------*/

static int IC = IC_FIRST_ADDRESS;
static int DC = DC_FIRST_ADDRESS;
typedef enum {false, true} bool;

extern int errorCounter;
extern const char commands[16][4];

/*----------------- Functions Declaration -----------------*/

int checkCMD(char *);
void startLoop(char *, int);
void endLoop(char *);

#endif
