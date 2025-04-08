SRC_DIR := src
BUILD_DIR := build
INPUT_DIR := inputs
OUT_DIR := outputs

SCANNER := scanner
SCANNER_SRC := $(SRC_DIR)/scanner.l
SCANNER_EXE := $(BUILD_DIR)/$(SCANNER)

INPUT ?= default


.PHONY: all clean run-lex build-lex

build-lex: $(SCANNER_EXE)

$(SCANNER_EXE): $(SCANNER_SRC)
	@echo "> Compiling Lexical Analizer..."
	@mkdir -p $(BUILD_DIR)
	flex -o $(BUILD_DIR)/lex.yy.c $(SCANNER_SRC)
	gcc -I$(SRC_DIR) $(BUILD_DIR)/lex.yy.c -o $(SCANNER_EXE) -lfl
	@echo "> Finished: $(SCANNER_EXE)"

run-lex: build-lex
	@mkdir -p $(OUT_DIR)
	@if [ -f $(INPUT_DIR)/$(INPUT).cm ]; then \
			echo "> Testing: $(INPUT).cm - (Outputs put into /outputs/)"; \
			$(SCANNER_EXE) < $(INPUT_DIR)/$(INPUT).cm | tee $(OUT_DIR)/$(INPUT).out; \
		else \
			echo "> Error: $(INPUT_DIR)/$(INPUT).cm not found."; \
		fi

all: run-lex

clean:
	@echo "> Cleaning build and output files..."
	rm -rf $(BUILD_DIR)/*
	rm -rf $(OUT_DIR)/*