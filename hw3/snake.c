#include <stdio.h>
#include <stdlib.h>
#include <time.h>
//#include <C:/msys64/ucrt64/include/ncurses/ncurses.h>
#include <ncurses/ncurses.h>
#include <inttypes.h>
#include <string.h>
#include <unistd.h>

//#define MIN_X  0
//#define MIN_Y  2
#define FIELD_WIDTH   60    //ширина
#define FIELD_HEIGHT  20    //высота
#define FIELD_TOP     2     // строка, с которой начинается поле
#define FIELD_LEFT    2     // столбец, с которого начинается поле

enum {LEFT=1, UP, RIGHT, DOWN, STOP_GAME=KEY_F(10)};
enum {MAX_TAIL_SIZE=100, START_TAIL_SIZE=10, MAX_FOOD_SIZE=20, FOOD_EXPIRE_SECONDS=10};


// Здесь храним коды управления змейкой
struct control_buttons
{
    int down;
    int up;
    int left;
    int right;
}control_buttons;

struct control_buttons default_controls = {KEY_DOWN, KEY_UP, KEY_LEFT, KEY_RIGHT};

/*
 Голова змейки содержит в себе
 x,y - координаты текущей позиции
 direction - направление движения
 tsize - размер хвоста
 *tail -  ссылка на хвост
 */
typedef struct snake_t
{
    int x;
    int y;
    int direction;
    size_t tsize;
    struct tail_t *tail;
    struct control_buttons controls;
} snake_t;

/*
 Хвост это массив состоящий из координат x,y
 */
typedef struct tail_t
{
    int x;
    int y;
} tail_t;

void initTail(struct tail_t t[], size_t size)
{
    struct tail_t init_t={0,0};
    for(size_t i=0; i<size; i++)
    {
        t[i]=init_t;
    }
}
void initHead(struct snake_t *head, int x, int y)
{
    head->x = x;
    head->y = y;
    head->direction = RIGHT;
}

void initSnake(snake_t *head, size_t size, int x, int y)
{
tail_t*  tail  = (tail_t*) malloc(MAX_TAIL_SIZE*sizeof(tail_t));
    initTail(tail, MAX_TAIL_SIZE);
    initHead(head, x, y);
    head->tail = tail; // прикрепляем к голове хвост
    head->tsize = size+1;
    head->controls = default_controls;
}

/*
 Движение головы с учетом текущего направления движения
 */
void go(struct snake_t *head)
{
    char ch = '@';
    int max_x=0, max_y=0;
    getmaxyx(stdscr, max_y, max_x); // macro - размер терминала
    mvprintw(head->y, head->x, " "); // очищаем один символ
    switch (head->direction)
    {
        case LEFT:
            if(head->x <= 0) // Циклическое движение, чтобы не
// уходить за пределы экрана
                head->x = max_x;
            mvprintw(head->y, --(head->x), "%c", ch);
        break;
        case RIGHT:
            mvprintw(head->y, ++(head->x), "%c", ch);
        break;
        case UP:
            mvprintw(--(head->y), head->x, "%c", ch);
        break;
        case DOWN:
            mvprintw(++(head->y), head->x, "%c", ch);
        break;
        default:
        break;
    }
    refresh();
}

void changeDirection(struct snake_t* snake, const int32_t key)
{
    if (key == snake->controls.down)
        snake->direction = DOWN;
    else if (key == snake->controls.up)
        snake->direction = UP;
    else if (key == snake->controls.right)
        snake->direction = RIGHT;
    else if (key == snake->controls.left)
        snake->direction = LEFT;
}

/*
 Движение хвоста с учетом движения головы
 */
void goTail(struct snake_t *head)
{
    char ch = '*';
    mvprintw(head->tail[head->tsize-1].y, head->tail[head->tsize-1].x, " ");
    for(size_t i = head->tsize-1; i>0; i--)
    {
        head->tail[i] = head->tail[i-1];
        if( head->tail[i].y || head->tail[i].x)
            mvprintw(head->tail[i].y, head->tail[i].x, "%c", ch);
    }
    head->tail[0].x = head->x;
    head->tail[0].y = head->y;
}

void gameOverScreen(void)
{
    int key = 0;

    clear();                          // очищаем весь экран
    mvprintw(LINES / 2,               // середина экрана по вертикали
             (COLS - 24) / 2,         // центрируем по горизонтали
             "End game, press 'F10'");
    refresh();

    // Ждём именно F10, игнорируя остальные клавиши
    while ((key = getch()) != KEY_F(10))
    {
        // ничего не делаем — просто ждём
    }

    endwin();                         // выходим из curses-режима
    exit(0);                          // завершаем программу
}

//проверка змейки на границе
int isOutOfBounds(struct snake_t *head)
{
    if (head->x < FIELD_LEFT) return 1;
    if (head->x > FIELD_LEFT + FIELD_WIDTH - 1) return 1;
    if (head->y < FIELD_TOP) return 1;
    if (head->y > FIELD_TOP + FIELD_HEIGHT - 1) return 1;
    return 0;
}

//проверка на врезание в себя
int isSelfCollision(struct snake_t *head)
{
    for (size_t i = 1; i < head->tsize; i++)
    {
        if (head->tail[i].x == head->x &&
            head->tail[i].y == head->y)
        {
            return 1;
        }
    }
    return 0;
}

//проверка на конец игры
void snakeFail(struct snake_t *head)
{
    if (isOutOfBounds(head)||isSelfCollision(head))
        gameOverScreen();
}

//рамка
void drawBorder()
{
    int top    = FIELD_TOP - 1;
    int bottom = FIELD_TOP + FIELD_HEIGHT;
    int left   = FIELD_LEFT - 1;
    int right  = FIELD_LEFT + FIELD_WIDTH;

    // Горизонтальные линии
    for (int x = left; x <= right; x++)
    {
        mvprintw(top, x, "=");
        mvprintw(bottom, x, "=");
    }
    // Вертикальные линии
    for (int y = top; y <= bottom; y++)
    {
        mvprintw(y, left, "|");
        mvprintw(y, right, "|");
    }
    refresh();
}

int main()
{
snake_t* snake = (snake_t*)malloc(sizeof(snake_t));
/*    
    //посмотреть размер экрана
    initscr();
    printw("Terminal size: %d x %d", LINES, COLS);
    refresh();
    getch();
    //*******
*/
    initSnake(snake,START_TAIL_SIZE,10,10);
    initscr();
    drawBorder();
    keypad(stdscr, TRUE); // Включаем F1, F2, стрелки и т.д.
    raw();                // Отключаем line buffering
    noecho();            // Отключаем echo() режим при вызове getch
    curs_set(FALSE);    //Отключаем курсор
    mvprintw(0, 0," Use arrows for control. Press 'F10' for EXIT");
    timeout(0);    //Отключаем таймаут после нажатия клавиши в цикле
    int key_pressed=0;
    while( key_pressed != STOP_GAME )
    {
        key_pressed = getch(); // Считываем клавишу
        go(snake);
        snakeFail(snake);
        goTail(snake);
        timeout(100); // Задержка при отрисовке
        changeDirection(snake, key_pressed);
    }
    free(snake->tail);
    free(snake);
    endwin(); // Завершаем режим curses mod
    return 0;
}
