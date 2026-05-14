#include "../include/utils.h"
#include "../include/enfa.h"
#include "../include/dfa.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

struct ENFA make_enfa(
    struct set_of_states states,
    int alphabet_size,
    char alphabet_symbols[],
    struct set_of_states transition_table[][alphabet_size],
    int start_state,
    struct set_of_states final_states 
){
    struct ENFA enfa;

    // copy alphabet
    enfa.alphabet.count = alphabet_size;
    for(int i = 0; i < alphabet_size; i++){
        enfa.alphabet.symbols[i] = alphabet_symbols[i];
    }

    // copy states
    int num_states = states.count;
    enfa.states = states;

    // given start state is same as state position so directly copy
    enfa.start_state = start_state;

    // convert final state values to positions
    enfa.final_states = final_states;

    // allocate transition table
    enfa.transition_table = malloc(num_states * sizeof(struct set_of_states *));
    for(int i = 0; i < num_states; i++){
        enfa.transition_table[i] = malloc(alphabet_size * sizeof(struct set_of_states));
    }

    // copy transition table, converting destination values to positions
    for(int i = 0; i < num_states; i++){
        memcpy(
            enfa.transition_table[i],
            transition_table[i],
            alphabet_size * sizeof(struct set_of_states)
        );
    }

    return enfa;
}

void print_enfa(struct ENFA *enfa){
    printf("-------------------------------------\n");

    // Alphabet
    printf("Alphabet: ");
    for(int i = 0; i < enfa->alphabet.count; i++){
        printf("%c ", enfa->alphabet.symbols[i]);
    }
    printf("\n");

    // States
    printf("States:\n");
    for(int i = 0; i < WordsNeeded; i++){
        print_binary(enfa->states.words[i]);
    }
    printf("\n");

    // Final States
    printf("Final States:\n");
    for(int i = 0; i < WordsNeeded; i++){
        print_binary(enfa->final_states.words[i]);
    }
    printf("\n");

    // Transition Table
    int num_states = enfa->states.count;
    printf("Transition Table:\n");

    for(int i = 0; i < num_states; i++){
    printf("q%d: ", i);
    for(int j = 0;
        j < enfa->alphabet.count;
        j++)
    {
        printf("%c -> ",
               enfa->alphabet.symbols[j]);

        print_set(
            enfa->transition_table[i][j]
        );

        printf("   ");
    }

    printf("\n");
}

    // Start state
    printf("Start state: %d\n",
           enfa->start_state);

    printf("-------------------------------------\n");
}

void union_set(struct set_of_states *dest,const struct set_of_states *src){
    for(int i = 0; i < WordsNeeded; i++){
        uint64_t new_bits = src->words[i] & ~dest->words[i];

        dest->count += __builtin_popcountll(new_bits);

        dest->words[i] |= src->words[i];
    }
}


/* Found a way to handle epislion cycles!
	for each state in an epsilon cycle, all states are identical!!!
	so we can directly replace them with a new state.
	
	turns out this is HORRIBLY inefficient, so just keep a visited array >:(
*/
struct set_of_states eclose(struct ENFA *e_nfa, int given_state){ //recursive function did not work and hence lets try using a stack
											   					 // Courtesy abdur hehehe
	struct set_of_states res_states;
  clear_set(&res_states);
	
	//add given state to res
  add_to_set(&res_states, given_state);

	//define a stack to add all subsequent E Xitions
	int stack[e_nfa->states.count];
	int top = -1;
	
	stack[++top] = given_state;
	
	while(top != -1){ //while stack is not empty
		int curr_state = stack[top--];
		
		struct set_of_states *temp_set = &e_nfa->transition_table[curr_state][0];
		
    for(int word_i = 0; word_i < WordsNeeded;word_i++){
      uint64_t word = temp_set->words[word_i];

      // skip empty words
      while(word){
        // index of lowest set bit
        int offset = __builtin_ctzll(word);

        int sub_curr_state = word_i * 64 + offset;

        // bounds check
        if(sub_curr_state < e_nfa->states.count){
          // not visited
          if(!in_set(&res_states, sub_curr_state)){
            add_to_set(&res_states, sub_curr_state);
            stack[++top] = sub_curr_state;
          }
        }
        // remove processed bit
        word &= (word - 1);
      }
    }
	}
	
	return res_states;
}

struct set_of_states transition_enfa(struct ENFA *e_nfa, struct set_of_states *temp, int symbol_index){ // basically exactly same as eclose but for a specific symbol

  struct set_of_states res_states;
  clear_set(&res_states);

  int num_states = e_nfa->states.count;

  /* new algorithm for bitsets!
    for(each curr_state in temp)
      temp_set = set from transtion on symbol_index

      for(each word in temp_set)
        OR it with res_states wrods by direct comparison
        <main problem is keeping res_states.count coherent>
  */

  for(int word_i = 0; word_i<WordsNeeded; word_i++){
    uint64_t word1 = temp->words[word_i];

    while(word1){ //skip empty words
      int curr_state = word_i * 64 +__builtin_ctzll(word1); //find first state from the left
      
      struct set_of_states *temp_set = &e_nfa->transition_table[curr_state][symbol_index]; //get resultant set of states on Xition
      
      //merge/ union of words in temp_set and res_states
      for(int word_j = 0; word_j<WordsNeeded; word_j++){
        uint64_t word2 = temp_set->words[word_j];
       
        //words temp_set but not in res_states
        uint64_t new_bits = word2 & ~res_states.words[word_j];

        res_states.count += __builtin_popcountll(new_bits);
        
        //Union of word in temp_set and res_states
        res_states.words[word_j] |= word2; 
        
      }
      word1 &= (word1 -1);
    }
  }

	return res_states;
}

int is_same_set(struct set_of_states *a, struct set_of_states *b){
	if(a->count != b->count) return 0;
	
  for(int i=0; i<WordsNeeded; i++){
    if(a->words[i] != b->words[i]) return 0;
  }	
	
	return 1;
}

int in_set_of_sets(struct set_of_set *sets, struct set_of_states *temp){
	int num_sets = sets->count;
	
	for(int i = 0; i<num_sets; i++){
		if(is_same_set(&sets->set[i] , temp) == 1) return 1;
	}
	
	return 0;
}

int in_stack(struct set_of_states stack[], int top, struct set_of_states *temp){
    for(int i = 0; i <= top; i++){
        if(is_same_set(&stack[i], temp)) return 1;
    }
    return 0;
}

int get_set_index(struct set_of_set *sets, struct set_of_states *after_eclose){
	int num_sets = sets->count;
	
	for(int i =0; i<num_sets; i++ ){
		if(is_same_set(&sets->set[i],after_eclose) == 1) return i;
	}
	
	return -1;
}

struct set_of_states eclose_set(struct ENFA *e_nfa, struct set_of_states *set){
  struct set_of_states res_states;
  clear_set(&res_states);
  
  int num_states = e_nfa->states.count;

  //for each state in set, take eclose and add to res_states
  for(int word_i = 0; word_i<WordsNeeded; word_i++){
    uint64_t word1 = set->words[word_i];

    while(word1){ //skip empty words
      int curr_state = word_i * 64 +__builtin_ctzll(word1); //find first state from the left

      if(curr_state >= num_states) break;

      struct set_of_states after_eclose = eclose(e_nfa, curr_state); //get resultant set of states on Xition

      //merge/ union of words in temp_set and res_states
      for(int word_j = 0; word_j<WordsNeeded; word_j++){
        uint64_t word2 = after_eclose.words[word_j];

        //words temp_set but not in res_states
        uint64_t new_bits = word2 & ~res_states.words[word_j];

        res_states.count += __builtin_popcountll(new_bits);

        //  Union of word in temp_set and res_states
        res_states.words[word_j] |= word2;  
      }
      word1 &= (word1 -1);
    }
  }


  return res_states;
}

struct DFA convert_to_dfa(struct ENFA e_nfa){
	/* start by taking eclose of the start state of nfa
	 * then starting from the set of all start states,perform all possible transitions and store them in an array of set_of_states
	 * (you have to make sure no state is repeated)
	 * start defning the components of a dfa
	 * 0 will be start state because we store the set of start states at loc 0 in array of set_of_states
	 * transition table has been explained below
	*/
	
	//Keep track of all possible set of states reachable by the nfa on all possible transitions.
	struct set_of_set result_sets; 
	result_sets.count = 0;
	
	//starting with eclose of start state on nfa
	result_sets.set[result_sets.count++] = eclose(&e_nfa,e_nfa.start_state); 

	int alphabet_size = e_nfa.alphabet.count;
	
	//Define a stack of set of states where we also have a list of visited stacks
	struct set_of_states set_stack[200];
	int top = -1;
	
	set_stack[++top] = result_sets.set[0]; //add ecloseure of q0(start state) to stack
	
	int contains_dead_state = 0;
	//finding all possible set of states reachable in epsilon nfa
	while(top != -1){ //till stack is not empty
		struct set_of_states temp = set_stack[top--]; //pop

		//all possible transitions for the current state
		for(int a = 1; a < alphabet_size; a++){  // start at 1 to skip epsilon (epsilon always stored at 0)
		    struct set_of_states after_transition = transition_enfa(&e_nfa, &temp, a);
		    struct set_of_states after_eclose = eclose_set(&e_nfa, &after_transition);

		    if(!contains_dead_state && !after_eclose.count){ // this means dead state!
				  contains_dead_state = 1;
		    	continue;
		    }  

		    if(!in_set_of_sets(&result_sets, &after_eclose) &&
		       !in_stack(set_stack, top, &after_eclose)){ // if set is not already considered / in stack, add it to record of sets of states
		        result_sets.set[result_sets.count++] = after_eclose;
		        set_stack[++top] = after_eclose;
		    }
		}
	}
	
	//defining all attributes of the dfa

	//defining start state
	int start_state = 0;

	//defining alphabet
	int res_a_count = e_nfa.alphabet.count-1;
	char res_alphabet[res_a_count];
	for(int i = 0; i<res_a_count ; i++){
		res_alphabet[i] = e_nfa.alphabet.symbols[i+1];
	}
	
	//defning all possible states in dfa
	int num_sets = (contains_dead_state == 1) ? result_sets.count+1 : result_sets.count;
	//if it contains a dead state, will need to add another state to the dfa
	struct set_of_states res_states;
  clear_set(&res_states);

	int dead_state_index = -1;

	//defining transition fucntion
	int res_transition_table[num_sets][e_nfa.alphabet.count-1];
	
	//if dead state exists define it to be the last index
	if(contains_dead_state == 1){
		dead_state_index = num_sets-1;
		for(int a = 0; a < res_a_count; a++){ //each transtion will lead to itself
			res_transition_table[dead_state_index][a] = dead_state_index;
		}
	}

	for(int i = 0; i<num_sets; i++){
		add_to_set(&res_states,i); //adding state names to resulting states in dfa in this loop to avoid repetition
		
		if(i == dead_state_index) continue;
		
		struct set_of_states curr_set = result_sets.set[i];
		
		//find all possible transitions for the given state
		for(int a = 1; a < alphabet_size; a++){  // start at 1 to skip epsilon
		    struct set_of_states after_transition = transition_enfa(&e_nfa, &curr_set, a);
		    struct set_of_states after_eclose = eclose_set(&e_nfa, &after_transition); // need to write eclose of set function
			
			//DID not consider what would happen if it lead to a dead state!
			if(after_eclose.count == 0){ //i.e. exists in dead state
				res_transition_table[i][a-1] = dead_state_index;
			}
			else{
				// match it to the respecting index in the dfa and store that value and store in table
				res_transition_table[i][a-1] = get_set_index(&result_sets, &after_eclose); 
			}
		}
	}
	
	//defining final states
	struct set_of_states res_final_states;
  clear_set(&res_final_states);
	//any set that contains atleast 1 final state
	
  for(int i = 0; i<num_sets; i++){
		if(i == dead_state_index) continue;

		struct set_of_states *curr_set = &result_sets.set[i];
    
    int is_final = 0;

	  for(int word_i=0; word_i<WordsNeeded; word_i++){
      uint64_t word = curr_set->words[word_i];
      
      while(word){

        int state = word_i * 64 +__builtin_ctzll(word);

        if(in_set(&e_nfa.final_states, state)){
          add_to_set(&res_final_states, i);

          is_final=1;
          break;
        }
        word &= (word-1);
      }

    }
	}
	
	//pass it to make_dfa and return !!!!
	struct DFA res_dfa = make_dfa(
		res_states,
		res_a_count,
		res_alphabet,
		res_transition_table,
		start_state,
		res_final_states
	);
		
	return res_dfa;
}
