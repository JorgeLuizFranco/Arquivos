CXX=gcc
CXXFLAGS=-Wall -Wextra
EXEC=programa

SRC_FOLDER=src
BUILD_FOLDER=build

SRC_FILES=$(wildcard $(SRC_FOLDER)/*.c) $(wildcard $(SRC_FOLDER)/**/*.c) $(wildcard $(SRC_FOLDER)/**/**/*.c)

OBJ_FOLDER=$(BUILD_FOLDER)/obj
OBJ_FILES=$(patsubst $(SRC_FOLDER)/%.c, $(OBJ_FOLDER)/%.o, $(SRC_FILES))

CXXFLAGS+= -I$(SRC_FOLDER)

all : $(OBJ_FILES)
	$(CXX) $(CXXFLAGS) $(OBJ_FILES) -o $(EXEC)

$(OBJ_FOLDER)/%.o : $(SRC_FOLDER)/%.c
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_FOLDER)
	clear

run:
	./programa

zip:
	zip -r trabalho2_arquivos.zip Makefile README.md src/

restore:
	rm -rf ./binarios/*
	cp -r ./binarios_originais/* ./binarios
	rm -rf ./indices/*
	cp -r ./indices_originais/* ./indices
