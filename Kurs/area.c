#include <stdio.h>
#include <unistd.h>
#include <math.h>

#define EPS1 0.00001 // точность нахожденяи точек пересечения
#define EPS2 0.001   // точность нахожденяи площади

//функции, образующие фигуру, для которой нужно найти искомую площадь
static double f(double x) { return 0.6 * x + 3; }                     // f(x) = 0.6 * x + 3;
static double g(double x) { return (x - 2) * (x - 2) * (x - 2) - 1; } // g(x) = (x-2)^3 - 1;
static double h(double x) { return 3 / x; }                           // h(x) = (3/x);

static int iterations = 0; //глобальная переменная количества итераций

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
        iterations++;
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

//функция для -r теста функции root
//проверяем путем нахождения разницы в значениях функций в найденных root() точках и сравенения её с 0
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

/*
 * integral: вычисляет определённый интеграл от функции f на [a, b]
 *           методом трапеций с точностью eps2.
 *
 * Параметры:
 *   f    - подынтегральная функция
 *   a, b - границы интегрирования
 *   eps2 - требуемая точность
 *
 * Возвращает: приближённое значение интеграла
 */
double integral(func_t f, double a, double b, double eps2)
{
    int n = 1;
    double h = b - a;
    double prev = 0.5 * h * (f(a) + f(b));
    double curr;

    do {
        n *= 2;
        h = (b - a) / n;
        curr = 0.0;
        for (int i = 0; i < n; i++) {
            double x0 = a + i * h;
            double x1 = x0 + h;
            curr += 0.5 * h * (f(x0) + f(x1));
        }
        if (fabs(curr - prev) < eps2)
            break;
        prev = curr;
    } while (n < (1 << 24)); /* защита от бесконечного цикла */

    return curr;
}

/*
 * integral_diff: площадь между двумя кривыми на [a, b]
 *                = ∫ |f(x) - g(x)| dx
 * как integral, только вместо оси абсцисс используется вторая функция
 */
double integral_diff(func_t f, func_t g, double a, double b, double eps2)
{
    int n = 1;
    double h = b - a;
    double prev = 0.5 * h * (fabs(f(a) - g(a)) + fabs(f(b) - g(b)));
    double curr;

    do {
        n *= 2;
        h = (b - a) / n;
        curr = 0.0;
        for (int i = 0; i < n; i++) {
            double x0 = a + i * h;
            double x1 = x0 + h;
            curr += 0.5 * h * (fabs(f(x0) - g(x0)) + fabs(f(x1) - g(x1)));
        }
        if (fabs(curr - prev) < eps2)
            break;
        prev = curr;
    } while (n < (1 << 24));

    return curr;
}

//функции для теста integral
static double one(double x)  { (void)x; return 1.0; }   //f(x)=1
static double lin(double x)  { return x; }              //f(x)=x
static double sq(double x)   { return x * x; }          //f(x)=x^2
static double sinf_(double x) { return sin(x); }        //f(x)=sin

//тест integral
//путем поиска площади простых функций и сравнения с заранее известными величинами 
void testintegral(void)
{
    printf("__________test integral_________\n");

    struct {
        const char *name;
        func_t f;
        double a, b;
        double exact;
    } cases[] = {
        {"f(x)=1     [0,1]",   one,    0.0, 1.0, 1.0},
        {"f(x)=x     [0,1]",   lin,    0.0, 1.0, 0.5},
        {"f(x)=x^2   [0,1]",   sq,     0.0, 1.0, 1.0/3.0},
        {"f(x)=sin   [0,pi]",  sinf_,    0.0, M_PI, 2.0},
    };

    for (size_t i = 0; i < sizeof(cases)/sizeof(cases[0]); i++)
    {
        double got = integral(cases[i].f, cases[i].a, cases[i].b, EPS2);
        double err = fabs(got - cases[i].exact);
        printf("%-20s got=%.10f  exact=%.10f  |err|=%.3e  %s\n",
               cases[i].name, got, cases[i].exact, err,
               (err < EPS2 ? "OK" : "FAIL"));
    }
}

//нулевая функция для теста integral_diff
static double fnull(double x)  { (void)x; return 0; } //f(x)=0

//тест integral_diff
//тот же тест integral, только в качестве второй функции в integral_diff вводится ось абсцисс выраженная через функцию f(x)=0;
void testintegral_diff(void)
{
    printf("__________test integral_diff_________\n");

    struct {
        const char *name;
        func_t f;
        double a, b;
        double exact;
    } cases[] = {
        {"f(x)=1     [0,1]",   one,    0.0, 1.0, 1.0},
        {"f(x)=x     [0,1]",   lin,    0.0, 1.0, 0.5},
        {"f(x)=x^2   [0,1]",   sq,     0.0, 1.0, 1.0/3.0},
        {"f(x)=sin   [0,pi]",  sinf_,    0.0, M_PI, 2.0},
    };

    for (size_t i = 0; i < sizeof(cases)/sizeof(cases[0]); i++)
    {
        double got = integral_diff(cases[i].f, fnull, cases[i].a, cases[i].b, EPS2);
        double err = fabs(got - cases[i].exact);
        printf("%-20s got=%.10f  exact=%.10f  |err|=%.3e  %s\n",
               cases[i].name, got, cases[i].exact, err,
               (err < EPS2 ? "OK" : "FAIL"));
    }
}

int main(int argc, char *argv[])
{

    int rez = 0;
    if (argc == 1) // если программу запустили без аргументов
    {
        double summ = 0; 

        //определяем точки пересечения
        double A = root(f, h, -7, -5, EPS1);
        double B = root(f, h, 0, 2, EPS1);
        double C = root(g, h, -1, -0.1, EPS1);
        double D = root(g, h, 2, 4, EPS1);

        //считаем площади фигур, образованных пересечением по паре функций между 
        //соответсвующими точками и складываем их в сумму
        summ += integral_diff(f, h, A, C, EPS2); // площадь от A до C между f и h
        summ += integral_diff(f, g, C, B, EPS2); // площадь от C до B между f и g
        summ += integral_diff(h, g, B, D, EPS2); // площадь от B до D между h и g
        
        //печать
        printf("The area of the figure formed by the functions\n"); 
        printf("   f(x) = 0.6 * x + 3\n");
        printf("   g(x) = (x-2)^3 - 1\n");
        printf("   h(x) = (3/x)\n");
        printf("S = %f\n",summ);
        return 0;
    }
    while ((rez = getopt(argc, argv, "habrid")) != -1)
    {
        switch (rez)
        {
        case 'h':
            printf("This program calculates the area of intersection of the functions.\n");
            printf("Usage: program [options]\n");
            printf("  -h           This help text\n");
            printf("  -a           The abscissas of the points\n"); // печать абсцисс пересечения
            printf("               of intersection of the curves are printed\n");
            printf("  -b           The number of iterations required to\n"); // печать количества итераций для поиска точек
            printf("               find the intersection points is printed\n");
            printf("  -r           TEST root\n");
            printf("  -i           TEST integral\n");
            printf("  -d           TEST integral_diff\n");
            return 0;
            break;
        case 'a': // печать абсцисс пересечения
            printf("_____print abscissas_______\n");
            printf("f/h x1 = %.10f\n", root(f, h, -7, -5, EPS1));
            printf("f/h x2 = %.10f\n", root(f, h, 0, 2, EPS1));
            printf("g/h x1 = %.10f\n", root(g, h, -1, -0.1, EPS1));
            printf("g/h x2 = %.10f\n", root(g, h, 2, 4, EPS1));
            break;
        case 'b': // печать количества итераций для поиска точек
            root(f, h, -7, -5, EPS1);
            root(f, h, 0, 2, EPS1);
            root(g, h, -1, -0.1, EPS1);
            root(g, h, 2, 4, EPS1);
            printf("___The number of iterations required to______\n"); 
            printf("___find the intersection points = %d \n",iterations);
            break;
        case 'r': // test root
            printf("__________test root_________\n");
            testroot();
            break;
        case 'i': // test integral
            testintegral();
            break;
        case 'd': //test integral_diff
            testintegral_diff();
            break;
        case '?':
            printf("Error found! Use -h for help.\n");
            break;
        };
    }
    return 0;
}