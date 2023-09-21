CC = gcc
CFLAGS = -Wall -pthread
LDFLAGS = -lpigpio -lrt
SRC_DIR = ./src
BUILD_DIR = ./build
OUTPUT_NAME = test_delay
SRC = $(SRC_DIR)/main.c
OUT = $(BUILD_DIR)/$(OUTPUT_NAME)

all: $(OUT)

run: $(OUT)
	./$(OUT)

clean:
	rm -rf $(BUILD_DIR)

$(OUT): $(SRC)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)