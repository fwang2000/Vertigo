# Vertigo
Isometric Puzzle Game built with C++ and OpenGL. Trailer here: https://www.youtube.com/watch?v=ptA22Ele9fc&ab_channel=RachelKoh.

## Build & Run

The game is a CMake project. Source lives in `Vertigo/`; run all commands from there:

```bash
cd Vertigo
```

Controls: **W A S D** to move.

### macOS

```bash
# 1. Install dependencies (Homebrew)
brew install cmake pkg-config glfw sdl2 sdl2_mixer glm

# 2. Configure (the policy flag is for the vendored ext/glm cmake config)
cmake -B build -S . -DCMAKE_POLICY_VERSION_MINIMUM=3.5

# 3. Build and run
cmake --build build -j
./build/vertigo
```

### Windows

Uses the prebuilt GLFW/SDL2 libraries in `ext/`. Either open the folder in Visual
Studio (CMake project) and build the `vertigo` target, or from a developer prompt:

```bat
cmake -B build -S .
cmake --build build --config Release
```

Then run the resulting `vertigo.exe` (the required `.dll`s are copied next to it
automatically).

### Linux

```bash
sudo apt install cmake pkg-config libglfw3-dev libsdl2-dev libsdl2-mixer-dev libglm-dev
cmake -B build -S . -DCMAKE_POLICY_VERSION_MINIMUM=3.5
cmake --build build -j
./build/vertigo
```

### Web (WebAssembly, play in the browser)

Run `./build_web.sh` and see [README_WEB.md](Vertigo/README_WEB.md) for the
Emscripten build and itch.io upload steps.

## Code overview

world_init.cpp (called in world_syste.cpp to generate objects):
* createExplorer - creating player (main character)
* createTile - creating tiles (main space for character to move)
* createTileShadow - creating the shadows under the tiles for parallax effect (moves slower/faster when nearer/further from the player)
* createFire - creating fire (main character will pick up and have interaction with different object)
* createObject - creating objects (objects will block main character to move and have interaction with fire)
* createEnemy - creating the enemy AI
* createLight - creating a point light

world_system.cpp:
* step - Update our game world
* restart_game - Reset the world state to its initial state
* on_key - WASD keys for player movement
* player_move - call by on_key for handling movement of player
* fire_move - call by player_move if player is with fire
* UpdatePlayerCoordinates - update player coordinate after player move
* checkForTile - check whether tile state is valid
* Interact - for interation with object and fire
* SetSprite - setting direction player is facing
* initTileCreation - creating tiles objects
* searchForTile - get next tile in the direction of movement
* UpdateParallax - updates items that have parallax behavior

render_system.cpp:
* RenderSystem::createProjectionMatrix function - modified to allow the camera to follow the user

physics_system.cpp:
* PhysicsSystem::oscillate - Oscillate objects will have a offset of a certain amount which varies based on time

ai_system.cpp:
* AISystem::step - checks if its the enemy's turn, then does a BFS search to find the shortest path to the player
* enemyTranslate/enemyTranslateFace - sets up the enemy's animation when moving to another tile
