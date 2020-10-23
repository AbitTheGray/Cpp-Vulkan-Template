# VulkanHpp - Test

Attempt to cross-compile Vulkan on Linux for Windows.

This will allow other projects to compile Windows EXE from Linux without the need for library to be present on the system as all libraries will be.

## Compilation

`VULKAN_DLL` must be defined using `-DVULKAN_DLL=vulkan-1.dll` as CMake option.

The DLL can be downloaded from [vulkan.lunarg.com](https://vulkan.lunarg.com/sdk/home), `Latest Runtime/Zip` option under Windows (then there are `x64` and `x86` libraries for 64-bit and 32-bit systems).
