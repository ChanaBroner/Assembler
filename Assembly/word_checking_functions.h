FILE *pre_process(char fileName[MAX_WORD_LENGTH], macro **macroHead, macro **currMacro, macro **lastMacro, error **errorHead, error **currError, error **lastError);
void all_functions(char *fileName, macro **macroHead, macro **currMacro, macro **lastMacro, error **errorHead, error **currError, error **lastError);
void first_level(dataRow **dataHead, dataRow **currData, dataRow **lastData, codeRow **codeHead, codeRow **currCode, codeRow **lastCode, error **errorHead, error **currError, error **lastError, symbolRow **smblHead,symbolRow **currSmbl,symbolRow **lastSmbl,entryNode **entryHead,entryNode **entryCurr,entryNode **entryLast);
int read_word(char word[MAX_WORD_LENGTH]);
int check_word(char word[MAX_WORD_LENGTH], int *numInEnum, int isOperLine, char **errMsg, int numLine, error **errorHead, error **currError, error **lastError,symbolRow **smblHead);
void second_level(char *fileName, dataRow **dataHead, dataRow **currData, dataRow **lastData, codeRow **codeHead, codeRow **lastCode, codeRow **currCode, error **errorHead, error **currError, error **lastError,symbolRow **smblHead);
void delete_ladel(symbolRow **smblHead, symbolRow **lastSmbl);

