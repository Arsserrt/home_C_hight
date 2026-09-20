//D2-Найти брата
#include <stdio.h>
#include <stdlib.h>
//#include <locale.h>
#include <windows.h>

typedef int datatype;

typedef struct tree {
    datatype key;
    struct tree *left, *right;
    struct tree *parent; // ссылка на родителя
} tree;

// Функция поиска брата по ключу
tree * findBrother(tree *root, int key) {
    if (root == NULL) return 0;
    
    // Ищем узел с заданным ключом
    tree *node = root;
    while (node != NULL && node->key != key) {
        if (key < node->key)
            node = node->left;
        else
            node = node->right;
    }
    
    // Узел с таким ключом не найден
    if (node == NULL) return 0;
    
    // Узел не имеет родителя (корень) — брата быть не может
    if (node->parent == NULL) return 0;
    
    // Ищем брата — второго ребёнка родителя
    tree *parent = node->parent;
    if (parent->left == node)
        return parent->right;
    else
        return parent->left;
}

// Вставка узла в дерево поиска
tree * insert(tree *root, int key) {
    tree *newNode = (tree *)malloc(sizeof(tree));
    newNode->key = key;
    newNode->left = newNode->right = newNode->parent = NULL;
    
    if (root == NULL) return newNode;
    
    tree *cur = root;
    tree *parent = NULL;
    while (cur != NULL) {
        parent = cur;
        if (key < cur->key)
            cur = cur->left;
        else if (key > cur->key)
            cur = cur->right;
        else {
            free(newNode);
            return root; // дубликат — не добавляем
        }
    }
    
    newNode->parent = parent;
    if (key < parent->key)
        parent->left = newNode;
    else
        parent->right = newNode;
    
    return root;
}

// Красивый вывод дерева (повёрнуто на 90°)
void printTree(tree *root, int level) {
    if (root == NULL) return;
    printTree(root->right, level + 1);
    for (int i = 0; i < level; i++) printf("    ");
    printf("%d\n", root->key);
    printTree(root->left, level + 1);
}

void freeTree(tree *root) {
    if (root == NULL) return;
    freeTree(root->left);
    freeTree(root->right);
    free(root);
}

int main(void) {
    // Принудительно устанавливаем UTF-8 для ввода и вывода
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
    tree *root = NULL;
    int n, key;
    
    printf("Сколько узлов вставить? ");
    if (scanf("%d", &n) != 1 || n <= 0) return 1;
    
    printf("Введите %d ключей (через пробел):\n", n);
    for (int i = 0; i < n; i++) {
        int v;
        if (scanf("%d", &v) != 1) {
            printf("Ошибка ввода!\n");
            freeTree(root);
            return 1;
        }
        root = insert(root, v);
    }
    
    printf("\nПостроенное дерево (повёрнуто на 90°):\n");
    printTree(root, 0);
    printf("\n");
    
    printf("Введите ключ для поиска брата: ");
    if (scanf("%d", &key) != 1) {
        freeTree(root);
        return 1;
    }
    
    tree *brother = findBrother(root, key);
    
    if (brother == NULL)
        printf("Брат не найден (узла с ключом %d нет, либо у него нет брата).\n", key);
    else
        printf("Брат узла с ключом %d имеет ключ %d\n", key, brother->key);
    
    freeTree(root);
    return 0;
}