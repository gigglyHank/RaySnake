# Raylib version
CC := g++
STANDARD := -std=c++17
COMPILER_FLAGS := -Wall
LINKER_FLAGS := -lraylib #-lopengl32 -lgdi32 -lwinmm
#---------------------------
SRC_DIR := ./src
OBJS_DIR := ./objs
BUILD_DIR := ./build
#---------------------------
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(SRC_FILES:$(SRC_DIR)/%.cpp=$(OBJS_DIR)/%.o)
#---------------------------
INCLUDE_DIR := -I.#/include
LIBRARY_DIR := -L.#/lib
#---------------------------
OUTPUT_EXE := snake
#---------------------------
.PHONY: compile_run compile debug run clean_debug clean_build clean all all_no_run 
#---------------------------
compile_run: $(BUILD_DIR)/$(OUTPUT_EXE) run

$(BUILD_DIR)/$(OUTPUT_EXE): $(OBJ_FILES)
	@$(CC) $(STANDARD) $(COMPILER_FLAGS) $^ -o $@ $(INCLUDE_DIR) $(LIBRARY_DIR) $(LINKER_FLAGS)

# Compile source files into object files
$(OBJS_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(STANDARD) $(COMPILER_FLAGS) -c $< -o $@ $(INCLUDE_DIR) $(LIBRARY_DIR) $(LINKER_FLAGS)

clean_build:
ifneq ("$(wildcard $(BUILD_DIR)/$(OUTPUT_EXE))", "")
	@rm $(BUILD_DIR)/$(OUTPUT_EXE)
	@echo "[ Make:CLEAN BUILD ] $(OUTPUT_EXE) removed."
endif

clean_debug:
ifneq ("$(wildcard $(BUILD_DIR)/debug)", "")
	@rm $(BUILD_DIR)/debug
	@echo "[ Make:CLEAN DEBUG ] debug removed."
endif

run:
	@echo -e "[ Make:RUN ] ...\n"
	@$(BUILD_DIR)/$(OUTPUT_EXE)
	@echo -e "\n[ Make:RUN ] complete."

compile: $(OBJ_FILES)
	@$(CC) $(STANDARD) $(COMPILER_FLAGS) $^ -o $(BUILD_DIR)/$(OUTPUT_EXE) $(INCLUDE_DIR) $(LIBRARY_DIR) $(LINKER_FLAGS)
	@echo "[ Make:COMPILE ] complete."

debug:
	@echo "[ Make:DEBUG ] ..."
	@$(CC) $(STANDARD) $(COMPILER_FLAGS) -g $(SRC_FILES) -o build/debug $(INCLUDE_DIR) $(LIBRARY_DIR) $(LINKER_FLAGS)
	@echo "[ Make:DEBUG ] complete."

clean: clean_debug clean_build
ifneq ("$(wildcard $(OBJS_DIR)/*.o)", "")
	@rm $(OBJS_DIR)/*.o
	@echo "[ Make:CLEAN ] Objects removed."
	@echo "[ Make:CLEAN ] complete."
endif

all: clean compile run
all_no_run: clean compile

