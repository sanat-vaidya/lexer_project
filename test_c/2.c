#include <stdio.h>
#include <stdint.h>
#include "../include/utils.h"
#include "../include/dfa.h"
#include "../include/enfa.h"

int main(){

    // ==================================================
    // DEFINE STATES
    // ==================================================

    struct set_of_states states;
    clear_set(&states);

    add_to_set(&states, 0);
    add_to_set(&states, 1);
    add_to_set(&states, 2);
    add_to_set(&states, 3);

    // ==================================================
    // DEFINE ALPHABET
    // index 0 MUST be epsilon
    // ==================================================

    char alphabet[] = {'e', 'a', 'b'};
    int alphabet_size = 3;

    // ==================================================
    // DEFINE TRANSITION TABLE
    // [state][symbol]
    // symbol index:
    // 0 = epsilon
    // 1 = a
    // 2 = b
    // ==================================================

    struct set_of_states tt[4][3];

    // clear all sets first
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 3; j++){
            clear_set(&tt[i][j]);
        }
    }

    // q0 --e--> q1,q2
    add_to_set(&tt[0][0], 1);
    add_to_set(&tt[0][0], 2);

    // q1 --e--> q3
    add_to_set(&tt[1][0], 3);

    // q2 --a--> q2
    add_to_set(&tt[2][1], 2);

    // q2 --b--> q3
    add_to_set(&tt[2][2], 3);

    // q3 --a--> q3
    add_to_set(&tt[3][1], 3);

    // ==================================================
    // FINAL STATES
    // ==================================================

    struct set_of_states final_states;
    clear_set(&final_states);

    add_to_set(&final_states, 3);

    // ==================================================
    // BUILD ENFA
    // ==================================================

    struct ENFA enfa = make_enfa(
        states,
        alphabet_size,
        alphabet,
        tt,
        0,
        final_states
    );

    // ==================================================
    // PRINT ENFA
    // ==================================================

    printf("\n===== ENFA =====\n");
    print_enfa(enfa);

    // ==================================================
    // TEST ECLOSE
    // ==================================================

    printf("\n===== ECLOSE TESTS =====\n");

    for(int i = 0; i < 4; i++){

        struct set_of_states res =
            eclose(enfa, i);

        printf("eclose(%d):\n", i);

        for(int w = 0; w < WordsNeeded; w++){
            print_binary(res.words[w]);
        }

        printf("\n");
    }

    return 0;
}
