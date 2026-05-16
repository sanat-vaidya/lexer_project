# Regex to DFA Engine

A from-scratch implementation of a regex engine in C, built on automata theory.
The pipeline goes: **Regular Expression → ENFA → DFA → Minimized DFA → Run**.

---

## Project Structure

```
project/
├── include/
│   ├── utils.h       -- shared structs and utility functions
│   ├── dfa.h         -- DFA struct and operations
│   ├── enfa.h        -- Epsilon-NFA struct and operations
│   └── regex.h       -- regex to ENFA conversion (Thompson's construction)
└── src/
    ├── utils.c
    ├── dfa.c
    ├── enfa.c
    ├── regex.c
    └── main.c
```

---

## How it works

### 1. Regular Expression → ENFA (Thompson's Construction)
Given a regex string like `(0|1)*011`, the program:
- Finds the alphabet by scanning the regex
- Inserts explicit concatenation operators (`.`) where needed
- Converts infix to postfix
- Builds an ENFA using Thompson's construction by processing the postfix expression

Supported regex operators:
- `|` — union
- `*` — kleene closure
- `.` — concatenation (inserted automatically)
- `()` — grouping

### 2. ENFA → DFA (Subset Construction)
- Computes epsilon closure of the start state
- Uses a stack to explore all reachable sets of NFA states
- Each unique set of NFA states becomes one DFA state
- Dead states are handled with a `-1` sentinel in the transition table

### 3. DFA Minimization (Table Filling Algorithm)
- Builds a distinguishable matrix marking final vs non-final state pairs
- Iterates until no new pairs are marked distinguishable
- Partitions equivalent states into groups
- Builds a new minimized DFA from the partitions

### 4. Run DFA
- Takes the minimized DFA and an input string
- Steps through the transition table character by character
- Returns accept or reject

---

## Key Data Structures

### `set_of_states` (Bitset)
States are stored as a bitmask using an array of `uint64_t` words.
Each bit position represents a state — bit `i` set means state `i` is in the set.

```c
typedef struct set_of_states{
    int count;                   // number of states in the set
    uint64_t words[WordsNeeded]; // bitset — bit i set means state i is in set
} set_of_states;
```

### `DFA`
```c
typedef struct DFA{
    struct alphabet alphabet;
    struct set_of_states states;
    int **transition_table;            // [state][symbol] -> state position
    int start_state;
    struct set_of_states final_states;
} DFA;
```

### `ENFA`
```c
typedef struct ENFA{
    struct alphabet alphabet;
    struct set_of_states states;
    struct set_of_states **transition_table; // [state][symbol] -> set_of_states
    int start_state;
    struct set_of_states final_states;
} ENFA;
```

---

## Known Bugs / Limitations

- `MaxStates` is a compile-time constant — very large NFAs may exceed it
- No support for escape characters in regex (e.g. `\*` to match literal `*`)
- Regex syntax errors are not caught — undefined behavior on malformed input
- Memory is never freed — `make_dfa` and `make_enfa` malloc but there is no `free_dfa` / `free_enfa`

---

## Ideas Left to Do

### Performance
- `static inline` for small frequently called functions like `in_states`, `add_to_set`
- `__builtin_ctzll()` for finding lowest set bit in a word
- `__builtin_popcountll()` for counting states in a set instead of maintaining `count` manually
- `union_sets()` function for merging two `set_of_states` with a single word-by-word OR loop
- Hopcroft's algorithm for minimization — O(n log n) vs current O(n²) table filling
- Hashmap for `get_set_index` and `in_set` — currently O(n) linear scan

### Code Quality
- Adding `const` keyword to read-only function parameters
- `free_dfa()` and `free_enfa()` to fix memory leaks
- Better error handling for malformed regex input

### Features
- Support for more regex operators — `+` (one or more), `?` (zero or one), character classes `[a-z]`
- Escape characters in regex
- Lexical analyzer built on top of this engine

---

## References

- Sipser, *Introduction to the Theory of Computation* — Chapter 1
- Hopcroft, Motwani, Ullman, *Introduction to Automata Theory* — Chapter 2
- Aho, Lam, Sethi, Ullman, *Compilers: Principles, Techniques and Tools* (Dragon Book) — Chapter 3
- CLRS, *Introduction to Algorithms* — Chapter 22
