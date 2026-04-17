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
	char *str_ptr;
	
	char str[1000] = "\0"; //initializing string to empty in case no input is given
	
	str_ptr = str;
	
	scanf("%s",str);


	check_in_regex(argv[1],str);

	return 0;
}
