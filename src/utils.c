#include "../include/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

//--------sorting codes--------------
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

int partition_c(char arr[], int low, int high){
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
//----------------------------------------------------

//copying stuff
void copy_table(int dest[][MaxSizeOfAlphabet], int src[][MaxSizeOfAlphabet], int row_size, int col_size){
	for(int i = 0; i<row_size; i++){
		for(int j =0; j<col_size;j++){
			dest[i][j] = src[i][j];
		}
	}
}

void copy_alphabet(struct alphabet *dest, struct alphabet src){
	dest->count = src.count;
	for(int i = 0; i<src.count; i++){
		dest->symbols[i] = src.symbols[i];
	}
}
//----------------------------------------------------

int get_index(struct alphabet alphabet, char ch){
  //Currently this functions returns the corresponding index for a given input symbol
	int size = alphabet.count;
	for(int i = 0; i<size; i++){
		if(alphabet.symbols[i] == ch) return i;
	}
	
	return -1;
}


//----Opetations with states, set of states------------------------

  /*Since im going to be trying to change the fundamental implemention of 
   * set_of_states, i will need a lot of functions to go along with it.
    each set_of_states function will be represented by set_<operation>
  */

void add_to_set(struct set_of_states *set, unsigned int state){
  set->words[state/StatesPerWord] |= (1ULL << state%StatesPerWord);
}

int in_set(const struct set_of_states *set, unsigned int state) {
    // 1. Shift instead of division (64 = 2^6)
    // 2. Mask instead of modulo (state % 64 is the same as state & 63)
    // 3. Return 1 or 0 explicitly
    return (set->words[state >> 6] & (1ULL << (state & 63))) != 0;
}

void copy_set(struct set_of_states *dest, const struct set_of_states src){
  for(int i = 0; i<WordsNeeded; i++){
    dest->words[i] = src.words[i];
  }
}


int in_states(int states[],int count,int current_state){
	for(int i = 0;i<count;i++){
		if(states[i] == current_state) return 1;
	}
	
	return 0;
}

void sort_states(struct set_of_states *set){
	/*quick_sort(set->states,0,set->count-1);*/
}

//checks if a given state is in the set of final states
int in_final(struct set_of_states final_states,int current_state){
  /*	
	for(int i = 0;i <final_states.count;i++){
		if(final_states.states[i] == current_state) return 1;
	}
  */	
	return 0;
}

int get_position(struct set_of_states states, int state_value){
	//gets the corresponding index for the value of a state
    /*
    for(int i = 0; i < states.count; i++){
        if(states.states[i] == state_value) return i;
    }
    */
    return -1;
}

//-------------------------------------------------------------------------------------------

//infix to postfix
int precedence(char ch){
	 if(ch == '*') return 3;
	 else if(ch == '.') return 2;
	 else if(ch == '+' || ch == '|') return 1;
	 else return -1;
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
//--------------------------------------------------------------------------



