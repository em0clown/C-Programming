#ifndef SMARTHOME_H  
#define SMARTHOME_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    char moduleName[50];
    int powerConsumption;
    int crew;
    int wearLevel;
} SpaceModule;

void fillStationWithRandomData(SpaceModule *station, int size);
void printStationData(SpaceModule *station, int size);  
void bubbleSortByWearLevel(SpaceModule *station, int size);

#endif