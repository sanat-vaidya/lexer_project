#include "../include/utils.h"
#include "../include/dfa.h"
#include "../include/enfa.h"
#include "../include/regex.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char *complete_regex(char *regex){
  int len = strlen(regex);

  // generous allocation
  char *res_str = malloc(3 * len + 1);
  if(res_str == NULL){
    printf("malloc failed\n");
    exit(1);
  }

  char *ptr = regex;
  int i = 0;

  int prev_was_operand = 0;

  while(*ptr){

    // ---------- escaped char ----------
    if(*ptr == '\\'){

      ptr++;

      if(*ptr == '\0'){
        printf("Malformed escape sequence\n");
        exit(1);
      }
      // implicit concatenation
      if(prev_was_operand){
        res_str[i++] = '.';
      }
      res_str[i++] = '\\';
      res_str[i++] = *ptr;

      prev_was_operand = 1;
    }
    // ---------- opening bracket ----------
    else if(*ptr == '('){
      // a(b) => a.(b)
      if(prev_was_operand){
        res_str[i++] = '.';
      }
      res_str[i++] = '(';
      prev_was_operand = 0;
    }
    // ---------- regex operators ----------
    else if(is_op(*ptr)){
      res_str[i++] = *ptr;
      // only kleene star behaves like operand end
      prev_was_operand = (*ptr == '*');
    }
    // ---------- normal character ----------
    else{
      // ab => a.b
      if(prev_was_operand){
        res_str[i++] = '.';
      }

      res_str[i++] = *ptr;
      prev_was_operand = 1;
    }
    ptr++;
  }

  res_str[i] = '\0';
  return res_str;
}
static inline void shift_copy_set(
    struct set_of_states *dest,
    const struct set_of_states *src,
    int offset
){
    const int word_shift = offset >> 6;
    const int bit_shift  = offset & 63;

    clear_set(dest);   // <--- IMPORTANT

    if(bit_shift == 0){
        for(int i = WordsNeeded - 1 - word_shift; i >= 0; i--){
            dest->words[i + word_shift] = src->words[i];
        }
    }
    else{
        const int inv_shift = 64 - bit_shift;

        for(int i = WordsNeeded - 1 - word_shift; i >= 0; i--){
            uint64_t curr = src->words[i];

            dest->words[i + word_shift] |= curr << bit_shift;

            if(i + word_shift + 1 < WordsNeeded){
                dest->words[i + word_shift + 1] |=
                    curr >> inv_shift;
            }
        }
    }

    dest->count = src->count;
}

struct ENFA char_to_enfa(struct alphabet *alphabet, char ch){
    //defining new states
    struct set_of_states states;
    clear_set(&states);
    add_to_set(&states,0);
    add_to_set(&states,1);
    
    // build transition table — all empty except state 0 on ch -> {1}
    struct set_of_states table[2][alphabet->count];
    for(int i = 0; i < 2; i++){
        for(int j = 0; j < alphabet->count; j++){
            clear_set(&table[i][j]);  // default empty
        }
    }
    
    // find which index ch maps to and set that transition
    for(int j = 0; j < alphabet->count; j++){
        if(alphabet->symbols[j] == ch){
            add_to_set(&table[0][j],1); // state 0 on ch -> state 1
            break;
        }
    }
    
    struct set_of_states finals;
    clear_set(&finals);
    add_to_set(&finals,1);
    
    return make_enfa(
        states,
        alphabet->count,
        alphabet->symbols,
        table,
        0,   // start state
        finals
    	);
}

struct ENFA concat_op(struct alphabet *alphabet, struct ENFA *exp1, struct ENFA *exp2){
	//defining new states
	int new_state_count = exp1->states.count + exp2->states.count;
	struct set_of_states states;
  clear_set(&states);

	for(int i = 0; i<new_state_count; i++) add_to_set(&states, i);
	
	//defining final states
	struct set_of_states finals;
  clear_set(&finals);
  add_to_set(&finals, new_state_count-1);
	
	int num_alphabet = alphabet->count;
	int exp1_count = exp1->states.count;

	//defining transition table
	struct set_of_states transition_table[new_state_count][num_alphabet];
	
	//Copying all transitions from exp1 and exp2 to new table
	for(int i = 0; i<new_state_count; i++){
		if(i<exp1_count){ //for all states of exp1, copy as it is
			for(int j = 0; j<num_alphabet; j++){
				transition_table[i][j] = exp1->transition_table[i][j];
			}
		}
		else{
			int exp2_index = i - exp1_count; //the index of all states in exp2 will be offset

			//for all states in exp2, the resultant states in its transition table also need to be offsetted
			for(int j = 0; j<num_alphabet; j++){ 
				clear_set(&transition_table[i][j]);

        struct set_of_states *src = &exp2->transition_table[exp2_index][j];

        for(int word_i = 0; word_i < WordsNeeded; word_i++){
          uint64_t word = src->words[word_i];

            while(word){
              int state = word_i * 64 + __builtin_ctzll(word);

              add_to_set(&transition_table[i][j],state + exp1_count);
              word &= (word - 1);
            }
        }
			}
		}
	}
	
	//adding epsilon transition from exp1 to exp2
  clear_set(&transition_table[exp1_count-1][0]);
  add_to_set(&transition_table[exp1_count-1][0], exp1_count);
	
	return make_enfa(
		states,
		alphabet->count,
		alphabet->symbols,
		transition_table,
		0, //start state
		finals
		);
}

struct ENFA or_op(struct alphabet *alphabet, struct ENFA *exp1, struct ENFA *exp2){
	//defining new states
	int new_state_count = exp1->states.count + exp2->states.count + 2;
	struct set_of_states states;
  clear_set(&states);

	for(int i = 0; i<new_state_count; i++) add_to_set(&states, i);
	
	//defining final states
	struct set_of_states finals;
  clear_set(&finals);
  add_to_set(&finals, new_state_count-1);
	
	int num_alphabet = alphabet->count;
	int exp1_count = exp1->states.count;
	int exp2_offset = exp1_count+1;
	int new_final_index = new_state_count-1;

	//defining transition table
	struct set_of_states transition_table[new_state_count][num_alphabet];

	//Copying all transitions from exp1 and exp2 to new table
	for(int i = 0; i<new_state_count; i++){
		if(i == 0){
			//defining epsilon transtions for new start state	
      clear_set(&transition_table[0][0]);
      add_to_set(&transition_table[0][0], 1);
      add_to_set(&transition_table[0][0], exp1_count+1);
			for(int j = 1; j<num_alphabet; j++){
        clear_set(&transition_table[0][j]);
			}
		}
		else if(i>0 && i<=exp1_count){ //for all states of exp1, offset by1
			int exp1_index = i - 1; //the index of all states in exp1 will be offset by 1
			int j = 0;

			if(i == exp1_count){ //is exp1's finalstate
				//here we can directly add epsilon transtion beacuse final states dont have outgoing Xitions
        clear_set(&transition_table[i][0]);
        add_to_set(&transition_table[i][0], new_final_index);
				j = 1; //to ensure epsilon transition isnt written twice 
			}

			//for all states in exp1, the resultant states in its transition table also need to be offsetted
			for(; j < num_alphabet; j++){
        shift_copy_set(&transition_table[i][j],&exp1->transition_table[exp1_index][j],1);
      }
		}
		else if(i>exp1_count && i < new_final_index){
			int exp2_index = i - exp2_offset; //the index of all states in exp2 will be offset
			int j = 0;

			if(i == new_final_index-1){
				clear_set(&transition_table[i][0]);
        add_to_set(&transition_table[i][0], new_final_index);
				j = 1; //to ensure epsilon transition isnt written twice 
			}
			
			//for all states in exp2, the resultant states in its transition table also need to be offsetted
			for(; j < num_alphabet; j++){
        shift_copy_set(&transition_table[i][j],&exp2->transition_table[exp2_index][j],exp2_offset);
      }
		}
		else{
			//setting all transitions from new final state 
			for(int j = 0; j<num_alphabet; j++){
        clear_set(&transition_table[i][j]);
			}
		}
	}
	
	
	return make_enfa(
		states,
		alphabet->count,
		alphabet->symbols,
		transition_table,
		0, //start state
		finals
		);
	
}

struct ENFA kleene_closure_op(struct alphabet *alphabet, struct ENFA *exp1){
	//defining new states
	int new_state_count = exp1->states.count + 2;
  struct set_of_states states;
  clear_set(&states);

	for(int i = 0; i<new_state_count; i++) add_to_set(&states, i);
	
	//defining final states
	struct set_of_states finals;
  clear_set(&finals);
  add_to_set(&finals, new_state_count-1);
	
	int num_alphabet = alphabet->count;
	int exp1_count = exp1->states.count;
	int new_final_index = new_state_count-1;

	//defining transition table
	struct set_of_states transition_table[new_state_count][num_alphabet];
	
	//defining different loops for each set of states and copying them into transition table
	for(int i = 0; i<new_state_count; i++){
		if(i == 0){
			//defining epsilon transtions for new start state	
      clear_set(&transition_table[0][0]);
      add_to_set(&transition_table[0][0], 1);
      add_to_set(&transition_table[0][0], new_final_index);

			for(int j = 1; j<num_alphabet; j++){
				clear_set(&transition_table[0][j]);
			}
			
		}
		else if(i <= exp1_count){ //for all states in exp1
			int exp1_index = i - 1;
			int j = 0;
			
			if(i == exp1_count){
        clear_set(&transition_table[i][0]);
        add_to_set(&transition_table[i][0], 1);
        add_to_set(&transition_table[i][0], new_final_index);
				j = 1;
			}
			
			for(; j<num_alphabet; j++){ 
        shift_copy_set(&transition_table[i][j],&exp1->transition_table[exp1_index][j],1);
      }
		}
		else{
			//define all null transtions for the new final state
			for(int j = 0; j<num_alphabet; j++){ 
        clear_set(&transition_table[i][j]);
			}
		}
	}
	
	
	return make_enfa(
		states,
		alphabet->count,
		alphabet->symbols,
		transition_table,
		0, //start state
		finals
		);

}

struct alphabet find_alphabet(char *regex){
  struct alphabet res_alphabet;
  res_alphabet.count = 0;
  
  //epsilon always at index 0
  res_alphabet.symbols[res_alphabet.count++] = '\0';
  
  char *ptr = regex;
  
  while(*ptr){
    
    //is escape sequence \+ \| \* \(
    if(*ptr == '\\' ){ 
      ptr++;

      if(*ptr == '\0'){
        printf("Malformed escape sequence\n");
        exit(1);
      }

      if(get_index(&res_alphabet,*ptr) == -1){
        res_alphabet.symbols[res_alphabet.count++] = *ptr;
      }

      ptr++;
      continue;

    }

    if(is_op(*ptr) == 0){
      if(get_index(&res_alphabet,*ptr) == -1){
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

  char regex_copy[str_len + 1];
  strcpy(regex_copy, regex);
 
  remove_all_whitespace(regex_copy);
  	
	struct alphabet regex_alphabet= find_alphabet(regex_copy);
  
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
	char *completed_exp = complete_regex(regex_copy);	
	char *postfix = infix_to_postfix(completed_exp);

  if(postfix == NULL || strlen(postfix) == 0){
    printf("Invalid regex: %s\n", regex);
    exit(1);
  }

	struct ENFA enfa_stack[str_len];
	int top = -1;

	char *ptr = postfix;
	
	//struct alphabet myalphabet = alphabet1;
  
  if(is_op(*ptr)){
    printf("Malformed regex\n");
    exit(1);
  }

  if(*ptr == '\\'){
   ptr++;
   if(*ptr == '\0'){
     printf("Malformed escape sequence\n");
     exit(1);
   }
  }
	enfa_stack[++top] = char_to_enfa(&regex_alphabet,*(ptr++));
  
	struct ENFA temp;
	while(top != -1 && *ptr){
    if(top < 0){
      printf("Something wrong with your regex (%s)\n",postfix);
      exit(1);
    }

		if(*ptr == '*'){
			struct ENFA first = enfa_stack[top--];
			temp = kleene_closure_op(&regex_alphabet,&first);
		}
		else if(*ptr == '+' || *ptr == '|'){
			struct ENFA second = enfa_stack[top--];
			struct ENFA first = enfa_stack[top--];
			temp = or_op(&regex_alphabet,&first,&second);
		}
		else if(*ptr == '.'){
			struct ENFA second = enfa_stack[top--];
			struct ENFA first = enfa_stack[top--];
			temp = concat_op(&regex_alphabet,&first,&second);
		}
    else if(*ptr == '\\'){ //handle escaped characters
      ptr++;

      if(*ptr == '\0'){
        printf("Malformed escape sequence\n");
        exit(1);
      }
      
      temp = char_to_enfa(&regex_alphabet, *ptr);
    }
		else{ //This should later be changed to check if the given symbol is in the alphabet
			temp = char_to_enfa(&regex_alphabet,*ptr);
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

struct DFA regex_to_dfa(char *regex){
	struct ENFA result_enfa = convert_to_enfa(regex);
	
	//printf("result:\n");
	//print_enfa(result_enfa);
	
	struct DFA result_dfa = convert_to_dfa(result_enfa);

	//printf("dfa created!\n");
	//print_dfa(result_dfa);

	struct DFA minimized_dfa = minimize_dfa(result_dfa);
	
	//printf("Minimized Dfa:\n");
	//print_dfa(minimized_dfa);

	
	return minimized_dfa; 
}

