//A1-Шифр Цезаря
#include <stdio.h>

int main() {
    int N;
    char c;
    
    scanf("%d", &N);
    
    while ((c = getchar()) != '.') {
        if (c >= 'a' && c <= 'z') {
            int n = c - 'a';
            int p = (n + N) % 26;
            putchar('a' + p);
        } else if (c >= 'A' && c <= 'Z') {
            int n = c - 'A';
            int p = (n + N) % 26;
            putchar('A' + p);
        } else if (c == ' ') {
            putchar(' ');
        }
    }
    
    putchar('.');   
    return 0;
}