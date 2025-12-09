#include <stdio.h> 
#include <string.h> 
#define MAXSTACK 200 
#define MAXTOK 200 
#define MAXSYM 50 
// ---------------------- 
// Grammar (LL(1) form) 
// E  → x K 
// K  → - T | / T 
// T  → N R 
// R  → O N | ε 
// O  → - | / 
// N  → p | q | r 
// ---------------------- 
char *NT[] = {"E","K","T","R","O","N"}; 
#define NNT 6 
// Terminals in fixed order: x, -, /, p, q, r, $ 
char *TERMINALS[] = {"x","-","/","p","q","r","$"}; 
#define NTER 7 
// Productions (RHS): 
//  1: E → x K 
//  2: K → - T 
//  3: K → / T 
//  4: T → N R 
//  5: R → O N 
//  6: R → ε 
//  7: O → - 
//  8: O → / 
//  9: N → p 
// 10: N → q 
// 11: N → r 
char *RHS[] = { 
"x K",     // 1: E -> x K 
"- T",     // 2: K -> - T 
"/ T",    
 // 3: K -> / T 
"N R",     // 4: T -> N R 
"O N",     // 5: R -> O N 
"ε",      
 // 6: R -> ε 
"-",       
"/",       
"p",       
"q",      
"r"       
// 7: O -> - 
// 8: O -> / 
// 9: N -> p 
 //10: N -> q 
 //11: N -> r 
}; 
// LL(1) table: nonterminal x terminal -> production index 
// Row order: E, K, T, R, O, N 
// Col order: x, -, /, p, q, r, $ 
int TABLE[NNT][NTER] = { 
// x  -  /  p  q  r  $ 
{  1, 0, 0, 0, 0, 0, 0},  // E 
{  0, 2, 3, 0, 0, 0, 0},  // K 
{  0, 0, 0, 4, 4, 4, 0},  // T 
{  0, 5, 5, 0, 0, 0, 6},  // R 
{  0, 7, 8, 0, 0, 0, 0},  // O 
{  0, 0, 0, 9,10,11, 0}   // N 
}; 
// ---------------------- 
// Stack 
// ---------------------- 
char stack[MAXSTACK][MAXSYM]; 
int top = -1; 
void push(char *s){ strcpy(stack[++top], s); } 
char* pop(){ return (top>=0)?stack[top--]:NULL; } 
void print_stack(){ 
printf("["); 
for(int i=top;i>=0;i--){ 
printf("%s",stack[i]); 
if(i>0) printf(", "); 
} 
printf("]"); 
} 
// ---------------------- 
// Helpers 
// ---------------------- 
int find_nt(char *x){ 
for(int i=0;i<NNT;i++) 
if(strcmp(NT[i],x)==0) return i; 
    return -1; 
} 
 
int find_t(char *x){ 
    for(int i=0;i<NTER;i++) 
        if(strcmp(TERMINALS[i],x)==0) return i; 
    return -1; 
} 
 
int tokenize(char *line, char tokens[][MAXSYM]){ 
    int n=0; 
    char *p = strtok(line," \t\n"); 
    while(p){ 
        strcpy(tokens[n++], p); 
        p = strtok(NULL," \t\n"); 
    } 
    // ensure $ at end 
    if(n==0 || strcmp(tokens[n-1],"$")!=0) 
        strcpy(tokens[n++], "$"); 
    return n; 
} 
 
// ---------------------- 
// Parser 
// ---------------------- 
int main(){ 
    char line[500]; 
    printf("Enter input string (tokens separated by spaces, e.g. 'x - p'):\n"); 
    fgets(line,sizeof(line),stdin); 
 
    char input[MAXTOK][MAXSYM]; 
    int n = tokenize(line, input); 
 
    printf("\nTokens detected:\n"); 
    for(int i=0;i<n;i++) printf("%s ",input[i]); 
    printf("\n\n"); 
 
    // initialize stack: bottom '$', then start symbol E 
    push("$"); 
    push(NT[0]); // "E" 
    int ip = 0; 
 
    printf("%-25s %-10s %-10s %-25s\n","Stack","Lookahead","Top","Production Applied"); 
    printf("-------------------------------------------------------------------\n"); 
 
    while(top >= 0){ 
        char X[MAXSYM]; 
        strcpy(X, pop());      // top of stack 
        char *a = input[ip];   // current lookahead 
 
        printf("%-25s %-10s %-10s ","", a, X); 
 
        int tindex = find_t(X); 
 
        if(tindex != -1){ // X is terminal 
            if(strcmp(X,a) == 0){ 
                printf("%-25s\n","match"); 
                ip++; // consume input token 
            } else { 
                printf("REJECTED (terminal mismatch)\n"); 
                return 0; 
            } 
            print_stack(); printf("\n"); 
            continue; 
        } 
 
        int ntindex = find_nt(X); 
        int aindex = find_t(a); 
 
        if(ntindex == -1 || aindex == -1){ 
            printf("REJECTED (symbol not in grammar)\n"); 
            return 0; 
        } 
 
        int prod = TABLE[ntindex][aindex]; 
 
        if(prod == 0){ 
            printf("REJECTED\nNo rule for (%s,%s)\n", X,a); 
            return 0; 
        } 
 
        char *rhs = RHS[prod-1]; 
 
        if(strcmp(rhs, "ε") == 0){ 
            // epsilon production 
            printf("%-25s\n","epsilon"); 
        } else { 
            printf("%-25s\n", rhs); 
        } 
 
        // push RHS of production in reverse order (if not epsilon) 
        if(strcmp(rhs, "ε") != 0){ 
            char temp[200]; 
            strcpy(temp, rhs); 
            char *p = strtok(temp," "); 
            char symbols[10][MAXSYM]; 
            int k=0; 
            while(p){ 
                strcpy(symbols[k++], p); 
                p = strtok(NULL," "); 
            } 
            for(int i = k-1; i >= 0; i--) 
                push(symbols[i]); 
        } 
        print_stack(); printf("\n"); 
    } 
 
    // ACCEPT if we consumed everything including '$' 
    if(ip == n){ 
        printf("\nACCEPTED\n"); 
    } else { 
        printf("\nREJECTED: input not fully consumed\n"); 
    } 
    return 0;  }