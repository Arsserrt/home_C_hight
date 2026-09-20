//D1-Вид сверху
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ====== Определение структуры дерева ====== */
typedef struct tree {
    int key;
    struct tree *left, *right;
} tree;

#ifndef BT_UP_VIEW_PRINT
#define BT_UP_VIEW_PRINT(x) printf("%d", (int)(x))
#endif

typedef struct {
    tree *node;
    int hd;          /* horizontal distance */
} bt_up_qitem;

/*
 * Гарантирует, что в массиве *arr есть элемент с индексом idx.
 * Для неотрицательных дистанций: idx = hd.
 * Для отрицательных дистанций:    idx = -hd - 1.
 */
static int bt_up_ensure(tree ***arr, size_t *cap, size_t idx)
{
    size_t oldCap, newCap, extra;
    tree **tmp;

    if (idx < *cap)
        return 1;

    oldCap = *cap;
    newCap = idx + 1;
    extra  = newCap / 2 + 16;
    newCap += extra;

    tmp = (tree **)realloc(*arr, newCap * sizeof *tmp);
    if (tmp == NULL)
        return 0;

    memset(tmp + oldCap, 0, (newCap - oldCap) * sizeof *tmp);

    *arr = tmp;
    *cap = newCap;
    return 1;
}

/* Добавление узла в динамическую очередь обхода в ширину. */
static int bt_up_push(bt_up_qitem **q, size_t *cap, size_t *head, size_t *tail,
                      tree *node, int hd)
{
    if (*tail == *cap) {
        if (*head > 0) {
            size_t used = *tail - *head;
            if (used > 0) {
                memmove(*q, *q + *head, used * sizeof **q);
            }
            *tail = used;
            *head = 0;
        } else {
            size_t newCap = (*cap == 0 ? 64 : *cap * 2);
            bt_up_qitem *tmp = (bt_up_qitem *)realloc(*q, newCap * sizeof *tmp);
            if (tmp == NULL)
                return 0;

            *q   = tmp;
            *cap = newCap;
        }
    }

    (*q)[*tail].node = node;
    (*q)[*tail].hd   = hd;
    ++(*tail);

    return 1;
}

void btUpView(tree *root)
{
    tree **pos = NULL;      /* hd >= 0: index = hd       */
    tree **neg = NULL;      /* hd <  0: index = -hd - 1  */
    bt_up_qitem *q = NULL;

    size_t posCap = 0, negCap = 0;
    size_t qCap = 64, head = 0, tail = 0;
    size_t maxPos = 0, maxNeg = 0;
    int hasNeg = 0;

    if (root == NULL)
        return;

    q = (bt_up_qitem *)malloc(qCap * sizeof *q);
    if (q == NULL)
        return;

    if (!bt_up_push(&q, &qCap, &head, &tail, root, 0))
        goto cleanup;

    while (head < tail) {
        tree *cur = q[head].node;
        int hd    = q[head].hd;
        ++head;

        if (hd >= 0) {
            size_t idx = (size_t)hd;

            if (!bt_up_ensure(&pos, &posCap, idx))
                goto cleanup;

            if (pos[idx] == NULL)
                pos[idx] = cur;

            if (idx > maxPos)
                maxPos = idx;
        } else {
            size_t idx = (size_t)(-(long long)hd - 1);

            if (!bt_up_ensure(&neg, &negCap, idx))
                goto cleanup;

            if (neg[idx] == NULL)
                neg[idx] = cur;

            if (!hasNeg || idx > maxNeg) {
                maxNeg = idx;
                hasNeg = 1;
            }
        }

        if (cur->left != NULL) {
            if (!bt_up_push(&q, &qCap, &head, &tail, cur->left, hd - 1))
                goto cleanup;
        }

        if (cur->right != NULL) {
            if (!bt_up_push(&q, &qCap, &head, &tail, cur->right, hd + 1))
                goto cleanup;
        }
    }

    {
        int first = 1;
        size_t i;

        /* Сначала отрицательные горизонтальные позиции: от самых левых к корню. */
        if (hasNeg) {
            i = maxNeg;
            for (;;) {
                if (neg[i] != NULL) {
                    if (!first)
                        putchar(' ');
                    first = 0;
                    BT_UP_VIEW_PRINT(neg[i]->key);
                }

                if (i == 0)
                    break;
                --i;
            }
        }

        /* Затем неотрицательные позиции: от корня вправо. */
        for (i = 0; i <= maxPos; ++i) {
            if (pos[i] != NULL) {
                if (!first)
                    putchar(' ');
                first = 0;
                BT_UP_VIEW_PRINT(pos[i]->key);
            }
        }

        putchar('\n');
    }

cleanup:
    free(pos);
    free(neg);
    free(q);
}

/* ====== Вставка в BST ====== */
static tree *bst_insert(tree *root, int key)
{
    if (root == NULL) {
        tree *node = (tree *)malloc(sizeof(tree));
        if (node == NULL) {
            fprintf(stderr, "Ошибка выделения памяти\n");
            exit(1);
        }
        node->key = key;
        node->left = node->right = NULL;
        return node;
    }

    if (key < root->key)
        root->left = bst_insert(root->left, key);
    else if (key > root->key)
        root->right = bst_insert(root->right, key);
    /* равные ключи игнорируем */

    return root;
}

/* ====== Освобождение дерева ====== */
static void free_tree(tree *root)
{
    if (root == NULL)
        return;
    free_tree(root->left);
    free_tree(root->right);
    free(root);
}

/* ====== main ====== */
int main(void)
{
    char line[4096];

    printf("Введите числа через пробел (конец строки - Enter):\n");
    if (fgets(line, sizeof(line), stdin) == NULL) {
        fprintf(stderr, "Ошибка чтения строки\n");
        return 1;
    }

    tree *root = NULL;
    char *p = line;
    int value;
    int count = 0;

    /* Разбор чисел из строки */
    while (*p != '\0') {
        /* Пропускаем пробелы и переводы строк */
        while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
            ++p;
        if (*p == '\0')
            break;

        char *end;
        long v = strtol(p, &end, 10);
        if (end == p) {
            /* Не число — прекращаем разбор */
            break;
        }
        value = (int)v;
        root = bst_insert(root, value);
        ++count;
        p = end;
    }

    if (count == 0) {
        printf("Не введено ни одного числа.\n");
        return 1;
    }

    printf("\nПостроено BST из %d чисел.\n", count);
    printf("Вид сверху: ");
    btUpView(root);

    free_tree(root);
    return 0;
}