// Header
#include "world_system.hpp"
#include "world_init.hpp"
#include <Windows.h>

// stlib
#include <cassert>
#include <sstream>

#include "physics_system.hpp"
#include <iostream>

// Game configuration

// Create the world
WorldSystem::WorldSystem()
	: level(2) {
	// Seeding rng with random device
	rng = std::default_random_engine(std::random_device()());
}

WorldSystem::~WorldSystem() {
	// Destroy music components
	if (background_music != nullptr)
		Mix_FreeMusic(background_music);

	// Destroy all created components
	registry.clear_all_components();

	// Close the window
	glfwDestroyWindow(window);
}

// Debugging
namespace {
	void glfw_err_cb(int error, const char* desc) {
		fprintf(stderr, "%f: %s", error, desc);
	}
}

// World initialization
// Note, this has a lot of OpenGL specific things, could be moved to the renderer
GLFWwindow* WorldSystem::create_window() {
	///////////////////////////////////////
	// Initialize GLFW
	glfwSetErrorCallback(glfw_err_cb);
	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize GLFW");
		return nullptr;
	}

	//-------------------------------------------------------------------------
	// If you are on Linux or Windows, you can change these 2 numbers to 4 and 3 and
	// enable the glDebugMessageCallback to have OpenGL catch your mistakes for you.
	// GLFW / OGL Initialization
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#if __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	glfwWindowHint(GLFW_RESIZABLE, 0);

	// Create the main window (for rendering, keyboard, and mouse input)
	window = glfwCreateWindow(window_width_px, window_height_px, "Vertigo", nullptr, nullptr);
	if (window == nullptr) {
		fprintf(stderr, "Failed to glfwCreateWindow");
		return nullptr;
	}

	// Setting callbacks to member functions (that's why the redirect is needed)
	// Input is handled using GLFW, for more info see
	// http://www.glfw.org/docs/latest/input_guide.html
	glfwSetWindowUserPointer(window, this);
	auto key_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2, int _3) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_key(_0, _1, _2, _3); };
	auto cursor_pos_redirect = [](GLFWwindow* wnd, double _0, double _1) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_mouse_move({ _0, _1 }); };
	glfwSetKeyCallback(window, key_redirect);
	glfwSetCursorPosCallback(window, cursor_pos_redirect);

	//////////////////////////////////////
	// Loading music and sounds with SDL
	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		fprintf(stderr, "Failed to initialize SDL Audio");
		return nullptr;
	}
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1) {
		fprintf(stderr, "Failed to open audio device");
		return nullptr;
	}

	background_music = Mix_LoadMUS(audio_path("time-9307.wav").c_str());

	if (background_music == nullptr) {
		fprintf(stderr, "Failed to load sounds\n %s\n %s\n %s\n make sure the data directory is present",
			audio_path("time-9307.wav").c_str()
			);
		return nullptr;
	}

	return window;
}

void WorldSystem::init(RenderSystem* renderer_arg) {
	this->renderer = renderer_arg;
	// Playing background music indefinitely
	Mix_PlayMusic(background_music, -1);

	// Set all states to default
	restart_game();
}

// Update our game world
bool WorldSystem::step(float elapsed_ms_since_last_update) {
	
	/*
	// Updating window title with points
	std::stringstream title_ss;
	title_ss << "Points: " << points;
	glfwSetWindowTitle(window, title_ss.str().c_str());
	*/

	// TODO Update checking of out of screen
	// Removing out of screen entities
	auto& motions_registry = registry.motions;

	// Processing the explorer state
	assert(registry.screenStates.components.size() <= 1);
	ScreenState& screen = registry.screenStates.components[0];

	Motion& player_motion = motions_registry.get(player_explorer);

	rotateAll(elapsed_ms_since_last_update);

	// Update fire position

	if (registry.fire.has(fire)){
		Fire& fire_component = registry.fire.get(fire);

		if (motions_registry.has(fire) && fire_component.active == true){
			Object& fire_object = registry.objects.get(fire);
			Motion& fire_motion = motions_registry.get(fire);
			Player& player = registry.players.get(player_explorer);
			if (fire_motion.position.z < 0){
				fire_motion.acceleration = vec3({0, 0, 0});
				fire_motion.velocity = vec3({0, 0, 0});
				fire_component.inUse = false;
			}
			if (fire_component.inUse == false) {
				fire_object.model = player.model;
				fire_motion.position = player_motion.position + vec3({0.5, 0.5, 1});
			}
		}
	}
	
	for (Entity entity : registry.holdTimers.entities) {
		// progress timer
		HoldTimer& counter = registry.holdTimers.get(entity);
		Motion& timer_motion = registry.motions.get(entity);

		if (counter.increasing){
			counter.counter_ms += elapsed_ms_since_last_update;
			if (counter.counter_ms > counter.max_ms) {
				if (counter.reverse_when_max){
					counter.counter_ms = 2 * counter.max_ms - counter.counter_ms;
					counter.increasing = false;
				}
				else{
					counter.counter_ms = counter.max_ms;
				}
			}
		}

		else{
			counter.counter_ms -= elapsed_ms_since_last_update;
			if (counter.counter_ms < 0) {
				if (counter.reverse_when_max){
					counter.counter_ms = -counter.counter_ms;
					counter.increasing = true;
				}
				else{
					counter.counter_ms = 0;
				}
			}
		}

		timer_motion.scale.z = counter.counter_ms/counter.max_ms;
	}

	// handle burnable animations here
	for (Entity entity : registry.burnables.entities) {

		Burnable& counter = registry.burnables.get(entity);
		if (counter.activate == true){
			counter.counter_ms += elapsed_ms_since_last_update;
		}

		if (counter.counter_ms > counter.max_ms) {
			registry.burnables.remove(entity);
			Tile* tile = registry.tiles.get(entity);
			tile->tileState = TileState::V;
		}
	}

	return true;
}

void WorldSystem::rotateAll(float elapsed_ms_since_last_update) {

	glm::mat4 model = glm::mat4(1.f);

	if (rot.status == BOX_ANIMATION::STILL) return;
		float rads;
		if (elapsed_ms_since_last_update > rot.remainingTime) {
			rads = radians((90.f / rot.animationTime) * rot.remainingTime);
			rot.remainingTime = 0.f;
		} else {
			rads = radians((90.f / rot.animationTime) * elapsed_ms_since_last_update);
			rot.remainingTime -= elapsed_ms_since_last_update;
		}

	for (Tile* tile : registry.tiles.components) {
		switch (rot.status) {
		case BOX_ANIMATION::UP:
			tile->model = rotate(glm::mat4(1.0f), -rads, vec3(1.0f, 0.0f, 0.0f)) * tile->model;
			break;
		case BOX_ANIMATION::DOWN:
			tile->model = rotate(glm::mat4(1.0f), rads, vec3(1.0f, 0.0f, 0.0f)) * tile->model;
			break;
		case BOX_ANIMATION::LEFT:
			tile->model = rotate(glm::mat4(1.0f), -rads, vec3(0.0f, 1.0f, 0.0f)) * tile->model;
			break;
		case BOX_ANIMATION::RIGHT:
			tile->model = rotate(glm::mat4(1.0f), rads, vec3(0.0f, 1.0f, 0.0f)) * tile->model;
			break;
		default:
			break;
		}
	}

	for (Text& text : registry.text.components) {

		switch (rot.status) {
		case BOX_ANIMATION::UP:
			text.model = rotate(glm::mat4(1.0f), -rads, vec3(1.0f, 0.0f, 0.0f)) * text.model;
			break;
		case BOX_ANIMATION::DOWN:
			text.model = rotate(glm::mat4(1.0f), rads, vec3(1.0f, 0.0f, 0.0f)) * text.model;
			break;
		case BOX_ANIMATION::LEFT:
			text.model = rotate(glm::mat4(1.0f), -rads, vec3(0.0f, 1.0f, 0.0f)) * text.model;
			break;
		case BOX_ANIMATION::RIGHT:
			text.model = rotate(glm::mat4(1.0f), rads, vec3(0.0f, 1.0f, 0.0f)) * text.model;
			break;
		}
	}

	for (Object& object : registry.objects.components) {

		switch (rot.status) {
		case BOX_ANIMATION::UP:
			object.model = rotate(glm::mat4(1.0f), -rads, vec3(1.0f, 0.0f, 0.0f)) * object.model;
			break;
		case BOX_ANIMATION::DOWN:
			object.model = rotate(glm::mat4(1.0f), rads, vec3(1.0f, 0.0f, 0.0f)) * object.model;
			break;
		case BOX_ANIMATION::LEFT:
			object.model = rotate(glm::mat4(1.0f), -rads, vec3(0.0f, 1.0f, 0.0f)) * object.model;
			break;
		case BOX_ANIMATION::RIGHT:
			object.model = rotate(glm::mat4(1.0f), rads, vec3(0.0f, 1.0f, 0.0f)) * object.model;
			break;
		}
	}

	for (Oscillate& oscillate : registry.oscillations.components){
		switch (rot.status) {
		case BOX_ANIMATION::UP:
			oscillate.amplitude = rotate(glm::mat4(1.0f), -rads, vec3(1.0f, 0.0f, 0.0f)) * vec4(oscillate.amplitude, 0);
			oscillate.center = oscillate.amplitude;
			break;
		case BOX_ANIMATION::DOWN:
			oscillate.amplitude = rotate(glm::mat4(1.0f), rads, vec3(1.0f, 0.0f, 0.0f)) * vec4(oscillate.amplitude, 0);
			oscillate.center = oscillate.amplitude;
			break;
		case BOX_ANIMATION::LEFT:
			oscillate.amplitude = rotate(glm::mat4(1.0f), -rads, vec3(0.0f, 1.0f, 0.0f)) * vec4(oscillate.amplitude, 0);
			oscillate.center = oscillate.amplitude;
			break;
		case BOX_ANIMATION::RIGHT:
			oscillate.amplitude = rotate(glm::mat4(1.0f), rads, vec3(0.0f, 1.0f, 0.0f)) * vec4(oscillate.amplitude, 0);
			oscillate.center = oscillate.amplitude;
			break;
		}
	}

	// TODO: rotate all objects that are rendered on screen
	if (rot.remainingTime == 0.f)
	{
		rot.status = BOX_ANIMATION::STILL;
	}
}

// Reset the world state to its initial state
void WorldSystem::restart_game() {
	// Debugging for memory/component leaks
	registry.list_all_components();
	printf("Restarting\n");

	// Remove all entities that we created
	// All that have a motion, we could also iterate over all bug, eagles, ... but that would be more cumbersome
	while (registry.motions.entities.size() > 0)
		registry.remove_all_components_of(registry.motions.entities.back());

	while (registry.tiles.entities.size() > 0)
		registry.remove_all_components_of(registry.tiles.entities.back());

	while (registry.text.entities.size() > 0)
		registry.remove_all_components_of(registry.text.entities.back());

	while (registry.renderRequests.entities.size() > 0)
		registry.remove_all_components_of(registry.objects.entities.back());

	while (registry.renderRequests.entities.size() > 0)
		registry.remove_all_components_of(registry.renderRequests.entities.back());

	load_level();

	// Debugging for memory/component leaks
	registry.list_all_components();
}

void WorldSystem::load_level() {

	Coordinates startingpos = {0, 0, 0};
	glm::mat4 translateMatrix = glm::mat4(1.f);
	// Load a level
	cube.loadFromExcelFile(tile_path("level" + std::to_string(level) + ".csv"));
	cube.createAdjList();
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < cube.size; j++) {
			for (int k = 0; k < cube.size; k++) {

				Entity tile = createTile(cube.faces[i][j][k]);

				if (cube.faces[i][j][k]->tileState == TileState::S) {
					startingpos.f = i;
					startingpos.r = j;
					startingpos.c = k;
					translateMatrix = cube.faces[i][j][k]->model;
				}
				
				if (cube.faces[i][j][k]->tileState == TileState::N) {

					createColumn(renderer, Coordinates{ i, j, k }, cube.faces[i][j][k]->model);
				}

				if (cube.faces[i][j][k]->tileState == TileState::F) {

					fire = createFire(renderer, Coordinates{ i, j, k }, cube.faces[i][j][k]->model);
				}

				if (cube.faces[i][j][k]->tileState == TileState::O) {
					// Create constantly moving tile
					createConstMovingTile(tile, Coordinates{ i, j, k }, cube.faces[i][j][k]->model);
				}
			}
		}
	}

	cube.loadTextFromExcelFile(text_path("text" + std::to_string(level) + ".csv"));
	for (uint i = 0; i < cube.text.size(); i++) {
		createText(cube.text[i]);
	}

	cube.loadModificationsFromExcelFile(modifications_path("modifications" + std::to_string(level) + ".csv"));

	for (uint i = 0; i < registry.oscillations.size(); i++){
		Entity e = registry.oscillations.entities[i];
		Oscillate& o = registry.oscillations.components[i];
		if (registry.tiles.has(e)){
			Tile* tile = registry.tiles.get(e);
			ConstMovingTile* t = (ConstMovingTile*) cube.getTile(tile->coords);
			o.center = (vec3({t->endCoords.c, t->endCoords.r, 0}) - vec3({t->startCoords.c, t->startCoords.r, 0})) / vec3(2.0f);
			o.amplitude = o.center;
		}
	}

	renderer->setCube(cube);

	// Create a new explorer
	player_explorer = createExplorer(renderer, startingpos, translateMatrix);
	registry.colors.insert(player_explorer, {1, 1, 1});

	obtainedFire = false;
	faceDirection = Direction::UP;
	SetSprite(currDirection = Direction::RIGHT);
}

// Compute collisions between entities
void WorldSystem::handle_collisions() {
	// Loop over all collisions detected by the physics system
	auto& collisionsRegistry = registry.collisions; // TODO: @Tim, is the reference here needed?
	for (uint i = 0; i < collisionsRegistry.components.size(); i++) {
		// The entity and its collider
		Entity entity = collisionsRegistry.entities[i];
		Entity entity_other = collisionsRegistry.components[i].other;
	}

	// Remove all collisions from this simulation step
	registry.collisions.clear();
}

// Should the game be over ?
bool WorldSystem::is_over() const {
	return bool(glfwWindowShouldClose(window));
}

// On key callback
void WorldSystem::on_key(int key, int, int action, int mod) {

	/*if (gameState != GameState::IDLE) {
		return;
	}*/

	Direction dir = currDirection;

	Tile* tile = cube.getTile(registry.players.get(player_explorer).playerPos);

	if (action == GLFW_RELEASE && rot.status == BOX_ANIMATION::STILL) {
		switch (key)
		{
		case GLFW_KEY_W:
			dir = Direction::UP;
			player_move(vec3({0, 1, 0}), dir);
			break;
		case GLFW_KEY_S:
			dir = Direction::DOWN;
			player_move(vec3({0, -1, 0}), dir);
			break;
		case GLFW_KEY_A:
			dir = Direction::LEFT;
			player_move(vec3({-1, 0, 0}), dir);
			break;
		case GLFW_KEY_D:
			dir = Direction::RIGHT;
			player_move(vec3({1, 0, 0}), dir);
			break;
		case GLFW_KEY_I:
			Interact(tile);
			break;
		case GLFW_KEY_B:
			Burn(tile);
			break;
		default:
			break;
		}
	}

	// Fire release
	if (action == GLFW_PRESS && key == GLFW_KEY_ENTER) {
		if (!registry.fire.has(fire)){
			return;
		}
		Fire& fire_component = registry.fire.get(fire);
		if (!(fire_component.inUse) && fire_component.active){
			// If fire is picked up and has yet to be shot, add to holdTimer
			Player& player = registry.players.get(player_explorer);
			fire_gauge = createFireGauge(renderer, player.playerPos, player.model);
		}
	}

	if (action == GLFW_RELEASE && key == GLFW_KEY_ENTER && registry.holdTimers.has(fire_gauge) && gameState != GameState::MENU) {
		HoldTimer& holdTimer = registry.holdTimers.get(fire_gauge);
		float power = holdTimer.counter_ms/holdTimer.max_ms;
		registry.remove_all_components_of(fire_gauge);
		UsePower(currDirection, power);
	}

	if (action == GLFW_PRESS && key == GLFW_KEY_R) {
		cube.reset();
		faceDirection = Direction::UP;
		restart_game();
	}

	SetSprite(dir);

	// Menu page esc
	if (action == GLFW_RELEASE && key == GLFW_KEY_ESCAPE) {
		gameState = GameState::MENU;
		menu = createMenu(renderer);
	}

	if (action == GLFW_RELEASE && gameState == GameState::MENU) {
		switch (key)
		{
		case GLFW_KEY_UP:
			changeMenu(0);
			break;
		case GLFW_KEY_DOWN:
			changeMenu(1);
			break;
		case GLFW_KEY_RIGHT:
			changeMenu(2);
			break;
		case GLFW_KEY_LEFT:
			changeMenu(3);
			break;
		case GLFW_KEY_ENTER:
			changeMenu(4);
			break;
		default:
			break;
		}
	}
}

void WorldSystem::on_mouse_move(vec2 mouse_position) 
{
	(vec2)mouse_position; // dummy to avoid compiler warning
}

void WorldSystem::player_move(vec3 movement, Direction direction) 
{
	int dir = static_cast<int>(faceDirection) * -1;
	Direction trueDirection = mod(direction, dir);

	Coordinates newCoords = searchForTile(trueDirection);
	Tile* tile = cube.getTile(newCoords);

	// printf("%d, %d, %d\n", tile->coords.f, tile->coords.r, tile->coords.c);

	if (tile->tileState == TileState::B || tile->tileState == TileState::E	|| tile->tileState == TileState::I || tile->tileState == TileState::N) {
		return;
	}
	
	Player& player = registry.players.get(player_explorer);
	Motion& motion = registry.motions.get(player_explorer);

	if (tile->tileState == TileState::F){
		Object& fire_object = registry.objects.get(fire);
		Motion& fire_motion = registry.motions.get(fire);
		Fire& fire_component = registry.fire.get(fire);
		fire_component.active = true;
		fire_object.model = player.model;
		fire_motion.scale = {0.2f, 0.2f, 0.2f};
	}

	if (motion.position != motion.destination){
		return;
	}

	gameState = GameState::MOVING;
	
	if (player.playerPos.f != newCoords.f) {
		// play cube rotation animation based on DIRECTION, not trueDirection
		float multiplier = cube.size-1.f;
		Tile* curTile = cube.getTile(player.playerPos);

		faceDirection = mod(faceDirection, curTile->adjList[static_cast<int>(trueDirection)].second);
		rot.remainingTime = 500.f;
		rot.animationTime = 500.f;
		
		switch (direction) {
			case Direction::UP:
				rot.status = BOX_ANIMATION::DOWN;
				for (Tile* t : registry.tiles.components) {
					switch (t->direction) {
					case FACE_DIRECTION::FRONT:
						t->direction = FACE_DIRECTION::BOTTOM;
						break;
					case FACE_DIRECTION::TOP:
						t->direction = FACE_DIRECTION::FRONT;
						break;
					case FACE_DIRECTION::BACK:
						t->direction = FACE_DIRECTION::TOP;
						break;
					case FACE_DIRECTION::BOTTOM:
						t->direction = FACE_DIRECTION::BACK;
						break;
					default:
						break;
					}
				}
				motion.destination = motion.position + vec3(0.f, (multiplier * -1.f), 0.f);
				motion.remaining_time = 500;
				motion.move_z = true;
				break;
			case Direction::RIGHT:
				rot.status = BOX_ANIMATION::LEFT;
				for (Tile* t : registry.tiles.components) {

					switch (t->direction) {
					case FACE_DIRECTION::FRONT:
						t->direction = FACE_DIRECTION::LEFT;
						break;
					case FACE_DIRECTION::LEFT:
						t->direction = FACE_DIRECTION::BACK;
						break;
					case FACE_DIRECTION::BACK:
						t->direction = FACE_DIRECTION::RIGHT;
						break;
					case FACE_DIRECTION::RIGHT:
						t->direction = FACE_DIRECTION::FRONT;
						break;
					default:
						break;
					}
				}
				motion.destination = motion.position + vec3((multiplier * -1.f), 0.f, 0.f);
				motion.remaining_time = 500;
				motion.move_z = true;
				break;
			case Direction::LEFT:
				rot.status = BOX_ANIMATION::RIGHT;
				for (Tile* t : registry.tiles.components) {
					switch (t->direction) {
					case FACE_DIRECTION::FRONT:
						t->direction = FACE_DIRECTION::RIGHT;
						break;
					case FACE_DIRECTION::LEFT:
						t->direction = FACE_DIRECTION::FRONT;
						break;
					case FACE_DIRECTION::BACK:
						t->direction = FACE_DIRECTION::LEFT;
						break;
					case FACE_DIRECTION::RIGHT:
						t->direction = FACE_DIRECTION::BACK;
						break;
					default:
						break;
					}
				}
				motion.destination = motion.position + vec3((multiplier * 1.f), 0.f, 0.f);
				motion.remaining_time = 500;
				motion.move_z = true;
				break;
			case Direction::DOWN:
				rot.status = BOX_ANIMATION::UP;
				for (Tile* t : registry.tiles.components) {
					switch (t->direction) {
					case FACE_DIRECTION::FRONT:
						t->direction = FACE_DIRECTION::TOP;
						break;
					case FACE_DIRECTION::TOP:
						t->direction = FACE_DIRECTION::BACK;
						break;
					case FACE_DIRECTION::BACK:
						t->direction = FACE_DIRECTION::BOTTOM;
						break;
					case FACE_DIRECTION::BOTTOM:
						t->direction = FACE_DIRECTION::FRONT;
						break;
					default:
						break;
					}
				}
				motion.destination = motion.position + vec3(0.f, (multiplier * 1.f), 0.f);
				motion.remaining_time = 500;
				motion.move_z = true;
				break;
		}

	} else {
		motion.move_z = false;
		motion.destination = motion.position + vec3({1.f, 1.f, 0}) * movement;
		motion.remaining_time = 500;
	}

	player.playerPos = newCoords; // same as UpdatePlayerCoordinates
	Tile* currtile = cube.getTile(registry.players.get(player_explorer).playerPos);	

	if (tile->tileState == TileState::Z) {
		next_level();
	}
}

void WorldSystem::changeMenu(int dir){
	Menu& curr = registry.menus.get(menu);
	
	if ((dir == 4) && (curr.option == 0 || curr.option == 4)) {
		gameState = GameState::IDLE;
		cube.reset();
		faceDirection = Direction::UP;
		restart_game();
		return;
	}
	curr.changeOption(dir);

	TEXTURE_ASSET_ID id = TEXTURE_ASSET_ID::ON_LEVELS;

	switch (curr.option) {
	case 0:
		id = TEXTURE_ASSET_ID::ON_X;
		break;
	case 1:
		id = TEXTURE_ASSET_ID::ON_LEVELS;
		break;
	case 2:
		id = TEXTURE_ASSET_ID::ON_SOUND;
		break;
	case 3:
		id = TEXTURE_ASSET_ID::ON_TUTORIAL;
		break;
	case 4:
		id = TEXTURE_ASSET_ID::OFF_X;
		break;
	case 5:
		id = TEXTURE_ASSET_ID::OFF_LEVELS;
		break;
	case 6:
		id = TEXTURE_ASSET_ID::OFF_SOUND;
		break;
	case 7:
		id = TEXTURE_ASSET_ID::OFF_TUTORIAL;
		break;
	default:
		id = TEXTURE_ASSET_ID::ON_X;
	}
	
	RenderRequest& menuRequest = registry.renderRequests.get(menu);
	menuRequest.used_texture = id;
}

void WorldSystem::Interact(Tile* tile) 
{
	if (tile->tileState != TileState::W) {
		return;
	}

	SwitchTile* s_tile = (SwitchTile*)tile;

	if (s_tile->toggled) {
		return;
	}

	gameState = GameState::INTERACTING;

	// change texture of switch tile to success switch tile
	if (tile->tileState == TileState::W) {
		Entity successTile = getCurrentTileEntity();
		RenderRequest& switchRequest = registry.renderRequests.get(successTile);
		switchRequest.used_texture = TEXTURE_ASSET_ID::SWITCH_TILE_SUCCESS;
	}

	if (s_tile->targetTile->tileState == TileState::I) {

		Entity tile = getTileFromRegistry(s_tile->targetTile->coords);
		RenderRequest& request = registry.renderRequests.get(tile);
		request.used_texture = TEXTURE_ASSET_ID::TILE;
	}
	else {

		if (s_tile->targetTile->tileState == TileState::E) {
			
			return;
		}

		Entity src_tile_entity = getTileFromRegistry(s_tile->targetTile->coords);
		Tile* src_tile = cube.getTile(s_tile->targetTile->coords);
		RenderRequest& src_request = registry.renderRequests.get(src_tile_entity);

		Entity dest_tile_entity = getTileFromRegistry(s_tile->targetCoords);
		Tile* dest_tile = cube.getTile(s_tile->targetCoords);
		RenderRequest& dest_request = registry.renderRequests.get(dest_tile_entity);

		src_request.used_texture = TEXTURE_ASSET_ID::EMPTY;

		TEXTURE_ASSET_ID id = TEXTURE_ASSET_ID::TILE;

		dest_tile->tileState = src_tile->tileState;
		// dest_tile->model = src_tile->model;
		// dest_tile->status = BOX_ANIMATION::STILL;
		if (src_tile->tileState == TileState::S) {
			SwitchTile* new_s_tile = static_cast<SwitchTile*>(dest_tile);
			id = TEXTURE_ASSET_ID::SWITCH_TILE;
		}
		src_tile->tileState = TileState::E;

		dest_request.used_texture = id;
	}

	s_tile->action();
}

void WorldSystem::UsePower(Direction direction, float power) 
{
	Fire& fire_component = registry.fire.get(fire);

	// If fire is already shot, do not reshoot
	if (fire_component.inUse){
		return;
	}

	fire_component.inUse = true;

	Motion& motion = registry.motions.get(fire);
	float p = 3.0f;
	motion.acceleration = vec3(0, 0, -2 * p);

	switch (direction) 
	{
	case Direction::DOWN:
		motion.velocity = vec3(0, -p, 2 * p) * (power + 0.2f);
		break;
	case Direction::UP:
		motion.velocity = vec3(0, p, 2 * p) * (power + 0.2f);
		break;
	case Direction::LEFT:
		motion.velocity = vec3(-p, 0, 2 * p) * (power + 0.2f);
		break;
	case Direction::RIGHT:
		motion.velocity = vec3(p, 0, 2 * p) * (power + 0.2f);
		break;
	default:
		motion.velocity = vec3(0, 0, 0);
	}
}

void WorldSystem::Burn(Tile* tile) {

	if (tile->tileState == TileState::B) {
		gameState = GameState::BURNING;
		Burnable& burned = registry.burnables.get(getCurrentTileEntity());
		burned.activate = true;
	}
}

void WorldSystem::SetSprite(Direction direction) {

	//if (direction == currDirection) {
	//	return;
	//}

	RenderRequest& request = registry.renderRequests.get(player_explorer);

	TEXTURE_ASSET_ID id = TEXTURE_ASSET_ID::EXPLORER_DOWN;
	switch (direction) {
	case Direction::UP:
		request.used_texture = TEXTURE_ASSET_ID::EXPLORER_LEFT;
		break;
	case Direction::DOWN:
		request.used_texture = TEXTURE_ASSET_ID::EXPLORER_RIGHT;
		break;
	case Direction::LEFT:
		request.used_texture = TEXTURE_ASSET_ID::EXPLORER_UP;
		break;
	case Direction::RIGHT:
		request.used_texture = TEXTURE_ASSET_ID::EXPLORER_DOWN;
		break;
	default:
		break;
	}

	currDirection = direction;
}

Coordinates WorldSystem::searchForTile(Direction direction) {

	Player& player = registry.players.get(player_explorer);
	Coordinates coords = player.playerPos;

	switch (direction)
	{
	case Direction::UP:
		if (coords.r == 0) {
			return cube.getTile(coords)->adjList[0].first;
		} else {
			coords.r--;
		}
		break;
	case Direction::RIGHT:
		if (coords.c == cube.size - 1) {
			return cube.getTile(coords)->adjList[1].first;
		} else {
			coords.c++;
		}
		break;
	case Direction::DOWN:
		if (coords.r == cube.size - 1) {
			return cube.getTile(coords)->adjList[2].first;
		} else {
			coords.r++;
		}
		break;
	case Direction::LEFT:
		if (coords.c == 0) {
			return cube.getTile(coords)->adjList[3].first;
		} else {
			coords.c--;
		}
		break;
	}

	return coords;
}

// Get entity of the current tile player is on using getTileFromRegistry(player coordinates) 
Entity WorldSystem::getCurrentTileEntity() {
	Player& player = registry.players.get(player_explorer);
	return getTileFromRegistry(player.playerPos);
}

Entity WorldSystem::getTileFromRegistry(Coordinates coordinates) {

	int index = 9 * coordinates.f + 3 * coordinates.r + coordinates.c; 
	Tile* tile = registry.tiles.components.at(index);
	// printf("%d, %d, %d\n", tile->coords.f, tile->coords.r, tile->coords.c);
	return registry.tiles.entities.at(index);
}

void WorldSystem::next_level() {
	
		int w, h;
		glfwGetWindowSize(window, &w, &h);
		cube.reset();
		level++;
		faceDirection = Direction::UP;
		restart_game();
}
