#include "parabola.h"

int main() {
    int M;
    
    printf("Сколько парабол? ");
    scanf("%d", &M);
    
    Parabola* parabolas = createParabolas(M);
    
    int k = countParabolaMin(parabolas, M);
    printf("\nПарабол с минимумом: %d\n", k);
    
    if (k > 0) {
        int index = findHighMinParabola(parabolas, M);
        printf("\nПарабола с самым высоким минимумом:\n");
        printf("№%d: y = %.1fx²", index + 1, parabolas[index].a);
        
        if (parabolas[index].b != 0) {
            if (parabolas[index].b > 0) printf(" + %.1fx", parabolas[index].b);
            else printf(" - %.1fx", -parabolas[index].b);
        }
        
        if (parabolas[index].c != 0) {
            if (parabolas[index].c > 0) printf(" + %.1f", parabolas[index].c);
            else printf(" - %.1f", -parabolas[index].c);
        }
        
        printf("\nМинимум = %.2f\n", getMinValue(parabolas[index]));
    }
    
    free(parabolas);
    
    return 0;
}