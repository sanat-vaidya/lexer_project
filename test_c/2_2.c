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

    struct set_of_states tt[4][3];

    // initialize everything empty
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 3; j++){
            clear_set(&tt[i][j]);
        }
    }

    // q0 --e--> q1,q2
    add_to_set(&tt[0][0], 1);
    add_to_set(&tt[0][0], 2);

    // q1 --a--> q2
    add_to_set(&tt[1][1], 2);

    // q2 --b--> q3
    add_to_set(&tt[2][2], 3);

    struct set_of_states final_states;
    clear_set(&final_states);
    add_to_set(&final_states, 3);

    int start_state = 0;

    struct ENFA enfa = make_enfa(
        states,
        alphabet_size,
        alphabet,
        tt,
        start_state,
        final_states
    );

    // ==========================
    // print_enfa
    // ==========================

    printf("\n===== ENFA =====\n");
    print_enfa(&enfa);

    // ==========================
    // eclose tests
    // ==========================

    printf("\n===== ECLOSE TESTS =====\n");

    for(int i = 0; i < 4; i++){
        struct set_of_states res =
            eclose(&enfa, i);

        printf("eclose(%d) = ", i);
        print_set(res);
        printf("\n");
    }

    // ==========================
    // transition_enfa tests
    // ==========================

    printf("\n===== TRANSITION TESTS =====\n");

    struct set_of_states ec0 =
        eclose(&enfa, 0);

    printf("eclose(q0) = ");
    print_set(ec0);
    printf("\n");

    struct set_of_states after_a =
        transition_enfa(&enfa, &ec0, 1);

    printf("transition(eclose(q0), a) = ");
    print_set(after_a);
    printf("\n");

    struct set_of_states after_b =
        transition_enfa(&enfa, &after_a, 2);

    printf("transition(result, b) = ");
    print_set(after_b);
    printf("\n");

    // ==========================
    // is_same_set tests
    // ==========================

    printf("\n===== SAME SET TESTS =====\n");

    struct set_of_states s1;
    clear_set(&s1);
    add_to_set(&s1, 1);
    add_to_set(&s1, 2);

    struct set_of_states s2;
    clear_set(&s2);
    add_to_set(&s2, 1);
    add_to_set(&s2, 2);

    struct set_of_states s3;
    clear_set(&s3);
    add_to_set(&s3, 2);
    add_to_set(&s3, 3);

    printf("s1 == s2 ? %d\n",
           is_same_set(&s1, &s2));

    printf("s1 == s3 ? %d\n",
           is_same_set(&s1, &s3));

    // ==========================
    // in_set_of_sets tests
    // ==========================

    printf("\n===== SET OF SETS TEST =====\n");

    struct set_of_set all_sets;
    all_sets.count = 0;

    all_sets.set[
        all_sets.count++
    ] = s1;

    all_sets.set[
        all_sets.count++
    ] = after_a;

    printf("s2 in all_sets ? %d\n",
           in_set_of_sets(
               &all_sets,
               &s2
           ));

    printf("s3 in all_sets ? %d\n",
           in_set_of_sets(
               &all_sets,
               &s3
           ));

    return 0;
}
