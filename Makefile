# Compiler
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Iengine/ecs/include -Iengine/rendering/include
LDFLAGS :=

# Detect OS
ifeq ($(OS),Windows_NT)
    SFML_LIBS := -Lthird_party/SFML/windows/lib -lsfml-graphics -lsfml-window -lsfml-system
    SFML_INCLUDES := -Ithird_party/SFML/windows/include
    GTEST_LIBS := -Lthird_party/googletest/windows/lib -lgtest -lgtest_main -lpthread
    OPENGL_LIB := -lopengl32
    BUILD_DIR := build/windows
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
    	SFML_LIBS := -Lthird_party/SFML/install/linux/lib -lsfml-graphics -lsfml-window -lsfml-system
		SFML_INCLUDES := -Ithird_party/SFML/install/linux/include
        GTEST_LIBS := -Lthird_party/googletest/linux/lib -lgtest -lgtest_main -lpthread
        OPENGL_LIB := -lGL
        BUILD_DIR := build/linux
    endif
    ifeq ($(UNAME_S),Darwin)
        SFML_LIBS := -Lthird_party/SFML/macos/lib -lsfml-graphics -lsfml-window -lsfml-system
        SFML_INCLUDES := -Ithird_party/SFML/macos/include
        GTEST_LIBS := -Lthird_party/googletest/macos/lib -lgtest -lgtest_main -lpthread
        OPENGL_LIB := -framework OpenGL
        BUILD_DIR := build/macos
    endif
endif

CXXFLAGS += $(SFML_INCLUDES)

# Directories
SRC_DIR := src
ECS_DIR := engine/ecs
RENDER_DIR := engine/rendering
TEST_DIR := tests

# Create build directories
$(shell mkdir -p $(BUILD_DIR)/ecs $(BUILD_DIR)/rendering $(BUILD_DIR)/tests)

# Source files
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
ECS_SRC := $(wildcard $(ECS_DIR)/src/*.cpp)
RENDER_SRC := $(wildcard $(RENDER_DIR)/src/*.cpp)
TEST_SRC := $(wildcard $(TEST_DIR)/*.cpp)

# Object files
OBJ := $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(SRC_FILES))
ECS_OBJ := $(patsubst %.cpp,$(BUILD_DIR)/ecs/%.o,$(ECS_SRC))
RENDER_OBJ := $(patsubst %.cpp,$(BUILD_DIR)/rendering/%.o,$(RENDER_SRC))
TEST_OBJ := $(patsubst %.cpp,$(BUILD_DIR)/tests/%.o,$(TEST_SRC))

# Targets
EXEC := $(BUILD_DIR)/game
TEST_EXEC := $(BUILD_DIR)/ecs_tests

# Default target
all: $(EXEC)

# Game executable
$(EXEC): $(OBJ) $(ECS_OBJ) $(RENDER_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(SFML_LIBS) $(OPENGL_LIB)

# ECS tests executable
$(TEST_EXEC): $(ECS_OBJ) $(TEST_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(GTEST_LIBS) $(OPENGL_LIB)

# Compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/ecs/%.o: $(ECS_DIR)/src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/rendering/%.o: $(RENDER_DIR)/src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/tests/%.o: $(TEST_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Phony targets
.PHONY: clean run test

run: $(EXEC)
	./$(EXEC)

test: $(TEST_EXEC)
	./$(TEST_EXEC)

clean:
	rm -rf $(BUILD_DIR)/*
