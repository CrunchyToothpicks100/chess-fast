# Chess Fast

A C-style chess engine that computes legal moves, checks, checkmate, pins,
and more. Work in progress. Instead of creating a 2D matrix of "squares",
this takes a different approach. Each `Piece` has a type (enum), color (bool),
position (8-bits), and a 64-bit mask.

Each 64-bit mask represents an 8x8 chess board, and each one is pre-computed
via `constexpr`. These masks tell you where each `Piece` can travel (by
default), depending on the type and position. This can be used with other
bit masks to determine legal moves.

These masks are written in hexadecimal and stored in `allMasks.cppm`.

*Note: `Piece` currently uses a maskPtr, which gets moved along with the
position via `movePos()`. This will likely change, since scanning through
pieces means dereferencing a pointer for each mask, which outweighs the
benefits of 2 CPU cycle piece movement.*

Theoretically, this can be used to quickly scan through possible moves, attacks,
and tactics using bit-wise logic rather than iteratively scanning the board
in each possible direction.

`DataPiece` inherits from `Piece` and can be used to print masks and other
data for debugging.

The current build uses C++20.

## Future

Once legal move logic and game state is done, my next goal would be to create
a separate UI (or CLI?) that also stores the game state, and then try to
decouple this from the chess engine. This way, a user sends a move request
to the chess engine, and then the chess engine sends a response, which updates
UI separately.

At this point, the chess engine can be used as a relay for a 2-player game.
This isn't the purpose of the chess engine, but it will probably be good
for debugging interactively.

Then, we can duplicate the game state for all possible legal moves, and
then create asynchronous queues for processing tactics, strategy, space,
tension, king safety, piece activity, etc. If a move is simply "bad" from
the get-go, the process can be freed and another logical core can double
it's horsepower. That's the thought, anyways.

The chess engine should also be aware of the clock (and store the clock)
so that the time spent on each move depends on the complexity of the position
and the remaining time.

After that, we can test each updated chess engine by having it battle the
previous chess engine.

After that, try to implement learning algorithms so that the chess engine
improves itself. I think. We'll see.

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
./build/main
```

Build output and the compilation database are generated in
`build/`. There is currently no automated test suite.
