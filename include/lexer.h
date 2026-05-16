#ifndef LEXER_H
#define LEXER_H

#include "utils.h"
#include "dfa.h"
#include "regex.h"

#define MAX_TOKEN_NAME  64
#define MAX_REGEX_LEN   2048
#define MAX_RULES       128
#define MAX_TOKEN_LEN   256

// one token rule — name, regex, and compiled dfa
typedef struct TokenRule{
    char name[MAX_TOKEN_NAME];
    char regex[MAX_REGEX_LEN];
    struct DFA dfa;
} TokenRule;

// a single matched token
typedef struct Token{
    char name[MAX_TOKEN_NAME];   // which rule matched
    char value[MAX_TOKEN_LEN];   // actual matched string
} Token;

// the lexer — holds all rules loaded from lang.def
typedef struct Lexer{
    TokenRule rules[MAX_RULES];
    int num_rules;
} Lexer;

int load_language(Lexer *lexer, char *filename);
struct Token next_token(Lexer *lexer, char *input, int *pos);
void lex_file(Lexer *lexer, char *filename);

#endif
