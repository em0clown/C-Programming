#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 256
#define MAX_NAME 100
#define MAX_LEVEL 20

typedef struct {
    char name[MAX_NAME];
    int id;
    char level[MAX_LEVEL];
} Employee;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Использование: %s <filename.csv>\n", argv[0]);
        printf("Пример: %s staff.csv\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        printf("Ошибка: Не можем открыть файл '%s'\n", argv[1]);
        printf("Убедитесь, что файл существует и у вас есть права на чтение.\n");
        return 1;
    }

    Employee employees[100];
    int count = 0;
    char line[MAX_LINE];
    while (fgets(line, MAX_LINE, file) != NULL && count < 100) {
        line[strcspn(line, "\n")] = '\0';
        char *token = strtok(line, ";");
        if (token == NULL) continue;
        strcpy(employees[count].name, token);
        token = strtok(NULL, ";");
        if (token == NULL) continue;
        employees[count].id = atoi(token);
        token = strtok(NULL, ";");
        if (token == NULL) continue;
        strcpy(employees[count].level, token);
        count++;
    }
    
    fclose(file);
    if (count == 0) {
        printf("Ошибка: Данные в файле не найдены или файл пуст.\n");
        return 1;
    }
    printf("\n%6s | %-25s | %-8s | %-10s\n", "№", "Name", "ID", "Level");
    printf("%4s-+-%-25s-+-%-8s-+-%-10s\n", "----", "-------------------------", "--------", "----------");
    for (int i = 0; i < count; i++) {
        printf("%4d | %-25s | %-8d | %-10s\n", 
               i + 1, 
               employees[i].name, 
               employees[i].id, 
               employees[i].level);
    }
    printf("\nВсего сотрудников: %d\n", count);
    return 0;
}