CC = g++
CFLAGS := -std=c++17 -Wall -Wextra -Wpedantic -Wstrict-aliasing -g -O2
SOURCE = src src/**
INCLUDE = -Ilib -Iinclude
LIB = bin/mfw.dll -static-libgcc -static-libstdc++ -Wl,-Bstatic -lstdc++ -lpthread -Wl,-Bdynamic

SRC = $(wildcard $(patsubst %, %/*.cpp, $(SOURCE)))
OBJ = $(SRC:%.cpp=%.o)
DEP = $(OBJ:%.o=%.d)
BIN = bin

PROGRAM = demo
OUT = $(BIN)/$(PROGRAM)

ifeq ($(OS), Windows_NT)
	OUT = $(BIN)/$(PROGRAM).exe
endif

.PHONY: all clean

all: bin app

run:
	@./$(OUT)

debug:
	@gdb -q $(OUT) --eval-command=run --eval-command=exit

clean:
	@rm -rf $(BIN) $(OBJ) $(DEP) $(OUT)
	@echo Cleaned

build: bin
	@cp lib/mfw.dll bin/mfw.dll

bin:
	@mkdir -p $(BIN)

app: build $(OBJ) 
	$(CC) -o $(OUT) $(OBJ) $(LIB)
	@echo "[100%] Compilation completed"

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $< -MMD

-include $(DEP)
