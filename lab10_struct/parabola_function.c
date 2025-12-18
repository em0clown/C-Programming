#include "parabola.h"

Parabola* createParabolas(int M) {
    Parabola* parabolas = (Parabola*)malloc(M * sizeof(Parabola));
    if (parabolas == NULL) {
        printf("Ошибка выделения памяти!\n");
        exit(1);
    }
    
    printf("Введите коэффициенты для %d парабол (a b c):\n", M);
    for (int i = 0; i < M; i++) {
        printf("Парабола %d: ", i + 1);
        scanf("%lf %lf %lf", &parabolas[i].a, &parabolas[i].b, &parabolas[i].c);
    }
    
    return parabolas;
}

Parabola createParabola(double a, double b, double c) {
    Parabola p;
    p.a = a;
    p.b = b;
    p.c = c;
    return p;
}

int hasMinimum(Parabola p) {
    return p.a > 0; 
}

double getMinValue(Parabola p) {
    double x = -p.b / (2 * p.a);
    return p.a * x * x + p.b * x + p.c;
}


int countParabolasWithMin(Parabola* parabolas, int M) {
    int count = 0;
    for (int i = 0; i < M; i++) {
        if (hasMinimum(parabolas[i])) {
            count++;
        }
    }
    return count;
}


int findHighestMinParabola(Parabola* parabolas, int M) {
    int maxIndex = -1;
    double maxMinValue = -1e9; 
    
    for (int i = 0; i < M; i++) {
        if (hasMinimum(parabolas[i])) {
            double currentMin = getMinValue(parabolas[i]);
            if (currentMin > maxMinValue) {
                maxMinValue = currentMin;
                maxIndex = i;
            }
        }
    }
    
    return maxIndex;
}