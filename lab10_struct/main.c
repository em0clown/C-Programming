#include "parabola.h"

int main() {
    int M;
    
    printf("Введите количество парабол M: ");
    scanf("%d", &M);
    
    if (M <= 0) {
        printf("Количество парабол должно быть положительным!\n");
        return 1;
    }
    
    Parabola* parabolas = createParabolas(M);
    int k = countParabolasWithMin(parabolas, M);
    printf("\nКоличество парабол с минимумом: %d\n", k);
    
    if (k > 0) {
        int highestIndex = findHighestMinParabola(parabolas, M);
        
        printf("\nПарабола с наибольшим минимумом:\n");
        printf("Индекс: %d\n", highestIndex + 1);
        printf("Коэффициенты: a = %.2lf, b = %.2lf, c = %.2lf\n", 
               parabolas[highestIndex].a, 
               parabolas[highestIndex].b, 
               parabolas[highestIndex].c);
        printf("Значение минимума: %.2lf\n", getMinValue(parabolas[highestIndex]));
    } else {
        printf("Нет парабол с минимумом\n");
    }
    
    free(parabolas);
    
    Parabola demo = createParabola(1, -2, 1);
    printf("Создана парабола: y = %.1lfx² + %.1lfx + %.1lf\n", 
           demo.a, demo.b, demo.c);
    printf("Имеет минимум: %s\n", hasMinimum(demo) ? "Да" : "Нет");
    printf("Значение минимума: %.2lf\n", getMinValue(demo));
    
    return 0;
}