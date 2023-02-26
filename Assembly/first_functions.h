char *is_label(char word[MAX_WORD_LENGTH], int numLine, error **errorHead, error **currError, error **lastError, symbolRow **smblHead);
int is_oper(char *word);
int check_if_label_exists(char *word);
int is_register(char *word);
int is_string(char *word);
char *is_number(char *word, int isOperLine);
int is_directive(char *word);
char * is_struct(char *word, int isOperLine, int numLine);
char *is_correct_var_name(char *word, int numLine);
int check_if_exists_before(char *word, symbolRow **smblHead);
int go_to_end_line();

void from_binary_to_weirld32(char binary[FIELD_SIZE+1], char word32Base[FIELD_SIZE_32_BASE+1]);
void int_to_werid32(int numCode, char word32Base[FIELD_SIZE_32_BASE + 1]);
char get_char_from_int_weird32(int number);
int get_address_of_symbol(char *labelName, int numLine, error **errorHead, error **currError, error **lastError,symbolRow **smblHead);
int is_extern_label(char *label, int ic, error **errorHead, error **currError, error **lastError,symbolRow **smblHead);





