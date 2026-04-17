#include "../include/utils.h"
#include "../include/enfa.h"
#include "../include/dfa.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

struct ENFA make_enfa(
    int num_states,
    int states[],
    int alphabet_size,
    char alphabet_symbols[],
    struct set_of_states transition_table[][alphabet_size],
    int start_state,
    int num_final_states,
    int final_states[]
){
    struct ENFA enfa;

    // copy alphabet
    enfa.alphabet.count = alphabet_size;
    for(int i = 0; i < alphabet_size; i++){
        enfa.alphabet.symbols[i] = alphabet_symbols[i];
    }

    // copy states
    enfa.states.count = num_states;
    for(int i = 0; i < num_states; i++){
        enfa.states.states[i] = states[i];
    }

    // convert start state value to position
    enfa.start_state = -1;
    for(int i = 0; i < num_states; i++){
        if(states[i] == start_state) enfa.start_state = i;
    }

    // convert final state values to positions
    enfa.final_states.count = num_final_states;
    for(int i = 0; i < num_final_states; i++){
        for(int j = 0; j < num_states; j++){
            if(states[j] == final_states[i]) enfa.final_states.states[i] = j;
        }
    }

    // allocate transition table
    enfa.transition_table = malloc(num_states * sizeof(struct set_of_states *));
    for(int i = 0; i < num_states; i++){
        enfa.transition_table[i] = malloc(alphabet_size * sizeof(struct set_of_states));
    }

    // copy transition table, converting destination values to positions
    for(int i = 0; i < num_states; i++){
        for(int j = 0; j < alphabet_size; j++){
            int count = transition_table[i][j].count;
            enfa.transition_table[i][j].count = count;
            for(int k = 0; k < count; k++){
                int dest_value = transition_table[i][j].states[k];
                for(int m = 0; m < num_states; m++){
                    if(states[m] == dest_value){
                        enfa.transition_table[i][j].states[k] = m;
                        break;
                    }
                }
            }
        }
    }

    return enfa;
}

void print_enfa(struct ENFA enfa){

	printf("-------------------------------------\n");

	printf("Alphabet: ");
	for(int i = 0;i<enfa.alphabet.count;i++){
		printf("%c ",enfa.alphabet.symbols[i]);
	}
	printf("\n");
	
	printf("States: ");
	for(int i = 0;i<enfa.states.count;i++){
		printf("%d ",enfa.states.states[i]);
	}
	printf("\n");
	
	printf("Final States: ");
	for(int i = 0;i<enfa.final_states.count;i++){
		printf("%d ",enfa.final_states.states[i]);
	}
	printf("\n");
	
	printf("Transition Table: \n");
	for(int i = 0;i<enfa.states.count;i++){
		for(int j = 0; j<enfa.alphabet.count; j++){	
			if(enfa.transition_table[i][j].count == 0) printf("E ");
			for(int k = 0; k<enfa.transition_table[i][j].count; k++){				
				printf("%d,",enfa.transition_table[i][j].states[k]);
			}
			printf(" ");	
		}
		printf("\n");
	}
	printf("\n");
	
	printf("Start state: ");
	printf("%d ",enfa.start_state);
	printf("\n");
	
	printf("-------------------------------------\n");
	
}

/* Found a way to handle epislion cycles!
	for each state in an epsilon cycle, all states are identical!!!
	so we can directly replace them with a new state.
	
	turns out this is HORRIBLY inefficient, so just keep a visited array >:(
*/
struct set_of_states eclose(struct ENFA e_nfa, int given_state){ //recursive function did not work and hence lets try using a stack
											   					 // Courtesy abdur hehehe
	struct set_of_states res_states;
	res_states.count = 0;
	
	//add given state to res
	res_states.states[res_states.count++] = given_state;
	
	//define a stack to add all subsequent E Xitions
	int stack[e_nfa.states.count];
	int top = -1;
	
	stack[++top] = given_state;
	
	while(top != -1){ //while stack is not empty
		int curr_state = stack[top--];
		
		struct set_of_states temp_set = e_nfa.transition_table[curr_state][0];
		
		for(int i = 0; i<temp_set.count; i++){
			int sub_curr_state = temp_set.states[i];
			if(in_states(res_states.states, res_states.count, sub_curr_state) == 0){
				res_states.states[res_states.count++] = sub_curr_state;
				stack[++top] = sub_curr_state;
			}
		}
	}
	
	sort_states(&res_states); //it is important to sort this array so that the comparison becomes easier later on :3
	
	return res_states;
}

struct set_of_states transition_enfa(struct ENFA e_nfa, struct set_of_states temp, int symbol_index){ // basically exactly same as eclose but for a specific symbol

	struct set_of_states res_states;
	res_states.count = 0;
	
	int num_states = temp.count;
	
	for(int i = 0; i<num_states; i++){
		int curr_state = temp.states[i];  // get the actual state position
		struct set_of_states curr_set_states = e_nfa.transition_table[curr_state][symbol_index];
		
		int num_curr_states = curr_set_states.count;
		for(int j = 0; j<num_curr_states; j++){
			if(in_states(res_states.states, res_states.count, curr_set_states.states[j]) == 0){
				res_states.states[res_states.count++] = curr_set_states.states[j];
			}
		}
		
	}
	
	sort_states(&res_states); //it is important to sort this array so that the comparison becomes easier later on :3
	
	return res_states;
}

int is_same_set(struct set_of_states a, struct set_of_states b){
	if(a.count != b.count) return 0;
	
	int num_states = a.count;
	for(int i = 0; i<num_states; i++){
		if(a.states[i] != b.states[i]) return 0;
	}
	
	return 1;
}

int in_set(struct set_of_set sets, struct set_of_states temp){
	int num_sets = sets.count;
	
	for(int i = 0; i<num_sets; i++){
		if(is_same_set(sets.set[i] , temp) == 1) return 1;
	}
	
	return 0;
}

int in_stack(struct set_of_states stack[], int top, struct set_of_states temp){
    for(int i = 0; i <= top; i++){
        if(is_same_set(stack[i], temp)) return 1;
    }
    return 0;
}

int get_set_index(struct set_of_set sets, struct set_of_states after_eclose){
	int num_sets = sets.count;
	
	for(int i =0; i<num_sets; i++ ){
		if(is_same_set(sets.set[i],after_eclose) == 1) return i;
	}
	
	return -1;
}

struct set_of_states eclose_set(struct ENFA e_nfa, struct set_of_states set){
    struct set_of_states res_states;
    res_states.count = 0;

    // take eclose of each state in the set and union the results
    for(int i = 0; i < set.count; i++){
        struct set_of_states curr_eclose = eclose(e_nfa, set.states[i]);

        for(int j = 0; j < curr_eclose.count; j++){
            if(in_states(res_states.states, res_states.count, curr_eclose.states[j]) == 0){
                res_states.states[res_states.count++] = curr_eclose.states[j];
            }
        }
    }

    sort_states(&res_states);  // sort so is_same_set comparison works correctly

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
	result_sets.set[result_sets.count++] = eclose(e_nfa,e_nfa.start_state); 
	
	
	//Define a stack of set of states where we also have a list of visited stacks
	struct set_of_states set_stack[200];
	int top = -1;
	
	set_stack[++top] = result_sets.set[0]; //add ecloseure of q0(start state) to stack
	
	int contains_dead_state = 0;
	//finding all possible set of states reachable in epsilon nfa
	while(top != -1){ //till stack is empty
		struct set_of_states temp = set_stack[top--]; //pop

		int alphabet_size = e_nfa.alphabet.count;
		//all possible transitions for the current state
		for(int a = 1; a < alphabet_size; a++){  // start at 1 to skip epsilon
		    struct set_of_states after_transition = transition_enfa(e_nfa, temp, a);
		    struct set_of_states after_eclose = eclose_set(e_nfa, after_transition);

		    if(after_eclose.count == 0){ // this means dead state!
				contains_dead_state = 1;
		    	continue;
		    }  

		    if(in_set(result_sets, after_eclose) == 0 &&
		       in_stack(set_stack, top, after_eclose) == 0){ // if set is not already considered / in stack, add it to record of sets of states
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
	int res_states[num_sets];
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

	int alphabet_size = e_nfa.alphabet.count;
	for(int i = 0; i<num_sets; i++){
		res_states[i] = i; //adding state names to resulting states in dfa in this loop to avoid repetition
		
		if(i == dead_state_index) continue;
		
		struct set_of_states curr_set = result_sets.set[i];
		
		//find all possible transitions for the given state
		for(int a = 1; a < alphabet_size; a++){  // start at 1 to skip epsilon
		    struct set_of_states after_transition = transition_enfa(e_nfa, curr_set, a);
		    struct set_of_states after_eclose = eclose_set(e_nfa, after_transition); // need to write eclose of set function
			
			//DID not consider what would happen if it lead to a dead state!
			if(after_eclose.count == 0){ //i.e. exists in dead state
				res_transition_table[i][a-1] = dead_state_index;
			}
			else{
				// match it to the respecting index in the dfa and store that value and store in table
				res_transition_table[i][a-1] = get_set_index(result_sets, after_eclose); 
			}
		}
	}
	
	//defining final states
	int res_final_states[num_sets];
	int res_final_count = 0;
	//any set that contains atleast 1 final state
	for(int i = 0; i<num_sets; i++){
		if(i == dead_state_index) continue;
		struct set_of_states curr_set = result_sets.set[i];
		for(int j = 0; j<curr_set.count; j++){
			if(in_final(e_nfa.final_states, curr_set.states[j]) == 1){
				res_final_states[res_final_count++] = i;
				break;
			}
		}
	}
	
	//pass it to make_dfa and return !!!!
	struct DFA res_dfa = make_dfa(
		num_sets,
		res_states,
		res_a_count,
		res_alphabet,
		res_transition_table,
		start_state,
		res_final_count,
		res_final_states
	);
		
	return res_dfa;
}
