#include <stdio.h> 
#include <string.h> 
#define NUM_STATES 9 
#define NUM_INPUTS 4 
// DFA states 
enum { D0, D1, D2, D3, D4, D5, D6, D7, DEAD }; 
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
    "",        // D1 
    "a*b+,c*a*b+",      // D2 
    "",      // D3 
    "a*b+,c*a*b+", // D4 
    "",//D5 
    "ba*c", // D6 
    "c*a*b+",//D7 
    NULL         // DEAD 
}; 
int main() { 
    // DFA transition table: next_state[current_state][input_symbol] 
    int next_state[NUM_STATES][NUM_INPUTS] = { 
        /* D0 */ { D1, D2, D3, DEAD }, 
        /* D1 */ { D1, D4, D3, DEAD }, 
        /* D2 */ { D5, D4, D6, DEAD }, 
        /* D3 */ { D3, D7, D3, DEAD }, 
        /* D4 */ { DEAD, D4, DEAD, DEAD }, 
        /* D5 */ { D5, DEAD, D6, DEAD }, 
        /* D6 */ {  DEAD, DEAD, DEAD, DEAD }, 
        /* D7 */ { DEAD, D7, DEAD, DEAD }, 
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