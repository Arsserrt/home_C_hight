//C0-Вычеркивание

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define MAX_LEN 100000

int main() 
{
    char str[MAX_LEN];
    scanf("%s", str);
    int len = strlen(str);
    
    bool found[1000] = {false};
    int count = 0;
    
    for (int i = 0; i < len - 2; i++) 
    {
        if (str[i] == '0') continue;  // первая цифра не 0
        for (int j = i + 1; j < len - 1; j++) 
        {
            for (int k = j + 1; k < len; k++) 
            {
                int num = (str[i] - '0') * 100 
                        + (str[j] - '0') * 10 
                        + (str[k] - '0');
                if (!found[num]) 
                {
                    found[num] = true;
                    count++;
                }
            }
        }
    }
    
    printf("%d\n", count);
    return 0;
}