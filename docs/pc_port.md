# Native PC port bootstrap

This directory starts a **separate, clean native-runtime effort**. It does not
change the GameCube matching build, does not contain game data, and must not
be presented as a finished port.

## Current state

`pc/` builds `smstrikers_pc_bootstrap`, a small host-native command-line
program. It deliberately has only two jobs:

1. establish a CMake/C++20 build that works on desktop platforms; and
2. accept a user-owned game extraction, calculate the SHA-1 of its
   `sys/main.dol`, and identify one of the three supported retail releases.

It **does not** load game assets, execute PowerPC code, emulate GameCube
hardware, or start gameplay. No proprietary game files are copied into the
repository or build output.

Build and run it independently of the GameCube matching build:

```sh
cmake -S pc -B build/pc -DCMAKE_BUILD_TYPE=Release
cmake --build build/pc
ctest --test-dir build/pc --output-on-failure
./build/pc/smstrikers_pc_bootstrap --help
```

To validate a lawful extraction without copying any data into the build, pass
the root of an extracted disc (the directory that contains `sys/main.dol`):

```sh
./build/pc/smstrikers_pc_bootstrap --game-dir /path/to/extracted-game
```

The bootstrap accepts only the three `main.dol` SHA-1 values already used by
the matching configuration: `G4QE01` (North America), `G4QP01` (Europe), and
`G4QJ01` (Japan). This prevents a future runtime from silently operating on an
unknown revision. It checks only the executable today; content-file validation
will arrive with the content mount.

The first content-mount API is now available in `smstrikers_pc_platform`. It
canonically mounts the supplied extraction root, accepts only non-empty
relative paths, and refuses traversal outside that root (including through a
symlink). The CLI exposes this boundary with an optional `--asset` query:

```sh
./build/pc/smstrikers_pc_bootstrap --game-dir /path/to/extracted-game --asset files/example.bin
```

The query only resolves the file and reports its byte count. It neither copies
nor exports game data; the reusable mount can later provide the file reads
needed by a host replacement for `nlFileGC`.

`smstrikers_pc_platform` also now has `AsyncFileReader`, which returns a
`std::future<FileReadResult>` for a mounted path. It is intentionally limited
to read-only byte loading and clear error reporting, but establishes the
completion-based shape needed to replace the GameCube DVD asynchronous-read
calls used by the game's file layer. Future task-manager integration should
adapt this interface rather than block the desktop frame loop.

`FrameScheduler` provides the other half of that future loop: a deterministic
fixed-step accumulator (60 Hz by default), interpolation alpha, and a capped
catch-up limit that prevents a long desktop stall from causing an unbounded
number of simulation updates. It does not invoke game tasks yet; when the
platform boundary reaches the task manager, each returned step will drive one
fixed update and the native renderer can use the interpolation alpha.

`InputMapper` is the first host-input boundary. It translates keyboard events
into one normalized controller state: WASD drives the left stick, arrow keys
drive the right stick, Space/J/K/L map to A/B/X/Y, Enter maps to Start, and
Q/E map to L/R plus their analogue triggers. A future window/controller
backend will feed it events and add physical gamepad support without exposing
host-library types to gameplay code.

## Why this cannot be a compiler-only conversion

The reconstructed program is built for the GameCube's PowerPC environment.
The existing startup code initializes GameCube OS services and then enters the
game's endless task loop. Its rendering code calls GX, its content layer calls
DVD, its input layer calls PAD/SI, and its audio layer calls AX/AI/DSP.
Replacing those components is runtime engineering, not a change of compiler
target. In particular, desktop builds cannot include the PowerPC assembly,
memory-mapped hardware accesses, or fixed-width pointer assumptions unchanged.

## Implementation plan

The following sequence preserves the matching build while making each porting
step testable on PC:

1. **Platform boundary:** define host interfaces for clock, logging, files,
   threading, input, audio, and graphics. Keep the original Dolphin SDK calls
   behind a GameCube implementation; do not scatter `#ifdef`s through gameplay
   code.
2. **Content mount:** document and implement a read-only mount for files
   extracted from a user-owned disc. Include supported-version checks using the
   already published `main.dol` hashes. Never commit extracted data.
3. **Host services:** replace DVD/card/OS/PAD dependencies with standard C++
   filesystem, save-data, timing, and controller services. Add deterministic
   tests for the interfaces.
4. **Rendering:** translate the `NL/gl` layer and GX command/state model to a
   portable graphics backend. A command recorder plus frame-image regression
   tests should precede shader or renderer modernization.
5. **Audio:** provide an AX/AI/DSP-compatible mixing boundary backed by a PC
   audio API, then validate timing, stream looping, and channel behavior.
6. **Integration:** replace the GameCube entry sequence with a desktop event
   loop, connect the existing task manager, and validate menus, matches, save
   data, input, audio, and rendering with a lawfully supplied extraction.

Each phase needs an independent review for legal distribution boundaries and
for regressions in the original matching build.

## Non-goals

- Distributing a ROM, disc image, extracted assets, or a retail-equivalent PC
  game executable.
- Claiming compatibility before the native implementations above exist.
- Replacing or weakening the repository's GameCube decompilation workflow.
