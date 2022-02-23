#pragma once
#include "common.hpp"
#include <vector>
#include <array>
#include <unordered_map>
#include "../ext/stb_image/stb_image.h"

enum class Direction
{
	LEFT = 0,
	RIGHT = 1,
	UP = 2,
	DOWN = 3
};

// Player component
struct Player
{
	vec3 playerPos;
};

struct Object
{
	vec2 position;
	int cubeFace;
	bool alive;
	bool interactible;
	bool fireInteactible;
};

struct Fire
{
	bool active = false;
	bool inUse = false;
	vec3 firePos;
};

enum class BOX_ANIMATION {
	STILL = 0,
	UP = 1,
	DOWN = 2,
	LEFT = 3,
	RIGHT = 4
};

// Set each character to their placement in the alphabet for easy conversion from the CSV to TileState
// Letters are 0-indexed: i.e. A = 0
enum class TileState
{
	S = 18,
	F = 5,
	V = 21,
	O = 14,
	B = 2,
	I = 8,
	M = 12,
	N = 13,
	W = 22,
	U = 20,
	E = 4,
	Z = 25
};

struct Tile
{
	BOX_ANIMATION status = BOX_ANIMATION::STILL;
	glm::mat4 model;
	int degrees = 0;
	TileState tileState = TileState::E;
};

// represents the entire cube
// front -> left -> right -> top -> bottom -> back
struct Cube
{
	bool loadFromExcelFile(std::string filename);
	std::array<std::vector<std::vector<Tile>>, 6> faces;
	int size = 0;
	int getSize() { return this->size; }
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

struct Parallax {
	vec2 position = { 0, 0 };
	vec2 displacement = { 0, 0 };
	vec2 factor = { 0.05f, 0.05f };
};

struct Motion {
	vec2 position = { 0, 0 };
	vec2 velocity = { 0, 0 };
	vec2 scale = { 10, 10 };
	float angle = 0;
	Direction direction = Direction::DOWN;
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


// A timer that will be associated to dying chicken
struct DeathTimer
{
	float counter_ms = 3000;
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
	EXPLORER_DOWN = 0,
	EXPLORER_UP = EXPLORER_DOWN + 1,
	EXPLORER_LEFT = EXPLORER_UP + 1,
	EXPLORER_RIGHT = EXPLORER_LEFT + 1,
	TILE = EXPLORER_RIGHT + 1,
	INVISIBLE_TILE = TILE + 1,
	SWITCH_TILE = INVISIBLE_TILE + 1,
	UP_TILE = SWITCH_TILE + 1,
	END_TILE = UP_TILE + 1,
	TILE_SHADOW = END_TILE + 1,
	FIRE = TILE_SHADOW + 1,
	OBJECT = FIRE + 1,
	TEXTURE_COUNT = OBJECT + 1
};
const int texture_count = (int)TEXTURE_ASSET_ID::TEXTURE_COUNT;

enum class EFFECT_ASSET_ID {
	COLOURED = 0,
	TEXTURED = COLOURED + 1,
	TILE = TEXTURED + 1,
	EFFECT_COUNT = TILE + 1
};
const int effect_count = (int)EFFECT_ASSET_ID::EFFECT_COUNT;

enum class GEOMETRY_BUFFER_ID {
	SPRITE = 0,
	SCREEN_TRIANGLE = SPRITE + 1,
	CUBE = SCREEN_TRIANGLE + 1,
	GEOMETRY_COUNT = CUBE + 1
};
const int geometry_count = (int)GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;

struct RenderRequest {
	TEXTURE_ASSET_ID used_texture = TEXTURE_ASSET_ID::TEXTURE_COUNT;
	EFFECT_ASSET_ID used_effect = EFFECT_ASSET_ID::EFFECT_COUNT;
	GEOMETRY_BUFFER_ID used_geometry = GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;
};
