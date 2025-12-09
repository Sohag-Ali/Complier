#include <stdio.h> 
#include <string.h> 
#define MAXSTACK 200 
#define MAXTOK   200 
#define MAXSYM   50 
// ---------------------- 
// Grammar (LL(1) form) 
// S → a B D h 
// B → c C 
// C → b C | ε 
// D → E F 
// E → g | ε 
// F → f | ε 
// ---------------------- 
// Nonterminals 
char *NT[] = {"S","B","C","D","E","F"}; 
#define NNT 6 
// Terminals in fixed order: a, b, c, f, g, h, $ 
char *TERMINALS[] = {"a","b","c","f","g","h","$"}; 
#define NTER 7 
// Productions (RHS): 
//  1: S → a B D h 
//  2: B → c C 
//  3: C → b C 
//  4: C → ε 
//  5: D → E F 
//  6: E → g 
//  7: E → ε 
//  8: F → f 
//  9: F → ε 
char *RHS[] = { 
"a B D h",  // 1 
"c C",     
 // 2 
"b C",     
"ε",        
 // 3 
// 4 
"E F",      // 5 
"g",        
// 6 
"ε",        
"f",       
"ε"        
}; 
// 7 
 // 8 
 // 9 
// LL(1) table: nonterminal x terminal -> production index 
// Row order: S, B, C, D, E, F 
// Col order: a, b, c, f, g, h, $ 
int TABLE[NNT][NTER] = { 
// a  b  c  f  g  h  $ 
{  1, 0, 0, 0, 0, 0, 0}, // S 
{  0, 0, 2, 0, 0, 0, 0}, // B 
{  0, 3, 0, 4, 4, 4, 0}, // C 
{  0, 0, 0, 5, 5, 5, 0}, // D 
{  0, 0, 0, 7, 6, 7, 0}, // E 
{  0, 0, 0, 8, 0, 9, 0}  // F 
}; 
// ---------------------- 
// Stack 
// ---------------------- 
char stack[MAXSTACK][MAXSYM]; 
int top = -1; 
void push(char *s){ strcpy(stack[++top], s); } 
char* pop(){ return (top >= 0) ? stack[top--] : NULL; } 
void print_stack(){ 
printf("["); 
for(int i = top; i >= 0; i--){ 
printf("%s", stack[i]); 
if(i > 0) printf(", "); 
    } 
    printf("]"); 
} 
 
// ---------------------- 
// Helpers 
// ---------------------- 
int find_nt(char *x){ 
    for(int i = 0; i < NNT; i++) 
        if(strcmp(NT[i], x) == 0) return i; 
    return -1; 
} 
 
int find_t(char *x){ 
    for(int i = 0; i < NTER; i++) 
        if(strcmp(TERMINALS[i], x) == 0) return i; 
    return -1; 
} 
 
int tokenize(char *line, char tokens[][MAXSYM]){ 
    int n = 0; 
    char *p = strtok(line, " \t\n"); 
    while(p){ 
        strcpy(tokens[n++], p); 
        p = strtok(NULL, " \t\n"); 
    } 
    // ensure $ at end 
    if(n == 0 || strcmp(tokens[n-1], "$") != 0) 
        strcpy(tokens[n++], "$"); 
    return n; 
} 
 
// ---------------------- 
// Parser 
// ---------------------- 
int main(){ 
    char line[500]; 
    printf("Enter input string (tokens separated by spaces, e.g. 'a c b g f h'):\n"); 
    fgets(line, sizeof(line), stdin); 
 
    char input[MAXTOK][MAXSYM]; 
    int n = tokenize(line, input); 
 
    printf("\nTokens detected:\n"); 
    for(int i = 0; i < n; i++) printf("%s ", input[i]); 
    printf("\n\n"); 
 
    // init stack: bottom '$', then start symbol S 
    push("$"); 
    push(NT[0]); // "S" 
    int ip = 0; 
 
    printf("%-25s %-10s %-10s %-25s\n","Stack","Lookahead","Top","Production Applied"); 
    printf("-------------------------------------------------------------------\n"); 
 
    while(top >= 0){ 
        char X[MAXSYM]; 
        strcpy(X, pop());      // stack top 
        char *a = input[ip];   // lookahead 
 
        printf("%-25s %-10s %-10s ","", a, X); 
 
        int tindex = find_t(X); 
 
        if(tindex != -1){ // X is terminal 
            if(strcmp(X, a) == 0){ 
                printf("%-25s\n","match"); 
                ip++; // consume token 
            } else { 
                printf("REJECTED (terminal mismatch)\n"); 
                return 0; 
            } 
            print_stack(); printf("\n"); 
            continue; 
        } 
 
        int ntindex = find_nt(X); 
        int aindex  = find_t(a); 
 
        if(ntindex == -1 || aindex == -1){ 
            printf("REJECTED (symbol not in grammar)\n"); 
            return 0; 
        } 
 
        int prod = TABLE[ntindex][aindex]; 
 
        if(prod == 0){ 
            printf("REJECTED\nNo rule for (%s,%s)\n", X, a); 
            return 0; 
        } 
 
        char *rhs = RHS[prod - 1]; 
 
        if(strcmp(rhs, "ε") == 0){ 
            // epsilon production 
            printf("%-25s\n","epsilon"); 
        } else { 
            printf("%-25s\n", rhs); 
        } 
 
        // push RHS symbols in reverse order (if not epsilon) 
        if(strcmp(rhs, "ε") != 0){ 
            char temp[200]; 
            strcpy(temp, rhs); 
            char *p = strtok(temp, " "); 
            char symbols[10][MAXSYM]; 
            int k = 0; 
            while(p){ 
                strcpy(symbols[k++], p); 
                p = strtok(NULL, " "); 
            } 
            for(int i = k - 1; i >= 0; i--) 
                push(symbols[i]); 
        } 
print_stack(); printf("\n"); 
} 
// ACCEPT if we consumed all tokens including '$' 
if(ip == n){ 
printf("\nACCEPTED\n"); 
} else { 
printf("\nREJECTED: input not fully consumed\n"); 
} 
return 0; 
}