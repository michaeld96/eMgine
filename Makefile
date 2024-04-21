# Compiler
CXX := clang++  

# Compiler flags
CXXFLAGS := -std=c++17 -O3 #-Wall

# Conditional addition of -stdlib=libc++ for 'me' target
ifeq ($(MAKECMDGOALS),me)
CXXFLAGS += -stdlib=libc++ -g `sdl2-config --cflags` -DSDL_VIDEO_DRIVER_COCOA
endif

# Project directory
PROJECT_DIR := .

# Include Path
INCLUDES := -I$(PROJECT_DIR)/include \
            -I$(PROJECT_DIR)/libs \
            -I$(PROJECT_DIR)/libs/SDL2/include \
            -I$(PROJECT_DIR)/libs/LuaBridge-2.8/Source \
            -I$(PROJECT_DIR)/libs/lua-5.4.3/src \
            -I$(PROJECT_DIR)/libs/box2d-2.4.1/include \
			-I$(PROJECT_DIR)/libs/imgui \
			-I$(PROJECT_DIR)/libs/ImGuiFileDialog
			


# Libraries to link against
LDLIBS := `sdl2-config --libs` -lSDL2_image -lSDL2_ttf -lSDL2_mixer -llua5.4 

# Source directorys
SRC_DIR := $(PROJECT_DIR)/src
BOX2D_SRC_DIRS := $(wildcard $(PROJECT_DIR)/libs/box2d-2.4.1/src/*)


# ImGui Source Files
IMGUI_SRC_DIR := $(PROJECT_DIR)/libs/imgui
IMGUI_SOURCES += $(IMGUI_SRC_DIR)/imgui.cpp $(IMGUI_SRC_DIR)/imgui_demo.cpp $(IMGUI_SRC_DIR)/imgui_draw.cpp $(IMGUI_SRC_DIR)/imgui_widgets.cpp $(IMGUI_SRC_DIR)/imgui_tables.cpp
IMGUI_SOURCES += $(IMGUI_SRC_DIR)/imgui_impl_sdl2.cpp $(IMGUI_SRC_DIR)/imgui_impl_sdlrenderer2.cpp

# ImGuiFileDialog Source Files
IMGUIFILEDIALOG_SRC_DIR := $(PROJECT_DIR)/libs/ImGuiFileDialog
IMGUIFILEDIALOG_SOURCES := $(IMGUIFILEDIALOG_SRC_DIR)/ImGuiFileDialog.cpp


# Source Files
SOURCES := $(wildcard $(SRC_DIR)/*.cpp) $(foreach dir,$(BOX2D_SRC_DIRS),$(wildcard $(dir)/*.cpp)) $(IMGUI_SOURCES)
SOURCES += $(IMGUIFILEDIALOG_SOURCES)

# Object files
OBJ_DIR := $(PROJECT_DIR)/obj
OBJECTS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SOURCES))

# Target executable
TARGET := $(PROJECT_DIR)/game_engine_linux

# Default build
all: $(TARGET)

# 'me' build
me: $(TARGET)

# Linking
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) $(LDLIBS) -o $(TARGET)

# Compiling
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

run: 
	./$(TARGET)

# Cleaning up
clean:
	rm -rf $(OBJ_DIR) $(TARGET)

.PHONY: all clean run