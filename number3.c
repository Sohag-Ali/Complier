#include <stdio.h> 
#include <string.h> 
#define MAXSTACK 200 
#define MAXTOK 200 
#define MAXSYM 50 
// ---------------------- 
// Grammar 
// E  → T E' 
// E' → + T E' | - T E' | ε 
// T  → F T' 
// T' → * F T' | / F T' | ε 
// F  → ( E ) | id 
// ---------------------- 
char *NT[] = {"E","Eprime","T","Tprime","F"}; 
#define NNT 5 
char *TERMINALS[] = {"id","(","+", "-", "*", "/", ")", "$"}; 
#define NTER 8 
// Productions (RHS): 
// 1: E      → T Eprime 
// 2: Eprime → + T Eprime 
// 3: Eprime → - T Eprime 
// 4: Eprime → ε 
// 5: T      → F Tprime 
// 6: Tprime → * F Tprime 
// 7: Tprime → / F Tprime 
// 8: Tprime → ε 
// 9: F      → ( E ) 
// 10:F      → id 
char *RHS[] = { 
"T Eprime",      // 1 
"+ T Eprime",    // 2 
"- T Eprime",    // 3 
    "",              // 4 epsilon 
    "F Tprime",      // 5 
    "* F Tprime",    // 6 
    "/ F Tprime",    // 7 
    "",              // 8 epsilon 
    "( E )",         // 9 
    "id"             // 10 
}; 
 
// LL(1) table: nonterminal x terminal -> production index 
// Row order: E, Eprime, T, Tprime, F 
// Col order: id, (, +, -, *, /, ), $ 
int TABLE[NNT][NTER] = { 
    // id  (   +   -   *   /   )   $ 
    {  1,  1,  0,  0,  0,  0,  0,  0},  // E 
    {  0,  0,  2,  3,  0,  0,  4,  4},  // Eprime 
    {  5,  5,  0,  0,  0,  0,  0,  0},  // T 
    {  0,  0,  8,  8,  6,  7,  8,  8},  // Tprime 
    { 10,  9,  0,  0,  0,  0,  0,  0}   // F 
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
int find_nt(char *x){ for(int i=0;i<NNT;i++) if(strcmp(NT[i],x)==0) return i; return -1; } 
int find_t(char *x){ for(int i=0;i<NTER;i++) if(strcmp(TERMINALS[i],x)==0) return i; return -1; } 
 
int tokenize(char *line, char tokens[][MAXSYM]){ 
    int n=0; 
    char *p = strtok(line," \t\n"); 
    while(p){ strcpy(tokens[n++], p); p=strtok(NULL," \t\n"); } 
    if(n==0 || strcmp(tokens[n-1],"$")!=0) strcpy(tokens[n++], "$"); 
    return n; 
} 
 
// ---------------------- 
// Parser 
// ---------------------- 
int main(){ 
    char line[500]; 
    printf("Enter input string (tokens separated by spaces, e.g. 'id + id'):\n"); 
    fgets(line,sizeof(line),stdin); 
 
    char input[MAXTOK][MAXSYM]; 
    int n = tokenize(line, input); 
 
    printf("\nTokens detected:\n"); 
    for(int i=0;i<n;i++) printf("%s ",input[i]); 
    printf("\n\n"); 
 
    push("$"); 
    push(NT[0]); // start symbol 
    int ip=0; 
 
    printf("%-25s %-10s %-10s %-25s\n","Stack","Lookahead","Top","Production Applied"); 
    printf("-------------------------------------------------------------------\n"); 
 
    while(top>=0){ 
        //get top of stack 
        char X[MAXSYM]; 
        strcpy(X,pop()); 
 
        //lookahead 
        char *a = input[ip]; 
 
        printf("%-25s %-10s %-10s ","" , a, X); 
 
        //check if top of stack is terminal 
        int tindex = find_t(X); 
 
        if(tindex!=-1){ // terminal 
            if(strcmp(X,a)==0){ 
                printf("%-25s\n","match"); 
                //moving to next input token 
                ip++; 
            } else 
            {   //will be printed on production applied column of output 
                printf("REJECTED\n"); return 0; } 
 
            print_stack(); printf("\n"); 
            continue; 
        } 
 
        int ntindex = find_nt(X); 
        int aindex = find_t(a); 
 
        //check if something out of grammar 
        if(ntindex==-1 || aindex==-1){ printf("REJECTED\n"); return 0; } 
 
        //find production rule from 2D parsing table 
        int prod = TABLE[ntindex][aindex]; 
 
        if(prod==0){ printf("REJECTED\nNo rule for (%s,%s)\n", X,a); return 0; } 
 
        if(strlen(RHS[prod-1])==0) printf("%-25s\n","epsilon"); 
 
        else printf("%-25s\n", RHS[prod-1]); 
 
        //push the production rule to stack 
        if(strlen(RHS[prod-1])>0){ 
            char temp[200]; strcpy(temp,RHS[prod-1]); 
            char *p = strtok(temp," "); 
            char symbols[10][MAXSYM]; 
            int k=0; 
            while(p){ strcpy(symbols[k++],p); 
            p=strtok(NULL," "); } 
            for(int i=k-1;i>=0;i--) push(symbols[i]); 
        } 
        print_stack(); printf("\n"); 
    } 
    //if finally stack empty then accepted 
    if(strcmp(input[ip],"")==0) printf("\nACCEPTED\n"); 
    else printf("\nREJECTED: input not fully consumed\n"); 
 
    return 0; 
}