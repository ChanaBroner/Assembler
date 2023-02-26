#include "include_define_struct.h"
#include "word_checking_functions.h"
#include "second_functions.h"

extern FILE *file;

int get_oper_rules(int numInEnum, int canGet[2]) 
{
	int numFields = 0;
	int firstArg = 0, secondArg = 0;
	switch (numInEnum) 
	{
		case 0: /*mov*/
		case 2: /*add*/
		case 3: /*sub*/
			firstArg = 2 | 4 | 8;
			secondArg = 1 | 2 | 4 | 8;
			numFields = 2;
			break;
		case 1: /*cmp*/
			firstArg = 1| 2 | 4 | 8;
			secondArg = 1 | 2 | 4 | 8;
			numFields = 2;
			break;
		case 6:/*lea*/
			firstArg = 2 | 4 | 8;
			secondArg = 2| 4;
			numFields = 2;
			break;
		case 5: /*not*/
		case 4: /*clr*/
		case 7: /*inc*/
		case 8: /*dec*/
		case 9: /*jmp*/
		case 10:/*bne*/
		case 11:/*get*/
		case 13: /*jsr*/
			firstArg = 2 | 4 | 8;
			numFields = 1;
			break;
		case 12: /*prn*/
			firstArg = 1 | 2 | 4 | 8;
			numFields = 1;
			break;
		case 14:/*rts*/
		case 15:/*hlt*/
			numFields = 0;
			break;
	}
	canGet[0] =secondArg;
	canGet[1] = firstArg;
	return numFields;
}

void enter_string_into_char_array(char word[], int *dc, dataRow **dataHead, dataRow **currData, dataRow **lastData) 
{
	int i = 1;
	dataRow *last = *lastData;
	enter_int_into_bits_array((*currData)->data,(int)(word[i]), FIELD_SIZE, 0);
	for (i = 2; i < strlen(word)-1; i++) 
	{
		*currData = (dataRow*)malloc(sizeof(dataRow));
		(*currData)->dcVal = (*dc)++;
		(*currData)->next = NULL;
		enter_int_into_bits_array((*currData)->data, (int)(word[i]), FIELD_SIZE, 0);
		last->next = *currData;
		last = *currData;
	}
	/*adding the '\0' letter*/
	*currData = (dataRow*)malloc(sizeof(dataRow));
	(*currData)->dcVal = (*dc)++;
	(*currData)->next = NULL;
	enter_int_into_bits_array((*currData)->data, 0, FIELD_SIZE, 0);
	last->next = *currData;
	*lastData = *currData;
}

int oper_checking(int numInEnum, int *ic, int *isCorrect, int numLine, codeRow **currCode, codeRow **lastCode, error **errorHead, error **currError, error **lastError,symbolRow **smblHead)
{
	char *errMsg = NULL;
	char word[MAX_WORD_LENGTH+1];
	char arguments[MAX_OPER_GET][MAX_WORD_LENGTH];
	int numVarGet, i;
	int canGet[MAX_OPER_GET];
	int defaultCanGet = (1 | 2 | 4 | 8);
	int numCommas = 0;
	
	int endWord = SPACE;
	canGet[0] = defaultCanGet;
	canGet[1] = defaultCanGet;
	numVarGet = get_oper_rules(numInEnum, canGet);
	if(numVarGet==0)/*if method has no oprands*/
	{
		fseek(file, -1, SEEK_CUR);
		endWord = read_word(word);
		numCommas = -1;
	}
	for (i = 0; i < numVarGet && endWord != ENDF && endWord != ENDL; i++)
	{

		endWord = read_word(word);
		while (!strcmp(word, ",") && endWord != ENDF && endWord != ENDL) 
		{
			/*count commas*/
			numCommas++;
			endWord = read_word(word);
		}
		/*if the word is empty - error*/
		if (word[0] == 0) 
		{
			errMsg = "\nToo few arguments for operation";
			add_error_to_list(errMsg, numLine, errorHead, currError, lastError);
			*isCorrect = 0;
			return endWord;
		}
		if(numVarGet==1)
		{
				strcpy(arguments[1], word);
		}
		else
			strcpy(arguments[i], word);	
	}

	/*if line did not end - error*/
	if (endWord != ENDL && endWord != ENDF) 
	{
		errMsg = "\nToo many arguments for operation";
		add_error_to_list(errMsg, numLine, errorHead, currError, lastError);
		*isCorrect = 0;
		return endWord;
	}
	/*if read too few arguments*/
	if (i < numVarGet) 
	{
		errMsg = "\nToo few arguments for operation";
		add_error_to_list(errMsg, numLine, errorHead, currError, lastError);
		*isCorrect = 0;
		return endWord;
	}
	/*if have not correct sum of commas - error:*/
	if (numCommas != numVarGet - 1) 
	{
		errMsg = "\nUn correct number of commas in line";
		add_error_to_list(errMsg, numLine, errorHead, currError, lastError);
		*isCorrect = 0;
		return endWord;
	}
	errMsg = enter_fields_into_row(arguments, ic, canGet, numLine, currCode, lastCode, errorHead, currError, lastError,smblHead);
	if (errMsg) 
		add_error_to_list(errMsg, numLine, errorHead, currError, lastError);

	return endWord;
}

char *enter_fields_into_row(char arguments[MAX_OPER_GET][MAX_WORD_LENGTH], int *ic, int canGet[2], int numLine, codeRow **currCode, codeRow **lastCode, error **errorHead, error **currError, error **lastError,symbolRow **smblHead) 
{
	/*registerCode - code row for registers. if null - first register in operation*/
	codeRow *tempCode, *last = *currCode, *registerCode = NULL;
	int numInEnum[MAX_OPER_GET] = { -1,-1 }, j = 0;
	char *p;
	int number, i, noMethod, numBits, methodNum = 0;
	int sortWord[2];
	int placeToEnterMethod;
	char *errMsg = NULL;
	for (i = 0; i < MAX_OPER_GET; i++) 
	{
		if (i == 0)/*destenation operand*/
		{
			noMethod=4;
			numBits=2;
			placeToEnterMethod = 4;
		}
		else /*source operand*/
		{
			noMethod=6;
			numBits=2;
			placeToEnterMethod = 6;
		}
		/*if doesn't need this argument - enter 0 to method place in curr-code node:*/
		if ((!arguments[i]) || arguments[i][0] == 0)
		{
			enter_int_into_bits_array((*currCode)->data, 0, numBits, noMethod);
		}
		else 
		{
			/*is-oper-line = 1*/
			sortWord[i] = check_word(arguments[i], &numInEnum[i], 1, &errMsg, numLine, errorHead, currError, lastError,smblHead);
			/*if argument is error*/
			if (sortWord[i] == ERROR) 
			{
				errMsg = "\nUn Correct argument";
				return errMsg;
			}
			/*if the argument sort cannot be given for this operation*/
			if ((canGet[i] & ((int)sortWord[i])) != (int)sortWord[i]) 
			{
				errMsg = "\nUn correct operand for operation";
				return errMsg;
			}


			switch (sortWord[i]) 
			{
				/*const number - method: 0 */
				case 1:
					methodNum = 0;
					/*temp code for number value*/
					tempCode = (codeRow*)malloc(sizeof(codeRow));
					tempCode->address = (*ic)++;
					tempCode->numLine = numLine;
					tempCode->next = NULL;

					p = arguments[i];
					p++;
					number = strtol(p, &p, DECIMAL);
					/*enter the number to the var of operation*/
					enter_int_into_bits_array((tempCode)->data, number, 8, 0);
					/*enter the addressing method for argument*/
					enter_int_into_bits_array(((*currCode)->data), methodNum, 2, placeToEnterMethod);
					last->next = tempCode;
					last = tempCode;
					/*enter the ARE bits - 0:*/
					enter_ARE_bits(tempCode, 0);
					break;
		
				case 2:
					methodNum = 1;
					tempCode = (codeRow*)malloc(sizeof(codeRow));
					strcpy(tempCode->word, arguments[i]);
					tempCode->address = (*ic)++;
					tempCode->numLine = numLine;
					/*enter 00000000 becouse variable address unknown*/
					enter_int_into_bits_array(tempCode->data, 0, 8, 0);
					/*enter the ARE bits -2: '10'*/
					enter_ARE_bits(tempCode, 2);
					tempCode->next = NULL;

					last->next = tempCode;
					last = tempCode;
					/*enter the addressing method for argument*/
					enter_int_into_bits_array(((*currCode)->data), methodNum, 2, placeToEnterMethod);

					break;

				/*struct - method num: 2*/
				case 4:
					methodNum = 2;

					/*enter the addressing method for first argument*/
					enter_int_into_bits_array(((*currCode)->data), methodNum, 2, placeToEnterMethod);
					/*place for enter the structe address:*/
					tempCode = (codeRow*)malloc(sizeof(codeRow));
					/*enter structe name to row-word*/
					strcpy(tempCode->word, arguments[i]);
					/*enter 00000000 because address of structe-name is unknown*/
					enter_int_into_bits_array(tempCode->data, 0, 8, 0);
					tempCode->address = (*ic)++;
					tempCode->numLine = numLine;
					tempCode->next = NULL;
					/*enter the ARE bits - 2*/
					enter_ARE_bits(tempCode, 2);
					last->next = tempCode;
					last = tempCode;
					/*temp-code for number*/
					tempCode = (codeRow*)malloc(sizeof(codeRow));
					tempCode->address = (*ic)++;
					tempCode->numLine = numLine;
					tempCode->next = NULL;
					p = arguments[i];
					while(*(p+(j++))!='.');
					number = strtol(p + j, &p, FIELD_SIZE);
					/*enter the number to the var of operation*/
					enter_int_into_bits_array((tempCode)->data, number, 8, 0);
					enter_ARE_bits(tempCode, 0);
					last->next = tempCode;
					last = tempCode;
					break;

				/*register - method num 3*/
				case 8:
					methodNum = 3;
					/*if null - create the registers-code-row*/
					if (!registerCode) 
					{
						registerCode = (codeRow*)malloc(sizeof(codeRow));
						registerCode->address = (*ic)++;
						registerCode->numLine = numLine;
						strcat(registerCode->word, arguments[i]);
						registerCode->next = NULL;
					}
					/*enter the register num to half of char array for registers in row*/
					/*if ite first argument*/
					if (i == 0) 
					{
						enter_int_into_bits_array((registerCode->data), numInEnum[i], 4, 0);
					}
					/*if its second argument*/
					else 
					{
						enter_int_into_bits_array((registerCode->data), numInEnum[i], 4, 4);
					}

					last->next = registerCode;
					last = registerCode;
					/*enter the addressing method for first argument*/
					enter_int_into_bits_array(((*currCode)->data), methodNum, 2, placeToEnterMethod);

					/*enter the ARE bits - 0*/
					enter_ARE_bits(registerCode, 0);
					break;

					/*variable-name - method num 1*/
			}
		}
	}
	
	enter_ARE_bits(*currCode, 0);
	
	/*placing the curr-code to the end of the list*/
	*currCode = last;
	*lastCode = *currCode;
	(*lastCode)->next=NULL;
return errMsg;
}

int data_checking(int numInEnum, int *dc, int *isCorrect, int numLine, dataRow **dataHead, dataRow **currData, dataRow **lastData, error **errorHead, error **currError, error **lastError,symbolRow **smblHead) 
{
	int numVarGet = 0;
	int endWord;

	int canGet = 1 | 16;
	numVarGet = get_dircet_rules(numInEnum, &canGet);
	endWord = enter_data_into_row(canGet, dc, numVarGet, numLine, isCorrect, dataHead, currData, lastData, errorHead, currError, lastError,smblHead);
	return endWord;
}

int get_dircet_rules(int numInEnum, int *canGet) 
{
	int maxNumFields = 0;
	switch (numInEnum) 
	{
		/*DATA:*/
		case 0:
			maxNumFields = 38/*the maximum numbers: 80 chars line -(.data = 5 chars + space) = (74/2)+1=38*/;
			*canGet = 1;/*NUMBER*/
			break;
		/*STRING*/
		case 1:
			maxNumFields = 1;
			*canGet = 16;/*STRNG*/
			break;
		/*STRUCT*/
		case 2:
			maxNumFields = 2;
			*canGet =1;/*The first argument is number*/
			break;
	}
	return maxNumFields;
}

int enter_data_into_row(int canGet, int *dc, int maxNumVarGet, int numLine, int *isCorrect, dataRow **dataHead, dataRow **currData, dataRow **lastData, error **errorHead, error **currError, error **lastError,symbolRow **smblHead) 
{
	int numCommas = 0, sumVarGet = 0, i, numCommasShouldHave = 0;
	int endWord = SPACE;
	int sortWord;
	char word[MAX_WORD_LENGTH], *errMsg = NULL;
	int numInEnum = 0;

	for (i = 0; ((i < maxNumVarGet) && (endWord != ENDF) && (endWord != ENDL)); i++) 
	{
		endWord = read_word(word);
		while ((!strcmp(word, ",")) && (endWord != ENDF) && (endWord != ENDL)) 
		{
			/*count commas*/
			numCommas++;
			endWord = read_word(word);
		}
		/*if the word is empty - error*/
		if (word[0] == 0)
		{ 
			if (i < MIN_ARG_VAR_GET) 
			{
				errMsg = "\nTwo few arguments for operation";
				add_error_to_list(errMsg, numLine, errorHead, currError, lastError);
				*isCorrect = 0;
				return endWord;
			}
		}
		sumVarGet++;
		sortWord = check_word(word, &numInEnum, 0, &errMsg, numLine, errorHead, currError, lastError,smblHead);
		if (sortWord == ERROR) 
		{
			errMsg = "\nProblem with variable";
			add_error_to_list(errMsg, numLine, errorHead, currError, lastError);
			*isCorrect = 0;
			return endWord;
		}
		/*if this argument cannot be given for this operation*/
		if ((sortWord&canGet) != sortWord) 
		{
			errMsg = "\nNot match type of argument";
			add_error_to_list(errMsg, numLine, errorHead, currError, lastError);
			*isCorrect = 0;
			return endWord;
		}
		else 
		{
			add_data_to_list(word, sortWord, dc, dataHead, currData, lastData);
		}
		/*If struct, the second argument in struct is string*/
		if((canGet == (int)NUMBER) && (maxNumVarGet == 2))
			canGet = STRNG;

		if (errMsg) 
		{
			add_error_to_list(errMsg, numLine, errorHead, currError, lastError);
			*isCorrect = 0;
			return endWord;
		}
	}

	/*if line did not end - error*/
	if (endWord != ENDL && endWord != ENDF) 
	{
		errMsg = "\nToo many arguments for operation";
		add_error_to_list(errMsg, numLine, errorHead, currError, lastError);
		*isCorrect = 0;
		return endWord;
	}
	if (i < MIN_ARG_VAR_GET) 
	{
		errMsg = "\nToo few arguments for operation";
		add_error_to_list(errMsg, numLine, errorHead, currError, lastError);
		*isCorrect = 0;
		return endWord;
	}
		numCommasShouldHave = sumVarGet - 1;

	/*if have not correct sum of commas - error:*/
	if (numCommas != numCommasShouldHave) 
	{
		errMsg = "\nUn correct number of commas in line";
		add_error_to_list(errMsg, numLine, errorHead, currError, lastError);
		*isCorrect = 0;
		return endWord;
	}

	return endWord;
}

void enter_ARE_bits(codeRow *tempCode, int AREtoEnter)
{
	enter_int_into_bits_array(tempCode->data, AREtoEnter, 2, 8);
}

void enter_int_into_bits_array(char array[FIELD_SIZE+1], int toEnter, int numberBits, int startInd) 
{
	unsigned int mask = 1U;
	int i;

	for (i = startInd + numberBits - 1; i >= startInd; i--) 
	{
		array[i] = (toEnter & mask) ? '1' : '0';
		mask <<= 1;
	}
}
