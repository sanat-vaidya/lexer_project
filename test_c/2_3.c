#include <stdio.h>
#include <stdint.h>
#include "../include/utils.h"
#include "../include/dfa.h"
#include "../include/enfa.h"

int main(){

    // ==========================
    // BUILD ENFA
    // ==========================

    struct set_of_states states;
    clear_set(&states);

    for(int i = 0; i < 4; i++){
        add_to_set(&states, i);
    }

    char alphabet[] = {'e', 'a', 'b'};
    int alphabet_size = 3;

    struct set_of_states tt[4][3];

    // clear transition table
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 3; j++){
            clear_set(&tt[i][j]);
        }
    }

    // q0 --e--> q1,q2
    add_to_set(&tt[0][0], 1);
    add_to_set(&tt[0][0], 2);

    // q1 --a--> q1
    add_to_set(&tt[1][1], 1);

    // q1 --b--> q3
    add_to_set(&tt[1][2], 3);

    // q2 --a--> q3
    add_to_set(&tt[2][1], 3);

    // final state = q3
    struct set_of_states final_states;
    clear_set(&final_states);
    add_to_set(&final_states, 3);

    struct ENFA enfa = make_enfa(
        states,
        alphabet_size,
        alphabet,
        tt,
        0,
        final_states
    );

    // ==========================
    // PRINT ENFA
    // ==========================

    printf("\n===== ENFA =====\n");
    print_enfa(&enfa);

    // ==========================
    // ECLOSE TESTS
    // ==========================

    printf("\n===== ECLOSE TESTS =====\n");

    for(int i = 0; i < 4; i++){
        struct set_of_states res =
            eclose(&enfa, i);

        printf("eclose(q%d) = ", i);
        print_set(res);
        printf("\n");
    }

    // ==========================
    // TRANSITION TESTS
    // ==========================

    printf("\n===== TRANSITION TESTS =====\n");

    struct set_of_states ec0 =
        eclose(&enfa, 0);

    printf("eclose(q0) = ");
    print_set(ec0);
    printf("\n");

    struct set_of_states after_a =
        transition_enfa(
            &enfa,
            &ec0,
            1 // 'a'
        );

    printf("transition(ec0, a) = ");
    print_set(after_a);
    printf("\n");

    struct set_of_states after_b =
        transition_enfa(
            &enfa,
            &after_a,
            2 // 'b'
        );

    printf("transition(after_a, b) = ");
    print_set(after_b);
    printf("\n");

    // ==========================
    // ECLOSE_SET TEST
    // ==========================

    printf("\n===== ECLOSE_SET TEST =====\n");

    struct set_of_states eclosed =
        eclose_set(
            &enfa,
            &after_a
        );

    printf("eclose_set(after_a) = ");
    print_set(eclosed);
    printf("\n");

    // ==========================
    // is_same_set TEST
    // ==========================

    printf("\n===== SAME SET TEST =====\n");

    struct set_of_states s1;
    clear_set(&s1);
    add_to_set(&s1, 1);
    add_to_set(&s1, 3);

    struct set_of_states s2;
    clear_set(&s2);
    add_to_set(&s2, 1);
    add_to_set(&s2, 3);

    struct set_of_states s3;
    clear_set(&s3);
    add_to_set(&s3, 2);

    printf("s1 == s2 : %d\n",
           is_same_set(&s1, &s2));

    printf("s1 == s3 : %d\n",
           is_same_set(&s1, &s3));

    // ==========================
    // SET OF SETS TEST
    // ==========================

    printf("\n===== SET OF SETS TEST =====\n");

    struct set_of_set sets;
    sets.count = 0;

    sets.set[sets.count++] = s1;
    sets.set[sets.count++] = after_a;

    printf("s2 in set_of_sets : %d\n",
           in_set_of_sets(
               &sets,
               &s2
           ));

    printf("s3 in set_of_sets : %d\n",
           in_set_of_sets(
               &sets,
               &s3
           ));

    // ==========================
    // STACK TEST
    // ==========================

    printf("\n===== STACK TEST =====\n");

    struct set_of_states stack[10];
    int top = -1;

    stack[++top] = s1;
    stack[++top] = after_a;

    printf("s2 in stack : %d\n",
           in_stack(
               stack,
               top,
               &s2
           ));

    printf("s3 in stack : %d\n",
           in_stack(
               stack,
               top,
               &s3
           ));

    // ==========================
    // GET SET INDEX TEST
    // ==========================

    printf("\n===== GET SET INDEX =====\n");

    printf("index of s2 : %d\n",
           get_set_index(
               &sets,
               &s2
           ));

    printf("index of s3 : %d\n",
           get_set_index(
               &sets,
               &s3
           ));

    return 0;
}
