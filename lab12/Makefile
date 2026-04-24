CC = clang

CFLAGS = -Wall -Iinclude

SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj
BIN_DIR = bin

SOURCES = $(wildcard $(SRC_DIR)/*.c)

OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SOURCES))

TARGET = $(BIN_DIR)/smarthome

all: $(TARGET)
	@echo "Сборка завершена! Исполняемый файл: $(TARGET)"

$(OBJ_DIR) $(BIN_DIR):
	mkdir -p $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJECTS) | $(BIN_DIR)
	$(CC) $(OBJECTS) -o $@

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
	@echo "Очистка завершена"

run: $(TARGET)
	./$(TARGET)

info:
	@echo "Компилятор: $(CC)"
	@echo "Флаги: $(CFLAGS)"
	@echo "Исходные файлы: $(SOURCES)"
	@echo "Объектные файлы: $(OBJECTS)"

.PHONY: all clean run info