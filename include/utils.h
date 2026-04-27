#ifndef UTILS_H
#define UTILS_H

#define MaxStates 500
#define MaxSizeOfAlphabet 30
#define MaxRegexLex 200
#define MaxSets 200
#define BitsPerWord 64
#define MaxWord (MaxStates/BitsPerWord + 1 )

//Definiton of common structs
/*typedef struct set_of_states{ //A structure that contains a list of states and the count
	int count;
	int states[MaxStates];
}set_of_states;
*/

typedef struct set_of_states{
  int count;
  unsigned long long words[MaxWord]; //sets represented as a bitmask
}set_of_states;

typedef struct set_of_set{
	int count;
	struct set_of_states set[MaxSets];
}set_of_set;

typedef struct alphabet{ //A structure that contains a list of states and the count
	int count;
	char symbols[MaxSizeOfAlphabet];
}alphabet;

//----------------------------------------------------

void swap(int *a, int *b);
int partition(int arr[], int low, int high);
void quick_sort(int arr[], int low, int high);
void swap_c(char *a, char *b);
int partition_c(char arr[], int low, int high);
void quick_sort_c(char arr[], int low, int high);
//void copy_states(struct set_of_states *dest, int states[], int size);
void copy_table(int dest[][MaxSizeOfAlphabet], int src[][MaxSizeOfAlphabet], int row_size, int col_size);
void copy_alphabet(struct alphabet *dest, struct alphabet src);
//int in_states(int states[],int count,int current_state);
//void sort_states(struct set_of_states *set);
//int in_final(struct set_of_states final_states,int current_state);
//int get_position(struct set_of_states states, int state_value);
int get_index(struct alphabet alphabet, char ch);
int precedence(char ch);
int is_op(char ch);
char *infix_to_postfix(char *in);
void clear_states(struct set_of_states *s);
void add_state(struct set_of_states *s, int state);
int in_states(struct set_of_states *s, int state);
int set_equal(struct set_of_states *a, struct set_of_states *b);
int in_final(struct set_of_states *fs,int state);
int set_empty(struct set_of_states *s);

#endif 
