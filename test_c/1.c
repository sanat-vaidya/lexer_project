#include <stdio.h>
#include <stdint.h>
#include "../include/utils.h"
#include "../include/dfa.h"

int main() {

    // ==========================================================
    // TEST 1: Already minimal DFA
    // Accepts strings ending in 'a'
    // q0 --a--> q1
    // q0 --b--> q0
    // q1 --a--> q1
    // q1 --b--> q0
    // ==========================================================

    printf("\n=========================================\n");
    printf("TEST 1: Already minimal DFA\n");
    printf("=========================================\n");

    struct set_of_states states1;
    clear_set(&states1);
    add_to_set(&states1, 0);
    add_to_set(&states1, 1);

    char alphabet[] = {'a', 'b'};
    int alphabet_size = 2;

    int tt1[2][2] = {
        {1, 0},   // q0
        {1, 0}    // q1
    };

    struct set_of_states final1;
    clear_set(&final1);
    add_to_set(&final1, 1);

    DFA dfa1 = make_dfa(
        states1,
        alphabet_size,
        alphabet,
        tt1,
        0,
        final1
    );

    printf("\n--- Original DFA ---\n");
    print_dfa(dfa1);

    printf("\n--- Running Original DFA ---\n");
    run_dfa(dfa1, "a");     // accept
    run_dfa(dfa1, "ba");    // accept
    run_dfa(dfa1, "aba");   // accept
    run_dfa(dfa1, "b");     // reject
    run_dfa(dfa1, "ab");    // reject
    run_dfa(dfa1, "");      // reject

    printf("\n--- Minimized DFA ---\n");
    DFA min1 = minimize_dfa(dfa1);
    print_dfa(min1);

    printf("\n--- Running Minimized DFA ---\n");
    run_dfa(min1, "a");
    run_dfa(min1, "ba");
    run_dfa(min1, "b");
    run_dfa(min1, "ab");


    // ==========================================================
    // TEST 2: DFA with equivalent states
    //
    // q1 and q2 are equivalent and should merge
    //
    //         a    b
    // q0 --> q1   q0
    // q1 --> q2   q1
    // q2 --> q2   q2
    //
    // final = {1,2}
    //
    // Minimized DFA should have 2 states
    // ==========================================================

    printf("\n=========================================\n");
    printf("TEST 2: Non-minimal DFA\n");
    printf("=========================================\n");

    struct set_of_states states2;
    clear_set(&states2);
    add_to_set(&states2, 0);
    add_to_set(&states2, 1);
    add_to_set(&states2, 2);

    int tt2[3][2] = {
        {1, 0},
        {2, 1},
        {2, 2}
    };

    struct set_of_states final2;
    clear_set(&final2);
    add_to_set(&final2, 1);
    add_to_set(&final2, 2);

    DFA dfa2 = make_dfa(
        states2,
        alphabet_size,
        alphabet,
        tt2,
        0,
        final2
    );

    printf("\n--- Original DFA ---\n");
    print_dfa(dfa2);

    printf("\n--- Running Original DFA ---\n");
    run_dfa(dfa2, "a");      // accept
    run_dfa(dfa2, "ba");     // accept
    run_dfa(dfa2, "bab");    // accept
    run_dfa(dfa2, "b");      // reject
    run_dfa(dfa2, "bb");     // reject
    run_dfa(dfa2, "");       // reject

    printf("\n--- Minimized DFA ---\n");
    DFA min2 = minimize_dfa(dfa2);
    print_dfa(min2);

    printf("\n--- Running Minimized DFA ---\n");
    run_dfa(min2, "a");
    run_dfa(min2, "ba");
    run_dfa(min2, "bab");
    run_dfa(min2, "b");
    run_dfa(min2, "bb");
    run_dfa(min2, "");

    return 0;
}
