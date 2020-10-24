# Vulkan - Build Test

Attempt to cross-compile Vulkan project on Linux for Windows with Github Actions compile check.

This will allow other projects to compile Windows EXE from Linux without the need for library to be present on the system as all libraries will be.
And also to be build by Github Actions which will allow automatization.

## Compilation

Environment variable `VULKAN_SKD` must be defined on Windows platform and is recommended on Unix.

The SDK can be downloaded from [vulkan.lunarg.com](https://vulkan.lunarg.com/sdk/home), `Latest Runtime/Zip` option under your operating system category.
Currently tested versions are `1.2.154.0` for Ubuntu (Linux) and `1.2.154.1` for Windows.

## Platforms

| Platform | Architecture | Implemenetd | Tested |
|----------|:------------:|:-----------:|:------:|
| Android  | ?            | ✓           |        |
| Windows  | x86          | ✓           | ✓      |
| Windows  | x86_64       | ✓           | ✓      |
| Unix     | x86          | ?           |        |
| Unix     | x86_64       | ✓           | ✓      |
