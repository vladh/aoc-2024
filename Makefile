C_FILES=$(shell find src -maxdepth 1 -name \*.c -printf '%P\n')
BIN_FILES_BARE=$(C_FILES:.c=)
BIN_FILES=$(addprefix bin/,${BIN_FILES_BARE})

.PHONY: all clean

all: $(BIN_FILES)

clean:
	rm bin/*

bin/%: src/%.c
	clang \
		-Ofast \
		-D_FORTIFY_SOURCE=2 \
		-Wall \
		-Wextra \
		-Wpedantic \
		-Wno-unused-result \
		-Wno-strict-prototypes \
		-Wno-gnu-statement-expression-from-macro-expansion \
		-fsanitize=undefined,unsigned-integer-overflow \
		-ggdb \
		--std=gnu99 \
		$< -o $@ -lm
