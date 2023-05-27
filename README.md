# ScientificWork
The program part of the scientific work on the topic of Global optimization.
There are implementation of algorithms for calculating the minimum of multidimensional functions and GUI for demonstration of their work.

## Install
```
git clone https://github.com/MikhailZarubin/ScientificWork
cd ScientificWork
git submodule update --recursive
```

## Build
```
cd build
cmake ..
cmake --build . --config Release
```

## Launch of Middleware
```
cd build/Release
global_optimization.exe [arguments]
```

## Launch of GUI
```
cd build
graphics_builder.py [arguments]
```

Additional information about the required and supported arguments is located in [Configuration Guide](https://github.com/MikhailZarubin/ScientificWork/tree/dev/docs/Configuration_Guide.md).