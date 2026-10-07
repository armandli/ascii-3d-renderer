# ascii-3d-renderer
3D rendering engine using ASCII characters, written in C++.

A C++ port following the footsteps of [kciter/ascii-3d-renderer.js](https://github.com/kciter/ascii-3d-renderer.js)
and the accompanying blog post [on building an ASCII 3D renderer](https://kciter.so/posts/ascii-3d-renderer/en/).

## Prerequisites
- CMake >= 3.28
- A C++26 capable compiler (recent clang or GCC)
- Ninja (optional; falls back to Unix Makefiles)

## Building
The top-level `Makefile` is a thin wrapper around CMake.

```sh
make            # configure + build (Debug) into build/
make test       # build and run tests with ctest
make run        # build and run the demo
make release    # Release build into build-release/
make clean      # clean build outputs
make distclean  # remove all build directories
```

Override variables as needed, e.g. `make BUILD_TYPE=RelWithDebInfo GENERATOR="Unix Makefiles"`.

## Layout
- `src/` – renderer library (`math/`, `scene/`, `render/`) and the demo `main.cpp`
- `test/` – GoogleTest unit tests
