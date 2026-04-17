#include "../include/utils.h"
#include "../include/dfa.h"
#include "../include/enfa.h"
#include "../include/regex.h"

char *complete_regex(char *regex){
	char *ptr = regex;
	
	int len = strlen(regex);	
	char *res_str = (char *)(malloc(sizeof(char) * (len*2-1)));

	int i = 0;

	char prev = *ptr;
	res_str[i++] = *ptr;
	ptr++;
	
	
	while(*ptr){
		if(isalnum(*ptr) || *ptr == '('){
			if(prev != '(' && prev != '+' && prev != '.' && prev != '|'){
				res_str[i++] = '.';
				res_str[i++] = *ptr;
			}
			else res_str[i++] = *ptr;
		}
		else{
			res_str[i++] = *ptr;
		}
		
		prev = *ptr;
		ptr++;
	}
	
	res_str[i] = '\0';
	
	return res_str;
}

struct ENFA char_to_enfa(struct alphabet alphabet, char ch){
    //defining new states
    int states[] = {0, 1};
    
    // build transition table — all empty except state 0 on ch -> {1}
    struct set_of_states table[2][alphabet.count];
    for(int i = 0; i < 2; i++){
        for(int j = 0; j < alphabet.count; j++){
            table[i][j].count = 0;  // default empty
        }
    }
    
    // find which index ch maps to and set that transition
    for(int j = 0; j < alphabet.count; j++){
        if(alphabet.symbols[j] == ch){
            table[0][j].count = 1;
            table[0][j].states[0] = 1;  // state 0 on ch -> state 1
            break;
        }
    }
    
    int finals[] = {1};
    
    return make_enfa(
        2,
        states,
        alphabet.count,
        alphabet.symbols,
        table,
        0,   // start state
        1,
        finals
    	);
}

struct ENFA concat_op(struct alphabet alphabet, struct ENFA exp1, struct ENFA exp2){
	//defining new states
	int new_state_count = exp1.states.count + exp2.states.count;
	int states[new_state_count];
	for(int i = 0; i<new_state_count; i++) states[i] = i;
	
	//defining final states
	int finals[] = {new_state_count-1};
	
	int num_alphabet = alphabet.count;
	int exp1_count = exp1.states.count;

	//defining transition table
	struct set_of_states transition_table[new_state_count][alphabet.count];
	
	//Copying all transitions from exp1 and exp2 to new table
	for(int i = 0; i<new_state_count; i++){
		if(i<exp1_count){ //for all states of exp1, copy as it is
			for(int j = 0; j<num_alphabet; j++){
				transition_table[i][j] = exp1.transition_table[i][j];
			}
		}
		else{
			int exp2_index = i - exp1_count; //the index of all states in exp2 will be offset

			//for all states in exp2, the resultant states in its transition table also need to be offsetted
			for(int j = 0; j<num_alphabet; j++){ 
				int curr_count = exp2.transition_table[exp2_index][j].count;
				transition_table[i][j].count = curr_count;
				for(int k = 0; k<curr_count; k++){
					transition_table[i][j].states[k] = exp2.transition_table[exp2_index][j].states[k] + exp1_count;
				}
			}
		}
	}
	
	//adding epsilon transition from exp1 to exp2
	transition_table[exp1_count-1][0].count = 1;
	transition_table[exp1_count-1][0].states[0] = exp1_count;
	
	return make_enfa(
		new_state_count,
		states,
		alphabet.count,
		alphabet.symbols,
		transition_table,
		0, //start state
		1,
		finals
		);
}

struct ENFA or_op(struct alphabet alphabet, struct ENFA exp1, struct ENFA exp2){
	//defining new states
	int new_state_count = exp1.states.count + exp2.states.count + 2;
	int states[new_state_count];
	for(int i = 0; i<new_state_count; i++) states[i] = i;
	
	//defining final states
	int finals[] = {new_state_count-1};
	
	int num_alphabet = alphabet.count;
	int exp1_count = exp1.states.count;
	int exp2_offset = exp1_count+1;
	int new_final_index = new_state_count-1;

	//defining transition table
	struct set_of_states transition_table[new_state_count][alphabet.count];

	//Copying all transitions from exp1 and exp2 to new table
	for(int i = 0; i<new_state_count; i++){
		if(i == 0){
			//defining epsilon transtions for new start state	
			transition_table[0][0].count = 2;
			transition_table[0][0].states[0] = 1;
			transition_table[0][0].states[1] = exp1_count+1;
			for(int j = 1; j<num_alphabet; j++){
				transition_table[0][j].count = 0;
			}
		}
		else if(i>0 && i<=exp1_count){ //for all states of exp1, offset by1
			int exp1_index = i - 1; //the index of all states in exp1 will be offset by 1
			int j = 0;

			if(i == exp1_count){
				//here we can directly add new transtion beacuse final states dont have outgoing Xitions
				transition_table[i][0].count = 1;
				transition_table[i][0].states[0] = new_final_index;
				j = 1;
			}

			//for all states in exp1, the resultant states in its transition table also need to be offsetted
			for(; j<num_alphabet; j++){ 
				int curr_count = exp1.transition_table[exp1_index][j].count;
				transition_table[i][j].count = curr_count;
				for(int k = 0; k<curr_count; k++){
					transition_table[i][j].states[k] = exp1.transition_table[exp1_index][j].states[k] + 1;
				}
			}
		}
		else if(i>exp1_count && i < new_final_index){
			int exp2_index = i - exp2_offset; //the index of all states in exp2 will be offset
			int j = 0;

			if(i == new_final_index-1){
				transition_table[i][0].count = 1;
				transition_table[i][0].states[0] = new_final_index;			
				j = 1;
			}
			
			//for all states in exp2, the resultant states in its transition table also need to be offsetted
			for(; j<num_alphabet; j++){ 
				int curr_count = exp2.transition_table[exp2_index][j].count;
				transition_table[i][j].count = curr_count;
				for(int k = 0; k<curr_count; k++){
				transition_table[i][j].states[k] = exp2.transition_table[exp2_index][j].states[k] + exp2_offset;
				}
			}
		}
		else{
			//setting all transitions from new final state 
			for(int j = 0; j<num_alphabet; j++){
				transition_table[new_final_index][j].count = 0;
			}
						
		}
	}
	
	
	return make_enfa(
		new_state_count,
		states,
		alphabet.count,
		alphabet.symbols,
		transition_table,
		0, //start state
		1,
		finals
		);
	
}

struct ENFA kleene_closure_op(struct alphabet alphabet, struct ENFA exp1){
	//defining new states
	int new_state_count = exp1.states.count + 2;
	int states[new_state_count];
	for(int i = 0; i<new_state_count; i++) states[i] = i;
	
	//defining final states
	int finals[] = {new_state_count-1};
	
	int num_alphabet = alphabet.count;
	int exp1_count = exp1.states.count;
	int new_final_index = new_state_count-1;

	//defining transition table
	struct set_of_states transition_table[new_state_count][alphabet.count];
	
	//defining different loops for each set of states and copying them into transition table
	for(int i = 0; i<new_state_count; i++){
		if(i == 0){
			//defining epsilon transtions for new start state	
			transition_table[0][0].count = 2;
			transition_table[0][0].states[0] = 1;
			transition_table[0][0].states[1] = new_final_index;
			for(int j = 1; j<num_alphabet; j++){
				transition_table[0][j].count = 0;
			}
			
		}
		else if(i <= exp1_count){ //for all states in exp1
			int exp1_index = i - 1;
			int j = 0;
			
			if(i == exp1_count){
				transition_table[i][0].count = 2;
				transition_table[i][0].states[0] = 1;
				transition_table[i][0].states[1] = new_final_index;
				j = 1;
			}
			
			for(; j<num_alphabet; j++){ 
				int curr_count = exp1.transition_table[exp1_index][j].count;
				transition_table[i][j].count = curr_count;
				for(int k = 0; k<curr_count; k++){
					transition_table[i][j].states[k] = exp1.transition_table[exp1_index][j].states[k] + 1;
				}
			}
		}
		else{
			//define all null transtions for the new final state
			for(int j = 0; j<num_alphabet; j++){ 
				transition_table[i][j].count = 0;
			}
		}
	}
	
	
	return make_enfa(
		new_state_count,
		states,
		alphabet.count,
		alphabet.symbols,
		transition_table,
		0, //start state
		1,
		finals
		);

}

struct alphabet find_alphabet(char *regex){
  struct alphabet res_alphabet;
  res_alphabet.count = 0;

  res_alphabet.symbols[res_alphabet.count++] = '\0';
  
  char *ptr = regex;
  
  while(*ptr){
    if(is_op(*ptr) == 0){
      if(get_index(res_alphabet,*ptr) == -1){
        res_alphabet.symbols[res_alphabet.count++] = *ptr;
      } 
    }
    
    ptr++;
  }

  quick_sort_c(res_alphabet.symbols,0,res_alphabet.count-1);

  return res_alphabet;
}

struct ENFA convert_to_enfa(char *regex){
	int str_len = strlen(regex);
	
	struct alphabet regex_alphabet= find_alphabet(regex);
  
	/*
	if(is_regex(*regex) == 0){ // check if it is regex at all (probably will need non regex checking lmao)
		printf("Given string is NOT a regular expression, try again\n");
		return res_enfa;
	}
	*/
	
	/*Problem with a direct postfix expression with no dots, while evaluating all operators other that . dont known how far to go till:
		=> add a helper function that completes a regex:
			ex- 10000 ==> 1.0.0.0.0 yay!
			this turns out to be the only solution because . is important to make e_nfa
	*/
	char *completed_exp = complete_regex(regex);	
	char *postfix = infix_to_postfix(completed_exp);

	struct ENFA enfa_stack[str_len];
	int top = -1;

	char *ptr = postfix;
	
	//struct alphabet myalphabet = alphabet1;

	enfa_stack[++top] = char_to_enfa(regex_alphabet,*(ptr++));

	struct ENFA temp;
	while(top != -1 && *ptr){
		if(*ptr == '*'){
			struct ENFA first = enfa_stack[top--];
			temp = kleene_closure_op(regex_alphabet,first);
		}
		else if(*ptr == '+' || *ptr == '|'){
			struct ENFA second = enfa_stack[top--];
			struct ENFA first = enfa_stack[top--];
			temp = or_op(regex_alphabet,first,second);
		}
		else if(*ptr == '.'){
			struct ENFA second = enfa_stack[top--];
			struct ENFA first = enfa_stack[top--];
			temp = concat_op(regex_alphabet,first,second);
		}
		else{ //This should later be changed to check if the given symbol is in the alphabet
			temp = char_to_enfa(regex_alphabet,*ptr);
		}
		ptr++;
		enfa_stack[++top] = temp;
	}

	free(completed_exp);
	free(postfix);
	return enfa_stack[top];
}

int check_in_regex(char *regex,char *str){
	struct ENFA result_enfa = convert_to_enfa(regex);
	
	//printf("result:\n");
	//print_enfa(result_enfa);
	
	struct DFA result_dfa = convert_to_dfa(result_enfa);

	//printf("dfa created!\n");
	//print_dfa(result_dfa);

	struct DFA minimized_dfa = minimize_dfa(result_dfa);
	
	//printf("Minimized Dfa:\n");
	//print_dfa(minimized_dfa);

	
	return run_dfa(minimized_dfa,str);
}
