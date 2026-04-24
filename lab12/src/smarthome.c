#include "smarthome.h"

static char *moduleNames[] = {"Жилой блок", "Лаборатория", "Командный центр", 
                             "Медблок", "Энергетический модуль", "Склад", 
                             "Спортзал", "Оранжерея", "Док-станция", 
                             "Научный комплекс", "Инженерный отсек", "Кухня"};
static char *modulePrefixes[] = {"Альфа", "Бета", "Гамма", "Дельта", "Эпсилон", 
                                "Зета", "Омега", "Пионер", "Восток", "Союз"};

void fillStationWithRandomData(SpaceModule *station, int size) {
    srand(time(NULL));
    
    for (int i = 0; i < size; i++) {
        char fullName[50];
        sprintf(fullName, "%s %s", 
                modulePrefixes[rand() % 10], 
                moduleNames[rand() % 12]);
        
        strcpy(station[i].moduleName, fullName);
        station[i].powerConsumption = 100 + rand() % 901;
        station[i].crew = 1 + rand() % 10;
        station[i].wearLevel = 5 + rand() % 96;
    }
}

void printStationData(SpaceModule *station, int size) {
    printf("\n--- МОДУЛИ СТАНЦИИ ---\n");
    for (int i = 0; i < size; i++) {
        printf("%d. %s | Энергия: %d кВт | Экипаж: %d | Износ: %d%%\n", 
               i+1, station[i].moduleName, 
               station[i].powerConsumption, 
               station[i].crew, 
               station[i].wearLevel);
    }
    
    int totalPower = 0, totalCrew = 0;
    for (int i = 0; i < size; i++) {
        totalPower += station[i].powerConsumption;
        totalCrew += station[i].crew;
    }
    printf("Всего: %d модулей, %d кВт, %d чел.\n", size, totalPower, totalCrew);
}

void bubbleSortByWearLevel(SpaceModule *station, int size) {
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            if (station[j].wearLevel > station[j + 1].wearLevel) {
                SpaceModule temp = station[j];
                station[j] = station[j + 1];
                station[j + 1] = temp;
            }
        }
    }
}
