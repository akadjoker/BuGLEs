# BuGL (BuGLEs)

> Same runtime project, now with `raylib`, more modular structure, and direction for desktop + web/mobile/Raspberry targets.

BuGL is a programmable runtime built around **BuLang**.
It is focused on code-first development for rendering, tools, gameplay systems, and engine architecture experiments.

## Recent Changes

- Rendering/input/window backend migrated to **raylib** (instead of SDL)
- Runtime structure improved for **modular composition**
- Cross-platform direction expanded to include **web/mobile/Raspberry Pi**

`Note:` executable name is `bugl` (project name can be BuGL or BuGLEs).

## Architecture (Modular by Design)

- `libbu/`: language core (lexer, parser, compiler, VM/runtime)
- `main/`: host runtime and native bindings (raylib + modules)
- `scripts/`: runnable BuLang examples/tests/tutorials
- `tests/`: CTest integration for language validation
- `vendor/`: third-party libraries (raylib, physics, navigation, etc.)

Optional modules can be enabled/disabled at configure time with CMake flags, for example:

- `BUGL_WITH_BOX2D`
- `BUGL_WITH_JOLT`
- `BUGL_WITH_ASSIMP`
- `BUGL_WITH_MESHOPT`
- `BUGL_WITH_MICROPATHER`
- `BUGL_WITH_OPENSTEER`
- `BUGL_WITH_RECAST`

## Platform Direction

- Stable day-to-day workflow: **desktop (Linux/Windows)**
- Existing platform hooks in source: **Emscripten/Web** and **Android**
- `raylib` backend helps portability for **mobile and Raspberry Pi** pipelines

This repository is being organized to keep platform-specific pieces isolated while sharing the same BuLang core.

## Quick Start

```bash
git clone git@github.com:akadjoker/BuGLEs.git
cd BuGLEs
cmake --preset linux-debug
cmake --build --preset linux-debug -j
./bin/bugl scripts/test_window.bu
```

Try a tutorial:

```bash
./bin/bugl scripts/tutorials/01_hello_triangle.bu
./bin/bugl scripts/tutorials/02_textured_quad.bu
```

## Build (Generic CMake)

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./bin/bugl scripts/test_window.bu
```

## Tests

Build and run the language test suite:

```bash
cmake --build build-debug --target bulang_test
cd build-debug
ctest --output-on-failure -L bulang
```

Or use the convenience target:

```bash
cmake --build build-debug --target bulang_run_tests
```

## Documentation

- [API.md](API.md)
- [BULANG_SYNTAX_REFERENCE.md](BULANG_SYNTAX_REFERENCE.md)
- [PLUGINS_API.md](PLUGINS_API.md)
- [CONTRIBUTING.md](CONTRIBUTING.md)

## License

MIT
