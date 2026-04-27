#include "../include/utils.h"
#include "../include/dfa.h"
#include "../include/enfa.h"
#include "../include/regex.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int main(int argc, char *argv[]){
    struct ENFA result_enfa = convert_to_enfa(argv[1]);
    struct DFA result_dfa = convert_to_dfa(result_enfa);
    printf("dfa done\n");
    
    print_dfa(result_dfa);

    struct DFA minimized_dfa = minimize_dfa(result_dfa);
    printf("minimize done\n");
    
    print_dfa(minimized_dfa);
    
    char str[1000] = "\0";

    while(scanf("%s", str) != -1){
        run_dfa(minimized_dfa, str);
    }
    
    return 0;
}
