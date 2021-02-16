# Vulkan Template

This is template project to allow fast and simple C++ project creation.

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

## Libraries

| Name | License | Version |
|------|---------|---------|
| ... | `MIT` / `GPLv3` | branch: `master` |

All libraries are used as `static library` to maximize optimization and limit problems with deployment and versions.

## Testing

- Set `BUILD_TESTING` CMake variable to `ON` (`-DBUILD_TESTING=ON`).
- Run `ctest` in build directory or use CLion and select `All CTest` as a run target.

Executable of tests have prefix `t_` (test `abc` would be executable `t_abc`).
After creating new test, add it to [`tests/CMakeLists.txt`](tests/CMakeLists.txt) as `add_subdirectory(abc)` (where `abc` is name of your test).
