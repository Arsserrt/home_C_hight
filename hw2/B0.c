//B0-Максимальный блок

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>  

typedef struct list {
    uint64_t address;
    size_t size;
    char comment[64];
    struct list *next;
} list;

uint64_t findMaxBlock(list *head) {
    if (head == NULL) {
        return 0;
    }
    
    list *current = head;
    list *max_block = head;
    size_t max_size = head->size;
    
    while (current != NULL) {
        if (current->size > max_size) {
            max_size = current->size;
            max_block = current;
        }
        current = current->next;
    }
    
    return max_block->address;
}

int main() {
    int n;
    list *head = NULL;
    list *tail = NULL;
    
    // Читаем количество блоков
    if (scanf("%d", &n) != 1) {
        printf("Ошибка ввода\n");
        return 1;
    }
    
    // Создаём список
    for (int i = 0; i < n; i++) {
        list *new_node = (list*)malloc(sizeof(list));
        if (new_node == NULL) {
            printf("Ошибка выделения памяти\n");
            list *current = head;
            while (current != NULL) {
                list *temp = current;
                current = current->next;
                free(temp);
            }
            return 1;
        }
        
        // %"SCNu64" - для uint64_t (беззнаковое 64-битное)
        // %zu - для size_t
        if (scanf("%" SCNu64 " %zu", &new_node->address, &new_node->size) != 2) {
            printf("Ошибка ввода данных\n");
            free(new_node);
            list *current = head;
            while (current != NULL) {
                list *temp = current;
                current = current->next;
                free(temp);
            }
            return 1;
        }
        
        strcpy(new_node->comment, "");
        new_node->next = NULL;
        
        if (head == NULL) {
            head = new_node;
            tail = new_node;
        } else {
            tail->next = new_node;
            tail = new_node;
        }
    }
    
    uint64_t result = findMaxBlock(head);
    
    // Используем правильный спецификатор для вывода uint64_t
    printf("%" PRIu64 "\n", result);
    
    // Освобождаем память
    list *current = head;
    while (current != NULL) {
        list *temp = current;
        current = current->next;
        free(temp);
    }
    
    return 0;
}