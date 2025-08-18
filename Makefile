# Compiler
CXX := g++
CC := gcc
CXXFLAGS := -std=c++17 -Wall -Wextra -Iengine/ecs/include -Iengine/rendering/include -Iengine/ecs/systems/include -Iengine/ecs/components/include
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
        SFML_LIBS := third_party/SFML/install/linux/lib/libsfml-graphics-s.a third_party/SFML/install/linux/lib/libsfml-window-s.a third_party/SFML/install/linux/lib/libsfml-system-s.a -lX11 -lXrandr -lXcursor -lXi -ludev -ldl -lpthread -lfreetype
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
COMPONENTS_DIR := engine/ecs/components
LOGGING_DIR := engine/logging
RENDER_DIR := engine/rendering
GLAD_DIR := third_party/OpenGL
TEST_DIR := tests

# Create build directories
$(shell mkdir -p $(BUILD_DIR)/ecs/src $(BUILD_DIR)/ecs/systems/src $(BUILD_DIR)/ecs/systems/tests $(BUILD_DIR)/ecs/components/src $(BUILD_DIR)/ecs/components/tests $(BUILD_DIR)/logging/src $(BUILD_DIR)/logging/tests $(BUILD_DIR)/rendering/src $(BUILD_DIR)/rendering/tests $(BUILD_DIR)/tests $(BUILD_DIR)/glad)
$(foreach module,$(TEST_MODULES),$(shell mkdir -p $(BUILD_DIR)/engine/$(module)/tests))

# Source files - only include main.cpp for the main executable
SRC_FILES := $(SRC_DIR)/main.cpp
ECS_SRC := $(wildcard $(ECS_DIR)/src/*.cpp)
SYSTEMS_SRC := $(wildcard $(ECS_DIR)/systems/src/*.cpp)
COMPONENTS_SRC := $(wildcard $(COMPONENTS_DIR)/src/*.cpp)
LOGGING_SRC := $(wildcard $(LOGGING_DIR)/src/*.cpp)
RENDER_SRC := $(wildcard $(RENDER_DIR)/src/*.cpp)
GLAD_SRC := $(GLAD_DIR)/src/glad.c
TEST_SRC := $(wildcard $(TEST_DIR)/*.cpp)

# Engine modules that have tests
TEST_MODULES := ecs logging rendering physics input resources
ENGINE_TEST_SRC := $(foreach module,$(TEST_MODULES),$(wildcard engine/$(module)/tests/*.cpp))
# Filter out SFML tests since we're not linking SFML libraries in tests
ENGINE_TEST_SRC := $(filter-out engine/rendering/tests/SFML%.cpp, $(ENGINE_TEST_SRC))
# Special handling for components (nested under ecs)
COMPONENTS_TEST_SRC := $(wildcard $(COMPONENTS_DIR)/tests/*.cpp)
SYSTEMS_TEST_SRC := $(wildcard $(ECS_DIR)/systems/tests/*.cpp)

# Test-only rendering sources (exclude SFML implementations)
TEST_RENDER_SRC := $(filter-out $(RENDER_DIR)/src/SFML%.cpp, $(RENDER_SRC))
TEST_RENDER_OBJ := $(patsubst $(RENDER_DIR)/%.cpp,$(BUILD_DIR)/rendering/%.o,$(TEST_RENDER_SRC))

# Object files
OBJ := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRC_FILES))
ECS_OBJ := $(patsubst $(ECS_DIR)/%.cpp,$(BUILD_DIR)/ecs/%.o,$(ECS_SRC))
SYSTEMS_OBJ := $(patsubst $(ECS_DIR)/systems/%.cpp,$(BUILD_DIR)/ecs/systems/%.o,$(SYSTEMS_SRC))
COMPONENTS_OBJ := $(patsubst $(COMPONENTS_DIR)/%.cpp,$(BUILD_DIR)/ecs/components/%.o,$(COMPONENTS_SRC))
LOGGING_OBJ := $(patsubst $(LOGGING_DIR)/%.cpp,$(BUILD_DIR)/logging/%.o,$(LOGGING_SRC))
RENDER_OBJ := $(patsubst $(RENDER_DIR)/%.cpp,$(BUILD_DIR)/rendering/%.o,$(RENDER_SRC))
GLAD_OBJ := $(BUILD_DIR)/glad/glad.o
TEST_OBJ := $(patsubst %.cpp,$(BUILD_DIR)/tests/%.o,$(TEST_SRC))
ENGINE_TEST_OBJ := $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(ENGINE_TEST_SRC))
COMPONENTS_TEST_OBJ := $(patsubst $(COMPONENTS_DIR)/%.cpp,$(BUILD_DIR)/ecs/components/%.o,$(COMPONENTS_TEST_SRC))
SYSTEMS_TEST_OBJ := $(patsubst $(ECS_DIR)/systems/%.cpp,$(BUILD_DIR)/ecs/systems/%.o,$(SYSTEMS_TEST_SRC))

# Targets
EXEC := $(BUILD_DIR)/game
SAFE_EXEC := $(BUILD_DIR)/game_safe
HEADLESS_EXEC := $(BUILD_DIR)/game_headless
MINIMAL_EXEC := $(BUILD_DIR)/minimal_sfml_test
SIMPLE_EXEC := $(BUILD_DIR)/simple_engine_test
TEST_EXEC := $(BUILD_DIR)/ecs_tests

# Default target
all: $(EXEC)

# Game executable
$(EXEC): $(OBJ) $(ECS_OBJ) $(SYSTEMS_OBJ) $(COMPONENTS_OBJ) $(LOGGING_OBJ) $(RENDER_OBJ) $(GLAD_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(SFML_LIBS) $(OPENGL_LIB)

# Safe game executable (better error handling)
$(SAFE_EXEC): $(BUILD_DIR)/main_safe.o $(ECS_OBJ) $(SYSTEMS_OBJ) $(COMPONENTS_OBJ) $(LOGGING_OBJ) $(RENDER_OBJ) $(GLAD_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(SFML_LIBS) $(OPENGL_LIB)

# Headless game executable (no SFML dependencies)
$(HEADLESS_EXEC): $(BUILD_DIR)/main_headless.o $(ECS_OBJ) $(SYSTEMS_OBJ) $(COMPONENTS_OBJ) $(LOGGING_OBJ) $(TEST_RENDER_OBJ) $(GLAD_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(OPENGL_LIB)

# Minimal SFML test (just SFML + simple window)
$(MINIMAL_EXEC): $(BUILD_DIR)/minimal_sfml_test.o
	$(CXX) $(CXXFLAGS) $^ -o $@ $(SFML_LIBS)

# Simple engine test (SFML + entity-like rendering)
$(SIMPLE_EXEC): $(BUILD_DIR)/simple_engine_test.o
	$(CXX) $(CXXFLAGS) $^ -o $@ $(SFML_LIBS)

# ECS tests executable
$(TEST_EXEC): $(ENGINE_TEST_OBJ) $(COMPONENTS_TEST_OBJ) $(SYSTEMS_TEST_OBJ) $(ECS_OBJ) $(SYSTEMS_OBJ) $(COMPONENTS_OBJ) $(LOGGING_OBJ) $(TEST_RENDER_OBJ) $(TEST_OBJ) $(GLAD_OBJ)
	$(CXX) $(TEST_CXXFLAGS) $^ -o $@ $(GTEST_LIBS) $(OPENGL_LIB)

# Compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/main_safe.o: $(SRC_DIR)/main_safe.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/main_headless.o: $(SRC_DIR)/main_headless.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/minimal_sfml_test.o: $(SRC_DIR)/minimal_sfml_test.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/simple_engine_test.o: $(SRC_DIR)/simple_engine_test.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/ecs/src/%.o: $(ECS_DIR)/src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/ecs/systems/src/%.o: $(ECS_DIR)/systems/src/%.cpp
	$(CXX) $(CXXFLAGS) -I$(ECS_DIR)/systems/include -c $< -o $@

$(BUILD_DIR)/ecs/systems/tests/%.o: $(ECS_DIR)/systems/tests/%.cpp
	$(CXX) $(TEST_CXXFLAGS) -I$(ECS_DIR)/systems/include -c $< -o $@

$(BUILD_DIR)/ecs/components/src/%.o: $(COMPONENTS_DIR)/src/%.cpp
	$(CXX) $(CXXFLAGS) -I$(COMPONENTS_DIR)/include -c $< -o $@

$(BUILD_DIR)/ecs/components/tests/%.o: $(COMPONENTS_DIR)/tests/%.cpp
	$(CXX) $(TEST_CXXFLAGS) -I$(COMPONENTS_DIR)/include -c $< -o $@

$(BUILD_DIR)/logging/src/%.o: $(LOGGING_DIR)/src/%.cpp
	$(CXX) $(CXXFLAGS) -I$(LOGGING_DIR)/include -c $< -o $@

$(BUILD_DIR)/logging/tests/%.o: $(LOGGING_DIR)/tests/%.cpp
	$(CXX) $(TEST_CXXFLAGS) -I$(LOGGING_DIR)/include -c $< -o $@

$(BUILD_DIR)/rendering/src/%.o: $(RENDER_DIR)/src/%.cpp
	$(CXX) $(CXXFLAGS) -I$(RENDER_DIR)/include -c $< -o $@

$(BUILD_DIR)/rendering/tests/%.o: $(RENDER_DIR)/tests/%.cpp
	$(CXX) $(TEST_CXXFLAGS) -I$(RENDER_DIR)/include -c $< -o $@

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
.PHONY: clean run test safe headless minimal simple

run: $(EXEC)
	./$(EXEC)

safe: $(SAFE_EXEC)
	./$(SAFE_EXEC)

headless: $(HEADLESS_EXEC)
	./$(HEADLESS_EXEC)

minimal: $(MINIMAL_EXEC)
	./$(MINIMAL_EXEC)

simple: $(SIMPLE_EXEC)
	./$(SIMPLE_EXEC)

test: $(TEST_EXEC)
	./$(TEST_EXEC)

clean:
	rm -rf $(BUILD_DIR)/*
