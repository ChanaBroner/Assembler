#include "include_define_struct.h"
#include "first_functions.h"
#include "second_functions.h"
#include "table_functions.h"

extern FILE *file;

int add_data_to_list(char *word, int sortWord, int *dc, dataRow **dataHead, dataRow **currData, dataRow **lastData) 
{
	int toCast;

	*currData = (dataRow*)malloc(sizeof(dataRow));
	(*currData)->dcVal = (*dc)++;
	(*currData)->next = NULL;
	/*if its the first node in list: */
	if (!(*lastData)) 
		*dataHead = *currData;
	else 
		(*lastData)->next = *currData;
	*lastData = *currData;

	switch (sortWord) 
	{
		case NUMBER:
			toCast = strtol(word, &word, DECIMAL);
			enter_int_into_bits_array(((*currData)->data), toCast, FIELD_SIZE, 0);
			break;
		case STRNG:
			enter_string_into_char_array(word, dc, dataHead, currData, lastData);
			break;
	}
	return 0;
}

void add_code_to_list(int numOper, int *ic, char word[MAX_WORD_LENGTH], int numLine, codeRow **codeHead, codeRow **currCode, codeRow **lastCode) 
{
	*currCode = NULL;
	*currCode = (codeRow*)malloc(sizeof(codeRow));
	strcpy((*currCode)->word, word);
	(*currCode)->numLine = numLine;
	(*currCode)->address = (*ic)++;
	(*currCode)->next = NULL; 
	enter_int_into_bits_array(((*currCode)->data), numOper, 4, 0);

	/*if its the first row in table*/
	if (!(*lastCode)) 
		*codeHead = *currCode;
	else 
		(*lastCode)->next = *currCode;
	*lastCode = *currCode;
}

void add_symbol_to_list(int isOper, int isExtrn, int ic, char word[MAX_WORD_LENGTH],symbolRow **currSmbl,symbolRow **smblHead, symbolRow **lastSmbl) 
{
	(*currSmbl) = (symbolRow*)malloc(sizeof(symbolRow));
	(*currSmbl)->address = ic;
	(*currSmbl)->isExtrn = isExtrn;
	(*currSmbl)->isOper = isOper;
	strcpy((*currSmbl)->label, word);
	(*currSmbl)->next = NULL;
	/*if its the first row in the list:*/
	if ((*lastSmbl) == NULL)
		*smblHead = *currSmbl;
	else 
		(*lastSmbl)->next = *currSmbl;
	*lastSmbl = *currSmbl;
}

void add_macro_to_list(char *name, char *content, macro **macroHead, macro **currMacro, macro **lastMacro) 
{
	*currMacro = (macro*)malloc(sizeof(macro));
	strcpy((*currMacro)->name, name);
	strcpy((*currMacro)->content, content);
	(*currMacro)->next = NULL;

	/*if its the first macro in the list:*/
	if (!(*lastMacro))
		*macroHead = *currMacro;
	else 
		(*lastMacro)->next = *currMacro;
	*lastMacro = *currMacro;
}

void add_error_to_list(char *errorMsg, int numLine, error **errorHead, error **currError, error **lastError) 
{
	*currError = (error*)malloc(sizeof(error));
	(*currError)->errorMsg = errorMsg;
	(*currError)->numLine = numLine;
	(*currError)->next = NULL;

	/*if its the first in list:*/
	if (!(*lastError))
		*errorHead = *currError;
	else 
		(*lastError)->next = *currError;
	*lastError = *currError;
}

int create_exter_symbol(int numLine, error **errorHead, error **currError, error **lastError,symbolRow **currSmbl,symbolRow **smblHead, symbolRow **lastSmbl) 
{
	char word[MAX_WORD_LENGTH], *errMsg;
	int endWord;
	int sortWord;
	/*variables for 'check-word' function */
	int numInEnum = 0;

	endWord = read_word(word);
	sortWord = check_word(word, &numInEnum, 0, &errMsg, numLine, errorHead, currError, lastError,smblHead);
	if (sortWord != VAR) 
	{
		errMsg = "\nAfter .extern directive must come a variable name";
		add_error_to_list(errMsg, numLine, errorHead, currError, lastError);
	}
	if (endWord != ENDL && endWord != ENDF) 
	{
		errMsg = "\nAfter .extern directive must one variable only";
		add_error_to_list(errMsg, numLine, errorHead, currError, lastError);
	}
	if (errMsg) 
	{
		return endWord;
	}
	/*check if label is correct*/
	errMsg =is_label(word, numLine, errorHead, currError, lastError,smblHead);
	if (errMsg) 
	{
		return endWord;
	}
	add_symbol_to_list(0, 1, 0, word,currSmbl,smblHead,lastSmbl);
	return endWord;
}

int create_entry_node(int numLine, error **errorHead, error **currError, error **lastError,symbolRow **smblHead,entryNode **entryHead,entryNode **entryCurr,entryNode **entryLast) 
{
	char word[MAX_WORD_LENGTH], *errMsg = NULL;
	int endWord, entryBefore = 0;
	int sortWord;
	entryNode *curr = *entryHead;
	int numInEnum = 0;

	endWord = read_word(word);
	sortWord = check_word(word, &numInEnum, 0, &errMsg, numLine, errorHead, currError, lastError,smblHead);

	/*entry line sould include one var*/
	if (sortWord != VAR) 
	{
		errMsg = "\nAfter '.entry' directive must come a variable name";
		add_error_to_list(errMsg, numLine, errorHead, currError, lastError);
		return endWord;
	}
	/*if not end of line - error, must get just one variable name*/
	if (endWord != ENDL && endWord != ENDF) 
	{
		errMsg = "\nAfter '.entry' directive must come one variable only";
		add_error_to_list(errMsg, numLine, errorHead, currError, lastError);
		return endWord;
	}
	errMsg = is_correct_var_name(word, numLine);
	if (errMsg)
	{
		return endWord;
	}
	while (curr) 
	{
		if (!strcmp(curr->label, word))
			entryBefore = 1;
		curr = curr->next;
	}
	if(entryBefore)
	{
		errMsg = "\nA label already declared as entry";
		add_error_to_list(errMsg, numLine, errorHead, currError, lastError);
		return endWord;	
	}
	*entryCurr = (entryNode*)malloc(sizeof(entryNode));
	strcpy((*entryCurr)->label, word);
	(*entryCurr)->line = numLine;
	(*entryCurr)->next = NULL;
	/*if its the first node in list*/
	if (!(*entryLast))
	{
		*entryHead = *entryCurr;
	}
	else 
	{
		(*entryLast)->next = *entryCurr;
	}
	*entryLast = *entryCurr;
	return endWord;
}

void write_entries(char *fileName, error **errorHead, error **currError, error **lastError,symbolRow **smblHead,entryNode **entryHead)
{
	char entFileName[MAX_WORD_LENGTH + SUFFIX_LENGTH + 1], *errMsg=NULL;
	char addressInBase32[FIELD_SIZE_32_BASE+1];
	FILE *entFile;
	entryNode *curr = *entryHead;
	symbolRow *last= *smblHead;
	int entryExist = 0, flag = 0;

	while (curr)
	{
		while(last)
		{
			if(!strcmp(curr->label,(last->label)))
			{
				(curr->line)= (last->address);/*changing to correct address for entry*/
				flag = 1;
			}
			last = last->next;
		}	
		if(!flag)/*entry declared but not used*/
		{
	       		errMsg = "\nEntry declared but not used";
			add_error_to_list(errMsg, curr->line, errorHead, currError, lastError);	
		}
		last= *smblHead;
		curr = curr->next;
		flag=0;
	}
	if(errMsg)
		return;
	curr= *entryHead;
	strcpy(entFileName,(strcat(fileName,ENT_END)));
	if(curr)/*only if there is entry label open file*/
	{
		if(!(entFile = fopen(entFileName,"w")))
		{
			printf("Cannot open file\n");
			return;
		}
	}

	while (curr)
	{
		char arrayLine[FIELD_SIZE+1];
		enter_int_into_bits_array(arrayLine, curr->line, FIELD_SIZE, 0);
		from_binary_to_weirld32(arrayLine, addressInBase32);
		fprintf(entFile,"%s\t",curr->label);
		fprintf(entFile,"%s\n",addressInBase32);
		curr = curr->next;
		entryExist = 1;		
	}
	if(entryExist)
		fclose(entFile);
	
}
void free_lists(dataRow **dataHead, dataRow **lastData, codeRow **codeHead, codeRow **lastCode, entryNode **entryHead, entryNode **entryLast, symbolRow **smblHead, symbolRow **lastSmbl, macro **macroHead, macro **lastMacro, error **errorHead, error **lastError) 
{

	FREE_LIST(error*, errorHead, lastError)
	FREE_LIST(dataRow*, dataHead, lastData)
	FREE_LIST(symbolRow*, smblHead, lastSmbl)
	FREE_LIST(codeRow*, codeHead, lastCode)
	FREE_LIST(entryNode*, entryHead, entryLast)
	FREE_LIST(macro*, macroHead, lastMacro)
}

void printErrors(error **errorHead) 
{
	error *curr = *errorHead;
	while (curr) 
	{
		printf("\nError: %s\tLine:%d", curr->errorMsg, curr->numLine);
		curr = curr->next;
	}
}
