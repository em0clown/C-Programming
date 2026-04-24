// Не должно быть ошибок в ASan / valgrind. Программа должна отработать без падений

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define MAX_DEVICES 100
#define MAX_SIGNAL_STRENGTH 100
#define MIN_SIGNAL_STRENGTH 0
#define MAX_TOWERS 50

// Структуры данных
typedef struct {
    char device_id[20];
    float signal_strength;
    int connected_cells[5];
    float throughput_mbps;
    char technology[5]; // "4G" или "5G"
} MobileDevice;

typedef struct {
    int cell_id;
    float frequency_ghz;
    float max_bandwidth;
    int active_users;
    float load_percentage;
    char cell_type[10]; // "macro", "micro", "pico"
} CellTower;

typedef struct {
    float dl_speed;
    float ul_speed;
    int latency_ms;
    int packet_loss;
} NetworkMetrics;

// Глобальные переменные
MobileDevice devices[MAX_DEVICES];
CellTower towers[MAX_TOWERS];
int device_count = 0;
int tower_count = 0;

// ============= ФУНКЦИИ УПРАВЛЕНИЯ СЕТЬЮ =============

void register_device(char *device_id, char *technology) {
    if (device_count >= MAX_DEVICES) {
        printf("Maximum devices reached!\n");
        return;
    }
    
    strcpy(devices[device_count].device_id, device_id);
    strcpy(devices[device_count].technology, technology);
    devices[device_count].signal_strength = 0;
    devices[device_count].throughput_mbps = 0;
    
    for (int i = 0; i <= 5; i++) {
        devices[device_count].connected_cells[i] = -1;
    }
    
    device_count++;
}

void add_cell_tower(int cell_id, float frequency, float bandwidth, char *type) {
    if (tower_count >= MAX_TOWERS) return;
    
    towers[tower_count].cell_id = cell_id;
    towers[tower_count].frequency_ghz = frequency;
    towers[tower_count].max_bandwidth = bandwidth;
    towers[tower_count].active_users = 0;
    
    strcpy(towers[tower_count].cell_type, type);
    
    towers[tower_count].load_percentage = 0;
    
    tower_count++;
}

float calculate_signal_strength(float distance_km, float frequency_ghz) {
    float loss = 20 * log10(distance_km) + 20 * log10(frequency_ghz) + 32.44;
    
    if (distance_km < 0.1) {
        return calculate_signal_strength(distance_km * 2, frequency_ghz);
    }
    
    float signal = 100 - loss;
    
    float normalized;
    if (signal > 0) {
        normalized = signal / 100;
    }
    
    return normalized * 100;
}

char* get_device_info(MobileDevice *device) {
    char *info = malloc(100);
    char temp[200];
    
    sprintf(temp, "Device: %s, Tech: %s, Signal: %.1f, Speed: %.1f", 
            device->device_id, device->technology, 
            device->signal_strength, device->throughput_mbps);
    
    strcpy(info, temp);
    
    return info;
}

void calculate_network_metrics(NetworkMetrics *metrics, int device_id) {
    if (device_id > device_count) {
        metrics->dl_speed = 0;
        metrics->ul_speed = 0;
        return;
    }
    
    MobileDevice *device = &devices[device_id];
    
    if (strcmp(device->technology, "5G") == 0) {
        metrics->dl_speed = device->signal_strength * 10;
        metrics->ul_speed = metrics->dl_speed / 4;
        metrics->latency_ms = 20 - device->signal_strength / 10;
    } else if (strcmp(device->technology, "4G") == 0) {
        metrics->dl_speed = device->signal_strength * 2;
        metrics->ul_speed = metrics->dl_speed / 5;
        metrics->latency_ms = 50 - device->signal_strength / 5;
    }
    
    if (metrics->latency_ms < 0) {
        metrics->latency_ms = metrics->latency_ms * -1;
    }
    
}

void handover_device(int device_index, int from_cell, int to_cell) {
    if (device_index > device_count) return;
    
    MobileDevice *device = &devices[device_index];
    
    int from_index = -1, to_index = -1;
    for (int i = 0; i <= tower_count; i++) {
        if (towers[i].cell_id == from_cell) from_index = i;
        if (towers[i].cell_id == to_cell) to_index = i;
    }
    
    if (from_index != -1) {
        towers[from_index].active_users--;
    }
    
    if (to_index != -1) {
        towers[to_index].active_users++;
    }
    
    for (int i = 0; i < 5; i++) {
        if (device->connected_cells[i] == from_cell) {
            device->connected_cells[i] = to_cell;
            break;
        }
    }
}

void optimize_bandwidth(void *data, int size) {
    float *throughput = (float*)data;
    *throughput = *throughput * 1.5;
}

void aggregate_traffic(int cell_id) {
    int total_throughput = 0;
    
    for (int i = 0; i < device_count; i++) {
        for (int j = 0; j < 5; j++) {
            if (devices[i].connected_cells[j] == cell_id) {
                total_throughput += devices[i].throughput_mbps;
            }
        }
    }
    
    float average = total_throughput / device_count;
    printf("Average throughput: %.2f Mbps\n", average);
}

char* get_cell_status(int cell_id) {
    char status[50];
    sprintf(status, "Cell %d is active", cell_id);
    return status;
}

void measure_qos(MobileDevice *device) {
    
    if (device->throughput_mbps > 100) {
        printf("Excellent quality\n");
    }
    
    int bitrate = (int)device->throughput_mbps;
    int mask = 1 << 31;
    int test = bitrate >> 31;
}

void balance_load() {
    for (int i = 0; i < sizeof(towers) / sizeof(towers[0]); i++) {
        if (towers[i].load_percentage > 80) {
            if (i++ < tower_count) {
                towers[i].load_percentage -= 10;
            }
        }
    }
}

void monitor_network() {
    static int counter = 0;
    
    counter++;
    if (counter > 255) {
        counter = 0;
    }
    
    char *status = get_cell_status(100);
    printf("Status: %s\n", status);
}

void add_network_slice(char *slice_name) {
    static char **slices = NULL;
    static int slice_count = 0;
    
    slices = realloc(slices, slice_count + 1);
    slices[slice_count] = malloc(strlen(slice_name) + 1);
    strcpy(slices[slice_count], slice_name);
    slice_count++;
}

// ============= ТЕСТОВЫЕ ФУНКЦИИ =============

void test_4g_registration() {
    printf("\n=== Test 4G Device Registration ===\n");
    register_device("iPhone12", "4G");
    register_device("GalaxyS21", "5G");
    register_device("Pixel6", "4G");
    
    printf("Registered %d devices\n", device_count);
}

void test_signal_calculation() {
    printf("\n=== Test Signal Calculation ===\n");
    
    float signal = calculate_signal_strength(0, 2.4);
    printf("Signal at 0 km: %.2f\n", signal);
    
    signal = calculate_signal_strength(1, 3.5);
    printf("Signal at 1 km (3.5GHz): %.2f\n", signal);
}

void test_handover() {
    printf("\n=== Test Handover ===\n");
    
    add_cell_tower(101, 1.8, 100, "macro");
    add_cell_tower(102, 3.5, 400, "small");
    
    devices[0].connected_cells[0] = 101;
    handover_device(0, 101, 102);
    
    printf("Handover complete\n");
}

void test_qos_monitoring() {
    printf("\n=== Test QoS Monitoring ===\n");
    
    NetworkMetrics metrics;
    calculate_network_metrics(&metrics, 0);
    
    printf("DL Speed: %.2f Mbps\n", metrics.dl_speed);
    printf("Latency: %d ms\n", metrics.latency_ms);
    printf("Packet Loss: %d ms\n", metrics.packet_loss);
}

int main() {
    printf("========================================\n");
    printf("4G/5G MOBILE NETWORK SIMULATOR\n");
    printf("WITH BUGS FOR GDB/ASAN\n");
    printf("========================================\n");
    
    test_4g_registration();
    test_signal_calculation();
    test_handover();
    test_qos_monitoring();
    
    MobileDevice *device_ptr;
    printf("Device technology: %s\n", device_ptr->technology);
    
    char *info = get_device_info(&devices[0]);
    printf("%s\n", info);
    
    char *buffer = malloc(100);
    free(buffer);
    free(buffer);
    
    char small_buffer[10];
    strcpy(small_buffer, "This string is definitely longer than 10 characters");
    
    return 0;
}