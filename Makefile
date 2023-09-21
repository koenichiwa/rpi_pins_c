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
ifndef TEST_OUTPUT
	$(error TEST_OUTPUT is not specified. Please provide a value when invoking make.)
endif
	@mkdir -p $(TEST_OUTPUT)
	./$(OUT) $(TEST_OUTPUT)

clean:
	rm -rf $(BUILD_DIR)

$(OUT): $(SRC)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	