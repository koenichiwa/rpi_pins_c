CC = gcc
CFLAGS = -Wall -pthread -O3
LDFLAGS = -lpigpio -lrt
SRC_DIR = ./src
BUILD_DIR = ./build
OUTPUT_NAME = test_delay
SRC = $(SRC_DIR)/main.c
OUT = $(BUILD_DIR)/$(OUTPUT_NAME)

all: $(OUT)

run: $(OUT)
ifndef OUTPUT_DIR
	$(error OUTPUT_DIR is not specified. Please provide a value when invoking make.)
endif
	@mkdir -p $(OUTPUT_DIR)
	./$(OUT) $(OUTPUT_DIR)

clean:
	rm -rf $(BUILD_DIR)

$(OUT): $(SRC)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
