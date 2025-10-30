# RaySnake
Classic Snake game made using Raylib 5.5 made for Linux
Following the tutorial provided by [educ8s](https://github.com/educ8s) this project aim to build
on what was created. Features and style changes will be made in the future to make it my own.

> Developed with Raylib 5.5 and C++, this project aims to demonstrates my skills in software/game development

# Prerequisites
- Raylib 5.5
- C++ compiler (GCC, Clang, MSVC)
- Make

> [!NOTE]
> I've provided a Linux build within the `build` folder. This should get you up and running.

# Build Instructions

| Command | Description |
| ------------- | -------------- |
| make | executes the `compile_run` command |
| compile_run | builds and runs the application |
| clean_build | removes the application |
| clean_debug | removes the debug version of the application |
| run | runs the application found in the `build` folder |
| compile | only compiles the `main.cpp` file found in `src` folder |
| debug | builds a debug version of the application |
| clean | executes `clean_build` and `clean_debug` commands then checks/removes object file found in `objs` |
| all | executes `clean`, `compile` and `run` |
| all_no_run | executes `clean` and `compile` |

