#include <stdio.h>
#include <stdlib.h>
#include <math.h>

struct parabola{
    double a, b, c;
};

int Min(struct parabola p){
    return p.a>0;
}

double calcMin(struct parabola p){
    return p.c-(p.b*p.b)/(4 * p.a);
}

int main(){
    struct parabola test1 = {1.0, 0.0, 0.0};
    struct parabola test2 = {-1.0, 0.0, 0.0};

    printf("Тест 1 - y=x^2: ");
    printf("%d\n", Min(test1));
    if (Min(test1)){
        printf("%2f\n", calcMin(test1));
    }

    printf("Тест 2 - y=-x^2: ");
    printf("%d\n", Min(test2));
    if (Min(test2)){
        printf("%2f\n", calcMin(test2));
    }
}