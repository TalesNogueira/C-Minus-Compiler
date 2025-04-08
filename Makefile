SRC_DIR := src
BUILD_DIR := build
INPUT_DIR := inputs
OUT_DIR := outputs

MAIN_SRC := $(SRC_DIR)/main.c
UTILS_SRC := $(SRC_DIR)/utils.c
SCANNER_SRC := $(SRC_DIR)/scanner.l

LEX_C := $(BUILD_DIR)/lex.yy.c
EXEC := $(BUILD_DIR)/compiler

INPUT ?= default

.PHONY: all clean run build

build: $(EXEC)

$(LEX_C): $(SCANNER_SRC)
	@echo "> Compiling Lexical Analizer..."
	@mkdir -p $(BUILD_DIR)
	@flex -o $@ $<

$(EXEC): $(LEX_C) $(MAIN_SRC) $(UTILS_SRC)
	@echo "> Compiling Compiler Executable..."
	@gcc -I$(SRC_DIR) $^ -o $@ -lfl
	@echo "> Generated: $(EXEC)"

run: build
	@mkdir -p $(OUT_DIR)
	@echo "> Running compiler..."
	@script -q -c "$(EXEC)" $(OUT_DIR)/default.out

all: run

clean:
	@echo "> Cleaning build and output files..."
	rm -rf $(BUILD_DIR)/*
	rm -rf $(OUT_DIR)/*
	clear