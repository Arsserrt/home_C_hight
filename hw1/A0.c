//A0-Максимум
#include <stdio.h>

int main() 
{
    int N;
    scanf("%d", &N);

    int max;
    int count = 0;

    for (int i = 0; i < N; i++) 
    {
        int x;
        scanf("%d", &x);

        if (i == 0) 
        {
            max = x;
            count = 1;
        } 
        else 
        {
            if (x > max) 
            {
                // Новый максимум
                max = x;
                count = 1;
            } else if (x == max) 
            {
                count++;
            }
        }
    }

    printf("%d\n", count);

    return 0;
}