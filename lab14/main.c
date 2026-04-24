#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

struct room {
    char name[50];
    int level;
    int number;
    int resolution;
};

struct node {
    struct node *down;  
    struct room data;   
    struct node *next;  
};

struct list {
    struct node *head;  
    struct node *tail;
    struct node *current;
    int size;
};

struct node* createNode(struct room data);
void addToEnd(struct list *list, struct room data); 
struct list* createEmptyList();
void printList(struct list *list);
void printCurrentNode(struct node *current);
void printMap(struct list *topList, struct list *bottomList, int currentList, struct node *currentNode);
void fillRandomRoom(struct room *room, int number);
struct node* getPrev(struct list *list, struct node *current);
void navigate(struct list *topList, struct list *bottomList);

int main(int argc, char *argv[]) {
    srand(time(NULL));
    
    int N, K; 
    
    if (argc == 3) {
        N = atoi(argv[1]);
        K = atoi(argv[2]);
    } else {
        printf("Введите количество элементов верхнего списка (k): ");
        scanf("%d", &N);
        printf("Введите количество элементов нижнего списка (n): ");
        scanf("%d", &K);
    }
    
    if (N <= 0 || K <= 0) {
        printf("Размеры списков должны быть положительными!\n");
        return 1;
    }
    
    struct list *bottomList = createEmptyList();
    for (int i = 1; i <= K; i++) {
        struct room r;
        fillRandomRoom(&r, i);
        addToEnd(bottomList, r);
    }
    
    struct list *topList = createEmptyList();
    for (int i = 1; i <= N; i++) {
        struct room r;
        fillRandomRoom(&r, K + i);
        addToEnd(topList, r);
    }
    
    struct node *t = topList->head;
    struct node *b = bottomList->head;
    while (t != NULL && b != NULL) {
        t->down = b;
        t = t->next;
        b = b->next;
    }
    
    printf("\n=== ВЕРХНИЙ СПИСОК (a_{n+1} ... a_{n+k}) ===\n");
    printList(topList);
    
    printf("\n=== НИЖНИЙ СПИСОК (a_1 ... a_n) ===\n");
    printList(bottomList);
    
    navigate(topList, bottomList);
    
    struct node *current = topList->head;
    while (current) {
        struct node *temp = current;
        current = current->next;
        free(temp);
    }
    free(topList);
    
    current = bottomList->head;
    while (current) {
        struct node *temp = current;
        current = current->next;
        free(temp);
    }
    free(bottomList);
    
    return 0;
}

struct node* createNode(struct room data) {
    struct node *newNode = (struct node*)malloc(sizeof(struct node));
    if (!newNode) {
        printf("Ошибка выделения памяти!\n");
        exit(1);
    }
    newNode->data = data;
    newNode->next = NULL;
    newNode->down = NULL; 
    return newNode;
}

struct list* createEmptyList() {
    struct list *list = (struct list*)malloc(sizeof(struct list));
    if (!list) {
        printf("Ошибка выделения памяти!\n");
        exit(1);
    }
    list->head = NULL;
    list->tail = NULL;
    list->current = NULL;
    list->size = 0;
    return list;
}

void addToEnd(struct list *list, struct room data) {
    struct node *newNode = createNode(data);
    
    if (list->head == NULL) {
        list->head = newNode;
        list->tail = newNode;
        list->current = newNode;
    } else {
        list->tail->next = newNode;
        list->tail = newNode;
    }
    list->size++;
}

struct node* getPrev(struct list *list, struct node *current) {
    if (list->head == NULL || list->head == current) return NULL;
    struct node *temp = list->head;
    while (temp != NULL && temp->next != current) {
        temp = temp->next;
    }
    return temp;
}

void printList(struct list *list) {
    struct node *current = list->head;
    int i = 1;
    
    while (current) {
        printf("%d. Комната: %s (a_%d), Уровень: %d, Размер: %d\n",
               i, current->data.name, current->data.number, current->data.level,
               current->data.resolution);
        
        if (current->down) {
            printf("   -> Указатель down ведет к: %s (a_%d)\n", current->down->data.name, current->down->data.number);
        } else {
            printf("   -> Указатель down: nil\n");
        }
        
        current = current->next;
        i++;
    }
    printf("Всего элементов: %d\n", list->size);
}

void printCurrentNode(struct node *current) {
    if (current) {
        printf("\n╔══════════════════════════════════════╗\n");
        printf("║       ТЕКУЩАЯ КОМНАТА                ║\n");
        printf("╠══════════════════════════════════════╣\n");
        printf("║ Название: %-26s   ║\n", current->data.name);
        printf("║ Индекс a: a%-25d ║\n", current->data.number);
        printf("║ Уровень:  %-26d ║\n", current->data.level);
        printf("║ Размер:   %-26d ║\n", current->data.resolution);
        
        if (current->down) {
            printf("╠══════════════════════════════════════╣\n");
            printf("║ Связана по down с:                   ║\n");
            printf("║   Название: %-24s ║\n", current->down->data.name);
            printf("║   Индекс a: a%-23d ║\n", current->down->data.number);
        } else {
            printf("╠══════════════════════════════════════╣\n");
            printf("║ Указатель down: nil                  ║\n");
        }
        printf("╚══════════════════════════════════════╝\n");
    }
}

void printMap(struct list *topList, struct list *bottomList, int currentList, struct node *currentNode) {
    printf("\n╔══════════════════════════════════════════════════════════════════╗\n");
    printf("║                          КАРТА ПОДЗЕМЕЛЬЯ                        ║\n");
    printf("╚══════════════════════════════════════════════════════════════════╝\n\n");
    
    printf("  ВЕРХНИЙ УРОВЕНЬ:\n  S -> ");
    struct node *temp = topList->head;
    while (temp) {
        if (temp == currentNode && currentList == 1) {
            printf("[★ a%d ★] ", temp->data.number);
        } else {
            printf("[ a%d ] ", temp->data.number);
        }
        
        if (temp->next) {
            printf("→ ");
        } else {
            printf("→ nil");
        }
        temp = temp->next;
    }
    printf("\n\n");
    
    printf("\t");
    temp = topList->head;
    while (temp) {
        if (temp->down) {
            printf("  |       ");
        } else {
            printf("\t");
        }
        temp = temp->next;
    }
    printf("\n");
    printf("\t");
    temp = topList->head;
    while (temp) {
        if (temp->down) {
            printf("  v     ");
        } else {
            printf("\t");
        }
        temp = temp->next;
    }
    printf("\n\n");
    
    printf("  НИЖНИЙ УРОВЕНЬ:\n       ");
    temp = bottomList->head;
    while (temp) {
        if (temp == currentNode && currentList == 2) {
            printf("[★ a%d ★] ", temp->data.number);
        } else {
            printf("[ a%d ] ", temp->data.number);
        }
        
        if (temp->next) {
            printf("→ ");
        } else {
            printf("→ nil");
        }
        temp = temp->next;
    }
    printf("\n\n");
}

void fillRandomRoom(struct room *room, int number) {
    char names[5][20] = {"Пещера", "Зал", "Лабиринт", "Темница", "Башня"};
    sprintf(room->name, "%s", names[rand() % 5]);
    room->level = rand() % 10 + 1;
    room->number = number; 
    room->resolution = rand() % 100 + 20;
}

void navigate(struct list *topList, struct list *bottomList) {
    char choice;
    int currentList = 1;
    struct node *currentNode = topList->current;
    int showMap = 1;
    
    while (1) {
        if (currentList == 1) {
            currentNode = topList->current;
        } else {
            currentNode = bottomList->current;
        }
        
        if (showMap) {
            printMap(topList, bottomList, currentList, currentNode);
        }
        
        printCurrentNode(currentNode);
        
        printf("\n> ");
        scanf(" %c", &choice);
        
        switch (choice) {
            case 'W':
            case '8':
                if (currentList == 2) {
                    struct node *tempTop = topList->head;
                    int found = 0;
                    while (tempTop != NULL) {
                        if (tempTop->down == bottomList->current) {
                            currentList = 1;
                            topList->current = tempTop;
                            printf("\n Поднялись на верхний уровень \n");
                            found = 1;
                            break;
                        }
                        tempTop = tempTop->next;
                    }
                    if (!found) {
                        printf("\n Подняться невозможно: над этой комнатой нет верхнего уровня! \n");
                    }
                } else {
                    printf("\n Вы уже на верхнем уровне! \n");
                }
                break;
                
            case 'S':
            case '2':
                if (currentList == 1) {
                    if (topList->current->down != NULL) {
                        currentList = 2;
                        bottomList->current = topList->current->down;
                        printf("\n Спустились на нижний уровень \n");
                    } else {
                        printf("\n Отсюда нет спуска вниз (указатель down = nil) \n");
                    }
                } else {
                    printf("\n Вы уже на нижнем уровне! \n");
                }
                break;
                
            case 'A':
            case '4': {
                struct list *activeList = (currentList == 1) ? topList : bottomList;
                struct node *prev = getPrev(activeList, activeList->current);
                if (prev) {
                    activeList->current = prev;
                    printf("\n◀ Переместились влево ▶\n");
                } else {
                    printf("\n Вы находитесь в начале списка! \n");
                }
                break;
            }
                
            case 'D':
            case '6': {
                struct list *activeList = (currentList == 1) ? topList : bottomList;
                if (activeList->current && activeList->current->next) {
                    activeList->current = activeList->current->next;
                    printf("\n▶ Переместились вправо ▶\n");
                } else {
                    printf("\n Достигнут конец списка (next = nil)! \n");
                }
                break;
            }
                
            case 'M':
            case 'm':
                showMap = !showMap;
                break;
                
            case 'Q':
            case 'q':
                printf("\n Выход из программы... \n");
                return;
                
            default:
                printf("\n Неверная команда!\n");
                break;
        }
    }
}