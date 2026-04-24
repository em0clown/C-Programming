#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME 100
#define MAX_LEVEL 20
#define MAX_EMPLOYEES 100

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
    
    fwrite(&count, sizeof(int), 1, file);
    size_t written = fwrite(employees, sizeof(Employee), count, file);
    
    if (written != count) {
        printf("Ошибка: Записано только %zu из %d сотрудников\n", written, count);
    } else {
        printf(" Сохранено %d сотрудников в '%s' (%ld байт)\n", 
               count, filename, ftell(file));
    }
    
    fclose(file);
}

void search_by_id(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Ошибка: Не удалось открыть файл '%s'\n", filename);
        return;
    }
    
    int count;
    if (fread(&count, sizeof(int), 1, file) != 1) {
        printf("Ошибка: Не удалось прочитать количество сотрудников\n");
        fclose(file);
        return;
    }
    
    Employee *employees = (Employee*)malloc(count * sizeof(Employee));
    if (employees == NULL) {
        printf("Ошибка: Не удалось выделить память\n");
        fclose(file);
        return;
    }
    
    if (fread(employees, sizeof(Employee), count, file) != count) {
        printf("Ошибка: Не удалось прочитать данные сотрудников\n");
        free(employees);
        fclose(file);
        return;
    }
    
    fclose(file);
    
    int search_id;
    printf("\nВведите ID для поиска: ");
    scanf("%d", &search_id);
    
    int found = -1;
    for (int i = 0; i < count; i++) {
        if (employees[i].id == search_id) {
            found = i;
            break;
        }
    }
    
    if (found != -1) {
        printf("\n СОТРУДНИК НАЙДЕН:\n");
        printf("  Имя: %s\n", employees[found].name);
        printf("  ID: %d\n", employees[found].id);
        printf("  Уровень: %s\n", employees[found].level);
    } else {
        printf("\n ОШИБКА: Сотрудник с ID %d не найден\n", search_id);
    }
    
    free(employees);
}

void compress_rle(const char *input_file, const char *output_file) {
    FILE *in = fopen(input_file, "rb");
    if (in == NULL) {
        printf("Ошибка: Не удалось открыть файл '%s' для сжатия\n", input_file);
        return;
    }
    
    FILE *out = fopen(output_file, "wb");
    if (out == NULL) {
        printf("Ошибка: Не удалось создать файл '%s'\n", output_file);
        fclose(in);
        return;
    }
    
    fseek(in, 0, SEEK_END);
    long original_size = ftell(in);
    fseek(in, 0, SEEK_SET);
    
    unsigned char current_byte, next_byte;
    unsigned char count;
    size_t bytes_read;
    
    printf("\n=== СЖАТИЕ RLE ===\n");
    printf("Сжатие файла '%s' -> '%s'\n", input_file, output_file);
    
    while (ftell(in) < original_size) {
        bytes_read = fread(&current_byte, 1, 1, in);
        if (bytes_read != 1) break;
        
        count = 1;
        
        while (count < 255 && ftell(in) < original_size) {
            fread(&next_byte, 1, 1, in);
            if (next_byte == current_byte) {
                count++;
            } else {
                fseek(in, -1, SEEK_CUR);
                break;
            }
        }
        
        fwrite(&count, 1, 1, out);
        fwrite(&current_byte, 1, 1, out);
    }
    
    fseek(out, 0, SEEK_END);
    long compressed_size = ftell(out);
    
    fclose(in);
    fclose(out);
    
    printf("  Размер до:   %ld байт (%.2f Кб)\n", original_size, original_size / 1024.0); //(1 - сжатый/оригинал) * 100%
    printf("  Размер после: %ld байт (%.2f Кб)\n", compressed_size, compressed_size / 1024.0);
    
    double compression_ratio = (1.0 - (double)compressed_size / original_size) * 100;
    printf("  Сжатие:      %.2f%%\n", compression_ratio);
    
    if (compressed_size < original_size) {
        printf(" Сжатие выполнено успешно!\n");
    } else {
        printf(" Файл не сжался (RLE неэффективен для этих данных)\n");
    }
}

void decompress_rle(const char *input_file, const char *output_file) {
    FILE *in = fopen(input_file, "rb");
    if (in == NULL) {
        printf("Ошибка: Не удалось открыть файл '%s' для распаковки\n", input_file);
        return;
    }
    
    FILE *out = fopen(output_file, "wb");
    if (out == NULL) {
        printf("Ошибка: Не удалось создать файл '%s'\n", output_file);
        fclose(in);
        return;
    }
    
    unsigned char count, value;
    size_t bytes_read;
    
    printf("\n=== РАСПАКОВКА RLE ===\n");
    printf("Распаковка файла '%s' -> '%s'\n", input_file, output_file);
    
    while (1) {
        bytes_read = fread(&count, 1, 1, in);
        if (bytes_read != 1) break;
        
        bytes_read = fread(&value, 1, 1, in);
        if (bytes_read != 1) {
            printf("Ошибка: Некорректный формат RLE файла\n");
            break;
        }
        
        for (int i = 0; i < count; i++) {
            fwrite(&value, 1, 1, out);
        }
    }
    
    fseek(out, 0, SEEK_END);
    long decompressed_size = ftell(out);
    
    fclose(in);
    fclose(out);
    
    printf("  Распаковано %ld байт\n", decompressed_size);
    printf("✓ Распаковка выполнена успешно!\n");
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
    printf("\n=== СОЗДАНИЕ ТЕСТОВОЙ БАЗЫ ===\n");
    save_to_binary("database.dat", employees, count);
}

void read_csv_and_save(const char *csv_filename) {
    FILE *file = fopen(csv_filename, "r");
    if (file == NULL) {
        printf("Ошибка: Не удалось открыть CSV файл '%s'\n", csv_filename);
        return;
    }
    
    Employee employees[MAX_EMPLOYEES];
    int count = 0;
    char line[256];
    
    printf("\n=== ЧТЕНИЕ CSV ФАЙЛА ===\n");
    
    while (fgets(line, sizeof(line), file) != NULL && count < MAX_EMPLOYEES) {
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
        printf("Ошибка: В CSV файле нет данных\n");
        return;
    }
    
    printf("Прочитано %d сотрудников из CSV\n", count);
    
    printf("\n%-30s | %-8s | %-10s\n", "Имя", "ID", "Уровень");
    printf("%-30s-+-%-8s-+-%-10s\n", "------------------------------", "--------", "----------");
    for (int i = 0; i < count; i++) {
        printf("%-30s | %-8d | %-10s\n", 
               employees[i].name, employees[i].id, employees[i].level);
    }
    
    save_to_binary("database.dat", employees, count);
}

void compare_files(const char *original, const char *decompressed) {
    FILE *f1 = fopen(original, "rb");
    FILE *f2 = fopen(decompressed, "rb");
    
    if (f1 == NULL || f2 == NULL) {
        printf("Ошибка: Не удалось открыть файлы для сравнения\n");
        if (f1) fclose(f1);
        if (f2) fclose(f2);
        return;
    }
    
    fseek(f1, 0, SEEK_END);
    long size1 = ftell(f1);
    fseek(f1, 0, SEEK_SET);
    
    fseek(f2, 0, SEEK_END);
    long size2 = ftell(f2);
    fseek(f2, 0, SEEK_SET);
    
    printf("\n=== ПРОВЕРКА ЦЕЛОСТНОСТИ ===\n");
    printf("Оригинал:      %ld байт\n", size1);
    printf("Распаковано:   %ld байт\n", size2);
    
    if (size1 != size2) {
        printf(" ОШИБКА: Размеры файлов не совпадают!\n");
        fclose(f1);
        fclose(f2);
        return;
    }
    
    int different = 0;
    unsigned char byte1, byte2;
    long pos;
    
    for (pos = 0; pos < size1; pos++) {
        fread(&byte1, 1, 1, f1);
        fread(&byte2, 1, 1, f2);
        if (byte1 != byte2) {
            different = 1;
            break;
        }
    }
    
    if (!different) {
        printf(" Файлы идентичны! Распаковка прошла корректно.\n");
    } else {
        printf(" ОШИБКА: Файлы различаются на позиции %ld (%02X vs %02X)\n", 
               pos, byte1, byte2);
    }
    fclose(f1);
    fclose(f2);
}

int main(int argc, char *argv[]) {
    int choice;
    
    if (argc == 2) {
        if (strcmp(argv[1], "search") == 0) {
            search_by_id("database.dat");
            return 0;
        } else {
            read_csv_and_save(argv[1]);
            return 0;
        }
    }
    do {
        printf("\n--- МЕНЮ ---\n");
        printf("1. Создать тестовую базу данных (database.dat)\n");
        printf("2. Загрузить данные из CSV файла\n");
        printf("3. Поиск сотрудника по ID\n");
        printf("4. СЖАТЬ database.dat -> 123.rle\n");
        printf("5. РАСПАКОВАТЬ 123.rle -> 123_restored.dat\n");
        printf("6. Сравнить оригинал с распакованным файлом\n");
        printf("7. Полный тест (создать -> сжать -> распаковать -> сравнить)\n");
        printf("0. Выход\n");
        printf("Ваш выбор: ");
        
        if (scanf("%d", &choice) != 1) {
            printf("Ошибка: Введите число\n");
            while (getchar() != '\n');
            continue;
        }
        
        switch (choice) {
            case 1:
                create_test_database();
                break;
            case 2: {
                char filename[100];
                printf("Введите имя CSV файла: ");
                scanf("%s", filename);
                read_csv_and_save(filename);
                break;
            }
            case 3:
                search_by_id("database.dat");
                break;
            case 4:
                compress_rle("123.jpg", "123.rle");
                break;
            case 5:
                decompress_rle("123.rle", "123_restored.dat");
                break;
            case 6:
                compare_files("123.dat", "123_restored.dat");
                break;
            case 7: {
                printf("\n=== ПОЛНЫЙ ТЕСТ ===\n");
                create_test_database();
                compress_rle("123.jpg", "123.rle");
                decompress_rle("123.rle", "123_restored.dat");
                compare_files("123.jpg", "123_restored.dat");
                break;
            }
            case 0:
                printf("До свидания!\n");
                break;
            default:
                printf("Неверный выбор. Попробуйте снова.\n");
        }
    } while (choice != 0);
    
    return 0;
}