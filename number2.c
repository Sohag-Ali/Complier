#include <stdio.h> 
#include <string.h> 
#define NUM_STATES 11 
#define NUM_INPUTS 4 
// DFA states 
enum { D0, D1, D2, D3, D4, D5, D6, D7, D8, D9, DEAD }; 
// Input mapping: a -> 0, b -> 1, other -> 2 
int get_input(char c) { 
if (c == 'a') return 0; 
if (c == 'b') return 1; 
if (c == 'c') return 2; 
return 3; // invalid input 
} 
// Accepting states with token labels 
const char* accepting_tokens[NUM_STATES] = { 
    "",        // D0 ->  accepting 
    "a*b*",        // D1 
    "",      // D2 
    "a*b*",      // D3 
    "", // D4 
    "ab*c+",//D5 
    "a*b*", // D6 
    "a*bc+",//D7 
    "a*b*", // D8 
    "ab*c+,a*bc+",//D9 
    NULL         // DEAD 
}; 
 
int main() { 
    // DFA transition table: next_state[current_state][input_symbol] 
    int next_state[NUM_STATES][NUM_INPUTS] = { 
        /* D0 */ { D1, D2, DEAD, DEAD }, 
        /* D1 */ { D3, D4, D5, DEAD }, 
        /* D2 */ { D6, D6, D7, DEAD }, 
        /* D3 */ { D3, D2, DEAD, DEAD }, 
        /* D4 */ { D6, D8, D9, DEAD }, 
        /* D5 */ { DEAD, DEAD, D5, DEAD }, 
        /* D6 */ {  D6, D6, DEAD, DEAD }, 
        /* D7 */ { DEAD, DEAD, D7, DEAD }, 
        /* D8 */ { D6, D8, D5, DEAD }, 
        /* D9 */ { DEAD, DEAD, D9, DEAD }, 
        /* DEAD */{ DEAD, DEAD, DEAD, DEAD } 
    }; 
 
    char str[100]; 
    printf("Enter a string: "); 
    scanf("%s", str); 
 
    int state = D0; 
    for (int i = 0; str[i] != '\0'; i++) { 
        int input = get_input(str[i]); 
        state = next_state[state][input]; 
        if (state == DEAD) break; 
    } 
 
    // Check if the final state is accepting 
    if (accepting_tokens[state] != NULL) { 
        printf("%s Accepted by DFA: %s\n", str, accepting_tokens[state]); 
    } else { 
        printf("%s Rejected by all patterns\n", str); 
    } 
 
    return 0; 
}  