void read_line(char *line, int *flag, FILE *fileAs, int numLine, error **errorHead, error **currError, error **lastError);
void reset_string(char *string);
FILE *pre_process(char fileName[MAX_WORD_LENGTH], macro **macroHead, macro **currMacro, macro **lastMacro, error **errorHead, error **currError, error **lastError);
void get_word(char *line, char word[MAX_WORD_LENGTH], int *index);
