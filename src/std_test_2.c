#include "../include/utils.h"
#include "../include/dfa.h"
#include "../include/enfa.h"
#include "../include/regex.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int main(int argc, char *argv[]){
	
	//Read A string
	char str[1000] = "(a|b)*(a|b)(a|b)(a|b)(a|b)(a|b)(a|b)(a|b)(a|b)(a|b)(a|b)(a|b)(a|b)(a|b)(a|b)(a|b)(a|b)(a|b)(a|b)(a|b)(a|b)";
  //initializing string to empty in case no input is given
  
  struct DFA result_dfa = regex_to_dfa(str);
 //print_dfa(result_dfa);

  /*
  while(scanf("%s",str) != -1){
    run_dfa(result_dfa,str);
	  strcpy(str,"\0");
  }
  run_dfa(result_dfa,str);
  
  */
	return 0;
}
