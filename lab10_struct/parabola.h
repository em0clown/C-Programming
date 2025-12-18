#ifndef PARABOLA_H
#define PARABOLA_H

#include <stdio.h>
#include <stdlib.h>

typedef struct {
    double a, b, c;
} Parabola;

Parabola* createParabolas(int M);
Parabola createParabola(double a, double b, double c);
int hasMinimum(Parabola p);
double getMinValue(Parabola p);
int countParabolasWithMin(Parabola* parabolas, int M);
int findHighestMinParabola(Parabola* parabolas, int M);

#endif