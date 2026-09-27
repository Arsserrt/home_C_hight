#include <stdio.h>
#include <unistd.h>
#include <math.h>

#define EPS1 0.00001 // точность нахожденяи точек пересечения
#define EPS2 0.001   // точность нахожденяи площади

static double f(double x) { return 0.6 * x + 3; }                     // f(x) = 0.6 * x + 3;
static double g(double x) { return (x - 2) * (x - 2) * (x - 2) - 1; } // g(x) = (x-2)^3 - 1;
static double h(double x) { return 3 / x; }                           // h(x) = (3/x);

/* Тип указателя на функцию одной переменной */
typedef double (*func_t)(double);

/*
 * root: находит корень уравнения f(x) = g(x) на отрезке [a, b]
 *       с точностью eps1 методом деления отрезка пополам (бисекции).
 *
 * Параметры:
 *   f, g  - указатели на функции
 *   a, b  - границы отрезка (f(a)-g(a) и f(b)-g(b) должны иметь разные знаки)
 *   eps1  - требуемая точность по аргументу x
 *
 * Возвращает:
 *   приближённое значение корня x, при котором |f(x) - g(x)| <= eps1
 */
double root(func_t f, func_t g, double a, double b, double eps1)
{
    double fa = f(a) - g(a);
    double fb = f(b) - g(b);
    double c, fc;

    /* Проверка условия применимости метода */
    if (fa * fb > 0.0)
    {
        fprintf(stderr, "root:  not suitable for root"
                        "[%g, %g]\n",
                a, b);
        return NAN;
    }

    /* Если один из концов уже является корнем */
    if (fabs(fa) <= eps1)
        return a;
    if (fabs(fb) <= eps1)
        return b;

    /* Основной цикл бисекции */
    while ((b - a) > eps1)
    {
        c = 0.5 * (a + b);
        fc = f(c) - g(c);

        if (fabs(fc) <= eps1)
            return c;

        if (fa * fc < 0.0)
        {
            /* корень на [a, c] */
            b = c;
            fb = fc;
        }
        else
        {
            /* корень на [c, b] */
            a = c;
            fa = fc;
        }
    }

    /* Возвращаем середину итогового отрезка */
    return 0.5 * (a + b);
}

/*
double integral(f, a, b, eps2) // вычисление полощади под кривой
{
}
*/

void testroot()
{
    printf("intersection f/h 1\n");
    double a = -7, b = -5;
    double x = root(f, h, a, b, EPS1);
    if (!isnan(x))
    {
        printf("root:     x      = %.10f\n", x);
        printf("check: f(x)-h(x) = %.3e\n", f(x) - h(x));
    }
    printf("intersection f/h 2\n");
    a = 0, b = 2;
    x = root(f, h, a, b, EPS1);
    if (!isnan(x))
    {
        printf("root:     x      = %.10f\n", x);
        printf("check: f(x)-h(x) = %.3e\n", f(x) - h(x));
    }
    printf("intersection g/h 1\n");
    a = -1, b = -0.1;
    x = root(g, h, a, b, EPS1);
    if (!isnan(x))
    {
        printf("root:     x      = %.10f\n", x);
        printf("check: g(x)-h(x) = %.3e\n", g(x) - h(x));
    }
    printf("intersection g/h 2\n");
    a = 2, b = 4;
    x = root(g, h, a, b, EPS1);
    if (!isnan(x))
    {
        printf("root:     x      = %.10f\n", x);
        printf("check: g(x)-h(x) = %.3e\n", g(x) - h(x));
    }
}

int main(int argc, char *argv[])
{

    int rez = 0;
    if (argc == 1) // если программу запустили без аргументов
    {
        printf("Why was I created?\n");
        return 0;
    }
    while ((rez = getopt(argc, argv, "habri")) != -1)
    {
        switch (rez)
        {
        case 'h':
            printf("This program calculates the area of intersection of the functions.\n");
            printf("Usage: program [options]\n");
            printf("  -h           This help text\n");
            printf("  -a           The abscissas of the points\n"); // печать абсциссы пересечения
            printf("               of intersection of the curves are printed\n");
            printf("  -b           The number of iterations required to\n"); // печать количества итераций для поиска точек
            printf("               find the intersection points is printed\n");
            printf("  -r           TEST root\n");
            printf("  -i           TEST integral\n");
            return 0;
            break;
        case 'a':
            printf("_____print abscissas_______\n");
            printf("f/h x1 = %.10f\n", root(f, h, -7, -5, EPS1));
            printf("f/h x2 = %.10f\n", root(f, h, 0, 2, EPS1));
            printf("g/h x1 = %.10f\n", root(g, h, -1, -0.1, EPS1));
            printf("g/h x2 = %.10f\n", root(g, h, 2, 4, EPS1));
            break;
        case 'b':
            break;
        case 'r':
            printf("__________test root_________\n");
            testroot();
            break;
        case 'i':
            printf("found argument \"m = %s\".\n", optarg);
            break;
        case '?':
            printf("Error found! Use -h for help.\n");
            break;
        };
    }
    return 0;
}