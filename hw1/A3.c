//A3-Извлечение бит

#include <stdio.h>
#include <stdint.h>

int main() {
    uint32_t N;
    int K;
    
    scanf("%u", &N);
    scanf("%d", &K);
    
    uint32_t mask = (1 << K) - 1;
    uint32_t result = N & mask;
    
    printf("%u\n", result);
    
    return 0;
}