//C1-Префикс и суффикс

#include <stdio.h>
#include <string.h>

#define MAX_LEN 20002

void prefix_function(const char *s, int n, int *pi) 
{
    pi[0] = 0;
    for (int i = 1; i < n; i++) {
        int j = pi[i - 1];
        while (j > 0 && s[i] != s[j])
            j = pi[j - 1];
        if (s[i] == s[j])
            j++;
        pi[i] = j;
    }
}

int max_prefix_suffix(const char *a, int na, const char *b, int nb) 
{
    if (na == 0 || nb == 0) return 0;
    
    char s[MAX_LEN * 2 + 2];
    int n = 0;
    for (int i = 0; i < na; i++) s[n++] = a[i];
    s[n++] = '\x01';
    for (int i = 0; i < nb; i++) s[n++] = b[i];
    
    static int pi[MAX_LEN * 2 + 2];
    prefix_function(s, n, pi);
    return pi[n - 1];
}

int main() 
{
    static char a[MAX_LEN], b[MAX_LEN];
    
    if (scanf("%s", a) != 1) a[0] = '\0';
    if (scanf("%s", b) != 1) b[0] = '\0';
    
    int na = strlen(a);
    int nb = strlen(b);
    
    int ans1 = max_prefix_suffix(a, na, b, nb);
    int ans2 = max_prefix_suffix(b, nb, a, na);
    
    printf("%d %d\n", ans1, ans2);
    return 0;
}