# anthraxAI

The Vulkan-Based Game Engine

<div align="center">
<img alt="Preview" src="assets/textures/engine.png" width=80% />
</div>

[TOC]

## How to build

### Dependencies

To build this project, you need to have the following dependencies installed:

- [Vulkan SDK](https://vulkan.lunarg.com/sdk/home);
- X11/XCB libraries *(linux only)*;

If you're on linux, you can install them using the scripts under the `tools/linux` directory:

#### Install Vulkan SDK

```bash
./tools/linux/install-vulkan-sdk.sh
```

#### Install X11/XCB libraries

```bash
./tools/linux/install-x11-xcb.sh
```

If you're on windows, you can install the Vulkan SDK from the link above.

### Configure

This project uses CMake to generate the build files. To configure the project, you can use
the following commands:

```bash
cmake -B build -S .
```

During the configuration process, the shaders will be compiled using the `glslc` compiler.
If you don't want to compile them, you could pass the `-DAAI_COMPILE_SHADERS=OFF` flag to the
`cmake` command above.

### Build

After configuring the project, you can build it using the following command:

```bash
cmake --build build
```

## Build using Visual Studio Code

As you could notice, this project has the Visual Studio Code workspace file. If you want to build
and debug the project using Visual Studio Code, you can open the workspace file and install
the recommended extensions.

With the CMakeTools extension installed, you could select the compiler, and then build or run the
project using the <kbd>F7</kbd> and <kbd>F5</kbd> keys, respectively.

Don't forget that the <kbd>CTRL+SHIFT+P</kbd> key combination helps to `configure` and `build` as well.
