//A2-Циклический сдвиг
#include <stdio.h>
#include <stdint.h>

int main() 
{
    uint32_t N, result;
    int K;

    scanf("%u", &N);
    scanf("%d", &K);
    
    result = (N >> K) | (N << (32 - K));
    printf("%u\n", result);
    
    return 0;
}