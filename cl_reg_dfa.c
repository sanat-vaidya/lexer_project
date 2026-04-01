#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*
	int states[] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};

	char alphabet[] = {'0', '1'};

	//                   0    1
	int table[10][2] = {
		{10,  20},   // q0(val10):  0->q0,  1->q1
		{20,  30},   // q1(val20):  0->q1,  1->q2
		{30,  40},   // q2(val30):  0->q2,  1->q3
		{40,  10},   // q3(val40):  0->q3,  1->q0
		{50,  60},   // q4(val50):  0->q4,  1->q5  (duplicate of q0)
		{60,  70},   // q5(val60):  0->q5,  1->q6  (duplicate of q1)
		{70,  80},   // q6(val70):  0->q6,  1->q7  (duplicate of q2)
		{80,  50},   // q7(val80):  0->q7,  1->q4  (duplicate of q3)
		{90,  100},  // q8(val90):  0->q8,  1->q9  (duplicate of q0)
		{100, 30},   // q9(val100): 0->q9,  1->q2  (duplicate of q1... almost)
	};

	int finals[] = {10, 50, 90};  // all "0 ones mod 4" states are final

	DFA test_dfa = make_dfa(
		10,
		states,
		2,
		alphabet,
		table,
		10,   // start state
		3,
		finals
	);
--------------------------------------------------------------------------------------
	printf("\n");
	for(int i = 0 ;i<partition_count; i++){
		for(int j = 0;j<partitions[i].count;j++){
			printf("%d ",partitions[i].states[j]);
		}
		
		printf("\n");
	}
	printf("\n");
-------------------------------------------------------------------------------------
	printf("\n");
	for(int i = 0;i<num_states;i++){
		for(int j = 0;j<num_states;j++){
			printf("%d ",distinguishable_matrix[i][j]);
		}
		printf("\n");
	}
	printf("\n");
------------------------------------------------------------------------------------
	struct set_of_states table[5][3] = {
		//    epsilon        '0'          '1'
		{{1,{1}}, {1,{0}}, {1,{0}}},   // state 0
		{{0,{}},  {1,{2}}, {0,{}}},    // state 1
		{{0,{}},  {0,{}},  {1,{3}}},   // state 2
		{{0,{}},  {0,{}},  {1,{4}}},   // state 3
		{{0,{}},  {0,{}},  {0,{}}},    // state 4
	};

	int enfa_states[] = {0, 1, 2, 3, 4};
	char enfa_alphabet[] = {'e', '0', '1'};
	int enfa_finals[] = {4};

	ENFA test_enfa = make_enfa(
		5,
		enfa_states,
		3,
		enfa_alphabet,
		table,
		0,       // start state
		1,
		enfa_finals
	);
------------------------------------------------------------------------------------
	int st[] = {0,1,2,3};
	char al[] = {'0','1'};
	int tab[4][2] = {
		{1,0},
		{1,2},
		{3,3},
		{3,3}
	};
	int fin[] = {2,3};
	//----------------------------------

	struct set_of_states table2[4][3] = {
		//   epsilon    '0'       '1'
		{{1,{1}}, {0,{}},      {1,{3}}},   // state 0: e->{1}, 0->{},  1->{3}
		{{0,{}},     {1,{2}},  {1,{3}}},   // state 1: e->{},  0->{2}, 1->{3}
		{{0,{}},     {1,{2}},  {1,{3}}},   // state 2: e->{},  0->{2}, 1->{3}
		{{1,{1}}, {0,{}},      {0,{}}},       // state 3: e->{1}, 0->{},  1->{}
	};

	int enfa_states2[] = {0, 1, 2, 3};
	char enfa_alphabet2[] = {'e', '0', '1'};
	int enfa_finals2[] = {3};

	ENFA test_enfa2 = make_enfa(
		4,
		enfa_states2,
		4,
		enfa_alphabet2,
		table2,
		0,
		1,
		enfa_finals2
	);

	struct DFA converted_dfa2 = convert_to_dfa(test_enfa2);
	print_dfa(converted_dfa2);
	run_dfa(converted_dfa2, str_ptr);
------------------------------------------------------------------------------------
------------------------------------------------------------------------------------
*/

/* Basic idea is to write a regular expression -> E-NFA(done!) -> DFA(done!) -> minimize DFA (done!) -> run the algorithm (done!)
 *

*/

#define MaxStates 100
#define MaxSizeOfAlphabet 30
#define NUMSTATE 5
#define MAXREGEXLEN 200

void swap(int *a, int *b){
	int temp = *a;
	*a = *b;
	*b = temp;
}

int partition(int arr[], int low, int high){
	int pivot = arr[high];
	
	int pos_pivot = low-1;
	
	for(int j = low; j<high; j++){
		if(arr[j] < pivot){
			pos_pivot++;
			swap(&arr[pos_pivot], &arr[j]);
		}
	}
	
	swap(&arr[pos_pivot+1] , &arr[high]);
	return pos_pivot+1;
}

void quick_sort(int arr[], int low, int high){
	if(low<high){
		int pivot = partition(arr,low,high);
		
		quick_sort(arr,low, pivot -1);
		quick_sort(arr, pivot+1, high);
	}
}

void swap_c(char *a, char *b){
	char temp = *a;
	*a = *b;
	*b = temp;
}

int partition_c(char arr[], char low, char high){
	char pivot = arr[high];
	
	int pos_pivot = low-1;
	
	for(int j = low; j<high; j++){
		if(arr[j] < pivot){
			pos_pivot++;
			swap_c(&arr[pos_pivot], &arr[j]);
		}
	}
	
	swap_c(&arr[pos_pivot+1] , &arr[high]);
	return pos_pivot+1;
}

void quick_sort_c(char arr[], int low, int high){
	if(low<high){
		int pivot = partition_c(arr,low,high);
		
		quick_sort_c(arr,low, pivot -1);
		quick_sort_c(arr, pivot+1, high);
	}
}

typedef struct set_of_states{ //A structure that contains a list of states and the count
	int count;
	int states[MaxStates];
}set_of_states;

typedef struct set_of_set{
	int count;
	struct set_of_states set[200];
}set_of_set;

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

void sort_states(struct set_of_states *set){
	quick_sort(set->states,0,set->count-1);
}


typedef struct alphabet{ //A structure that contains a list of states and the count
	int count;
	char symbols[MaxSizeOfAlphabet];
}alphabet;

struct alphabet alphabet1 = {
	27,
	"\0abcdefghijklmnopqrstuvwxyz"
};

struct alphabet alphabet2 = {
	3,
	"\01"
};

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

typedef struct ENFA{
	struct alphabet alphabet;
	
	struct set_of_states states;

	struct set_of_states **transition_table; // struct of enfa transition table = 
						  					 // [current state][symbol] -> set_of_states
	
	int start_state;
	
	struct set_of_states final_states;
} ENFA;

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
    ENFA enfa;

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

    // copy final states, converting values to positions
    dfa.final_states.count = num_final_states;
    for(int i = 0; i < num_final_states; i++){
        for(int j = 0; j < num_states; j++){
            if(states[j] == final_states[i]) dfa.final_states.states[i] = j;
        }
    }

    // allocate transition table
    dfa.transition_table = malloc(num_states * sizeof(int*));

    for(int i = 0; i < num_states; i++){
        dfa.transition_table[i] = malloc(alphabet_size * sizeof(int));
    }

    // copy transition table, converting destination values to positions
    for(int i = 0; i < num_states; i++){
        for(int j = 0; j < alphabet_size; j++){
            int dest_value = transition_table[i][j];
            
            if(dest_value == -1){  // dead state sentinel, skip conversion
		        dfa.transition_table[i][j] = -1;
		        continue;
		    }
            
            for(int k = 0; k < num_states; k++){
                if(states[k] == dest_value){
                    dfa.transition_table[i][j] = k; // store position, not value
                    break;
                }
            }
        }
    }

    // convert start state value to position
    dfa.start_state = -1;
    for(int i = 0; i < num_states; i++){
        if(states[i] == start_state) dfa.start_state = i;
    }

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

//Currently this functions returns the corresponding index for a given input symbol
int get_index(struct alphabet alphabet, char ch){
	int size = alphabet.count;
	for(int i = 0; i<size; i++){
		if(alphabet.symbols[i] == ch) return i;
	}
	
	return -1;
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
		int alpha_index = get_index(dfa.alphabet,*str_ptr);
		if(alpha_index == -1){
			printf("Char: '%c' NOT inalphabet\n",*str_ptr);
			printf("\nNOOO :(\n");	
			return 0;
		}
		currstate = dfa.transition_table[currstate][alpha_index];
		
		if(currstate == -1){
			printf("\nNOOO :(\n");	
			return 0;
		}
		
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

int get_position(struct set_of_states states, int state_value){
	//gets the corresponding index for the value of a state
    for(int i = 0; i < states.count; i++){
        if(states.states[i] == state_value) return i;
    }
    return -1;
}

int is_distinguishable(struct DFA dfa, int num_states, int matrix[][num_states], int pos1, int pos2){
	
	/* VERY VERY VERY important to pass num_states because
	   if we are passing the matrix with size [num_states][num_states]
	   but the function had matrix[][MaxStates] the matrix is stored as [0][0] [0][1]...[0][99]
	   so matrix[2][j] will result in memory location sizeofint*2*(MaxStates) + j which is incorrect
	*/
    if(in_final(dfa.final_states, pos1) != in_final(dfa.final_states, pos2)){
        return 1;
    }

    for(int i = 0; i < dfa.alphabet.count; i++){
        // transition table now stores positions directly, no need for get_position
        int dest1 = dfa.transition_table[pos1][i];
        int dest2 = dfa.transition_table[pos2][i];
        
        /*	Assume your states are {10,20,30,40}
			here if you directly pass 10 into matrix[Xition_s1], this will go out of bounds
			hence we need a function (get_pos) to get the position of the value/name of the state
			-- now handled in make_dfa, transition table stores positions not values
		*/

        if(matrix[dest1][dest2] == 1 || matrix[dest2][dest1] == 1){
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

        if(in_states(visited, visited_count, i) == 0){ // check position not value

            int state_count = 0;

            visited[visited_count++] = i;
            partitions[partition_count].states[state_count++] = i; // store position not value

            for(int j = i + 1; j < num_states; j++){

                if(distinguishable_matrix[i][j] == 0) //important to pass i and j rather than state1 state2 (they represnt values and not state indices)
											           //because we are partitioning each state and dont care about the actual name/value of that state
				{
					partitions[partition_count].states[state_count++] = j; // store position not value
					visited[visited_count++] = j;
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

void initialize_matrix(struct DFA dfa, int distinguishable_matrix[][dfa.states.count]){
	
	int num_states = dfa.states.count;
	
	//Initialize matrix while marking final and not final states
	for(int i = 0;i<num_states;i++){
		for(int j = 0; j<num_states;j++){ // only consider lower triangle of the matrix
			// pass i and j (positions) not state values
			if(in_final(dfa.final_states,i) != in_final(dfa.final_states,j)){
				distinguishable_matrix[i][j] = 1; //again, PASS i and j, not state1 state2
				distinguishable_matrix[j][i] = 1;
			}
			else{
				distinguishable_matrix[i][j] = 0;
				distinguishable_matrix[j][i] = 0;
			}
		}
	}
}

void table_filling_algorithm(struct DFA dfa, int distinguishable_matrix[][dfa.states.count]){
	int num_states = dfa.states.count;
	
	initialize_matrix(dfa,distinguishable_matrix);
	
	//run the loop till no new change is made
	int flag = 1;
	int pass_count = 0;
	while(flag == 1){ //run the code till distinguisable matrix has no changes
		flag = 0;
		int i =1; //if i was not set to 1 inside this loop, loop wont run a second time.
		while(i<num_states){
			int j = 0;
			while(j<i){
				// pass i and j (positions) directly instead of state values
				if(is_distinguishable(dfa,num_states,distinguishable_matrix,i,j)){
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
	
}

struct DFA minimize_dfa(DFA dfa){
	int num_states = dfa.states.count;
		
	int distinguishable_matrix[num_states][num_states];
	
	//Run table filling algorithm and update the distinguishable_table
	table_filling_algorithm(dfa,distinguishable_matrix);

	//How to convert from distuinguishable matrix to dfa?
	//initialize new dfa to return
	struct DFA res_dfa;
	
	//Define Xititon table for min dfa
	//Find corresponding group of states that are equivalent and partition
	struct set_of_states partitions[num_states];
	int partition_count = partition_states(dfa,distinguishable_matrix,partitions);
	
	int res_Xition_table[partition_count][dfa.alphabet.count];
	
	for(int i = 0;i<partition_count;i++){
		int res_state = partitions[i].states[0]; // already a position
		for(int j = 0; j<dfa.alphabet.count; j++){
			int res_state_after_transition = get_partition(partitions, partition_count, dfa.transition_table[res_state][j]);
			res_Xition_table[i][j] = res_state_after_transition;
		}
	}

	//make set of states in min dfa
	int res_states[partition_count];
	for(int i = 0; i<partition_count; i++){
		res_states[i] = i;
	}
	
	//make set of final states in min dfa
	int res_final_states[partition_count];
	int res_final_states_count = 0;
	for(int i = 0; i<partition_count; i++){
		if(in_final(dfa.final_states,partitions[i].states[0]) == 1){ // states[0] is now a position, in_final compares positions
			res_final_states[res_final_states_count++] = i;
		}
	}
	
	//make new dfa
	res_dfa = make_dfa(partition_count,res_states,
					   dfa.alphabet.count,dfa.alphabet.symbols,
					   res_Xition_table,
					   get_partition(partitions,partition_count,dfa.start_state),
					   res_final_states_count,
					   res_final_states);
	
	return res_dfa;
	
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


int precedence(char ch){
	 if(ch == '*') return 3;
	 else if(ch == '.') return 2;
	 else if(ch == '+' || ch == '|') return 1;
	 else return -1;
}

int take_from_stack(){

}

int is_op(char ch) {
    return (ch == '*' || ch == '+' || ch == '|' || ch == '.' || ch == '(' || ch == ')');
}

char *infix_to_postfix(char *in){
	int in_len = strlen(in);
	
	char stack[in_len + 1];
	int top = -1;
	
	
	char *postfix = (char *)malloc(sizeof(char) * (in_len+1));
	int i = 0;
	
	while (*in) {
		char c = *in;
    	if(isalnum(c)){
    		postfix[i++] = c;
    	}
    	else if(c == '('){
    		stack[++top] = '(';
    	}
    	else if(c == ')'){
    		while(top != -1 && stack[top] != '('){
    			postfix[i++] = stack[top--];
    		}
    		top--;
    	}
    	else{
    		while(top != -1 && stack[top] != '(' && (precedence(stack[top]) > precedence(c) || (precedence(stack[top]) == precedence(c)))){
    			postfix[i++] = stack[top--];
    		}
    		stack[++top] = c;
    	}
    	in++;
    }
    
    while(top != -1){
    	postfix[i++] = stack[top--];
    }

    postfix[i] = '\0';
    
    return postfix;
}

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
	//print_enfa(result);
	
	struct DFA result_dfa = convert_to_dfa(result_enfa);

	//printf("dfa created!\n");
	//print_dfa(result_dfa);

	struct DFA minimized_dfa = minimize_dfa(result_dfa);
	
	//printf("Minimized Dfa:\n");
	//print_dfa(minimized);

	
	return run_dfa(minimized_dfa,str);
}

int main(int argc, char *argv[]){
	
	//Read A string
	char *str_ptr;
	
	char str[1000] = "\0"; //initializing string to empty in case no input is given
	
	str_ptr = str;
	
	scanf("%s",str);


	check_in_regex(argv[1],str);

	return 0;
}

/* Bug Tracker(*) / Increasing Efficiency(-) 
 * when adding random names for states, giving seg fault -> somewhere indexing is not being done right 
   => [add the same state names to transition table]
 * in all char->enfa functions,need to properly assign alphabet for each
 - Mapping of alphabets to their indices [Hashmap]
 - hashmap for storing states
 - 
*/

