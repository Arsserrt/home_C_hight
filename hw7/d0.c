//d0-Отсортировать слова
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Структура узла списка
struct list {
    char word[20];
    struct list *next;
};

// Функция создания нового узла
struct list* create_node(const char* word) {
    struct list* new_node = (struct list*)malloc(sizeof(struct list));
    if (new_node == NULL) {
        printf("Ошибка выделения памяти\n");
        exit(1);
    }
    strncpy(new_node->word, word, 19);
    new_node->word[19] = '\0';
    new_node->next = NULL;
    return new_node;
}

// Функция добавления узла в конец списка
void append_node(struct list** head, const char* word) {
    struct list* new_node = create_node(word);
    
    if (*head == NULL) {
        *head = new_node;
        return;
    }
    
    struct list* current = *head;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = new_node;
}

// Функция сортировки списка по алфавиту (сортировка вставками)
void sort_list(struct list** head) {
    if (*head == NULL || (*head)->next == NULL) {
        return;
    }
    
    struct list* sorted = NULL;
    struct list* current = *head;
    
    while (current != NULL) {
        struct list* next = current->next;
        
        // Вставка текущего узла в отсортированный список
        if (sorted == NULL || strcmp(current->word, sorted->word) < 0) {
            current->next = sorted;
            sorted = current;
        } else {
            struct list* temp = sorted;
            while (temp->next != NULL && strcmp(current->word, temp->next->word) > 0) {
                temp = temp->next;
            }
            current->next = temp->next;
            temp->next = current;
        }
        
        current = next;
    }
    
    *head = sorted;
}

// Функция вывода списка на экран
void print_list(struct list* head) {
    struct list* current = head;
    while (current != NULL) {
        printf("%s", current->word);
        if (current->next != NULL) {
            printf(" ");
        }
        current = current->next;
    }
    printf("\n");
}

// Функция освобождения памяти
void free_list(struct list** head) {
    struct list* current = *head;
    struct list* next;
    
    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
    
    *head = NULL;
}

// Функция разбора строки на слова и создания списка
struct list* parse_string(const char* str) {
    struct list* head = NULL;
    char word[20];
    int i = 0, j = 0;
    
    while (str[i] != '\0' && str[i] != '.') {
        if (str[i] == ' ') {
            if (j > 0) {
                word[j] = '\0';
                append_node(&head, word);
                j = 0;
            }
        } else {
            if (j < 19) {
                word[j++] = str[i];
            }
        }
        i++;
    }
    
    // Добавление последнего слова
    if (j > 0) {
        word[j] = '\0';
        append_node(&head, word);
    }
    
    return head;
}

int main() {
    char input[1000];
    if (fgets(input, sizeof(input), stdin) == NULL) {
        printf("Ошибка чтения строки\n");
        return 1;
    }
      
    // Создание списка из слов
    struct list* head = parse_string(input);
    
    // printf("\nСписок до сортировки:\n");
    // print_list(head);
    
    // Сортировка списка
    sort_list(&head);
    
    //printf("\nСписок после сортировки:\n");
    print_list(head);
    
    // Освобождение памяти
    free_list(&head);
    return 0;
}