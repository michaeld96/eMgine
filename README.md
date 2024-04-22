# eMgine Game Engine

Welcome to eMgine, a versatile game engine developed as an extension of EECS 498-007 (Game Engine Architecture) at the University of Michigan. eMgine is equipped with an intuitive editor that allows for the creation and editing of scene files and Lua scripts within the application.

Built with a focus on ease of use and flexibility, eMgine incorporates popular libraries like `glm` for mathematics, `rapidJSON` for JSON handling, and a complete source compilation for `Dear ImGui`, `ImGuiFileDialog`, and `box2d`. The engine leverages SDL2 for cross-platform window management, input, and graphics, providing a comprehensive foundation for game development.

## System Requirements

To build and run eMgine, you'll need:

- SDL2 library installed on your system
- A C++17 compatible compiler

## Installation

eMgine is compatible with multiple IDEs and can be built using Visual Studio, Xcode, or Makefiles.

### Visual Studio

Open the provided Visual Studio Solution file and build the project using the IDE.

### Xcode

Open the Xcode project file and press the build button.

### Make

For those who prefer the command line or are working on Linux, you can build the project using the `make` command.

To compile with debugging symbols (useful for development), run:

```make
make me
```

To compile with optimizations for production, run:

```make 
make
```

## Running eMgine 
Once built, you can run eMgine directly from your IDE, or from the command line if you're using Make.

# Features 
eMgine comes with a range of features out of the box:

* Scene graph management
* Lua scripting for logic and automation
* Physics simulation powered by box2d
* Advanced rendering with Dear ImGui integration
* Asset management and an in-built file dialog

## Getting Started 
For community support, feature requests, or to report bugs, please use the GitHub issues page associated with this repository.

Happy Game Developing!

