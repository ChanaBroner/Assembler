/*By Racheli Salama ID: 324863448 and Chana Broner ID: 212368435*/

#include "include_define_struct.h"
#include "second_functions.h"
#include "first_functions.h"
#include "table_functions.h"
#include "word_checking_functions.h"


FILE *file;

int main(int argc, char *argv[])
{
	macro *macroHead = NULL, *currMacro = NULL, *lastMacro = NULL;
	error *errorHead = NULL, *currError = NULL, *lastError = NULL;
	int i = argc - 1;
	if (i == 0)
	{
		printf("\nExpected at least one file");
		exit(1);
	}
	while (i > 0) 
	{
		file = pre_process(argv[i], &macroHead, &currMacro, &lastMacro, &errorHead, &currError, &lastError);

		/*checks if file not found*/
		if (file) 
		{
			all_functions(argv[i], &macroHead, &currMacro, &lastMacro, &errorHead, &currError, &lastError);
			fclose(file);
		} 		
		else	
			fprintf(stderr, "\nSorry, something not correct with the file\n");
		i--;
	}
	return 0;
}

void all_functions(char *fileName, macro **macroHead, macro **currMacro, macro **lastMacro, error **errorHead, error **currError, error **lastError) 
{
	dataRow *dataHead = NULL, *currData = NULL, *lastData = NULL;
	codeRow *codeHead = NULL, *currCode = NULL, *lastCode = NULL;
	entryNode *entryHead=NULL, *entryCurr=NULL,*entryLast=NULL;
        symbolRow *smblHead = NULL, *currSmbl=NULL, *lastSmbl=NULL;
	
	char tempFileName[MAX_WORD_LENGTH];
	strcpy(tempFileName, fileName);
	first_level(&dataHead, &currData, &lastData, &codeHead, &currCode, &lastCode, errorHead, currError, lastError,&smblHead,&currSmbl,&lastSmbl, &entryHead, &entryCurr,&entryLast);

	if (*errorHead) 
	{
		printErrors(errorHead);
	}
	else 
	{
		second_level(fileName, &dataHead, &currData, &lastData, &codeHead, &lastCode, &currCode, errorHead, currError, lastError,&smblHead);
		if (*errorHead)
		{
			printErrors(errorHead);
		}
		else
		{ 
			write_entries(tempFileName, errorHead, currError, lastError,&smblHead,&entryHead);
			if (*errorHead)
				printErrors(errorHead);
		}
	}
	free_lists(&dataHead, &lastData, &codeHead, &lastCode, &entryHead, &entryLast, &smblHead, &lastSmbl, macroHead, lastMacro, errorHead, lastError);
	
}

void first_level(dataRow **dataHead, dataRow **currData, dataRow **lastData, codeRow **codeHead, codeRow **currCode, codeRow **lastCode, error **errorHead, error **currError, error **lastError, symbolRow **smblHead,symbolRow **currSmbl,symbolRow **lastSmbl,entryNode **entryHead,entryNode **entryCurr,entryNode **entryLast) 
{
	int ic = START_IC, dc = 0, numInEnum = 0, isFirstwordInLine = 1;
	int numLine = 1, isCorrect = 0;
	int sortWord;
	int sortLine;
	int isLabel = 0;
	char wordArr[MAX_WORD_LENGTH];
	int endOfWord = SPACE;
	char *errorMsg;
	
	while (endOfWord != ENDF)/*we have not reached the end of the file*/
	{
		errorMsg = NULL;
		endOfWord = read_word(wordArr);
		if (endOfWord == ERROR_END)/*its too long label / number*/
 		{
			sortWord = ERROR;
			errorMsg = "\nToo long argument";
			add_error_to_list(errorMsg, numLine, errorHead, currError, lastError);
			errorMsg = NULL;
		}

		sortWord = check_word(wordArr, &numInEnum, 1, &errorMsg, numLine, errorHead, currError, lastError, smblHead);
		/*check the sort of the LINE:*/
		switch (sortWord) 
		{
			case LABEL:
				sortLine = LABEL_LINE;
				break;
			case OPER:
				sortLine = COMMAND_LINE;
				break;
			case DIRCT:
				sortLine = DIRCT_LINE;
				break;
			/*other cases - error - un correct first word*/
			case VAR:
			case REGIS:
			case ERROR:
			default:
				sortLine = ERROR_LINE;
		}
		/*check if the line is commant*/
		if ((wordArr[0] == ';')&&(isFirstwordInLine)) 
		{
			sortLine = COMMANT;
		}
		/*check if the line is empty*/
		if (((wordArr[0] == '\0') && (endOfWord == ENDL) && isFirstwordInLine) || 
		     ((wordArr[0] == '\n') && isFirstwordInLine) || 
		      ((wordArr[0] =='\0') && (endOfWord ==(int) ENDF))) 
		{
			sortLine = EMPTY;
		}

		isFirstwordInLine = 0;

		switch (sortLine) 
		{
			char* word;
			case LABEL_LINE:
				if((endOfWord == ENDF) || (endOfWord == ENDL))
				{
					errorMsg = ("\nEmpty label");
					add_error_to_list(errorMsg, numLine, errorHead, currError, lastError);
					errorMsg = NULL;
				}
				else if(isLabel)
				{
					errorMsg = ("\nTwo labels cannot be declared on the same line");
					add_error_to_list(errorMsg, numLine, errorHead, currError, lastError);
					errorMsg = NULL;
					endOfWord = go_to_end_line();
				}		
				else 
				{
					word = strtok(wordArr, ":");
					isLabel = 1;
					add_symbol_to_list(0, 0, ic, word, currSmbl, smblHead, lastSmbl);
				}
				break;
			case COMMAND_LINE:
				/*change the isOper in currSymbol to true*/
				if (isLabel) 
				{
					(*currSmbl)->isOper = 1;
				}
				/*creating the row for the code:*/
				add_code_to_list(numInEnum, &ic, wordArr, numLine, codeHead, currCode, lastCode);
				/*calculating the operand and pass over the whole line - after this method pass to next line*/
				isCorrect = 1;
				endOfWord = oper_checking(numInEnum, &ic, &isCorrect, numLine, currCode, lastCode, errorHead, currError, lastError,smblHead);
				/*if there was error with the operand - add it to error list*/
				if (!isCorrect) 
				{
					endOfWord = go_to_end_line();
					errorMsg = NULL;
					if(isLabel)/*if there is a label before*/
					{
						delete_ladel(smblHead, lastSmbl);
					}
					break;
				}
				isLabel = 0;
				break;
			case DIRCT_LINE:
				if((endOfWord == ENDF) || (endOfWord == ENDL))
				{
					if((numInEnum == 0) || (numInEnum == 2))/*.data or.struct*/
						errorMsg = ("\nMissing numbers");

					else if(numInEnum == 1)/*.string*/
						errorMsg = ("\nMissing string");

					else if(numInEnum == 3)/*.entry*/
						errorMsg = ("\nMissing label after .entry");

					else /*.extern*/
						errorMsg = ("\nMissing label after .extern");

					add_error_to_list(errorMsg, numLine, errorHead, currError, lastError);
					errorMsg = NULL;
					if (isLabel)/*if there is a label before*/
					{
						delete_ladel(smblHead, lastSmbl);
					}
				}
				/*if it is struct or string or data*/
				else if (numInEnum < 3)
				{
					/*if there is a label before*/
					if (isLabel) 
					{
						/*change the address to dc (not ic)*/
						(*currSmbl)->address = dc;
					}
					isCorrect = 1;
			        	/*code line for direct line*/
					endOfWord = data_checking(numInEnum, &dc, &isCorrect, numLine, dataHead, currData, lastData, errorHead, currError, lastError,smblHead);
 
					/*if there were an error*/
					if (!isCorrect) 
					{
						endOfWord = go_to_end_line();
						errorMsg = NULL;
						if (isLabel)/*if there is a label before*/
						{
							delete_ladel(smblHead, lastSmbl);
						}
					}
					isLabel = 0;
				}
				/*if its entry command*/
				else if (numInEnum == (int)ENTRY) 
				{
					if(isLabel)/*if there is a label before .entry*/
					{
						delete_ladel(smblHead, lastSmbl);
					}
					endOfWord = create_entry_node(numLine, errorHead, currError, lastError,smblHead,entryHead,entryCurr,entryLast);
					/*passing to next line*/
					endOfWord = go_to_end_line();
				}
				/*if its extern command*/
				else if (numInEnum == (int)EXTERN) 
				{
					if(isLabel)/*if there is a label before .extern*/
					{
						delete_ladel(smblHead, lastSmbl);
					}
					endOfWord = create_exter_symbol(numLine, errorHead, currError, lastError,currSmbl,smblHead, lastSmbl);
				}
				break;
			case COMMANT:
				/*passing to next line*/
				endOfWord = go_to_end_line();
				break;
			case EMPTY:
				/*if empty - just continue*/
				break;
				/*if not one of the options - error*/
			case ERROR_LINE:
			default:
				/*if not send eny error msg: */
				if (!errorMsg) 
				{
					errorMsg = "\nProblem at the line - un-correct command";
					add_error_to_list(errorMsg, numLine, errorHead, currError, lastError);
					errorMsg = NULL;
					if(isLabel)/*if there is a label before*/
					{
						delete_ladel(smblHead, lastSmbl);
					}
				}

				/*passing to next line*/
				endOfWord = go_to_end_line();
		}		
		wordArr[0] = '\0';
		if (endOfWord == ENDL) 
		{
			isFirstwordInLine = 1;
			numLine++;
			isLabel = 0;
		}
	}
}

void delete_ladel(symbolRow **smblHead, symbolRow **lastSmbl)
{
	symbolRow *temp;
	if(!((*smblHead)->next))
	{
		free(*smblHead);
		*smblHead = NULL;
		*lastSmbl = NULL;
	}
	else
	{
		symbolRow *curr = *smblHead;
		while((curr->next) != *lastSmbl)
		{
			curr = curr->next;
		}
		curr->next = NULL;
		temp = *lastSmbl;
		*lastSmbl = curr;
		free(temp);
	}	
}

int read_word(char word[MAX_WORD_LENGTH]) 
{
	char c;
	int i = 0;
	int toReturn = SPACE;
	/*pass over the blank spaces before the word*/
	while (!feof(file) && ((c = getc(file)) == ' ' || c == '\t'));
	while (!feof(file) && i < MAX_WORD_LENGTH - 1 && c != ' '&& c != '\t' &&c != '\n'&&c != ',') 
	{
		(word)[i++] = c;
		c = getc(file);
	}
	/*if its too long label / number - error*/
	if (i == MAX_WORD_LENGTH - 1) 
	{
		return ERROR_END;
	}
	word[i] = 0;
	if (feof(file)) 
		return ENDF;

	if (c == ',') 
	{
		/*if the word is a comma - enter the comma into word*/
		if (!i) 
		{
			word[i++] = ',';
			word[i] = 0;
			return COMMA;
		}
		toReturn = COMMA;
	}
	while (c == ' ' || (c == '\t')) 
		c = getc(file);
	if (c == '\n') 
		toReturn = ENDL;
	/*if not end of file / of line - return the first letter in next word to the file*/
	else 
		fseek(file, -1, SEEK_CUR);
	return toReturn;
}


/*checks the word and returns its kind. and num-in-enum, if exists*/
/*is-oper-line - if its an operation line, numbers should check else-way then in directive line*/
int check_word(char word[MAX_WORD_LENGTH], int *numInEnum, int isOperLine, char **errMsg, int numLine, error **errorHead, error **currError, error **lastError,symbolRow **smblHead) 
{
	int sort = LABEL;
	*errMsg = NULL;
	if(!word)
		return ERROR;
	/*if its a label*/
	if (word[strlen(word) - 1] == ':')
	 {
		word[strlen(word) - 1]='\0';
		/*check if the given label is correct. if not return an error msg*/
		*errMsg = is_label(word, numLine, errorHead, currError, lastError, smblHead);
		/*if there is error in label*/
		if (*errMsg) 
		{
			sort = ERROR;
		}
		else 
			sort = LABEL;
		return sort;
	}

	/*if its number*/
	*errMsg = is_number(word, isOperLine);
	if (!*errMsg) 
		return NUMBER;

	*errMsg = NULL;
	/*if its a directive*/
	*numInEnum = is_directive(word);
	if (*numInEnum != -1) 
		return DIRCT;

	/*if its a struct: */
	*errMsg = is_struct(word, isOperLine, numLine);
	/*if its a correct struct*/
	if (!*errMsg)
		return SRTUCT;

	*errMsg = NULL;
	/*if its a register:*/
	sort = REGIS;
	*numInEnum = is_register(word);
	if (*numInEnum != -1) 
		return sort;

	/*if its a string*/
	if (is_string(word))
	{
		return STRNG;
	}
	sort = OPER;
	*numInEnum = is_oper(word);
	/*if it is not operand*/
	if (*numInEnum == -1) 
	{

		/*if its variable name*/
		*errMsg = is_correct_var_name(word, numLine);
		if (!*errMsg)
			sort = VAR;
		else sort = ERROR;
	}
	return sort;
}

void second_level(char *fileName, dataRow **dataHead, dataRow **currData, dataRow **lastData, codeRow **codeHead, codeRow **lastCode, codeRow **currCode, error **errorHead, error **currError, error **lastError,symbolRow **smblHead) 
{
	char obFileName[MAX_WORD_LENGTH + SUFFIX_LENGTH + 1];
	char extFileName[MAX_WORD_LENGTH + SUFFIX_LENGTH + 1];
	char ARE[NUMBER_BITS_ARE+1], word[MAX_WORD_LENGTH * 2];
	char tempFileName[MAX_WORD_LENGTH];
	char addressInBase32[FIELD_SIZE_32_BASE+1], codeInBase32[FIELD_SIZE_32_BASE+1], iVal[FIELD_SIZE_32_BASE+1], dVal[FIELD_SIZE_32_BASE+1];
	int address, isExtern = 0, icVal = START_IC, firstExtern = 1, dcVal;
	symbolRow *smb;
	
	/*create file for ob and extern*/
	FILE *obFile, *extFile;
	strcpy(tempFileName, fileName);	
	strcpy(obFileName,(strcat(fileName,OB_END)));
	strcpy(extFileName,(strcat(tempFileName, EXT_END)));
	
	*currCode = *codeHead;
	if(*lastCode)
		icVal = ((*lastCode)->address)+1;

	smb = *smblHead;
	while(smb)
	{
		if((!(smb->isExtrn)) && (!(smb->isOper)))/*data labels must be added icVal*/
			smb->address = ((smb->address) + icVal);
		smb = smb->next;
	}

	ARE[NUMBER_BITS_ARE] = 0;
	while (*currCode) 
	{
		ARE[0]= (*currCode)->data[8];
		ARE[1]= (*currCode)->data[9];
		
		/*if the two chars of data in row are equeals to "10" -> R (relocatable)*/
		if (!strcmp(ARE, "10")) 
		{
			strcpy(word, (*currCode)->word);
			address = get_address_of_symbol(word, (*currCode)->numLine, errorHead, currError, lastError, smblHead);
			/*if entry label - var declared*/
			if (address > 0) 
			{
				enter_int_into_bits_array((*currCode)->data, address, 8, 0);
			}

			isExtern = is_extern_label((*currCode)->word, (*currCode)->address, errorHead, currError, lastError,smblHead);
			if (isExtern)
			{
				/*extern- change ARE to 01*/
				(*currCode)->data[8]='0';
				(*currCode)->data[9]='1';
			}
		}		
		*currCode = (*currCode)->next;
	}



	if(*errorHead)/*if there are errors don't open object file*/
		return;

	/*opening the object file*/
	if(!(obFile = fopen(obFileName, "w")))
	{
		printf("Cannot open file\n");
		return;
	}
	if(!(*lastData))
		dcVal = 0;
	else
		dcVal = ((*lastData)->dcVal) + 1;
	int_to_werid32((icVal - START_IC), iVal);
	int_to_werid32(dcVal, dVal);
	fprintf(obFile,"%s\t",iVal);/*print ic value*/
	fprintf(obFile,"%s\n",dVal);/*print dc value*/

	*currCode = *codeHead;
	while (*currCode) 
	{
		ARE[0]= (*currCode)->data[8];
		ARE[1]= (*currCode)->data[9];
		int_to_werid32((*currCode)->address, addressInBase32);
		from_binary_to_weirld32((*currCode)->data, codeInBase32);
		
		if (!strcmp(ARE, "01"))/*extern*/ 
		{
			if(firstExtern)/*we need to open the extern file*/ 
			{
					/*opening the extern file*/
					if(!(extFile = fopen(extFileName, "w")))
					{
						printf("Cannot open file\n");
						return;
					}
					fprintf(extFile,"%s\t",(*currCode)->word);
					fprintf(extFile,"%s",addressInBase32);
					firstExtern = 0;
			}
			else
			{

					fprintf(extFile,"\n%s\t",(*currCode)->word);
					fprintf(extFile,"%s",addressInBase32);
			}
		}
		/*anyways - enter line into object file in weird-32-base*/
		fprintf(obFile,"%s\t",addressInBase32);
		fprintf(obFile,"%s\n",codeInBase32);		
		*currCode = (*currCode)->next;
	}

	/*printing data list*/
	*currData = *dataHead;

	while (*currData)
	{
		address = ((*currData)->dcVal)+icVal;
		int_to_werid32(address, addressInBase32);
		from_binary_to_weirld32((*currData)->data, codeInBase32);
		fprintf(obFile,"%s\t",addressInBase32);
		fprintf(obFile,"%s\n",codeInBase32);
		*currData = (*currData)->next;
	}

	fclose(obFile);
	if(!firstExtern)/*check if the extern file opened*/
		fclose(extFile);
}
