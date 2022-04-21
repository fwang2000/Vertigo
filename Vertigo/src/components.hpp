#pragma once
#include "common.hpp"
#include <vector>
#include <array>
#include <unordered_map>
#include <utility>
#include "../ext/stb_image/stb_image.h"

struct Coordinates
{
	bool equal(Coordinates a);
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
	float alpha = 0.7f;
	bool burning = false;
	bool burnable = false;
	vec3 color = vec3(1);
};

struct Burnable
{
	int counter_ms = 0;
	int max_ms = 1000;
	int num_intervals = 5;
	bool activate = false;
};

struct Animated
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
	float index = 0;
	int maxIndex = 24;
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
	H = 7,		// Target
	V = 21,		// Valid
	B = 1,		// Burnable
	I = 8,		// Invisible
	N = 13,		// Non-interactible
	O = 14,		// Constantly-Moving Tile
	G = 6,		// Button Tile
	C = 2,		// Controllable Tile
	M = 12,		// Move Tile
	T = 19,		// Throw Tile
	W = 22,		// Switch,
	R = 17,		// Right-Tile
	D = 3,		// Left-Tile
	L = 11,		// Down-Tile
	U = 20,		// Up-Tile
	E = 4,		// Empty
	A = 0,		// Enemy AI
	Z = 25,		// Finish
};

enum class BUTTON
{
	START = 0,
	LEVELS = 1,
	SOUND = 2,
	MUSIC = 3,
	SFX = 4,
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
	TileState tileState = TileState::E;
	std::unordered_map<int, std::pair<Coordinates, int>> adjList; // map of direction to Coordinates and direction to add
	bool highlighted = false;
	bool popup = false;
	int color = -1;
	virtual void action() { return; };
};

struct ControlTile : public Tile {
	bool controled = 0; // 0 is not controlled, 1 is controlled
};

struct UpTile : public Tile {
	Direction dir;
	int id;
	virtual void action();
};

struct SwitchTile : public Tile {

	Tile* targetTile;
	TileState targetTileState;
	Coordinates targetCoords;
	Entity device;
	bool toggled = false;
	int diff = 0; // to compensate for move tiles being on a different face
	virtual void action();
};

struct InvisibleTile : public Tile {
	bool toggled = false;
	virtual void action();
};

struct ConstMovingTile : public SwitchTile {
	Coordinates startCoords;
	Coordinates endCoords;
	virtual void action();
};

struct ButtonTile : public Tile {
	int button_id = 0;
	bool activated = true;
	virtual void action();
};

struct BurnableTile : public Tile {
	bool burned = false;
	Entity object;
	virtual void action();
};

struct ThrowTile : public SwitchTile {
	virtual void action();
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
	vec3 center = vec3({0, 0, 0});
	vec3 amplitude = vec3({0, 0, 0});
	double phase = 0;
	int period = 3000;
};

struct Motion
{
	bool interpolate = false; // 0 for interpolation, 1 for extrapolation
	bool move_z = false;
	vec3 position = {0, 0, 0};
	vec3 scale = {1, 1, 1};
	float rotation;

	// Extrapolation
	vec3 velocity = {0, 0, 0}; // Used if extrapolating
	vec3 acceleration = {0, 0, 0}; // Used if extrapolating

	// Interpolation
	vec3 destination = {0, 0, 0}; // Used if interpolating
	float remaining_time = 0; // Used if interpolating
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

struct Menu
{
	int option = 1;
	bool sound = true;
	void changeOption(int dir);
	void toggleSound() {sound = !sound;}

	// used for cutscenes
	bool auto_texture_id = false;
};

struct MenuButtons
{
};

struct Button
{
};

struct HoldTimer
{
	float counter_ms = 0;
	float max_ms = 3000;
	bool reverse_when_max = true;
	bool increasing = true;
};

// Single Vertex Buffer element for non-textured objects (object.vs.glsl)
struct ColoredVertex
{
	vec3 position;
	vec3 color;
	vec3 normal;
};

// Single Vertex Buffer element for textured sprites (textured.vs.glsl)
struct TexturedVertex
{
	vec3 position;
	vec2 texcoord;
};

struct LightedVertex
{
	vec3 position;
	vec2 texcoord;
	vec3 normal;
};

// Mesh data structure for storing vertex and index buffers
struct Mesh
{
	static bool loadFromOBJFile(std::string obj_path, std::vector<ColoredVertex>& out_vertices, std::vector<uint16_t>& out_vertex_indices, vec2& out_size);
	vec2 original_size = { 1,1 };
	std::vector<ColoredVertex> vertices;
	std::vector<uint16_t> vertex_indices;
};

float defaultTranslate(float elapsed);

float oneDimension(float elapsed);

float oneDimensionNegative(float elapsed);

float cosine(float elapsed);

float flipCosine(float elapsed);

float sine(float elapsed);

float flipSine(float elapsed);

struct Enemy
{
	Enemy()
	{
		translateX = &defaultTranslate;
		translateY = &defaultTranslate;
		translateZ = &defaultTranslate;
	}
	TransFuncPtr translateX;
	TransFuncPtr translateY;
	TransFuncPtr translateZ;
	vec3 axis;
	vec3 startingPos; // used for animation logic
	bool changingFaces = false;
	bool moving = false; // in the process of moving
	float elapsed = 0.f;
};

struct Billboard
{
	glm::mat4 model = glm::mat4(1.f);
};

struct LightSource
{

};

struct RestartTimer
{
	float counter_ms = restart_time;
};

struct TrackBallInfo
{
	bool leftClick = false;
	double prevX;
	double prevY;
	quat prevQuat = quat(1,0,0,0);
	quat rotation = quat(1,0,0,0);
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
	LEVEL = VERTIGO + 1,
	ONE = LEVEL + 1,
	WELCOME = ONE + 1,
	ESCAPE = WELCOME + 1,
	TIME = ESCAPE + 1,
	LUCK = TIME + 1,
	TWO = LUCK + 1,
	THREE = TWO + 1,
	EMPTY_PATH = THREE + 1,
	OTHER = EMPTY_PATH + 1,
	DEVICE = OTHER + 1,
	ACTIVATE = DEVICE + 1,
	NEW = ACTIVATE + 1,
	FOUR = NEW + 1,
	POSITION = FOUR + 1,
	FUNCTION = POSITION + 1,
	FIRE_ITEM = FUNCTION + 1,
	THROW = FIRE_ITEM + 1,
	FIVE = THROW + 1,
	BUSH = FIVE + 1,
	SIX = BUSH + 1,
	SEVEN = SIX + 1,
	EIGHT = SEVEN + 1,
	NINE = EIGHT + 1,
	UP = NINE + 1,
	TEN = UP + 1,
	ELEVEN = TEN + 1,
	TWELVE = ELEVEN + 1,
	DIFFERENT = TWELVE + 1,
	RELEASE = DIFFERENT + 1,
	THIRTEEN = RELEASE + 1,
	FOURTEEN = THIRTEEN + 1,
	FIFTEEN = FOURTEEN + 1,
	SIXTEEN = FIFTEEN + 1,
	SEVENTEEN = SIXTEEN + 1,
	EIGHTEEN = SEVENTEEN + 1,
	INVISIBLE = EIGHTEEN + 1,
	SWITCH = INVISIBLE + 1,
	EXPLORER_DOWN = SWITCH + 1,
	EXPLORER_UP = EXPLORER_DOWN + 1,
	EXPLORER_LEFT = EXPLORER_UP + 1,
	EXPLORER_RIGHT = EXPLORER_LEFT + 1,
	TILE = EXPLORER_RIGHT + 1,
	INVISIBLE_TILE = TILE + 1,
	SWITCH_TILE = INVISIBLE_TILE + 1,
	SWITCH_TILE_SUCCESS = SWITCH_TILE + 1,
	RIGHT_TILE = SWITCH_TILE_SUCCESS + 1,
	LEFT_TILE = RIGHT_TILE + 1,
	DOWN_TILE = LEFT_TILE + 1,
	UP_TILE = DOWN_TILE + 1,
	UP_TILE_SUCCESS = UP_TILE + 1,
	DIRECTION_TILE_SUCCESS = UP_TILE_SUCCESS + 1,
	CONTROL_TILE = DIRECTION_TILE_SUCCESS + 1,
	MOVE_TILE = CONTROL_TILE + 1,
	END_TILE = MOVE_TILE + 1,
	TILE_SHADOW = END_TILE + 1,
	CONST_MOV_TILE = TILE_SHADOW + 1,
	CONST_MOV_TILE_SUCCESS = CONST_MOV_TILE + 1,
	EMPTY = CONST_MOV_TILE_SUCCESS + 1,
	FIRE = EMPTY + 1,
	FIRE_SHADOW = FIRE + 1,
	FIRE_GAUGE = FIRE_SHADOW + 1,
	BUSH_SHEET = FIRE_GAUGE + 1,
	ON_LEVELS = BUSH_SHEET + 1,
	ON_SOUND = ON_LEVELS + 1,
	ON_TUTORIAL = ON_SOUND + 1,
	ON_X = ON_TUTORIAL + 1,
	OFF_LEVELS = ON_X + 1,
	OFF_SOUND = OFF_LEVELS + 1,
	OFF_TUTORIAL = OFF_SOUND + 1,
	OFF_X = OFF_TUTORIAL + 1,
	WOOD = OFF_X + 1,
	MARBLE = WOOD + 1,
	DISSOLVE = MARBLE + 1,
	TITLE_START = DISSOLVE + 1,
	TITLE_START_MUSIC = TITLE_START + 1,
	TITLE_START_MUSIC_SOUND = TITLE_START_MUSIC + 1,
	TITLE_START_SOUND = TITLE_START_MUSIC_SOUND + 1,
	TITLE_LEVELS = TITLE_START_SOUND + 1,
	TITLE_LEVELS_MUSIC = TITLE_LEVELS + 1,
	TITLE_LEVELS_MUSIC_SOUND = TITLE_LEVELS_MUSIC + 1,
	TITLE_LEVELS_SOUND = TITLE_LEVELS_MUSIC_SOUND + 1,
	TITLE_SOUND_OFF = TITLE_LEVELS_SOUND + 1,
	TITLE_SOUND_MUSIC = TITLE_SOUND_OFF + 1,
	TITLE_SOUND_NO_MUSIC = TITLE_SOUND_MUSIC + 1,
	TITLE_SOUND_OFF_MUSIC = TITLE_SOUND_NO_MUSIC + 1,
	TITLE_MUSIC_OFF = TITLE_SOUND_OFF_MUSIC + 1,
	TITLE_MUSIC_OFF_SOUND = TITLE_MUSIC_OFF + 1,
	TITLE_MUSIC_SOUND = TITLE_MUSIC_OFF_SOUND + 1,
	TITLE_MUSIC_NO_SOUND = TITLE_MUSIC_SOUND + 1,
	TITLE = TITLE_MUSIC_NO_SOUND + 1,
	MOVE_CONTROLS = TITLE + 1,
	BUTTON_START = MOVE_CONTROLS + 1,
	BUTTON_LEVELS = BUTTON_START + 1,
	BUTTON_SOUND_OFF = BUTTON_LEVELS + 1,
	BUTTON_SOUND_ON = BUTTON_SOUND_OFF + 1,
	BUTTON_LEVEL_1 = BUTTON_SOUND_ON + 1,
	BUTTON_LEVEL_2 = BUTTON_LEVEL_1 + 1,
	BUTTON_LEVEL_3 = BUTTON_LEVEL_2 + 1,
	BUTTON_LEVEL_4 = BUTTON_LEVEL_3 + 1,
	BUTTON_LEVEL_5 = BUTTON_LEVEL_4 + 1,
	BUTTON_LEVEL_6 = BUTTON_LEVEL_5 + 1,
	BUTTON_LEVEL_7 = BUTTON_LEVEL_6 + 1,
	BUTTON_LEVEL_8 = BUTTON_LEVEL_7 + 1,
	BUTTON_LEVEL_9 = BUTTON_LEVEL_8 + 1,
	BUTTON_LEVEL_10 = BUTTON_LEVEL_9 + 1,
	BUTTON_LEVEL_11 = BUTTON_LEVEL_10 + 1,
	BUTTON_LEVEL_12 = BUTTON_LEVEL_11 + 1,
	BUTTON_LEVEL_13 = BUTTON_LEVEL_12 + 1,
	BUTTON_LEVEL_14 = BUTTON_LEVEL_13 + 1,
	BUTTON_LEVEL_15 = BUTTON_LEVEL_14 + 1,
	BUTTON_LEVEL_16 = BUTTON_LEVEL_15 + 1,
	BUTTON_LEVEL_17 = BUTTON_LEVEL_16 + 1,
	BUTTON_LEVEL_18 = BUTTON_LEVEL_17 + 1,
	BUTTON_LEVEL_19 = BUTTON_LEVEL_18 + 1,
	BUTTON_LEVEL_20 = BUTTON_LEVEL_19 + 1,
	BUTTON_LEVEL_21 = BUTTON_LEVEL_20 + 1,
	BUTTON_LEVEL_22 = BUTTON_LEVEL_21 + 1,
	BUTTON_LEVEL_23 = BUTTON_LEVEL_22 + 1,
	BUTTON_LEVEL_24 = BUTTON_LEVEL_23 + 1,
	BUTTON_LEVEL_25 = BUTTON_LEVEL_24 + 1,
	BUTTON_LEVEL_LOCK = BUTTON_LEVEL_25 + 1,
	BURN_TARGET_TILE = BUTTON_LEVEL_LOCK + 1,
	RESTART_TEXT = BURN_TARGET_TILE + 1,
	TUTORIAL_ROOK = RESTART_TEXT + 1,
	TUTORIAL_FOLLOWS = TUTORIAL_ROOK + 1,
	TUTORIAL_TILE_INTERACTION = TUTORIAL_FOLLOWS + 1,
	ENTER = TUTORIAL_TILE_INTERACTION + 1,
	TARGET = ENTER + 1,
	INSTRUCTION_TEXT = TARGET + 1,
	NINETEEN = INSTRUCTION_TEXT + 1,
	TWENTY = NINETEEN + 1,
	LEFT = TWENTY + 1,
	MIDDLE = LEFT + 1,
	RIGHT = MIDDLE + 1,
	NUM_4 = RIGHT + 1,
	NUM_5 = NUM_4 + 1,
	NUM_6 = NUM_5 + 1,
	TRACKBALL_ROTATE = NUM_6 + 1,
	TRACKBALL_RESET = TRACKBALL_ROTATE + 1,
	TWENTY_ONE = TRACKBALL_RESET + 1,
	TWENTY_TWO = TWENTY_ONE + 1,
	TWENTY_THREE = TWENTY_TWO + 1,
	TWENTY_FOUR = TWENTY_THREE + 1,
	TWENTY_FIVE = TWENTY_FOUR + 1,
	CONGRATULATIONS = TWENTY_FIVE + 1,
	TEXTURE_COUNT = CONGRATULATIONS + 1
};
const int texture_count = (int)TEXTURE_ASSET_ID::TEXTURE_COUNT;

enum class EFFECT_ASSET_ID {
	COLOURED = 0,
	TILE = COLOURED + 1,
	TEXT = TILE + 1,
	PLAYER = TEXT + 1,
	FADE = PLAYER + 1,
	OBJECT = FADE + 1,
	FIRE = OBJECT + 1,
	MENU = FIRE + 1,
	BURNABLE = MENU + 1,
	BILLBOARD = BURNABLE + 1,
	EFFECT_COUNT = BILLBOARD + 1
};
const int effect_count = (int)EFFECT_ASSET_ID::EFFECT_COUNT;

enum class GEOMETRY_BUFFER_ID {
	SPRITE = 0,
	SCREEN_TRIANGLE = SPRITE + 1,
	COLUMN = SCREEN_TRIANGLE + 1,
	ANIMATED = COLUMN + 1,
	FIRE = ANIMATED + 1,
	TREE = FIRE + 1,
	GAUGE = TREE + 1,
	ENEMY = GAUGE + 1,
	LIGHTING = ENEMY + 1,
	POINT_LIGHT = LIGHTING + 1,
	DEVICE = POINT_LIGHT + 1,
	GEOMETRY_COUNT = DEVICE + 1
};
const int geometry_count = (int)GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;

struct RenderRequest {
	TEXTURE_ASSET_ID used_texture = TEXTURE_ASSET_ID::TEXTURE_COUNT;
	EFFECT_ASSET_ID used_effect = EFFECT_ASSET_ID::EFFECT_COUNT;
	GEOMETRY_BUFFER_ID used_geometry = GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;
};
