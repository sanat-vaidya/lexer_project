#include "../include/utils.h"
#include "../include/dfa.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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
    clear_states(&dfa.states);
    for(int i = 0; i < num_states; i++){
      add_state(&dfa.states, i);  // store positions 0,1,2... not values
    }

    // copy final states, converting values to positions
    clear_states(&dfa.final_states);
    for(int i = 0; i < num_final_states; i++){
      for(int j = 0; j < num_states; j++){
        if(states[j] == final_states[i]) add_state(&dfa.final_states, j);
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
 
  int num_states = dfa.states.count;

	printf("Alphabet: ");
	for(int i = 0;i<dfa.alphabet.count;i++){
		printf("%c ",dfa.alphabet.symbols[i]);
	}
	printf("\n");
	
	printf("States: ");
  for(int i = 0; i < num_states; i++){
    if(in_states(&dfa.states, i)) printf("%d ", i);
  }
	printf("\n");
	
  printf("Final States: ");
  for(int i = 0; i < num_states; i++){
    if(in_states(&dfa.final_states, i)) printf("%d ", i);
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
	
	
	if(in_final(&dfa.final_states,currstate)){ 
		printf("\nYAYAYYAYAYYA\n"); 
		return 1;
	}
	
	else{
		printf("\nNOOO :(\n");	
		return 0;
	}

}

int is_distinguishable(struct DFA dfa, int num_states, int matrix[][num_states], int pos1, int pos2){
	
	/* VERY VERY VERY important to pass num_states because
	   if we are passing the matrix with size [num_states][num_states]
	   but the function had matrix[][MaxStates] the matrix is stored as [0][0] [0][1]...[0][99]
	   so matrix[2][j] will result in memory location sizeofint*2*(MaxStates) + j which is incorrect
	*/
    if(in_final(&dfa.final_states, pos1) != in_final(&dfa.final_states, pos2)){
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

    int partition_count = 0;

    struct set_of_states visited;
    clear_states(&visited);
    
    for(int i = 0; i < num_states; i++){
      if(in_states(&visited, i) == 0){
        clear_states(&partitions[partition_count]);
        add_state(&visited, i);
        add_state(&partitions[partition_count], i);
      
       for(int j = i + 1; j < num_states; j++){
          if(distinguishable_matrix[i][j] == 0){
            add_state(&partitions[partition_count], j);
            add_state(&visited, j);
          }
        }
        partition_count++;
      }
    }

    return partition_count;
}

int get_partition(struct set_of_states partitions[], int partition_count, int original_state){
	for(int i = 0; i<partition_count; i++){
		if(in_states(&partitions[i], original_state)) return i;
	}
	
	return -1;
}

void initialize_matrix(struct DFA dfa, int distinguishable_matrix[][dfa.states.count]){
	
	int num_states = dfa.states.count;
	
	//Initialize matrix while marking final and not final states
	for(int i = 0;i<num_states;i++){
		for(int j = 0; j<num_states;j++){ // only consider lower triangle of the matrix
			// pass i and j (positions) not state values
			if(in_final(&dfa.final_states, i) != in_final(&dfa.final_states, j)){
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
  
  int res_final_states[partition_count];
  int res_final_states_count = 0;

	for(int i = 0; i < partition_count; i++){

    // get representative state of this partition
    int res_state = -1;
    for(int word = 0; word < MaxWord; word++){
        if(partitions[i].words[word] != 0){
            unsigned long long w = partitions[i].words[word];
            int bit = 0;
            while((w & 1) == 0){
                w >>= 1;
                bit++;
            }
            res_state = word * BitsPerWord + bit;
            break;
        }
    }

    // build transition table row for this partition
    for(int j = 0; j < dfa.alphabet.count; j++){
        res_Xition_table[i][j] = get_partition(partitions, partition_count, dfa.transition_table[res_state][j]);
    }

    // check if this partition is a final state
    if(in_final(&dfa.final_states, res_state) == 1){
        res_final_states[res_final_states_count++] = i;
    }
}

	//make set of states in min dfa
	int res_states[partition_count];
	for(int i = 0; i<partition_count; i++){
		res_states[i] = i;
	}
	
	//make set of final states in min dfa
	
	//make new dfa
	res_dfa = make_dfa(partition_count,res_states,
					   dfa.alphabet.count,dfa.alphabet.symbols,
					   res_Xition_table,
					   get_partition(partitions,partition_count,dfa.start_state),
					   res_final_states_count,
					   res_final_states);
	
	return res_dfa;
	
}
