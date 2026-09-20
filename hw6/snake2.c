#include <stdio.h>
#include <stdlib.h>
#include <time.h>
//#include <curses.h>
#include <ncurses/ncurses.h>
#include <inttypes.h>
#include <string.h>
#include <unistd.h>

#define MIN_Y  2
double DELAY = 0.1;
#define PLAYERS  2

#define FIELD_WIDTH   120    //ширина
#define FIELD_HEIGHT  21    //высота
#define FIELD_TOP     2     // строка, с которой начинается поле
#define FIELD_LEFT    2     // столбец, с которого начинается поле

enum {LEFT=1, UP, RIGHT, DOWN, STOP_GAME=KEY_F(10)};
enum {MAX_TAIL_SIZE=100, START_TAIL_SIZE=3, MAX_FOOD_SIZE=20, FOOD_EXPIRE_SECONDS=10,SEED_NUMBER=3,CONTROLS=2};


// Здесь храним коды управления змейкой
struct control_buttons
{
    int down;
    int up;
    int left;
    int right;
} control_buttons;

struct control_buttons default_controls[CONTROLS] = {{KEY_DOWN, KEY_UP, KEY_LEFT, KEY_RIGHT},
                                                    {'s', 'w', 'a', 'd'}};

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
    int color;
} snake_t;

/*
 Хвост это массив состоящий из координат x,y
 */
typedef struct tail_t
{
    int x;
    int y;
} tail_t;
/*
 Еда — это массив точек, состоящий из координат x,y, времени,
 когда данная точка была установлена, и поля, сигнализирующего,
 была ли данная точка съедена.
 */
struct food
{
    int x;
    int y;
    time_t put_time;
    char point;
    uint8_t enable;
} food[MAX_FOOD_SIZE];

void setColor(int objectType){
    attroff(COLOR_PAIR(1));
    attroff(COLOR_PAIR(2));
    attroff(COLOR_PAIR(3));
    switch (objectType)
    {
    case 1:
    { // SNAKE1
        attron(COLOR_PAIR(1));
        break;
    }
    case 2:
    { // SNAKE2
        attron(COLOR_PAIR(2));
        break;
    }
    case 3:
    { // FOOD
        attron(COLOR_PAIR(3));
        break;
    }
    }
}

void initFood(struct food f[], size_t size)
{
    struct food init = {0,0,0,0,0};
    for(size_t i=0; i<size; i++)
    {
        f[i] = init;
    }
}
/*
 Обновить/разместить текущее зерно на поле
 */
void putFoodSeed(struct food *fp)
{
    int max_x=FIELD_WIDTH, max_y=FIELD_HEIGHT;
    char spoint[2] = {0};
    //getmaxyx(stdscr, max_y, max_x);
    mvprintw(fp->y, fp->x, " ");
    fp->x = rand() % (max_x - 1) + 1; //внутри границ
    fp->y = rand() % (max_y - 2) + 2; //внутри границ
    fp->put_time = time(NULL);
    fp->point = '$';
    fp->enable = 1;
    spoint[0] = fp->point;
    setColor(3);
    mvprintw(fp->y, fp->x, "%s", spoint);
}

/*
 Разместить еду на поле
 */
void putFood(struct food f[], size_t number_seeds)
{
    for(size_t i=0; i<number_seeds; i++)
    {
        putFoodSeed(&f[i]);
    }
}

void refreshFood(struct food f[], int nfood)
{
    for(size_t i=0; i<nfood; i++)
    {
        if( f[i].put_time )
        {
            if( !f[i].enable || (time(NULL) - f[i].put_time) > FOOD_EXPIRE_SECONDS )
            {
                putFoodSeed(&f[i]);
            }
        }
    }
}
void initTail(struct tail_t t[], size_t size)
{
    struct tail_t init_t= {0,0};
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
//========================================================================
void initSnake(snake_t *head[], size_t size, int x, int y,int i, int color)
{
    head[i]    = (snake_t*)malloc(sizeof(snake_t));
tail_t*  tail  = (tail_t*) malloc(MAX_TAIL_SIZE*sizeof(tail_t));
    initTail(tail, MAX_TAIL_SIZE);
    initHead(head[i], x, y);
    head[i]->tail     = tail; // прикрепляем к голове хвост
    head[i]->tsize    = size+1;
     head[i]->controls = default_controls[i];
    //head[i]->controls = default_controls[0];
    head[i]->color    = color;
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
    attron(COLOR_PAIR(head->color));
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
    attron(COLOR_PAIR(head->color));
    for(size_t i = head->tsize-1; i>0; i--)
    {
        head->tail[i] = head->tail[i-1];
        if( head->tail[i].y || head->tail[i].x)
            mvprintw(head->tail[i].y, head->tail[i].x, "%c", ch);
    }
    head->tail[0].x = head->x;
    head->tail[0].y = head->y;
}

//========================================================================
//Проверка того, является ли какое-то из зерен съеденным,
_Bool haveEat(struct snake_t *head, struct food f[])
{
    for (size_t i = 0; i < MAX_FOOD_SIZE; i++)
    {
        if (f[i].enable && f[i].x == head->x && f[i].y == head->y)
        {
            f[i].enable = 0; // Помечаем зерно как съеденное
            return 1;        // Еда найдена
        }
    }
    return 0;
}

/*
 Увеличение хвоста на 1 элемент
 */

void addTail(struct snake_t *head)
{
    if (head->tsize >= MAX_TAIL_SIZE)
    {
        return; // Достигнут максимум
    }

    // Новая ячейка хвоста появляется в позиции последней
    head->tail[head->tsize].x = head->tail[head->tsize - 1].x;
    head->tail[head->tsize].y = head->tail[head->tsize - 1].y;
    head->tsize++;
}
//========================================================================
int checkDirection(snake_t* snake, int32_t key)
{
    // Запрещаем движение в противоположную сторону
    if (snake->direction == LEFT && key == snake->controls.right)
        return 0;
    if (snake->direction == RIGHT && key == snake->controls.left)
        return 0;
    if (snake->direction == UP && key == snake->controls.down)
        return 0;
    if (snake->direction == DOWN && key == snake->controls.up)
        return 0;

    // Проверяем, что нажата вообще одна из управляющих клавиш
    if (key == snake->controls.down ||
        key == snake->controls.up ||
        key == snake->controls.left ||
        key == snake->controls.right)
        return 1;

    return 0;
}

//Вынести тело цикла while из int main() в отдельную функцию update
//и посмотреть, как изменится профилирование
void update(struct snake_t *head, struct food f[], const int32_t key)
{
    clock_t begin = clock();
    go(head);
    goTail(head);
    if (checkDirection(head,key))
    {
        changeDirection(head, key);
    }
    refreshFood(food, SEED_NUMBER);// Обновляем еду
    if (haveEat(head,food))
    {
        addTail(head);
    }
    refresh();//Обновление экрана, вывели кадр анимации
    while ((double)(clock() - begin)/CLOCKS_PER_SEC<DELAY)
    {}
}
//========================================================================

//проверка змейки на границе
int isOutOfBounds(struct snake_t *head)
{
    if (head->x < FIELD_LEFT) return 1;
    if (head->x > FIELD_LEFT + FIELD_WIDTH - 1) return 1;
    if (head->y < FIELD_TOP) return 1;
    if (head->y > FIELD_TOP + FIELD_HEIGHT - 1) return 1;
    return 0;
}
//Проверка столкновения головы с собственным хвостом
_Bool isCrush(snake_t * snake)
{
    for (size_t i = 1; i < snake->tsize; i++)
    {
        if (snake->tail[i].x == snake->x && snake->tail[i].y == snake->y)
        {
            return 1; // Столкновение
        }
    }
    return 0;
}
//========================================================================

void repairSeed(struct food f[], size_t nfood, struct snake_t *head)
{
    for (size_t i = 0; i < head->tsize; i++)
        for (size_t j = 0; j < nfood; j++)
        {
            /* Если хвост совпадает с зерном */
            if (f[j].enable && head->tail[i].x == f[j].x && head->tail[i].y == f[j].y)
            {
                putFoodSeed(&f[j]); // Перемещаем зерно
            }
        }

    for (size_t i = 0; i < nfood; i++)
        for (size_t j = 0; j < nfood; j++)
        {
            /* Если два зерна на одной точке */
            if (i != j && f[i].enable && f[j].enable &&
                f[i].x == f[j].x && f[i].y == f[j].y)
            {
                putFoodSeed(&f[j]); // Перемещаем одно из зерен
            }
        }
}

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

//экран конца программы
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

int main()
{
//========================================================================   
snake_t* snakes[PLAYERS];
    for (int i = 0; i < PLAYERS; i++)
        initSnake(snakes,START_TAIL_SIZE,10+i*10,10+i*10,i,i+1);
//========================================================================

    initscr();
    drawBorder();
    keypad(stdscr, TRUE); // Включаем F1, F2, стрелки и т.д.
    raw();                // Откдючаем line buffering
    noecho();            // Отключаем echo() режим при вызове getch
    curs_set(FALSE);    //Отключаем курсор
    mvprintw(0, 0,"Use arrows for control. Press 'F10' for EXIT");
    timeout(0);    //Отключаем таймаут после нажатия клавиши в цикле
    initFood(food, MAX_FOOD_SIZE);
    putFood(food, SEED_NUMBER);// Кладем зерна
    int key_pressed=0;
    start_color();
    init_pair(1, COLOR_RED,COLOR_BLACK);
    init_pair(2, COLOR_BLUE,COLOR_BLACK);
    init_pair(3,COLOR_GREEN, COLOR_BLACK);
    while( key_pressed != STOP_GAME )
    {
        key_pressed = getch(); // Считываем клавишу
        for (int i = 0; i < PLAYERS; i++)
        {
            update(snakes[i], food, key_pressed);
            if(isCrush(snakes[i]) || isOutOfBounds(snakes[i]))
                gameOverScreen();//!!!!!!
            repairSeed(food, SEED_NUMBER, snakes[i]);
        }
    }
    for (int i = 0; i < PLAYERS; i++)
    {
        //printExit(snakes[i]);
        free(snakes[i]->tail);
        free(snakes[i]);
    }
    endwin(); // Завершаем режим curses mod
    return 0;
}
