#ifndef ENFA_H
#define ENFA_H

typedef struct ENFA{
	struct alphabet alphabet;
	
	struct set_of_states states;

	struct set_of_states **transition_table; 
  // struct of enfa transition table = 
	// [current state][symbol] -> set_of_states
	
	int start_state;
	
	struct set_of_states final_states;
} ENFA;

void print_enfa(struct ENFA enfa);
struct set_of_states eclose(struct ENFA e_nfa, int given_state);
struct set_of_states transition_enfa(struct ENFA e_nfa, struct set_of_states temp, int symbol_index);
int is_same_set(struct set_of_states a, struct set_of_states b);
int in_set(struct set_of_set sets, struct set_of_states temp);
int in_stack(struct set_of_states stack[], int top, struct set_of_states temp);
int get_set_index(struct set_of_set sets, struct set_of_states after_eclose);
struct set_of_states eclose_set(struct ENFA e_nfa, struct set_of_states set);
struct DFA convert_to_dfa(struct ENFA e_nfa);


#endif // ENFA_H
