#include "parabola.h"

Parabola* createParabolas(int M) {
    Parabola* parabolas = malloc(M * sizeof(Parabola));
    
    printf("Введите %d парабол (a b c):\n", M);
    for (int i = 0; i < M; i++) {
        printf("Парабола %d: ", i + 1);
        scanf("%lf %lf %lf", &parabolas[i].a, &parabolas[i].b, &parabolas[i].c);
    }
    
    return parabolas;
}

Parabola createParabola(double a, double b, double c) {
    Parabola p = {a, b, c};
    return p;
}

int hasMinimum(Parabola p) {
    return p.a > 0;
}

double getMinValue(Parabola p) {
    double x = -p.b / (2 * p.a);
    return p.a * x * x + p.b * x + p.c;
}

int countParabolaMin(Parabola* parabolas, int M) {
    int count = 0;
    for (int i = 0; i < M; i++) {
        if (hasMinimum(parabolas[i])) count++;
    }
    return count;
}

int findHightMinParabola(Parabola* parabolas, int M) {
    int bestIndex = -1;
    double bestValue = -1e9;
    
    for (int i = 0; i < M; i++) {
        if (hasMinimum(parabolas[i])) {
            double value = getMinValue(parabolas[i]);
            if (value > bestValue) {
                bestValue = value;
                bestIndex = i;
            }
        }
    }
    
    return bestIndex;
}