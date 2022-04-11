#pragma once

// internal
#include "common.hpp"

// stlib
#include <vector>
// #include <random>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>

#include "render_system.hpp"

enum class GameState {

	IDLE = 0,
	MOVING = 1,
	INTERACTING = 2,
	BURNING = 3,
	MENU = 4,
	ENEMY_MOVE = 5,
	RESTARTING = 6,
	TITLE_SCREEN = 7
};

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
	void tile_move(Direction direction, SwitchTile* tile);
	void player_move(vec3 movement, Direction direction);
	void Interact(Tile* tile);
	void Burn(Entity entity);
	void UsePower(Direction direction, float power);
	float count = 0;
	bool isControlTile(Tile* tile);

	// Fire Attributes
	vec2 fire_spot;
	bool obtainedFire;
	vec2 fire_destination;
	bool interacting = false;

	// Player Attributes
	void SetSprite(Direction direction);
	vec2 player_destination;
	bool moving = false;
	Direction faceDirection = Direction::UP; // always starts as up

	// Object Attributes
	bool activated = false;

	// restart level
	void restart_game();
	void load_level();
	void next_level();
	void initLevelStatus();

	// OpenGL window handle
	GLFWwindow* window;

	// Current level the player is on.
	unsigned int level;

	// Game state
	RenderSystem* renderer;
	Entity player_explorer;
	Cube cube;
	Object* currBurnable;
	Rotate rot; // command to rotate all tiles and text and objects
	Entity menu;
	Entity fire;
	Entity fire_gauge;
	Entity fire_shadow;
	Entity currentObject;
	GameState gameState = GameState::IDLE;
	bool sound_on = true;

	int currLevel = 0;
	int maxLevel = 0;

	Direction currDirection = Direction::RIGHT;

	// Helper Functions
	Coordinates searchForMoveTile(Direction direction, Coordinates coords);
	void button_select(ButtonTile* b);
	void changeMenu(int dir);
	Coordinates searchForTile(Direction direction, Coordinates coords);
	Entity getCurrentTileEntity();
	Entity getTileFromRegistry(Coordinates coordinates);
	void rotateAll(float elapsed_ms_since_last_update);

	// Music references
	Mix_Music* background_music;
	Mix_Chunk* burn_sound;
	Mix_Chunk* finish_sound;
	Mix_Chunk* fire_sound;
	Mix_Chunk* switch_sound;
	Mix_Chunk* move_fail_sound;
	Mix_Chunk* move_success_sound;
	Mix_Chunk* restart_sound;

	// C++ random number generator
	// std::default_random_engine rng;
	// std::uniform_real_distribution<float> uniform_dist; // number between 0..1

	friend class AISystem;
};