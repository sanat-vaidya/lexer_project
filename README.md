# Regex to DFA Engine

A from-scratch implementation of a regex engine in C, built on automata theory.
The pipeline goes: **Regular Expression → ENFA → DFA → Minimized DFA → Run**.

---

## Project Structure

```
project/
├── makefile          -- makefile for creating executible file ./automata, used for testing regex -> ./automata "<regex>"
├── Lex_makefile      -- makefile for the actual lex program
├── include/
│   ├── utils.h       -- shared structs and utility functions
│   ├── dfa.h         -- DFA struct and operations
│   ├── enfa.h        -- Epsilon-NFA struct and operations
│   ├── regex.h       -- regex to ENFA conversion (Thompson's construction)
│   └── lexer.h       -- lexer structs and interface
└── src/
    ├── utils.c
    ├── dfa.c
    ├── enfa.c
    ├── regex.c
    ├── lexer.c
    └── lex_analysis.c   -- entry point for the lexer
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

### 5. Lexer
The lexer is built on top of the regex engine and turns a source file into a stream of tokens. It has three stages:

**`load_language(lexer, filename)`** — reads a `.def` file that defines the token rules. Each line contains a token name and a regex pattern. Rules are processed top to bottom, so rules listed earlier have higher priority — this lets keywords take precedence over identifiers when both patterns would match. For each rule, `regex_to_dfa` is called to compile the pattern to a minimized DFA, which is stored in the lexer.

**`next_token(lexer, input, pos)`** — runs all compiled DFAs in parallel on the input starting at `pos`. It advances one character at a time, feeding each character to every live DFA. Whenever a DFA reaches a final state, the current position is recorded as a candidate match. When all DFAs have died, the longest match found is returned as a token. If two rules match at the same length, the earlier rule wins (enforcing keyword priority). If no rule matched at all, an `UNKNOWN` token is emitted for the current character and the position advances by one.

**`lex_file(lexer, filename)`** — reads the entire source file into a buffer, then repeatedly calls `next_token`, skipping whitespace between tokens, and prints each token as `NAME : value`.

The lexer entry point is `lex_analysis.c`, which takes two command-line arguments — a language definition file and a source file to tokenize:
```
./lexer lang.def source.c
```

---

## Defining a Language (`lang.def`)

The lexer is configured by a plain-text language definition file. Each line defines one token rule as a name and a regex pattern separated by whitespace. Lines starting with `#` are treated as comments and ignored.

```
# this is a comment
TOKEN_NAME   regex
```

**Rule priority** is determined by order — rules listed earlier match first when two patterns tie on length. Always put keywords before the general identifier rule so that `int` is recognized as `KEYWORD_INT` and not `IDENT`.

**Regex patterns** use the same operators supported by the engine: `|` for union, `*` for kleene closure, `()` for grouping, and `\` to escape special characters. Literal `(`, `)`, `+`, `*`, `|` must be escaped with a backslash when you want them to match as characters.

A minimal C-like language definition looks like this:

```
# keywords — must come before IDENT
KEYWORD_INT     int
KEYWORD_IF      if
KEYWORD_RETURN  return

# identifiers — letter or underscore, followed by any alphanumeric/underscore
IDENT   (a|b|...|z|A|...|Z|_)((a|b|...|z|A|...|Z|_|0|...|9)*)

# integer literals
INTEGER (1|2|3|4|5|6|7|8|9)(0|1|2|3|4|5|6|7|8|9)*|0

# multi-character operators before single-character ones
EQUAL_EQUAL ==
EQUALS      =

# escaped special characters
OR          \|\|
LPAREN      \(
RPAREN      \)
```

A few things to keep in mind:

- **Longest match wins.** `next_token` always returns the longest token it can, so `==` will correctly match `EQUAL_EQUAL` rather than two `EQUALS` tokens.
- **Equal-length ties go to the earlier rule.** This is how keyword priority over `IDENT` is enforced.
- **Operators that are also regex metacharacters must be escaped.** Characters like `(`, `)`, `|`, `*`, `+` need a leading `\` in the pattern when you want the lexer to match them literally.
- **Whitespace is skipped between tokens** by `lex_file` — there is no need to define a whitespace rule.

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
- Regex syntax errors are not caught — undefined behavior on malformed input
- Memory is never freed — `make_dfa` and `make_enfa` malloc but there is no `free_dfa` / `free_enfa`
- Escape sequence recognition is inconsistent — `find_alphabet` correctly handles `\x` sequences when scanning for alphabet symbols, but `complete_regex` advances past the escaped character without resetting `prev_was_operand`, which can cause the implicit concatenation dot to be inserted in the wrong place
- Sequences like `(a)(a)*` are mishandled 

---

## Ideas Left to Do

### Performance
- `static inline` for small frequently called functions like `in_states`, `add_to_set`
- `__builtin_ctzll()` for finding lowest set bit in a word
- `__builtin_popcountll()` for counting states in a set instead of maintaining `count` manually
- `union_sets()` function for merging two `set_of_states` with a single word-by-word OR loop
- Hopcroft's algorithm for minimization — O(n log n) vs current O(n²) table filling
- Hashmap for `get_set_index`  — currently O(n) linear scan

### Code Quality
- Adding `const` keyword to read-only function parameters
- `free_dfa()` and `free_enfa()` to fix memory leaks
- Better error handling for malformed regex input

### Features
- Support for more regex operators — `+` (one or more), `?` (zero or one), character classes `[a-z]`
- Escape characters in regex

---

## References

- Sipser, *Introduction to the Theory of Computation* — Chapter 1
- Hopcroft, Motwani, Ullman, *Introduction to Automata Theory* — Chapter 2
- Aho, Lam, Sethi, Ullman, *Compilers: Principles, Techniques and Tools* (Dragon Book) — Chapter 3
- CLRS, *Introduction to Algorithms* — Chapter 22
