#include <stdio.h>
#include <stdlib.h>

/* Basic idea is to write a regular expression, convert it to an E-NFA -> DFA -> minimize DFA-> run the algorithm
 *

*/

#define MaxStates 100
#define MaxSizeOfAlphabet 2
#define NUMSTATE 5

typedef struct set_of_states{ //A structure that contains a list of states and the count
	int count;
	int states[MaxStates];
}set_of_states;

void copy_states(struct set_of_states *dest, int states[], int size){
	dest->count = size;
	for(int i = 0; i<size; i++){
		dest->states[i] = states[i];
	}
}

void copy_table(int dest[][MaxSizeOfAlphabet], int src[][MaxSizeOfAlphabet], int row_size, int col_size){
	for(int i = 0; i<row_size; i++){
		for(int j =0; j<col_size;j++){
			dest[i][j] = src[i][j];
		}
	}
}


typedef struct alphabet{ //A structure that contains a list of states and the count
	int count;
	char symbols[MaxSizeOfAlphabet];
}alphabet;

void copy_alphabet(struct alphabet *dest, struct alphabet src){
	dest->count = src.count;
	for(int i = 0; i<src.count; i++){
		dest->symbols[i] = src.symbols[i];
	}
}

typedef struct DFA{  //basically a 5 tuple definition of a DFA, alphabet yet to be defined.
	struct alphabet alphabet;
	
	struct set_of_states states;

	int **transition_table;	
	
	int start_state;
	
	struct set_of_states final_states;
	
} DFA;

struct DFA make_dfa(
    int num_states,
    int states[],
    int alphabet_size,
    char alphabet_symbols[],
    int transition_table[][alphabet_size],
    int start_state,
    int num_final_states,
    int final_states[]
){
    DFA dfa;

    // copy alphabet
    dfa.alphabet.count = alphabet_size;
    for(int i = 0; i < alphabet_size; i++){
        dfa.alphabet.symbols[i] = alphabet_symbols[i];
    }

    // copy states
    dfa.states.count = num_states;
    for(int i = 0; i < num_states; i++){
        dfa.states.states[i] = states[i];
    }

    // copy final states
    dfa.final_states.count = num_final_states;
    for(int i = 0; i < num_final_states; i++){
        dfa.final_states.states[i] = final_states[i];
    }

    // allocate transition table
    dfa.transition_table = malloc(num_states * sizeof(int*));

    for(int i = 0; i < num_states; i++){
        dfa.transition_table[i] = malloc(alphabet_size * sizeof(int));
    }

    // copy transition table
    for(int i = 0; i < num_states; i++){
        for(int j = 0; j < alphabet_size; j++){
            dfa.transition_table[i][j] = transition_table[i][j];
        }
    }

    dfa.start_state = start_state;

    return dfa;
}

void print_dfa(struct DFA dfa){

	printf("-------------------------------------\n");

	printf("Alphabet: ");
	for(int i = 0;i<dfa.alphabet.count;i++){
		printf("%c ",dfa.alphabet.symbols[i]);
	}
	printf("\n");
	
	printf("States: ");
	for(int i = 0;i<dfa.states.count;i++){
		printf("%d ",dfa.states.states[i]);
	}
	printf("\n");
	
	printf("Final States: ");
	for(int i = 0;i<dfa.final_states.count;i++){
		printf("%d ",dfa.final_states.states[i]);
	}
	printf("\n");
	
	printf("Transition Table: \n");
	for(int i = 0;i<dfa.states.count;i++){
		for(int j = 0; j<dfa.alphabet.count; j++){			
			printf("%d ",dfa.transition_table[i][j]);
		}
		printf("\n");
	}
	printf("\n");
	
	printf("Start state: ");
	printf("%d ",dfa.start_state);
	printf("\n");
	
	printf("-------------------------------------\n");
	
}

//Currently this functions returns the corresponding index for a given input symbol
int get_index(char ch){
	if(ch == '1') return 1;
	else if(ch == '0') return 0;
	else return -1;
}

//checks if a given state is in the set of final states
int in_final(struct set_of_states final_states,int current_state){
	
	for(int i = 0;i <final_states.count;i++){
		if(final_states.states[i] == current_state) return 1;
	}
	
	return 0;
}

int in_states(int states[],int count,int current_state){
	for(int i = 0;i<count;i++){
		if(states[i] == current_state) return 1;
	}
	
	return 0;
}


//For now the run dfa will on strings contating 0 and 1

//Takes a dfa and a string as input and runs it to find if string willl be accepted
int run_dfa(DFA dfa, char *str_ptr){
	int currstate = dfa.start_state;
	
	while(*str_ptr){
		currstate = dfa.transition_table[currstate][get_index(*str_ptr)];
		
		str_ptr++;
	}
	
	
	if(in_final(dfa.final_states,currstate)){ 
		printf("\nYAYAYYAYAYYA\n"); 
		return 1;
	}
	
	else{
		printf("\nNOOO :(\n");	
		return 0;
	}

}

/*Function to minimize dfa
	- Given a dfa can it be minimized?
	- input = dfa (matrix)
	- output also a dfa (matrix)
*/

int get_position(struct set_of_states states, int state_value){
	//gets the corresponding index for the value of a state
    for(int i = 0; i < states.count; i++){
        if(states.states[i] == state_value) return i;
    }
    return -1;
}

int is_distinguishable(struct DFA dfa, int num_states, int matrix[][num_states], int state1, int state2){
	
	/* VERY VERY VERY important to pass num_states because
	   if we are passing the matrix with size [num_states][num_states]
	   but the function had matrix[][MaxStates] the matrix is stored as [0][0] [0][1]...[0][99]
	   so matrix[2][j] will result in memory location sizeofint*2*(MaxStates) + j which is incorrect
	*/
    if(in_final(dfa.final_states, state1) != in_final(dfa.final_states, state2)){
        return 1;
    }

    for(int i = 0; i < dfa.alphabet.count; i++){
        int Xition_s1 = dfa.transition_table[state1][i]; //this represents the name/value of resultant state and not the index
        int Xition_s2 = dfa.transition_table[state2][i];

        int pos1 = get_position(dfa.states, Xition_s1);
        int pos2 = get_position(dfa.states, Xition_s2);
        
        /*	Assume your states are {10,20,30,40}
			here if you directly pass 10 into matrix[Xition_s1], this will go out of bounds
			hence we need a function (get_pos) to get the position of the value/name of the state
		*/

        if(matrix[pos1][pos2] == 1 || matrix[pos2][pos1] == 1){
            return 1;
        }
    }

    return 0;
}

int partition_states(DFA dfa, int distinguishable_matrix[][dfa.states.count],struct set_of_states partitions[])
{
    int num_states = dfa.states.count;

    int visited[num_states];
    int visited_count = 0;

    int partition_count = 0;

    for(int i = 0; i < num_states; i++){
        int state1 = dfa.states.states[i];

        if(in_states(visited, visited_count, state1) == 0){

            int state_count = 0;

            visited[visited_count++] = state1;
            partitions[partition_count].states[state_count++] = state1;

            for(int j = i + 1; j < num_states; j++){
                int state2 = dfa.states.states[j];

                if(distinguishable_matrix[i][j] == 0 ) //important to pass i and j rather than state1 state2 (they represnt values and not state indices)
													   //because we are partitioning each state and dont care about the actual name/value of that state
				{
					partitions[partition_count].states[state_count++] = state2;
					visited[visited_count++] = state2;
				}
            }

            partitions[partition_count].count = state_count;
            partition_count++;
        }
    }

    return partition_count;
}

int get_partition(struct set_of_states partitions[], int partition_count, int original_state){
	for(int i = 0; i<partition_count; i++){
		int num_states = partitions[i].count;
		for(int j = 0; j< num_states; j++){
			if(original_state == partitions[i].states[j]) return i;
		}
	}
	
	return -1;
}

struct DFA minimize_dfa(DFA dfa){
	int num_states = dfa.states.count;
	
	int distinguishable_matrix[num_states][num_states];
	
	//Initialize matrix while marking final and not final states
	for(int i = 0;i<num_states;i++){
		int state1 = dfa.states.states[i];

		for(int j = 0; j<num_states;j++){ // only consider lower triangle of the matrix
			
			int state2 = dfa.states.states[j];
					
			if(in_final(dfa.final_states,state1) != in_final(dfa.final_states,state2)){
				distinguishable_matrix[i][j] = 1; //again, PASS i and j, not state1 state2
				distinguishable_matrix[j][i] = 1;
			}
			else{
				distinguishable_matrix[i][j] = 0;
				distinguishable_matrix[j][i] = 0;
			}
		}
	}
	
	//run the loop till no new change is made
	int flag = 1;
	
	int pass_count = 0;
	
	while(flag == 1){ //run the code till distinguisable matrix has no changes
		flag = 0;
		
		int i =1;
	
		while(i<num_states){
			int state1 = dfa.states.states[i];
		
			int j = 0;
			while(j<i){
				int state2 = dfa.states.states[j];
				
				if(is_distinguishable(dfa,num_states,distinguishable_matrix,state1,state2)){
					if(distinguishable_matrix[i][j] == 0){
						distinguishable_matrix[i][j] = 1;
						distinguishable_matrix[j][i] = 1;
						flag = 1;
					}
				}
				j++;
			}
			i++;
		}
		
	}

	//How to convert from distuinguishable matrix to dfa?
	
	//Find corresponding group of states that are equivalent and partition
	struct set_of_states partitions[num_states];
	int partition_count = partition_states(dfa,distinguishable_matrix,partitions);
	
	int res_Xition_table[partition_count][dfa.alphabet.count];
	
	for(int i = 0;i<partition_count;i++){
		int res_state = partitions[i].states[0];
		for(int j = 0; j<dfa.alphabet.count; j++){
			int res_state_after_transition = get_partition(partitions, partition_count, dfa.transition_table[res_state][j]);
			res_Xition_table[i][j] = res_state_after_transition;
		}
	}
	
	struct DFA res_dfa;
	
	int res_states[partition_count];
	for(int i = 0; i<partition_count; i++){
		res_states[i] = i;
	}
	
	int res_final_states[partition_count];
	int res_final_states_count = 0;
	for(int i = 0; i<partition_count; i++){
		if(in_final(dfa.final_states,partitions[i].states[0]) == 1){
			res_final_states[res_final_states_count++] = i;
		}
	}
	
	res_dfa = make_dfa(partition_count,res_states,
					   dfa.alphabet.count,dfa.alphabet.symbols,
					   res_Xition_table,
					   get_partition(partitions,partition_count,dfa.start_state),
					   res_final_states_count,
					   res_final_states);
	
	return res_dfa;
	
}
	
int main(){
	
	//Read A string
	char *str_ptr;
		
	char str[1000];
	scanf("%s",str);
	
	str_ptr = str;	
	
	// Consider dfa for if string contains 01
	/*
	struct DFA dfa = {
		{
			2,
			{'0','1'} //defineing the alphabet
		},
		{
			3,
			{0,1,2}, // set of all statss
		},
		
		{
			{1,0}, // transition table
			{1,2},
			{2,2}
		},
		0, // start state
		{
			1,
			{2}, // set of final states
		}
	};
	*/
	
	int states[] = {10, 20, 30, 40};

	char alphabet[] = {'0', '1'};

	int table[4][2] = {
		{20, 30},   // state 10: on 0->20, on 1->30
		{20, 30},   // state 20: on 0->20, on 1->30
		{20, 40},   // state 30: on 0->20, on 1->40  (final)
		{20, 40}    // state 40: on 0->20, on 1->40  (final, duplicate of 30)
	};

	int finals[] = {30, 40};

	DFA test_dfa = make_dfa(
		4,
		states,
		2,
		alphabet,
		table,
		10,        // start state
		2,
		finals
	);
	
	print_dfa(test_dfa);
	
	struct DFA min_dfa = minimize_dfa(test_dfa);
	
	print_dfa(min_dfa);
	
	return 0;
}
