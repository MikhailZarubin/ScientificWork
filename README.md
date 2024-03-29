# ScientificWork
The program part of the scientific work on the topic of Global optimization.
There are implementation of algorithms for calculating the minimum of multidimensional functions and GUI for demonstration of their work.

## Preparation
**Necessary tools:**
+ `git` (documentation located is https://git-scm.com/doc)
+ `cmake` (documentation located is https://cmake.org/documentation/)
+ `python` (documentation located is https://docs.python.org/3/)

**Necessary libraries:**
+ `numpy` (for installation execute `pip install numpy`)
+ `matprolib` (for installation execute `pip install matplotlib`)

## Install
```
git clone https://github.com/MikhailZarubin/ScientificWork
cd ScientificWork
git submodule update --init --recursive
```

## Build
```
mkdir build && cd build
cmake ..
cmake --build . --config Release
```

## Launch of Middleware
```
cd <project's folder>/Release
global_optimization.exe [arguments]
```

## Launch of GUI
```
cd <project's folder>/GUI
python graphics_builder.py [arguments]
```

Additional information about the required and supported arguments is located in [Configuration Guide](https://github.com/MikhailZarubin/ScientificWork/tree/dev/docs/Configuration_Guide.md).