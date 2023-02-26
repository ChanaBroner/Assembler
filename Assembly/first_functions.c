#include "include_define_struct.h"
#include "first_functions.h"
#include "second_functions.h"
#include "word_checking_functions.h"
#include "table_functions.h"

extern FILE *file;

int go_to_end_line() 
{
	char c;
	fseek(file, -1, SEEK_CUR);
	while (((c = getc(file)) != EOF) && c != '\n');
	
	if(c == EOF)
		return ENDF;/*end of file*/
	return ENDL;/*end of line*/
}	

int is_oper(char *word) 
{
	int numInEnum = -1;
	if(!word)
		return numInEnum; 
	if (!strcmp(word, "mov")) numInEnum = 0;
	else if (!strcmp(word, "cmp")) numInEnum = 1;
	else if (!strcmp(word, "add")) numInEnum = 2;
	else if (!strcmp(word, "sub")) numInEnum = 3;
	else if (!strcmp(word, "not")) numInEnum = 4;
	else if (!strcmp(word, "clr")) numInEnum = 5;
	else if (!strcmp(word, "lea")) numInEnum = 6;
	else if (!strcmp(word, "inc")) numInEnum = 7;
	else if (!strcmp(word, "dec")) numInEnum = 8;
	else if (!strcmp(word, "jmp")) numInEnum = 9;
	else if (!strcmp(word, "bne")) numInEnum = 10;
	else if (!strcmp(word, "get")) numInEnum = 11;
	else if (!strcmp(word, "prn")) numInEnum = 12;
	else if (!strcmp(word, "jsr")) numInEnum = 13;
	else if (!strcmp(word, "rts")) numInEnum = 14;
	else if (!strcmp(word, "hlt")) numInEnum = 15;
	
	return numInEnum;
}

char *is_label(char word[MAX_WORD_LENGTH], int numLine, error **errorHead, error **currError, error **lastError, symbolRow **smblHead) 
{
	char *errMsg = NULL;
	int isLabelDeclared = 0;
	errMsg = is_correct_var_name(word, numLine);
	if (errMsg)
		add_error_to_list(errMsg, numLine, errorHead, currError, lastError);
	/*if it's correct name - check if this label name wasn't declared before*/
	else
	{
		isLabelDeclared = check_if_exists_before(word, smblHead);
		if (isLabelDeclared)
		{
			errMsg = "\nVariable name declared already";
			add_error_to_list(errMsg, numLine, errorHead, currError, lastError);
		}
	}
	return errMsg;
}

int is_register(char *word) 
{
	int numInEnum = -1;
	if(word == NULL)
		return numInEnum;
	if (word[0] == 'r'&&strlen(word) == 2) 
	{
		if (word[1] == '0')numInEnum = 0;
		if (word[1] == '1')numInEnum = 1;
		if (word[1] == '2')numInEnum = 2;
		if (word[1] == '3')numInEnum = 3;
		if (word[1] == '4')numInEnum = 4;
		if (word[1] == '5')numInEnum = 5;
		if (word[1] == '6')numInEnum = 6;
		if (word[1] == '7')numInEnum = 7;
	}
	return numInEnum;
}

int is_string(char *word) 
{

	int i;
	if(word == NULL)
		return 0;
	for(i = 1; i < strlen(word) - 1; i++)
		if(! isprint(word[i]))/*checking that the string characters are visible (printable)*/ 
			return 0;
	if ((word[0] == '\"') && (strlen(word) > 2) && (word[strlen(word) - 1] == '\"'))/*checking that the string is surrounded by double quotes*/
		return 1;
	return 0;
}

char *is_number(char *word, int isOperLine) 
{
	char *errMsg = NULL;
	char *p;
	p = word;
	if (isOperLine)
	 {
		if (word[0] != '#')
		{
			errMsg = "\nUn correct represent of number";
			return errMsg;
		}
		else
			p++;
		
	}
	/*in any case - operation or directive line - check the number */
	isOperLine = strtol(p, &p, DECIMAL);/*using isOperLine operator in order to further pointer p to first char after number*/
	if (*p != 0) /*checks that no chars that are not numbers are in word*/
	{
		errMsg = "\nNot a correct number";
		return errMsg;
	}
	return errMsg;
}

int is_directive(char *word) 
{
	int numInEnum = -1;
	if (word[0] == '.') {
		if (!strcmp(word, ".data")) numInEnum = 0;
		if (!strcmp(word, ".string")) numInEnum = 1;
		if (!strcmp(word, ".struct")) numInEnum = 2;
		if (!strcmp(word, ".entry")) numInEnum = 3;
		if (!strcmp(word, ".extern")) numInEnum = 4;
	}
	return numInEnum;
}

char *is_correct_var_name(char *word, int numLine) 
{
	int result, i;
	char *errMsg = NULL;
	result = isalpha((int)word[0]);
	if (!result) 
		errMsg = "\nUncorrect label - first character must be a letter";
	else
	{
		result=is_oper(word);
		if(result!=-1)
			errMsg = "\nUncorrect label - label name is name of oper";
		result=is_directive(word);
		if(result!=-1)
			errMsg = "\nUncorrect label - label name is name of oper";
		else
		{
			for(i=1; i<strlen(word); i++)
			{
				if(!isalpha((int)word[i])&&!isdigit((int)word[i]))
					errMsg = "\nUncorrect label - label chars must be number or letter";
			}
		}
		
	}
	return errMsg;
}

char * is_struct(char *word, int isOperLine, int numLine)
{
	char * errMsg=NULL;
	int i=0;
	while(word[i]!='.'&& i<strlen(word))
		i++;
	if(word[i]=='.')
		if(word[i+1]=='1'||word[i+1]=='2')
			return errMsg;
		else
			errMsg = "\nUncorrect number - number must be 1 or 2";
	else
		errMsg = "\nMissing a period";
	return errMsg;
}

int check_if_exists_before(char *word, symbolRow **smblHead) 
{
	symbolRow *curr = *smblHead;
	while (curr) 
	{
		if (!strcmp(curr->label, word))
			return 1;
		curr = curr->next;
	}
	return 0;
}

void from_binary_to_weirld32(char binary[FIELD_SIZE+1], char word32Base[FIELD_SIZE_32_BASE+1])
{
	int i =FIELD_SIZE-1, value = 0, j = FIELD_SIZE_32_BASE - 1, k;
	int tempInt[FIELD_SIZE+1];/*converning from char to int array*/
	for(k = 0; k < FIELD_SIZE; k++)
	{
		if(binary[k] == '1')
			tempInt[k] = 1;
		else
			tempInt[k] = 0;
	}
	while (i >-1) 
	{
		value += tempInt[i--];
		value += tempInt[i--] * 2;
		value += tempInt[i--] * 4;
		value += tempInt[i--] * 8;
		value += tempInt[i--] * 16;
		
		word32Base[j--] = get_char_from_int_weird32(value);
		value = 0;
	}
	word32Base[FIELD_SIZE_32_BASE] = 0;
}

void int_to_werid32(int numCode, char word32Base[FIELD_SIZE_32_BASE + 1]) 
{
	char num32Base[FIELD_SIZE_32_BASE+1];
	int remainder = 0, i = 0, j = 0;
	/*converting to 32-base:*/
	while (numCode) 
	{
		remainder = numCode % WEIRD_BASE_NUMBER;
		num32Base[i++] = remainder;
		numCode /= WEIRD_BASE_NUMBER;
	}
	for (i = FIELD_SIZE_32_BASE - 1; i >-1 ; i--) 
	{
		word32Base[j++] = get_char_from_int_weird32(num32Base[i]);
	}
	word32Base[j] = 0;
}

char get_char_from_int_weird32(int number) 
{
	switch (number) {
	case 0:
		return '!';
	case 1:
		return '@';
	case 2:
		return '#';
	case 3:
		return '$';
	case 4:
		return '%';
	case 5:
		return '^';
	case 6:
		return '&';
	case 7:
		return '*';
	case 8:
		return '<';
	case 9:
		return '>';
	case 10:
		return 'a';
	case 11:
		return 'b';
	case 12:
		return 'c';
	case 13:
		return 'd';
	case 14:
		return 'e';
	case 15:
		return 'f';
	case 16:
		return 'g';
	case 17:
		return 'h';
	case 18:
		return 'i';
	case 19:
		return 'j';
	case 20:
		return 'k';
	case 21:
		return 'l';
	case 22:
		return 'm';
	case 23:
		return 'n';
	case 24:
		return 'o';
	case 25:
		return 'p';
	case 26:
		return 'q';
	case 27:
		return 'r';
	case 28:
		return 's';
	case 29:
		return 't';
	case 30:
		return 'u';
	case 31:
		return 'v';
	default:
		return '\0'; }
}

int get_address_of_symbol(char *labelName, int numLine, error **errorHead, error **currError, error **lastError,symbolRow **smblHead) 
{
	symbolRow *curr = *smblHead;
	char *label, left[MAX_LABEL_LENGTH];
	char *errMsg = NULL;
	strcpy(left,labelName);
	label = strtok(left, ".");/*if label is struct*/
	while (curr) 
	{
		/*if this label is */
		if (!strcmp(curr->label, label)) 
		{
			/*if its extern symbol - address = 0. else - address = ic/dc*/
			return curr->address;
		}
		curr = curr->next;
	}
	errMsg = "\nUn-declared variable";
	add_error_to_list(errMsg, numLine, errorHead, currError, lastError);
	return -1;
}

int is_extern_label(char *label, int ci, error **errorHead, error **currError, error **lastError,symbolRow **smblHead) 
{
	symbolRow *curr = *smblHead;
	char *errMsg = NULL;
	while (curr) 
	{
		if (!strcmp(curr->label, label)) 
		{
			if (curr->isExtrn) 
			{
				/*if the label used befor extern-declare - error*/
				if (curr->address > ci) 
				{
					errMsg = "\nCannot use a extern-label before declaration";
					add_error_to_list(errMsg, curr->address, errorHead, currError, lastError);
					return -1;
				}
				return 1;
			}
		}
		curr = curr->next;
	}
	return 0;
}
