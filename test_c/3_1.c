#include <stdio.h>
#include <stdint.h>
#include "../include/utils.h"
#include "../include/dfa.h"
#include "../include/enfa.h"
#include "../include/regex.h"

int main(){

    // -------------------------
    // Alphabet
    // epsilon must be index 0
    // -------------------------

    
    struct alphabet alpha = find_alphabet("abc");

    // -------------------------
    // char_to_enfa()
    // -------------------------

    struct ENFA a = char_to_enfa(
        &alpha,
        'a'
    );

    struct ENFA b = char_to_enfa(
        &alpha,
        'b'
    );

    struct ENFA c = char_to_enfa(
        &alpha,
        'c'
    );

    printf("\n===== char_to_enfa('a') =====\n");
    print_enfa(&a);

    // -------------------------
    // concat_op()
    // regex: ab
    // -------------------------

    struct ENFA ab = concat_op(
        &alpha,
        &a,
        &b
    );

    printf("\n===== concat_op(a, b) [ab] =====\n");
    print_enfa(&ab);

    // -------------------------
    // or_op()
    // regex: a|b
    // -------------------------

    struct ENFA a_or_b = or_op(
        &alpha,
        &a,
        &b
    );

    printf("\n===== or_op(a, b) [a|b] =====\n");
    print_enfa(&a_or_b);

    // -------------------------
    // Combined test
    // regex: (a|b)c
    // -------------------------

    struct ENFA a_or_b_c = concat_op(
        &alpha,
        &a_or_b,
        &c
    );

    printf("\n===== concat_op(a|b, c) [(a|b)c] =====\n");
    print_enfa(&a_or_b_c);

    return 0;
}
