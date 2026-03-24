#include <stdio.h>
#include <stdlib.h>

/* Basic idea is to write a regular expression, convert it to an E-NFA -> DFA -> minimize DFA-> run the algorithm
 *

*/

#define MaxStates 100
#define MaxSizeOfAlphabet 2
#define NUMSTATE 4

typedef struct set_of_states{ //A structure that contains a list of states and the count
	int count;
	int states[MaxStates];
}set_of_states;

typedef struct alphabet{ //A structure that contains a list of states and the count
	int count;
	int symbols[MaxSizeOfAlphabet];
}alphabet;

typedef struct DFA{  //basically a 5 tuple definition of a DFA, alphabet yet to be defined.
	struct alphabet alphabet;
	
	struct set_of_states states;

	int transition_table[NUMSTATE][2];	
	
	int start_state;
	
	struct set_of_states final_states;
	
} DFA;

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

int is_distinguishable(struct DFA dfa,int matrix[][MaxStates],int state1, int state2){

	if(in_final(dfa.final_states,state1) != in_final(dfa.final_states,state2)){
		return 1;
	}
	
	int i = 0;
	 
	while(i<dfa.alphabet.count){
		int Xition_s1 = dfa.transition_table[state1][i];
		int Xition_s2 = dfa.transition_table[state2][i];
		//printf("\n xs1: %d   xs2: %d\n ",Xition_s1,Xition_s2);
		
		if(matrix[Xition_s1][Xition_s2] == 1 || matrix[Xition_s2][Xition_s1] == 1 ){
			return 1;
		}
		
		i++;
	}
	
	return 0;
}

int partition_states(DFA dfa, int distinguishable_matrix[][dfa.states.count], 
                     struct set_of_states partitions[])
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

                if(distinguishable_matrix[state1][state2] == 0){
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

struct DFA minimize_dfa(DFA dfa){
	int num_states = dfa.states.count;
	
	int distinguishable_matrix[num_states][num_states];
	
	//Initialize matrix while marking final and not final states
	for(int i = 0;i<num_states;i++){
		int state1 = dfa.states.states[i];

		for(int j = 0; j<num_states;j++){ // only consider lower triangle of the matrix
			
			int state2 = dfa.states.states[j];
					
			if(in_final(dfa.final_states,state1) != in_final(dfa.final_states,state2)){
				distinguishable_matrix[state1][state2] = 1;
			}
			else{
				distinguishable_matrix[state1][state2] = 0;
			}
		}
	}
	
	//run the loop till no new change is made
	int flag = 1;
	
	int i = 1;
	int j = 0;
	
	int pass_count = 0;
	
	while(flag == 1){ //run the code till distinguisable matrix has no changes
		flag = 0;
	
		while(i<num_states){
			int state1 = dfa.states.states[i];
			j = 0;
			while(j<i){
				int state2 = dfa.states.states[j];
				
				if(is_distinguishable(dfa,distinguishable_matrix,state1,state2)){
					distinguishable_matrix[state1][state2] = 1;
					distinguishable_matrix[state2][state1] = 1;
					flag = 1;
				}

				j++;
			}
			i++;
		}
		
	}

	//How to convert from distuinguishable matrix to dfa?
	
	
	struct DFA *res_dfa = (DFA *)malloc(sizeof(DFA));
	res_dfa->alphabet = dfa.alphabet;
	
	//Find corresponding group of states that are equivalent
	struct set_of_states partitions[num_states];
	int partition_count = partition_states(dfa,distinguishable_matrix,partitions);
	
	/*
	int visited[num_states];
	int visited_count = 0;
	
	for(int i = 0; i<num_states;i++){
		int state1 = dfa.states.states[i];
		
		if(in_states(visited,visited_count,state1) == 0){
			int state_count = 0;

			visited[visited_count++] = state1;
			partitions[partition_count].states[state_count++] = state1;
			
			for(int j = i+1; j<num_states; j++){
				int state2 = dfa.states.states[j];
				
				if(distinguishable_matrix[state1][state2] == 0){
					partitions[partition_count].states[state_count++] = state2;
					visited[visited_count++] = state2;
				}
			}
			partitions[partition_count].count = state_count; 
			(partition_count)++;
		}
	}
	*/
	
	for(int i = 0;i<partition_count;i++){
		for(int j = 0; j<partitions[i].count;j++){
			printf("%d ",partitions[i].states[j]);
		}
		printf("\n");
	}
	printf("\n");
	
	return *res_dfa;
	
}

	
int main(){
	
	//Read A string
	char *str_ptr;
		
	char str[1000];
	scanf("%s",str);
	
	str_ptr = str;	
	
	// Consider dfa for if string contains 01
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
	
	struct DFA test_dfa = {
		{
			2,
			{'0','1'} //defineing the alphabet
		},
		{
			NUMSTATE,
			{0,1,2,3}, // set of all statss
		},
		
		{
			{1,0}, // transition table
			{1,2},
			{3,3},
			{3,3}
		},
		0, // start state
		{
			2,
			{2,3}, // set of final states
		}
	};
	
	//run_dfa(dfa,str);
	
	//int num_states = dfa.states.count;
		
	struct DFA new_dfa = minimize_dfa(test_dfa);
	
	return 0;
}
