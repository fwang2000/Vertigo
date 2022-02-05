# Vertigo
Isometric Puzzle Game built with C++ and OpenGL

run chicken.exe to start the program

world_init.cpp (called in world_syste.cpp to generate objects):
* createExplorer - creating player (main character)
* createTile - creating tiles (main space for character to move)
* createTileShadow - creating the shadows under the tiles for parallax effect (moves slower/faster when nearer/further from the player)
* createFire - creating fire (main character will pick up and have interaction with different object)
* createObject - creating objects (objects will block main character to move and have interaction with fire)

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
* 
physics_system.cpp:
* PhysicsSystem::oscillate - Oscillate objects will have a offset of a certain amount which varies based on time

grace day remarks:
Is it possible to carry over our granted graceday to future milestones?
One of our groupmates drop the course so we are granted for one graceday and move submission deadline to Monday from piazza (private question@157)
