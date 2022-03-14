#pragma once
#include "common.hpp"
#include <vector>
#include <array>
#include <unordered_map>
#include <utility>
#include "../ext/stb_image/stb_image.h"

struct Coordinates
{
	int f; // face
	int r; // row
	int c; // col
};

// Player component
struct Player
{
	Coordinates playerPos;
	glm::mat4 model = glm::mat4(1.f);
};

struct Object
{
	Coordinates objectPos;
	glm::mat4 model = glm::mat4(1.f);
};

struct Burnable
{
	int counter_ms = 0;
	int max_ms = 1000;
	int num_intervals = 5;
	bool activate = false;
};

struct Fire
{
	bool active = false;
	bool inUse = false;
	Coordinates firePos;
	glm::mat4 model = glm::mat4(1.f);
	int currFrame = 0;
	int spriteWidth, spriteHeight;
	int texWidth, texHeight;
};

struct Burnable
{
	int counter = 5;
	bool activate = false;
};

enum class BOX_ANIMATION {
	STILL = 0,
	UP = 1,
	DOWN = 2,
	LEFT = 3,
	RIGHT = 4
};

enum class FACE_DIRECTION {
	FRONT = 0,
	LEFT = 1,
	RIGHT = 2,
	TOP = 3,
	BOTTOM = 4,
	BACK = 5
};

// Set each character to their placement in the alphabet for easy conversion from the CSV to TileState
// Letters are 0-indexed: i.e. A = 0
enum class TileState
{
	S = 18,		// Start
	F = 5,		// Fire
	V = 21,		// Valid
	B = 1,		// Burnable
	I = 8,		// Invisible
	N = 13,		// Non-interactible
	O = 14,		// Constantly-Moving Tile
	C = 3,		// Controllable Tile
	T = 19,		// Teleporter
	W = 22,		// Switch
	U = 20,		// Up-Tile
	E = 4,		// Empty
	Z = 25,		// Finish
};

struct Rotate
{
	BOX_ANIMATION status = BOX_ANIMATION::STILL;
	float remainingTime = 0.f;
	float animationTime = 0.f;
};

struct Tile
{
	FACE_DIRECTION direction;
	glm::mat4 model;
	Coordinates coords;
	Coordinates currentPos;
	TileState tileState = TileState::E;
	std::unordered_map<int, std::pair<Coordinates, int>> adjList; // map of direction to Coordinates and direction to add
	virtual void action() { return; };
	
	void move(vec2 translation, vec2 delta_coords);
};

struct UpTile : public Tile {
	Direction dir = Direction::UP;
	virtual void action();
};

struct SwitchTile : public Tile {

	Tile* targetTile;
	Coordinates targetCoords;
	bool toggled = false;
	virtual void action();
};

struct InvisibleTile : public Tile {
	bool toggled = false;
	virtual void action();
};

struct BurnableTile : public Tile {
	bool burned = false;
	virtual void action();
};

struct FinishTile : public Tile {

};

struct StartTile : public Tile {

};

struct Text 
{
	glm::mat4 model;
	int texture_id;
};

// represents the entire cube
// front -> left -> right -> top -> bottom -> back
struct Cube
{
	bool loadFromExcelFile(std::string filename);
	bool loadTextFromExcelFile(std::string filename);
	bool loadModificationsFromExcelFile(std::string filename);
	void createAdjList();
	std::array<std::vector<std::vector<Tile*>>, 6> faces;
	std::vector<Text> text;
	int size = 0;
	Tile* getTile(Coordinates coord);
	void reset();
};


// All data relevant to the shape and motion of entities
// TODO: MOTION SEEMS LIKE A VERY INEFFICIENT WAY TO RENDER AND UPDATE ITEMS

struct Oscillate {
	vec2 displacement = { 0, 0 };
	vec2 amplitude = { 0, 10 };
	double phase = 0;
	int steps = 100;
};

struct Motion
{
	bool interpolate = false; // 0 for interpolation, 1 for extrapolation
	bool move_z = false;

	// Extrapolation
	vec3 velocity = {0, 0, 0}; // Used if extrapolating
	vec3 acceleration = {0, 0, 0}; // Used if extrapolating

	// Interpolation
	vec3 destination = {0, 0, 0}; // Used if interpolating
	float remaining_time = 0; // Used if interpolating

	// For rendering 3d coordinates to 2d screen
	vec2 origin = {0, 0};
	// Please don't change this unless you're changing the viewing angle
	// It's hard coded based on the current viewing angle of the cube
	vec2 x_vector = {sin(radians(72.0f)), cos(radians(72.0f))};
	vec2 y_vector = {0, 1};
	vec2 z_vector = {-sin(radians(30.0f)), -cos(radians(30.0f))};
	vec3 position = {0, 0, 0};

	vec2 scale = {10, 10};
};

// Stucture to store collision information
struct Collision
{
	// Note, the first object is stored in the ECS container.entities
	Entity other; // the second object involved in the collision
	Collision(Entity& other) { this->other = other; };
};

// Sets the brightness of the screen
struct ScreenState
{
	float darken_screen_factor = -1;
};

// A timer that will be associated to shot fire
struct ShootTimer
{
	float counter_ms = 10000;
};

struct HoldTimer
{
	float counter_ms = 0;
	float max_ms = 3000;
	bool reverse_when_max = true;
	bool increasing = true;
};

// Single Vertex Buffer element for non-textured meshes (coloured.vs.glsl & chicken.vs.glsl)
struct ColoredVertex
{
	vec3 position;
	vec3 color;
};

// Single Vertex Buffer element for textured sprites (textured.vs.glsl)
struct TexturedVertex
{
	vec3 position;
	vec2 texcoord;
};

// Mesh data structure for storing vertex and index buffers
struct Mesh
{
	static bool loadFromOBJFile(std::string obj_path, std::vector<ColoredVertex>& out_vertices, std::vector<uint16_t>& out_vertex_indices, vec2& out_size);
	vec2 original_size = { 1,1 };
	std::vector<ColoredVertex> vertices;
	std::vector<uint16_t> vertex_indices;
};

/**
 * The following enumerators represent global identifiers refering to graphic
 * assets. For example TEXTURE_ASSET_ID are the identifiers of each texture
 * currently supported by the system.
 *
 * So, instead of referring to a game asset directly, the game logic just
 * uses these enumerators and the RenderRequest struct to inform the renderer
 * how to structure the next draw command.
 *
 * There are 2 reasons for this:
 *
 * First, game assets such as textures and meshes are large and should not be
 * copied around as this wastes memory and runtime. Thus separating the data
 * from its representation makes the system faster.
 *
 * Second, it is good practice to decouple the game logic from the render logic.
 * Imagine, for example, changing from OpenGL to Vulkan, if the game logic
 * depends on OpenGL semantics it will be much harder to do the switch than if
 * the renderer encapsulates all asset data and the game logic is agnostic to it.
 *
 * The final value in each enumeration is both a way to keep track of how many
 * enums there are, and as a default value to represent uninitialized fields.
 */

enum class TEXTURE_ASSET_ID {
	VERTIGO = 0,
	START = VERTIGO + 1,
	LEVEL = START + 1,
	ONE = LEVEL + 1,
	TWO = ONE + 1,
	THREE = TWO + 1,
	FOUR = THREE + 1,
	BUSH = FOUR + 1,
	BURN = BUSH + 1,
	INVISIBLE = BURN + 1,
	SWITCH = INVISIBLE + 1,
	EXPLORER_DOWN = SWITCH + 1,
	EXPLORER_UP = EXPLORER_DOWN + 1,
	EXPLORER_LEFT = EXPLORER_UP + 1,
	EXPLORER_RIGHT = EXPLORER_LEFT + 1,
	TILE = EXPLORER_RIGHT + 1,
	INVISIBLE_TILE = TILE + 1,
	SWITCH_TILE = INVISIBLE_TILE + 1,
	SWITCH_TILE_SUCCESS = SWITCH_TILE + 1,
	UP_TILE = SWITCH_TILE_SUCCESS + 1,
	UP_TILE_SUCCESS = UP_TILE + 1,
	END_TILE = UP_TILE_SUCCESS + 1,
	TILE_SHADOW = END_TILE + 1,
	EMPTY = TILE_SHADOW + 1,
	FIRE = EMPTY + 1,
	FIRE_SHADOW = FIRE + 1,
	FIRE_GAUGE = FIRE_SHADOW + 1,
	BUSH_SHEET = FIRE_GAUGE + 1,
	FIRE_SHEET = BUSH_SHEET + 1,
	TEXTURE_COUNT = FIRE_SHEET + 1
};
const int texture_count = (int)TEXTURE_ASSET_ID::TEXTURE_COUNT;

enum class EFFECT_ASSET_ID {
	COLOURED = 0,
	TEXTURED = COLOURED + 1,
	TILE = TEXTURED + 1,
	TEXT = TILE + 1,
	PLAYER = TEXT + 1,
	FADE = PLAYER + 1,
	OBJECT = FADE + 1,
	FIRE = OBJECT + 1,
	EFFECT_COUNT = FIRE + 1
};
const int effect_count = (int)EFFECT_ASSET_ID::EFFECT_COUNT;

enum class GEOMETRY_BUFFER_ID {
	SPRITE = 0,
	SCREEN_TRIANGLE = SPRITE + 1,
	COLUMN = SCREEN_TRIANGLE + 1,
	ANIMATED = COLUMN + 1,
	GEOMETRY_COUNT = ANIMATED + 1,
};
const int geometry_count = (int)GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;

struct RenderRequest {
	TEXTURE_ASSET_ID used_texture = TEXTURE_ASSET_ID::TEXTURE_COUNT;
	EFFECT_ASSET_ID used_effect = EFFECT_ASSET_ID::EFFECT_COUNT;
	GEOMETRY_BUFFER_ID used_geometry = GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;
};
