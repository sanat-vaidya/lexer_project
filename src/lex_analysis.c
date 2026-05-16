#include "../include/utils.h"
#include "../include/dfa.h"
#include "../include/enfa.h"
#include "../include/regex.h"
#include "../include/lexer.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
    if(argc < 3){
        printf("usage: %s <lang.def> <source.c>\n", argv[0]);
        return 1;
    }
    
    Lexer lexer;
    if(load_language(&lexer, argv[1]) == 0){
        return 1;
    }

    lex_file(&lexer, argv[2]);

    return 0;
}
