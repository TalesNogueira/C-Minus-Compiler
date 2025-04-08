SRC_DIR := src
BUILD_DIR := build
INPUT_DIR := inputs
OUT_DIR := outputs

MAIN_SRC := $(SRC_DIR)/main.c
UTILS_SRC := $(SRC_DIR)/utils.c
SCANNER_SRC := $(SRC_DIR)/scanner.l
PARSER_SRC := $(SRC_DIR)/parser.y

LEX_C := $(SRC_DIR)/lex.yy.c
PARSER_C := $(SRC_DIR)/parser.tab.c
PARSER_H := $(SRC_DIR)/parser.tab.h

EXEC := $(BUILD_DIR)/compiler

INPUT ?= default

.PHONY: all clean run build

build: $(EXEC)

$(PARSER_C) $(PARSER_H): $(PARSER_SRC)
	@echo "> Compiling Syntax Analyzer (YACC-Bison)..."
	@bison -d -o $(PARSER_C) $(PARSER_SRC)

$(LEX_C): $(SCANNER_SRC) $(PARSER_H)
	@echo "> Compiling Lexical Analyzer (Flex)..."
	@mkdir -p $(BUILD_DIR)
	@flex -o $@ $<

$(EXEC): $(PARSER_C) $(PARSER_H) $(LEX_C) $(MAIN_SRC) $(UTILS_SRC)
	@echo "> Linking final executable..."
	@gcc -I$(SRC_DIR) -I$(BUILD_DIR) $^ -o $@ -lfl
	@chmod +x $@
	@echo "> Built: $(EXEC)"

run: build
	@mkdir -p $(OUT_DIR)
	@echo "> Running compiler..."
	@script -q -c "$(EXEC)" $(OUT_DIR)/default.out

all: run

clean:
	rm -rf $(BUILD_DIR)/*
	rm -rf $(OUT_DIR)/*
	rm -f $(SRC_DIR)/parser.tab.c
	rm -f $(SRC_DIR)/parser.tab.h
	rm -f $(SRC_DIR)/lex.yy.c
	clear