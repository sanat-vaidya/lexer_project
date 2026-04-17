#ifndef REGEX_H
#define REGEX_H

char *complete_regex(char *regex);
struct ENFA char_to_enfa(struct alphabet alphabet, char ch);
struct ENFA concat_op(struct alphabet alphabet, struct ENFA exp1, struct ENFA exp2);
struct ENFA or_op(struct alphabet alphabet, struct ENFA exp1, struct ENFA exp2);
struct ENFA kleene_closure_op(struct alphabet alphabet, struct ENFA exp1);
struct alphabet find_alphabet(char *regex);
struct ENFA convert_to_enfa(char *regex);
int check_in_regex(char *regex,char *str);


#endif
