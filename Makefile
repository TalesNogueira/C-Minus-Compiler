SRC_DIR := src
BUILD_DIR := build
INPUT_DIR := inputs
OUT_DIR := outputs

MAIN_SRC := $(SRC_DIR)/main.c
UTILS_SRC := $(SRC_DIR)/utils.c
SCANNER_SRC := $(SRC_DIR)/scanner.l
PARSER_SRC := $(SRC_DIR)/parser.y
SYMTAB_SRC := src/symbol_table.c
SEMANTIC_SRC := src/semantic_analyzer.c
MID_CODEGEN_SRC := $(SRC_DIR)/mid_codegen.c
ASSEMBLY_CODEGEN_SRC := $(SRC_DIR)/assembly_codegen.py
BIN_CODEGEN_SRC := $(SRC_DIR)/binary_codegen.py

LEX_C := $(SRC_DIR)/lex.yy.c
PARSER_C := $(SRC_DIR)/parser.tab.c
PARSER_H := $(SRC_DIR)/parser.tab.h

EXEC := $(BUILD_DIR)/compiler

REPORT ?= report

PYTHON := python3

.PHONY: all clean run build assembly

build: $(EXEC)

$(PARSER_C) $(PARSER_H): $(PARSER_SRC)
	@echo "> Compiling Syntax Analyzer (YACC-Bison)..."	
	@bison -Wno-counterexamples -d -o $(PARSER_C) $(PARSER_SRC)

$(LEX_C): $(SCANNER_SRC) $(PARSER_H)
	@echo "> Compiling Lexical Analyzer (Flex)..."
	@flex -o $@ $<

$(EXEC): $(PARSER_C) $(PARSER_H) $(LEX_C) $(MAIN_SRC) $(UTILS_SRC) $(SYMTAB_SRC) $(SEMANTIC_SRC) $(MID_CODEGEN_SRC)
	@echo "> Linking final executable..."
	@mkdir -p $(BUILD_DIR)
	@gcc -I$(SRC_DIR) $^ -o $@ -lfl
	@chmod +x $@
	@echo "> Built: $(EXEC)"

run: build
	@echo "> Running compiler..."
	@mkdir -p $(OUT_DIR)
	@echo	"/---------------------------------------------------------------------------\n>		    C- Compiler by Tales C. Nogueira\n---------------------------------------------------------------------------/"
	@script -q -c "$(EXEC)" $(OUT_DIR)/$(REPORT).log 2>&1

assembly: run
	@echo "> Generating Assembly Code (Python3)..."
	@$(PYTHON) $(ASSEMBLY_CODEGEN_SRC)

binary: assembly
	@echo "> Generating Binary Code (Python3)..."
	@$(PYTHON) $(BIN_CODEGEN_SRC)

all: binary
	@echo "> End of compilation."

clean:
	@rm -rf $(BUILD_DIR)/*
	@rm -rf $(OUT_DIR)/* Use only if you want to delete outputs when cleaning
	@rm -f $(LEX_C) $(PARSER_C) $(PARSER_H)
	@clear
	@echo "> Cleanup complete."