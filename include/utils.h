#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>

#define MaxStates 1024
#define MaxSizeOfAlphabet 30
#define NUMSTATE 5
#define MAXREGEXLEN 200
#define StatesPerWord 64
#define WordsNeeded (MaxStates/StatesPerWord)

//Definiton of common structs
/*typedef struct set_of_states{ //A structure that contains a list of states and the count
	int count;
	int states[MaxStates];
}set_of_states;
*/

typedef struct set_of_states{
  int count;
  uint64_t words[WordsNeeded];
} set_of_states;

void copy_set(struct set_of_states *dest,const set_of_states src);
int in_states(int states[],int count,int current_state);
void add_to_set(struct set_of_states *set, unsigned int state);
int in_set(const struct set_of_states *set, unsigned int states);
void print_binary(uint64_t value);
void sort_states(struct set_of_states *set);

typedef struct set_of_set{
	int count;
	struct set_of_states set[200];
}set_of_set;

typedef struct alphabet{ //A structure that contains a list of states and the count
	int count;
	char symbols[MaxSizeOfAlphabet];
}alphabet;

//----------------------------------------------------

int get_index(struct alphabet alphabet, char ch);
void swap(int *a, int *b);
int partition(int arr[], int low, int high);
void quick_sort(int arr[], int low, int high);
void swap_c(char *a, char *b);
int partition_c(char arr[], int low, int high);
void quick_sort_c(char arr[], int low, int high);
void copy_table(int dest[][MaxSizeOfAlphabet], int src[][MaxSizeOfAlphabet], int row_size, int col_size);
void copy_alphabet(struct alphabet *dest, struct alphabet src);
int precedence(char ch);
int is_op(char ch);
char *infix_to_postfix(char *in);

#endif 
