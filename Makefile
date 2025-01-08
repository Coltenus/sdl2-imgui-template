CXX = clang++

CFLAGS = -Wall -Wextra -Werror -std=c++23 -g -pedantic -I./libs/imgui -I./libs/implot
TARGET_CFLAGS = -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lopengl32 -lglew32 -lfmt
ifneq ($(OS),Windows_NT)
	CFLAGS += -D_POSIX_C_SOURCE=200809L
	TARGET_CFLAGS += -lpthread
endif

SRC_DIR = src
OBJ_DIR = bin
BIN_DIR = bin
IMGUI_DIR = libs/imgui
IMGUI_BACKEND_DIR = libs/imgui/backends
IMPLOT_DIR = libs/implot
SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))
SRC_LIBS = $(wildcard $(IMGUI_DIR)/*.cpp)
SRC_LIBS += $(IMGUI_BACKEND_DIR)/imgui_impl_sdl2.cpp $(IMGUI_BACKEND_DIR)/imgui_impl_opengl3.cpp
SRC_LIBS += $(wildcard $(IMPLOT_DIR)/*.cpp)
OBJ_LIBS = $(patsubst %.cpp,%.o,$(SRC_LIBS))

TARGET = $(BIN_DIR)/app

all: build run

build: $(TARGET)

rebuild: clean build

$(TARGET): $(OBJ_LIBS) $(OBJ_FILES)
	$(CXX) -o $@ $^ $(CFLAGS) $(TARGET_CFLAGS)
	$(CXX) -o $@_win $^ $(CFLAGS) $(TARGET_CFLAGS) -mwindows
	cp -r assets bin/

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) -c -o $@ $< $(CFLAGS)

$(IMGUI_DIR)/%.o: $(IMGUI_DIR)/%.cpp
	$(CXX) -c -o $@ $< $(CFLAGS)

$(IMGUI_BACKEND_DIR)/%.o: $(IMGUI_BACKEND_DIR)/%.cpp
	$(CXX) -c -o $@ $< $(CFLAGS)

$(IMPLOT_DIR)/%.o: $(IMPLOT_DIR)/%.cpp
	$(CXX) -c -o $@ $< $(CFLAGS)

clean:
	rm -rf $(OBJ_DIR)/*.o $(TARGET) $(TARGET)_win

run:
	./${TARGET}

modules:
	git submodule update --init --recursive --depth 1
	sed -i 's/#include <SDL/#include <SDL2\/SDL/g' libs/imgui/backends/imgui_impl_sdl2.cpp

clean_modules:
	rm -rf libs/imgui libs/implot

debug: $(TARGET)
ifeq ($(OS),Windows_NT)
	lldb ./${TARGET}.exe
else
	lldb ./${TARGET}
endif

count:
	@echo "Lines of code: `wc -l src/*.cpp src/*.h | tail -n 1 | tr -s ' ' | cut -d ' ' -f 2`"

.PHONY: all clean run build debug rebuild count
