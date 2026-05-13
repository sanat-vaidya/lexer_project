#ifndef DFA_H
#define DFA_H

typedef struct DFA{  //basically a 5 tuple definition of a DFA, alphabet yet to be defined.
	struct alphabet alphabet;
	
	struct set_of_states states;

	int **transition_table;	
	
	int start_state;
	
	struct set_of_states final_states;
	
} DFA;

struct DFA make_dfa(
    struct set_of_states states, 
    int alphabet_size,
    char alphabet_symbols[],
    int transition_table[][alphabet_size],
    int start_state,
    struct set_of_states final_states 
);

void print_dfa(struct DFA dfa);
int run_dfa(DFA dfa, char *str_ptr);
int is_distinguishable(struct DFA dfa, int num_states, int matrix[][num_states], int pos1, int pos2);
int partition_states(DFA dfa, int distinguishable_matrix[][dfa.states.count],struct set_of_states partitions[]);
int get_partition(struct set_of_states partitions[], int partition_count, int original_state);
void initialize_matrix(struct DFA dfa, int distinguishable_matrix[][dfa.states.count]);
void table_filling_algorithm(struct DFA dfa, int distinguishable_matrix[][dfa.states.count]);
struct DFA minimize_dfa(DFA dfa);


#endif 
