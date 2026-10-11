# Chess bot

A C++20 chess engine prototype using named modules. The current executable
creates the starting board at compile time and prints its piece values as an
8 × 8 hexadecimal grid. Move generation, search, and a playable interface are
not implemented yet.

**AI NOTE**: This README was mostly written with Codex. However, every line of code is typed with a keyboard. AI has been used for research into C++ shenanigans like modules

## Project layout

- `main.cpp`: piece encoding, starting-board initialization, and board output.
- `Square.cppm`: exports `Square`; contains directional travel lists, knight
  destinations, span views, and work-in-progress attack/defense scans.
- `CMakeLists.txt`: builds `main` and registers `Square.cppm` in a
  `CXX_MODULES` file set.
- `CMakePresets.json`: configures a Clang debug build with Ninja.
- `.clangd`: points clangd to `build/clang-debug/compile_commands.json`.

The program imports `Square` but does not instantiate it or run its scans.
A successful build therefore does not validate that analysis behavior.

## Board representation

The board is a flat `std::array<int8_t, 64>` with eight consecutive entries per
row. Empty squares are zero. Pieces use these bit masks:

| Value | Meaning |
| --- | --- |
| `0x01` | Pawn |
| `0x02` | Rook |
| `0x04` | Knight |
| `0x08` | Bishop |
| `0x10` | Queen |
| `0x20` | King |
| `0x40` | White flag |
| `0x80` | Black flag |

The current initializer sets the black flag on indices 0–15. It leaves the
pieces at indices 48–63 without the white flag; color encoding is still being
developed. `make_pos_board()` returns a board computed at compile time, while
the board in `main()` remains mutable.

## Build and run

Requirements for the supplied preset:

- CMake 3.28 or newer.
- Clang 18 and its dependency scanner (`clang-scan-deps-18`).
- Ninja with C++ module dependency support (1.11 or newer).
- C++20 standard library headers.

The preset uses `/usr/bin/clang++`, `/usr/bin/clang-scan-deps-18`, and
`/usr/bin/ninja`. Ensure `clang++` selects Clang 18, or adjust these paths to
match your installed toolchain.

```sh
cmake --preset clang-debug
cmake --build --preset clang-debug
./build/clang-debug/main
```

Build output and the compilation database are generated in
`build/clang-debug`. There is currently no automated test suite.

## Editing with clangd

Configure the project before editing so clangd can read the compilation
database, including `-std=c++20` and the module interface's compile command.
Register additional module interfaces in the `CXX_MODULES` file set, then
configure and build again.

For named-module discovery, use a recent clangd with
`--experimental-modules-support`. Module tooling remains experimental.
Standard headers belong in the global module fragment:

```cpp
module;

#include <vector>

export module Square;
```

If clangd reports missing builtin headers such as `stddef.h`, check the active
clangd installation and its resource directory first. Missing headers can
cause misleading errors inside standard library templates. After replacing
or reinstalling clangd, restart the editor so it launches the installed server.

References: [clangd modules](https://clangd.llvm.org/features#experimental-c20-modules-support),
[clangd troubleshooting](https://clangd.llvm.org/troubleshooting#cant-find-compiler-built-in-headers-stddefh-etc),
[CMake modules](https://cmake.org/cmake/help/latest/manual/cmake-cxxmodules.7.html),
[Clang standard modules](https://clang.llvm.org/docs/StandardCPlusPlusModules.html).
