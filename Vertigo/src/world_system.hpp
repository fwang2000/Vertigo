#pragma once

// internal
#include "common.hpp"

// stlib
#include <vector>
#include <random>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>

#include "render_system.hpp"

// Container for all our entities and game logic. Individual rendering / update is
// deferred to the relative update() methods
class WorldSystem
{
public:
	WorldSystem();

	// Creates a window
	GLFWwindow* create_window();

	// starts the game
	void init(RenderSystem* renderer);

	// Releases all associated resources
	~WorldSystem();

	// Steps the game ahead by ms milliseconds
	bool step(float elapsed_ms);

	// Check for collisions
	void handle_collisions();

    //
    void handle_player_tile_collisions(Entity * player, Entity * tile);

	// Should the game be over ?
	bool is_over()const;
private:
	// Input callback functions
	void on_key(int key, int, int action, int mod);
	void on_mouse_move(vec2 pos);

	// Movement Functions
	void player_move(vec2 velocity, vec2 distanceTo, Direction direction);
	void fire_move(vec2 velocity);
	void UpdateParallax(vec2 playerPos);
	void Interact(Direction direction);
	// bool checkForTile(Direction direction);
	float count = 0;

	// Fire Attributes
	vec2 fire_spot;
	bool obtainedFire;
	vec2 fire_destination;
	bool interacting = false;

	// Player Attributes
	void SetSprite(Direction direction);
	void UpdatePlayerCoordinates(Direction direction);
	vec2 player_destination;
	bool moving = false;
	Direction faceDirection = Direction::UP; // always starts as up

	// Object Attributes
	bool activated = false;

	// restart level
	void restart_game();
	void load_level();

	// OpenGL window handle
	GLFWwindow* window;

	// Current level the player is on.
	unsigned int level;

	// Game state
	RenderSystem* renderer;
	Entity player_explorer;
	Cube cube;
	Entity fire;
	Entity currentObject;

	Direction currDirection = Direction::DOWN;

	// Helper Functions
	Coordinates searchForTile(Direction direction);

	// C++ random number generator
	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist; // number between 0..1
};