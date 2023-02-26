/*This function calls the first_level function and if there are no errors it calls the second_level function and if there are no errors it prints the entery labels to file*/
void all_functions(char *fileName, macro **macroHead, macro **currMacro, macro **lastMacro, error **errorHead, error **currError, error **lastError);
/*This function is the first pass of the assembler if there are no errors we continue to the second level*/
void first_level(dataRow **dataHead, dataRow **currData, dataRow **lastData, codeRow **codeHead, codeRow **currCode, codeRow **lastCode, error **errorHead, error **currError, error **lastError, symbolRow **smblHead,symbolRow **currSmbl,symbolRow **lastSmbl,entryNode **entryHead,entryNode **entryCurr,entryNode **entryLast);
/*This function reads the next word from the file into a array she accepted and returns the value of the status-(enum) of the end wordword*/
int read_word(char word[MAX_WORD_LENGTH]);
/*checks the word and returns its kind. and num-in-enum, if exists*/
int check_word(char word[MAX_WORD_LENGTH], int *numInEnum, int isOperLine, char **errMsg, int numLine, error **errorHead, error **currError, error **lastError,symbolRow **smblHead);
/*This function complets the code for the labels and builds the machine code*/
void second_level(char *fileName, dataRow **dataHead, dataRow **currData, dataRow **lastData, codeRow **codeHead, codeRow **currCode, error **errorHead, error **currError, error **lastError,symbolRow **smblHead);
/*This function deploys macros and returns a pointer to the deployed file*/
FILE *pre_process(char fileName[MAX_WORD_LENGTH], macro **macroHead, macro **currMacro, macro **lastMacro, error **errorHead, error **currError, error **lastError);
/*This method removes the last label from the list of symbols*/
void delete_ladel(symbolRow **smblHead, symbolRow **lastSmbl);

/*macro*/
/*This function deploys macros and returns a pointer to the deployed file*/
FILE *pre_process(char fileName[MAX_WORD_LENGTH], macro **macroHead, macro **currMacro, macro **lastMacro, error **errorHead, error **currError, error **lastError);
/*This function initializes 'word' to be the first word in 'line'*/
void get_word(char *line, char word[MAX_WORD_LENGTH], int *index);
/*This function resets the string*/
void reset_string(char *string);
/*This function reads line 'numLine' from file 'fileAs' into 'line' and resets flag in case we have reached the end of the file*/
void read_line(char *line, int *flag, FILE *fileAs, int numLine, error **errorHead, error **currError, error **lastError);

/*table functions*/

/*This function creates code row for directive line*/
int add_data_to_list(char *word, int sortWord, int *dc, dataRow **dataHead, dataRow **currData, dataRow **lastData);
/*This function creates a new codeRow node and inserts the opcode of the operand*/
void add_code_to_list(int numOper, int *ic, char word[MAX_WORD_LENGTH], int numLine, codeRow **codeHead, codeRow **currCode, codeRow **lastCode);
/*This function adds label to table of symbols*/
void add_symbol_to_list(int isOper, int isExtrn, int ic, char word[MAX_WORD_LENGTH],symbolRow **currSmbl,symbolRow **smblHead, symbolRow **lastSmbl);
void add_error_to_list(char *errorMsg, int numLine, error **errorHead, error **currError, error **lastError);
void add_macro_to_list(char *name, char *content, macro **macroHead, macro **currMacro, macro **lastMacro); 
void free_all_lists(dataRow *dataHead, codeRow *codeHead, entryNode *entryHead, symbolRow *smblHead, macro *macroHead, error *errorHead);
/*This function checks a word and if correct adds node that contains an extern label to symbol list. it returns the value of the status-(enum) of the end word*/
int create_exter_symbol(int numLine, error **errorHead, error **currError, error **lastError,symbolRow **currSmbl,symbolRow **smblHead, symbolRow **lastSmbl);
/*This function checks a word and if correct adds node that contains an entry label to list. it returns the value of the status-(enum) of the end word*/
int create_entry_node(int numLine, error **errorHead, error **currError, error **lastError,symbolRow **smblHead,entryNode **entryHead,entryNode **entryCurr,entryNode **entryLast);
/*This function writes the name and address of entry label to the .ent file*/
void write_entries(char *fileName, error **errorHead, error **currError, error **lastError,symbolRow **smblHead,entryNode **entryHead);
/*This function prints all the errors meesages and address of error*/
void printErrors(error **errorHead);

/* first functions */
/*checks if label is worng- returns error message if worng if correct returns null*/
char *is_label(char word[MAX_WORD_LENGTH], int numLine, error **errorHead, error **currError, error **lastError, symbolRow **smblHead);
/*This function checks if given string represent an operation, and if so - returns which one otherwise returns -1*/
int is_oper(char *word);
/*checks if word is register- if yes returns num of register otherwise -1*/
int is_register(char *word);
/*checks if word is string- if yes returns 1 otherwise returns 0*/
int is_string(char *word);
/*checks if correct number in operation or directive line- in operation checks if # comes before number if correct returns null otherwise returns error message*/
char *is_number(char *word, int isOperLine);
/*This function checks if given string represent an directive, and if so - returns which one otherwise returns -1*/
int is_directive(char *word);
/*This function checks the format of struct in operation line*/
char * is_struct(char *word, int isOperLine, int numLine);
/*checks if label begins with an alphabetic char- if yes returns null otherwise returns error message*/
char *is_correct_var_name(char *word, int numLine);
/*checks if label is declared second time- if yes returns 1 otherwise 0*/
int check_if_exists_before(char *word, symbolRow **smblHead);
/*goes to end of line and returns 1 if reached end of file and 2 if reached end of line*/
int go_to_end_line();

void from_binary_to_weirld32(char binary[FIELD_SIZE+1], char word32Base[FIELD_SIZE_32_BASE+1]);
void int_to_werid32(int numCode, char word32Base[FIELD_SIZE_32_BASE + 1]);
char get_char_from_int_weird32(int number); 
/*return the address of given label if label is not in list add error to error list and return -1*/
int get_address_of_symbol(char *labelName, int numLine, error **errorHead, error **currError, error **lastError,symbolRow **smblHead);
/*gets label name and return 1 if this label is an external label or -1 if not*/
int is_extern_label(char *label, int ci, error **errorHead, error **currError, error **lastError,symbolRow **smblHead);
int check_if_file_opend(FILE *file);
/*This function writes the name and address of entry label to the .ent file*/
void write_entries(char *fileName);

/* second functions */

/*This function gets numInEnum of operation and returns the amount of arguments that she is ment to get, and initializes the array
to be the kind of source and destantion the operand can get*/
int get_oper_rules(int numInEnum, int canGet[2]);
/*Inserts all characters in binary view into an array of size 10 so that each cell represents a bit*/
void enter_string_into_char_array(char word[], int *dc, dataRow **dataHead, dataRow **currData, dataRow **lastData);
/*This function creates code lines for oper line and returns error message if something is wrong*/
char *enter_fields_into_row(char arguments[MAX_OPER_GET][MAX_WORD_LENGTH], int *ic, int canGet[2], int numLine);
/*This function checks if arguments are correct for the specific operand and how much fields are needed and returns the enum of end word*/
int calculate_oper(int numInEnum, int *ic, int *isCorrect, int numLine, codeRow **currCode, codeRow **lastCode, error **errorHead, error **currError, error **lastError,symbolRow **smblHead);
/*This function checks if arguments are correct for the specific direct line and creates binary code and returns the enum of end word*/
int calculate_data(int numInEnum, int *dc, int *isCorrect, int numLine, dataRow **dataHead, dataRow **currData, dataRow **lastData, error **errorHead, error **currError, error **lastError,symbolRow **smblHead);

/*This function gets num in enum of directive and updates wich kind of variables can come after, and returns how much*/
int get_dircet_rules(int numInEnum, int *canGet);
/*This function checks if arguments are correct for the specific direct and returns enum of end word*/
int enter_data_into_row(int canGet, int *dc, int maxNumVarGet, int numLine, int *isCorrect, dataRow **dataHead, dataRow **currData, dataRow **lastData, error **errorHead, error **currError, error **lastError,symbolRow **smblHead)

void enter_ARE_bits(codeRow *tempCode, int AREtoEnter);
/*Inserts a number in binary view into an array of size 10 so that each cell represents a bit*/
void enter_int_into_bits_array(char array[FIELD_SIZE+1], int toEnter, int numberBits, int startInd);


