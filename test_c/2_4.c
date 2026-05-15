#include <stdio.h>
#include <stdint.h>
#include "../include/utils.h"
#include "../include/dfa.h"
#include "../include/enfa.h"

int main(){

    // ==========================
    // Build ENFA
    // ==========================

    struct set_of_states states;
    clear_set(&states);

    add_to_set(&states, 0);
    add_to_set(&states, 1);
    add_to_set(&states, 2);
    add_to_set(&states, 3);

    char alphabet[] = {'e', 'a', 'b'};
    int alphabet_size = 3;

    struct set_of_states
        transition_table[4][3];

    // clear everything first
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 3; j++){
            clear_set(
                &transition_table[i][j]
            );
        }
    }

    // q0 --e--> q1,q2
    add_to_set(
        &transition_table[0][0], 1);
    add_to_set(
        &transition_table[0][0], 2);

    // q1 --a--> q1
    add_to_set(
        &transition_table[1][1], 1);

    // q1 --b--> q3
    add_to_set(
        &transition_table[1][2], 3);

    // q2 --a--> q3
    add_to_set(
        &transition_table[2][1], 3);

    // q3 has no transitions

    int start_state = 0;

    struct set_of_states final_states;
    clear_set(&final_states);
    add_to_set(&final_states, 3);

    struct ENFA enfa =
        make_enfa(
            states,
            alphabet_size,
            alphabet,
            transition_table,
            start_state,
            final_states
        );

    printf("\n===== ENFA =====\n");
    print_enfa(&enfa);


    // ==========================
    // Convert to DFA
    // ==========================

    printf(
        "\n===== CONVERTING "
        "TO DFA =====\n"
    );

    struct DFA dfa = minimize_dfa(convert_to_dfa(enfa));

    printf("\n===== DFA =====\n");
    print_dfa(dfa);


    // ==========================
    // Test DFA execution
    // ==========================

    printf(
        "\n===== RUN DFA =====\n"
    );

    printf("\"\"     -> ");
    run_dfa(dfa, "");

    printf("\"a\"    -> ");
    run_dfa(dfa, "a");

    printf("\"b\"    -> ");
    run_dfa(dfa, "b");

    printf("\"ab\"   -> ");
    run_dfa(dfa, "ab");

    printf("\"aaab\" -> ");
    run_dfa(dfa, "aaab");

    printf("\"bbb\"  -> ");
    run_dfa(dfa, "bbb");

    return 0;
}
