#include "../include/lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* load_langauge processes data TOP TO BOTTOM
 *  hence token rules written at the top will have higher
 * priority than ones below
 * this allows to give prefrence to KEYWORDS when they also match IDENT
 */

/* load_language
 * reads lang.def file line by line
 * for each line, parses the token name and regex
 * compiles the regex to a dfa using regex_to_dfa
 * stores the rule in the lexer
 * returns 1 on success, 0 on failure
 */
int load_language(Lexer *lexer, char *filename){
    FILE *file = fopen(filename, "r");
    if(file == NULL){
        printf("ERROR: could not open language file: %s\n", filename);
        return 0;
    }
    
    int num_rules = 0;
    lexer->num_rules = 0;

    // buffer to hold each line — name + space + regex
    char line[MAX_TOKEN_NAME + MAX_REGEX_LEN + 2];

    while(fgets(line, sizeof(line), file)){
        // skip empty lines and comments
        if(line[0] == '#' || line[0] == '\n') continue;

        char name[MAX_TOKEN_NAME];
        char regex[MAX_REGEX_LEN];

        // parse name and regex from line
        if(sscanf(line, "%s %s", name, regex) != 2){
            printf("WARNING: skipping malformed line: %s\n", line);
            continue;
        }

        // store name and regex
        strncpy(lexer->rules[num_rules].name, name, MAX_TOKEN_NAME);
        strncpy(lexer->rules[num_rules].regex, regex, MAX_REGEX_LEN);

        // compile regex to dfa — this is where your engine is used
        printf("compiling: %-16s -> %s\n", name, regex);
        lexer->rules[num_rules].dfa = regex_to_dfa(regex);
        num_rules++;

        if(num_rules >= MAX_RULES){
            printf("WARNING: max rules (%d) reached, ignoring rest\n", MAX_RULES);
            break;
        }
    }

    lexer->num_rules = num_rules;

    fclose(file);
    printf("loaded %d rules\n\n", lexer->num_rules);

    return 1;
}

/* next_token
 * runs all dfas in parallel on input starting at *pos
 * advances one character at a time
 * keeps track of the last position where any dfa was in a final state
 * when all dfas die, returns the longest match found
 * advances *pos past the matched token
 * if no rule matched, returns an error token and advances one character
 */
Token next_token(Lexer *lexer, char *input, int *pos){
  Token token;
  token.name[0] = '\0';
  token.value[0] = '\0';

  // track current state for each dfa
  int states[MAX_RULES];
  for(int i = 0; i < lexer->num_rules; i++){
    states[i] = lexer->rules[i].dfa.start_state;
  }

  int curr_pos = *pos;
  int last_match_pos  = *pos;   // end position of last successful match
  int last_match_rule = -1;     // which rule last matched

  while(input[curr_pos] != '\0'){
    char ch = input[curr_pos];
    int any_alive = 0;

    for(int i = 0; i < lexer->num_rules; i++){
      if(states[i] == -1) continue;  // this dfa already died

      int alpha_index = get_index(&lexer->rules[i].dfa.alphabet, ch);
      if(alpha_index == -1){
        states[i] = -1;  // character not in this dfas alphabet
        continue;
      }

      states[i] = lexer->rules[i].dfa.transition_table[states[i]][alpha_index];

      if(states[i] == -1) continue;  // hit dead state

      any_alive = 1;

      // check if this dfa is in a final state — if so record the match
      if(in_set(&lexer->rules[i].dfa.final_states, states[i])){

        int new_match_pos = curr_pos + 1;

        // strictly longer match -> always better
        if(new_match_pos > last_match_pos){
          last_match_pos = new_match_pos;
          last_match_rule = i;
        }

        // first ever match
        else if(last_match_rule == -1){
          last_match_pos = new_match_pos;
          last_match_rule = i;
        }

        // equal length -> DO NOTHING
        // earlier rule already stored
      }
    }

    if(!any_alive) break;  // all dfas dead — stop

    curr_pos++;
  }

  // build the token from the last successful match
  if(last_match_rule != -1){
    int match_len = last_match_pos - *pos;
    strncpy(token.name, lexer->rules[last_match_rule].name, MAX_TOKEN_NAME);
    strncpy(token.value, input + *pos, match_len);
    token.value[match_len] = '\0';
    *pos = last_match_pos;  // advance past matched token
  } else {
    // no rule matched — unknown character
    strncpy(token.name, "UNKNOWN", MAX_TOKEN_NAME);
    token.value[0] = input[*pos];
    token.value[1] = '\0';
    (*pos)++;  // skip unknown character
  }

  return token;
}

/* lex_file
 * opens the given .c file
 * reads it entirely into a buffer
 * skips whitespace between tokens
 * calls next_token repeatedly until end of file
 * prints each token as it is found
 */
void lex_file(Lexer *lexer, char *filename){
    FILE *file = fopen(filename, "r");
    if(file == NULL){
        printf("ERROR: could not open file: %s\n", filename);
        return;
    }

    // read entire file into buffer
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *input = malloc(file_size + 1);
    if(input == NULL){
        printf("ERROR: could not allocate memory for file\n");
        fclose(file);
        return;
    }

    fread(input, 1, file_size, file);
    input[file_size] = '\0';
    fclose(file);

    // tokenize the input
    int pos = 0;
    while(input[pos] != '\0'){

        // skip whitespace
        while(input[pos] == ' '  ||
              input[pos] == '\n' ||
              input[pos] == '\t' ||
              input[pos] == '\r'){
            pos++;
        }

        if(input[pos] == '\0') break;

        Token token = next_token(lexer, input, &pos);
        printf("%-16s : %s\n", token.name, token.value);
    }

    free(input);
}
