#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME 100
#define MAX_LEVEL 20

typedef struct {
    char name[MAX_NAME];
    int id;
    char level[MAX_LEVEL];
} Employee;

void save_to_binary(const char *filename, Employee *employees, int count) {
    FILE *file = fopen(filename, "wb");
    if (file == NULL) {
        printf("Ошибка: Не удалось создать файл '%s'\n", filename);
        return;
    }
    
    size_t written_count = fwrite(&count, sizeof(int), 1, file);
    if (written_count != 1) {
        printf("Ошибка: Не удалось записать количество сотрудников\n");
        fclose(file);
        return;
    }
    
    size_t written_employees = fwrite(employees, sizeof(Employee), count, file);
    if (written_employees != count) {
        printf("Ошибка: Записано только %zu из %d сотрудников\n", 
               written_employees, count);
    } else {
        printf("Успешно сохранено %d сотрудников в файл '%s'\n", count, filename);
        printf("Размер файла: %ld байт\n", ftell(file));
    }
    
    fclose(file);
}

void search_in_binary(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Ошибка: Не удалось открыть файл '%s'\n", filename);
        printf("Убедитесь, что файл существует и у вас есть права на чтение\n");
        return;
    }
    
    int count;
    size_t read_count = fread(&count, sizeof(int), 1, file);
    if (read_count != 1) {
        printf("Ошибка: Не удалось прочитать количество сотрудников\n");
        fclose(file);
        return;
    }
    
    if (count <= 0 || count > 1000) {
        printf("Ошибка: Некорректное количество сотрудников (%d)\n", count);
        fclose(file);
        return;
    }
    
    printf("В файле найдено %d сотрудников\n", count);
    
    Employee *employees = (Employee*)malloc(count * sizeof(Employee));
    if (employees == NULL) {
        printf("Ошибка: Не удалось выделить память для %d сотрудников\n", count);
        fclose(file);
        return;
    }
    
    size_t read_employees = fread(employees, sizeof(Employee), count, file);
    if (read_employees != count) {
        printf("Ошибка: Прочитано только %zu из %d сотрудников\n", 
               read_employees, count);
        free(employees);
        fclose(file);
        return;
    }
    
    fclose(file);
    
    int search_id;
    printf("\n========================================\n");
    printf("ПОИСК СОТРУДНИКА\n");
    printf("========================================\n");
    printf("Введите ID сотрудника для поиска: ");
    
    if (scanf("%d", &search_id) != 1) {
        printf("Ошибка: Введено некорректное значение ID\n");
        free(employees);
        return;
    }
    
    int found_index = -1;
    for (int i = 0; i < count; i++) {
        if (employees[i].id == search_id) {
            found_index = i;
            break;
        }
    }
    
    printf("\n========================================\n");
    if (found_index != -1) {
        printf(" СОТРУДНИК НАЙДЕН!\n");
        printf("========================================\n");
        printf("| %-16s | %-29s |\n", "Поле", "Значение");
        printf("----------------------------------------\n");
        printf("| %-12s | %-21s |\n", "Name", employees[found_index].name);
        printf("| %-12s | %-21d |\n", "ID", employees[found_index].id);
        printf("| %-12s | %-21s |\n", "Level", employees[found_index].level);
        printf("========================================\n");
    } else {
        printf(" ОШИБКА: Сотрудник с ID %d НЕ НАЙДЕН\n", search_id);
        printf("========================================\n");
        printf("Возможные причины:\n");
        printf("  - Неверно введен ID\n");
        printf("  - Сотрудник отсутствует в базе\n");
        printf("  - База данных не загружена\n");
    }
    
    free(employees);
}

void create_test_database() {
    Employee employees[] = {
        {"Elon Musk", 5001, "Level_A"},
        {"Jeff Bezos", 5002, "Level_B"},
        {"Bill Gates", 5003, "Level_C"},
        {"Mark Zuckerberg", 5004, "Level_A"},
        {"Tim Cook", 5005, "Level_B"},
        {"Satya Nadella", 5006, "Level_A"},
        {"Sundar Pichai", 5007, "Level_C"},
        {"Larry Page", 5008, "Level_B"},
        {"Sergey Brin", 5009, "Level_A"},
        {"Steve Jobs", 5010, "Level_C"}
    };
    int count = sizeof(employees) / sizeof(employees[0]);
    printf("\n=== СОЗДАНИЕ БАЗЫ ДАННЫХ ===\n");
    printf("Создано %d тестовых сотрудников\n", count);
    save_to_binary("database.dat", employees, count);
}

int main(int argc, char *argv[]) {
    int choice;
    
    printf("\n========================================\n");
    printf("СИСТЕМА УПРАВЛЕНИЯ СОТРУДНИКАМИ\n");
    printf("========================================\n");
    
    if (argc == 2 && strcmp(argv[1], "search") == 0) {
        search_in_binary("database.dat");
        return 0;
    }
    
    do {
        printf("\n--- МЕНЮ ---\n");
        printf("1. Создать тестовую базу данных (database.dat)\n");
        printf("2. Поиск сотрудника по ID\n");
        printf("3. Выход\n");
        printf("Ваш выбор: ");
        
        if (scanf("%d", &choice) != 1) {
            printf("Ошибка: Введите число\n");
            while (getchar() != '\n'); // Очистка буфера
            continue;
        }
        
        switch (choice) {
            case 1:
                create_test_database();
                break;
            case 2:
                search_in_binary("database.dat");
                break;
            case 3:
                printf("До свидания!\n");
                break;
            default:
                printf("Неверный выбор. Попробуйте снова.\n");
        }
    } while (choice != 3);
    
    return 0;
}