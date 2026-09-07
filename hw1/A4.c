//A4-Поиск последовательности бит

#include <stdio.h>
#include <stdint.h>

int main() {
    uint32_t N;
    int K;
    
    scanf("%u", &N);
    scanf("%d", &K);
    
    uint32_t max = 0;
    uint32_t mask = (1 << K) - 1;
    
    for (int i = 0; i <= 32 - K; i++) {
        uint32_t segment = (N >> i) & mask;
        if (segment > max) {
            max = segment;
        }
    }
    
    printf("%u\n", max);
    
    return 0;
}