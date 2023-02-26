#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAX_MACRO 800 /*maximum 10 lines for macro. 10*80=800*/
#define MAX_LINE 1000 /*max line is 80 but for preprossec we allow errors*/
#define LINE_LENGTH 80
#define MAX_LABEL_LENGTH 32
#define MAX_WORD_LENGTH 32
#define WEIRD_BASE_NUMBER 32 
#define START_IC 100
#define FIELD_SIZE 10
#define FIELD_SIZE_32_BASE 2
#define MIN_ARG_VAR_GET 1
#define MAX_OPER_GET 2
#define NUMBER_BITS_ARE 2
#define DECIMAL 10
#define SUFFIX_LENGTH 3
#define OB_END ".ob"
#define ENT_END ".ent"
#define EXT_END ".ext"
#define AM_END ".am"
#define AS_END ".as"


#define FREE_LIST(listKind, listHead, listLast){\
	listKind curr = *listHead;\
	listKind temp;\
	while (curr){\
		temp = curr->next;\
		free(curr);\
		curr = temp;\
	}\
	*listHead = NULL;\
	*listLast = NULL;\
}
/*-------------enums definition: -----------------*/
/*enum operation{mov,cmp,add,sub,not,clr,lea,inc,dec,jmp,bne,get,prn,jsr,rts,hlt};*/
enum operation { MOV, CMP, ADD, SUB, NOT, CLR, LEA, INC, DEC, JMP, BNE, GET, PRN, JSR, RTS, HLT};
enum directive { DATA, STRING, STRC, ENTRY, EXTERN };
enum sort_line { DIRCT_LINE, COMMAND_LINE, LABEL_LINE, COMMANT, EMPTY, ERROR_LINE };
enum sort_word { NUMBER = 1, VAR = 2, SRTUCT = 4, REGIS = 8, STRNG = 16, LABEL = 32, OPER = 64, DIRCT = 128, ERROR = 255 };
enum end_word { SPACE, ENDF, ENDL, COMMA, ERROR_END };
enum registers { R0, R1, R2, R3, R4, R5, R6, R7 };
/*-------------types definition: --------------*/


/*macros table*/
typedef struct macro{
char name[MAX_WORD_LENGTH];
char content[MAX_MACRO];
struct macro *next;
}macro;

/*errors list:*/
typedef struct error {
	char *errorMsg;
	int numLine;
	struct error *next;
}error;

/*symbols table - lable*/
typedef struct symbolRow {
	int isOper;
	int isExtrn;
	int address;
	char label[MAX_LABEL_LENGTH];
	struct symbolRow *next;
}symbolRow;

/*data table*/
typedef struct dataRow {
	int dcVal;
	char data[FIELD_SIZE+1];
	int numLine;
	struct dataRow *next;
}dataRow;

/*code table */
typedef struct codeRow {
	int address;
	char data[FIELD_SIZE+1];
	char word[MAX_WORD_LENGTH * 2 + 1];
	int numLine;
	struct codeRow *next;
}codeRow;

/*entry table */
typedef struct entryNode {
	char label[MAX_WORD_LENGTH];
	int line;
	struct entryNode *next;
}entryNode;
