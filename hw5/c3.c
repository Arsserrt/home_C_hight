//C2-Польская запись

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEN 100000

int main() 
{
    static char s[MAX_LEN];
    if (!fgets(s, sizeof(s), stdin)) return 0;
    
    static long long stack[MAX_LEN];
    int top = 0;
    
    char *token = strtok(s, " \t\n");
    while (token != NULL) 
    {
        char c = token[0];
        
        if (c >= '0' && c <= '9') 
        {
            stack[top++] = atoll(token);
        } else {
            long long b = stack[--top];
            long long a = stack[--top];
            long long res = 0;
            switch (c) 
            {
                case '+': res = a + b; break;
                case '-': res = a - b; break;
                case '*': res = a * b; break;
                case '/': res = a / b; break;
            }
            stack[top++] = res;
        }
        token = strtok(NULL, " \t\n");
    }
    
    printf("%lld\n", stack[top - 1]);
    return 0;
}