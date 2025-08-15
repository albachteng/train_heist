# Compiler
CXX := g++
CC := gcc
CXXFLAGS := -std=c++17 -Wall -Wextra -Iengine/ecs/include -Iengine/rendering/include
LDFLAGS :=

# Detect OS
ifeq ($(OS),Windows_NT)
    SFML_LIBS := -Lthird_party/SFML/windows/lib -lsfml-graphics -lsfml-window -lsfml-system
    SFML_INCLUDES := -Ithird_party/SFML/windows/include
    GTEST_LIBS := -Lthird_party/googletest/windows/lib -lgtest -lgtest_main -lpthread
    GTEST_INCLUDES := -Ithird_party/googletest/windows/include
    OPENGL_LIB := -lopengl32
    BUILD_DIR := build/windows
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        SFML_LIBS := -Lthird_party/SFML/install/linux/lib -lsfml-graphics -lsfml-window -lsfml-system
        SFML_INCLUDES := -Ithird_party/SFML/install/linux/include
        GTEST_LIBS := third_party/googletest/linux/lib/libgtest.a third_party/googletest/linux/lib/libgtest_main.a -lpthread
        GTEST_INCLUDES := -Ithird_party/googletest/linux/include
        OPENGL_LIB := -lGL
        BUILD_DIR := build/linux
    endif
    ifeq ($(UNAME_S),Darwin)
        SFML_LIBS := -Lthird_party/SFML/macos/lib -lsfml-graphics -lsfml-window -lsfml-system
        SFML_INCLUDES := -Ithird_party/SFML/macos/include
        GTEST_LIBS := -Lthird_party/googletest/macos/lib -lgtest -lgtest_main -lpthread
        GTEST_INCLUDES := -Ithird_party/googletest/macos/include
        OPENGL_LIB := -framework OpenGL
        BUILD_DIR := build/macos
    endif
endif

# Add SFML & GLAD includes
CXXFLAGS += $(SFML_INCLUDES) -I$(GLAD_DIR)/include
# Add GoogleTest includes only for test builds
TEST_CXXFLAGS := $(CXXFLAGS) $(GTEST_INCLUDES)

# Directories
SRC_DIR := src
ECS_DIR := engine/ecs
RENDER_DIR := engine/rendering
GLAD_DIR := third_party/OpenGL
TEST_DIR := tests

# Create build directories
$(shell mkdir -p $(BUILD_DIR)/ecs/src $(BUILD_DIR)/ecs/systems/src $(BUILD_DIR)/ecs/systems/tests $(BUILD_DIR)/rendering $(BUILD_DIR)/tests $(BUILD_DIR)/glad)
$(foreach module,$(TEST_MODULES),$(shell mkdir -p $(BUILD_DIR)/engine/$(module)/tests))

# Source files
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
ECS_SRC := $(wildcard $(ECS_DIR)/src/*.cpp)
SYSTEMS_SRC := $(wildcard $(ECS_DIR)/systems/src/*.cpp)
RENDER_SRC := $(wildcard $(RENDER_DIR)/src/*.cpp)
GLAD_SRC := $(GLAD_DIR)/src/glad.c
TEST_SRC := $(wildcard $(TEST_DIR)/*.cpp)

# Engine modules that have tests
TEST_MODULES := ecs rendering physics input resources
ENGINE_TEST_SRC := $(foreach module,$(TEST_MODULES),$(wildcard engine/$(module)/tests/*.cpp))
SYSTEMS_TEST_SRC := $(wildcard $(ECS_DIR)/systems/tests/*.cpp)

# Object files
OBJ := $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(SRC_FILES))
ECS_OBJ := $(patsubst $(ECS_DIR)/%.cpp,$(BUILD_DIR)/ecs/%.o,$(ECS_SRC))
SYSTEMS_OBJ := $(patsubst $(ECS_DIR)/systems/%.cpp,$(BUILD_DIR)/ecs/systems/%.o,$(SYSTEMS_SRC))
RENDER_OBJ := $(patsubst %.cpp,$(BUILD_DIR)/rendering/%.o,$(RENDER_SRC))
GLAD_OBJ := $(BUILD_DIR)/glad/glad.o
TEST_OBJ := $(patsubst %.cpp,$(BUILD_DIR)/tests/%.o,$(TEST_SRC))
ENGINE_TEST_OBJ := $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(ENGINE_TEST_SRC))
SYSTEMS_TEST_OBJ := $(patsubst $(ECS_DIR)/systems/%.cpp,$(BUILD_DIR)/ecs/systems/%.o,$(SYSTEMS_TEST_SRC))

# Targets
EXEC := $(BUILD_DIR)/game
TEST_EXEC := $(BUILD_DIR)/ecs_tests

# Default target
all: $(EXEC)

# Game executable
$(EXEC): $(OBJ) $(ECS_OBJ) $(SYSTEMS_OBJ) $(RENDER_OBJ) $(GLAD_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(SFML_LIBS) $(OPENGL_LIB)

# ECS tests executable
$(TEST_EXEC): $(ENGINE_TEST_OBJ) $(SYSTEMS_TEST_OBJ) $(ECS_OBJ) $(SYSTEMS_OBJ) $(TEST_OBJ) $(GLAD_OBJ)
	$(CXX) $(TEST_CXXFLAGS) $^ -o $@ $(GTEST_LIBS) $(OPENGL_LIB)

# Compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/ecs/src/%.o: $(ECS_DIR)/src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/ecs/systems/src/%.o: $(ECS_DIR)/systems/src/%.cpp
	$(CXX) $(CXXFLAGS) -I$(ECS_DIR)/systems/include -c $< -o $@

$(BUILD_DIR)/ecs/systems/tests/%.o: $(ECS_DIR)/systems/tests/%.cpp
	$(CXX) $(TEST_CXXFLAGS) -I$(ECS_DIR)/systems/include -c $< -o $@

$(BUILD_DIR)/rendering/%.o: $(RENDER_DIR)/src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/tests/%.o: $(TEST_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Explicit rules for engine module test objects
$(ENGINE_TEST_OBJ): $(BUILD_DIR)/%.o : %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(TEST_CXXFLAGS) -c $< -o $@

# Compile GLAD
$(GLAD_OBJ): $(GLAD_SRC)
	$(CC) -I$(GLAD_DIR)/include -c $< -o $@

# Phony targets
.PHONY: clean run test

run: $(EXEC)
	./$(EXEC)

test: $(TEST_EXEC)
	./$(TEST_EXEC)

clean:
	rm -rf $(BUILD_DIR)/*
