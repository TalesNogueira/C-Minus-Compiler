SRC_DIR := src

SCANNER := scanner
SCANNER_SRC := $(SRC_DIR)/scanner.l
SCANNER_EXE := $(SRC_DIR)/$(SCANNER)

TEST_DIR := tests
TEST ?= default

OUT_DIR := outputs

.PHONY: all clean run-lex build-lex

build-lex: $(SCANNER_EXE)

$(SCANNER_EXE): $(SCANNER_SRC)
	@echo "> Compiling Lexical Analizer..."
	cd $(SRC_DIR) && flex scanner.l && gcc lex.yy.c -o $(SCANNER) -lfl
	@echo "> Finished: $(SCANNER_EXE)"

run-lex: build-lex
	@mkdir -p $(OUT_DIR)
	@if [ -f $(TEST_DIR)/$(TEST).cm ]; then \
			echo "> Testing: $(TEST).cm - Outputs put into :/outputs/"; \
			$(SCANNER_EXE) < $(TEST_DIR)/$(TEST).cm | tee $(OUT_DIR)/$(TEST).out; \
		else \
			echo "> Error: $(TEST_DIR)/$(TEST).cm not found."; \
		fi

all: run-lex

clean:
	rm -f $(SRC_DIR)/lex.yy.c $(SRC_DIR)/$(SCANNER)
	rm -rf $(OUT_DIR)/*