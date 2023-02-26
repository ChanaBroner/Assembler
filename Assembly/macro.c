#include "include_define_struct.h"
#include "first_functions.h"
#include "table_functions.h"
#include "macro.h"

FILE *pre_process(char fileName[MAX_WORD_LENGTH], macro **macroHead, macro **currMacro, macro **lastMacro, error **errorHead, error **currError, error **lastError)
{
	FILE *fileAs, *fileAm;
	char fileNameMacro[MAX_WORD_LENGTH + SUFFIX_LENGTH];
	int flag = 1, numLine = 0, macc = 0, j = 0, index = 0;
	char sendFileName[MAX_WORD_LENGTH + SUFFIX_LENGTH];
  	char word[MAX_WORD_LENGTH];
	char label[MAX_WORD_LENGTH];
	char line[MAX_LINE];
	char content[MAX_LINE];
	char contentMacro[MAX_LINE];
	char endmacro[MAX_WORD_LENGTH];

	reset_string(contentMacro);
	
	strcpy(sendFileName,fileName);
	strcat(sendFileName,AS_END);/*creates a correct name for current input file*/
	
	if(!(fileAs = fopen(sendFileName, "r")))
        {
		printf("Cannot open file\n");
		exit(0);
	}
	
	strcpy(fileNameMacro,fileName);
	strcat(fileNameMacro, AM_END);/*creates a correct name for current input file*/
	
	if(!(fileAm = fopen(fileNameMacro, "w")))
	{
		printf("Cannot open file\n");
		exit(0);
	}

	while(flag)
	{
		if(macc)/*in the middle of macro*/
			read_line(content, &flag, fileAs, numLine, errorHead, currError, lastError); 
		
		else/*not in the middle of macro*/
			read_line(line, &flag, fileAs, numLine, errorHead, currError, lastError);		

		numLine++;
		index = 0;

		if(macc)/*in the middle of macro*/
		{
			get_word(content, endmacro, &index);
			if(strcmp(endmacro,"endmacro") == 0)
			{
				macc = 0; 
				add_macro_to_list(label, contentMacro, macroHead, currMacro, lastMacro);
				reset_string(contentMacro);
				j = 0;
			}
			else
			{
				int k;
				for(k = 0; content[k] != 0; k++)
				{
					contentMacro[j] = content[k];
					j++;
				}		
			}	
		}
		else /*no in the middle of macro*/
		{
			get_word(line, word, &index);
			if((strcmp(word, "macro")) == 0)
			{
				get_word(line, label, &index);
				macc = 1;
			}
			else
			{
				int getMacro = 0;
				macro *last = *macroHead;
				while(last)
				{
					if(strcmp(word,last->name) == 0)
					{
						fprintf(fileAm, "%s", last->content);
						getMacro = 1;
						break;
					}
					else
						last = last->next;
				}
				if(!getMacro)
					fprintf(fileAm, "%s", line);
                        	
			}
		}

		reset_string(line);
		reset_string(content);
	}

	fclose(fileAm);
	fclose(fileAs);
	return fopen(fileNameMacro, "r");
}
	
void get_word(char *line, char word[MAX_WORD_LENGTH], int *index)
{
	int i = 0;
	
	while(isspace(line[*index]))
	{
		(*index)++;
	}

	while((line[*index]) && (!(isspace(line[*index]))) && (line[*index] != ',') && ((*index) < strlen(line)))
	{
		word[i] = line[*index];
		i++;
		(*index)++;
	}
	word[i]='\0';
}

void reset_string(char *string)
{
	int i;
	for(i = 0; i < MAX_LINE; i++)
		string[i] = 0;
}

void read_line(char *line, int *flag, FILE *fileAs, int numLine, error **errorHead, error **currError, error **lastError)
{
	int i = 0;
	char ch;
	char *errMsg = NULL;

	while((ch = fgetc(fileAs))!=EOF && ch!='\n')
	{
		if(i == LINE_LENGTH)
		{
			errMsg = "\nLine to long\n";
			add_error_to_list(errMsg, numLine, errorHead, currError, lastError);			
		} 
		line[i] = ch; /*inserting the character into memory*/
		i++;
	}	

	if(ch == EOF)
		*flag = 0;
	else
		line[i] = '\n';
}
