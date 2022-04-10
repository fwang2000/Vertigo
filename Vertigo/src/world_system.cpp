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
	: level() {
	// Seeding rng with random device
	// rng = std::default_random_engine(std::random_device()());
}

WorldSystem::~WorldSystem() {
	// Destroy music components
	if (background_music != nullptr)
		Mix_FreeMusic(background_music);
	if (burn_sound != nullptr)
		Mix_FreeChunk(burn_sound);
	if (finish_sound != nullptr)
		Mix_FreeChunk(finish_sound);
	if (fire_sound != nullptr)
		Mix_FreeChunk(fire_sound);
	if (switch_sound != nullptr)
		Mix_FreeChunk(switch_sound);
	if (move_fail_sound != nullptr)
		Mix_FreeChunk(move_fail_sound);
	if (move_success_sound != nullptr)
		Mix_FreeChunk(move_success_sound);
	if (restart_sound != nullptr)
		Mix_FreeChunk(restart_sound);
	Mix_CloseAudio();

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
	auto mouse_button_callback = [](GLFWwindow* wnd, int _0, int _1, int _2) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_mouse_click(_0, _1, _2); };
	glfwSetKeyCallback(window, key_redirect);
	glfwSetCursorPosCallback(window, cursor_pos_redirect);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

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
	burn_sound = Mix_LoadWAV(audio_path("burn.wav").c_str());
	finish_sound = Mix_LoadWAV(audio_path("finish.wav").c_str());
	fire_sound = Mix_LoadWAV(audio_path("fire.wav").c_str());
	switch_sound = Mix_LoadWAV(audio_path("switch.wav").c_str());
	move_fail_sound = Mix_LoadWAV(audio_path("movefail.wav").c_str());
	move_success_sound = Mix_LoadWAV(audio_path("movesuccess.wav").c_str());
	restart_sound = Mix_LoadWAV(audio_path("restart.wav").c_str());

	if (background_music == nullptr || burn_sound == nullptr || finish_sound == nullptr || fire_sound == nullptr || switch_sound == nullptr || move_fail_sound == nullptr || move_success_sound == nullptr || restart_sound == nullptr) {
		fprintf(stderr, "Failed to load sounds\n %s\n %s\n %s\n %s\n %s\n %s\n %s\n %s\n make sure the data directory is present",
			audio_path("time-9307.wav").c_str(),
			audio_path("burn.wav").c_str(),
			audio_path("finish.wav").c_str(),
			audio_path("fire.wav").c_str(),
			audio_path("switch.wav").c_str(),
			audio_path("movefail.wav").c_str(),
			audio_path("movesuccess.wav").c_str(),
			audio_path("restart.wav").c_str());
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
	// TODO Update checking of out of screen
	// Removing out of screen entities
	auto& motions_registry = registry.motions;

	// Processing the explorer state
	assert(registry.screenStates.components.size() <= 1);
	ScreenState& screen = registry.screenStates.components[0];

	float min_counter_ms = restart_time;
	for (Entity e : registry.restartTimer.entities) {
		RestartTimer& counter = registry.restartTimer.get(e);
		counter.counter_ms -= elapsed_ms_since_last_update;
		if(counter.counter_ms < min_counter_ms){
		    min_counter_ms = counter.counter_ms;
		}
		// restart the game once the death timer expired
		if (counter.counter_ms < 0) {
			registry.restartTimer.remove(e);
			screen.darken_screen_factor = 0;
            cube.reset();
			faceDirection = Direction::UP;
			rot.status = BOX_ANIMATION::STILL;
			restart_game();
			return true;
		}
	}
	screen.darken_screen_factor = 1 - min_counter_ms / restart_time;

	rotateAll(elapsed_ms_since_last_update);

	Motion& player_motion = motions_registry.get(player_explorer);
	// Update fire position

	if (registry.fire.has(fire)){
		Fire& fire_component = registry.fire.get(fire);

		if (motions_registry.has(fire) && fire_component.active == true){
			Object& fire_object = registry.objects.get(fire);
			Motion& fire_motion = motions_registry.get(fire);
			Player& player = registry.players.get(player_explorer);
			if (fire_motion.position.z <= 0){
				fire_motion.acceleration = vec3({0, 0, 0});
				fire_motion.velocity = vec3({0, 0, 0});
				fire_component.inUse = false;
			}
			if (fire_component.inUse == false) {
				fire_object.model = player.model;
				fire_motion.position = player_motion.position + vec3({0.2, 0.2, 1});
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
		timer_motion.position.y = timer_motion.scale.z / 2 + player_motion.position.y;
		timer_motion.position.z = player_motion.position.z + 1;
	}

	// handle animations here
	for (Entity entity : registry.animated.entities) {

		Animated& counter = registry.animated.get(entity);
		if (counter.activate == true){
			counter.counter_ms += elapsed_ms_since_last_update;
		}

		if (counter.counter_ms > counter.max_ms) {
			registry.animated.remove(entity);
			Tile* tile = registry.tiles.get(entity);
			tile->tileState = TileState::V;
		}
	}

	if (gameState == GameState::BURNING) {
		
		if (currBurnable->alpha <= 0.f || !currBurnable->burning) {
			gameState = GameState::IDLE;

			Tile* tile = cube.getTile(registry.players.get(player_explorer).playerPos);
			int dir = static_cast<int>(faceDirection) * -1;
			Direction trueDirection = mod(currDirection, dir);
			Coordinates newCoords = searchForTile(trueDirection, tile->coords);
			Tile* btile = cube.getTile(newCoords);
			btile->tileState = TileState::V;
		}
	}

	// check that we have finished the moving animation
	if (gameState == GameState::MOVING) {
		if (player_motion.destination == player_motion.position) {
			// check that we reached the goal
			Player player = registry.players.get(player_explorer);

			Tile* tile = cube.getTile(player.playerPos);
			if (tile->tileState == TileState::Z) {
				Mix_PlayChannel(-1, finish_sound, 0);
				next_level();
			}

			// check if enemies need to move
			if (registry.enemies.entities.size() > 0) {
				gameState = GameState::ENEMY_MOVE;
			} else {
				gameState = GameState::IDLE;
			}
		}
	}
	
	// check if enemy has finished moving
	if (gameState == GameState::ENEMY_MOVE) {
		Enemy& enemy = registry.enemies.components[0];
		if (enemy.elapsed >= 500.f) {
			enemy.moving = false;
			enemy.elapsed = 0.f;
			gameState = GameState::IDLE;
			
		}
	}

	// check if enemy has captured
	if (gameState == GameState::IDLE && registry.enemies.entities.size() > 0) {
		Entity e = registry.enemies.entities[0];
		Object object = registry.objects.get(e);
		Player& player = registry.players.get(player_explorer);
		if (object.objectPos.equal(player.playerPos)) {
			// restart
			if (!registry.restartTimer.has(player_explorer)) {
				gameState = GameState::RESTARTING;
				Mix_PlayChannel(-1, restart_sound, 0);
				registry.restartTimer.emplace(player_explorer);
			}
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

	for (Billboard& billboard: registry.billboards.components) {
		switch (rot.status) {
		case BOX_ANIMATION::UP:
			billboard.model = rotate(glm::mat4(1.0f), -rads, vec3(1.0f, 0.0f, 0.0f)) * billboard.model;
			break;
		case BOX_ANIMATION::DOWN:
			billboard.model = rotate(glm::mat4(1.0f), rads, vec3(1.0f, 0.0f, 0.0f)) * billboard.model;
			break;
		case BOX_ANIMATION::LEFT:
			billboard.model = rotate(glm::mat4(1.0f), -rads, vec3(0.0f, 1.0f, 0.0f)) * billboard.model;
			break;
		case BOX_ANIMATION::RIGHT:
			billboard.model = rotate(glm::mat4(1.0f), rads, vec3(0.0f, 1.0f, 0.0f)) * billboard.model;
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

	while (registry.objects.entities.size() > 0)
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
					createLight(renderer, Coordinates{ i, j, k }, cube.faces[i][j][k]->model);
				}

				if (cube.faces[i][j][k]->tileState == TileState::B) {

					BurnableTile* b_tile = (BurnableTile*)cube.faces[i][j][k];

					b_tile->object = createBurnable(renderer, Coordinates{ i, j, k }, cube.faces[i][j][k]->model);

				}

				if (cube.faces[i][j][k]->tileState == TileState::F) {

					fire = createFire(renderer, Coordinates{ i, j, k }, cube.faces[i][j][k]->model);
				}

				if (cube.faces[i][j][k]->tileState == TileState::O) {
					// Create constantly moving tile
					createConstMovingTile(tile, Coordinates{ i, j, k }, cube.faces[i][j][k]->model);
				}

				if (cube.faces[i][j][k]->tileState == TileState::A) {
					createEnemy(renderer, Coordinates{ i, j, k }, cube.faces[i][j][k]->model);
				}
				if (cube.faces[i][j][k]->tileState == TileState::G) {
					createButtonTile(tile);
				}
			}
		}
	}

	cube.loadTextFromExcelFile(text_path("text" + std::to_string(level) + ".csv"));
	for (uint i = 0; i < cube.text.size(); i++) {
		createText(cube.text[i]);
	}

	cube.loadModificationsFromExcelFile(modifications_path("modifications" + std::to_string(level) + ".csv"));

	// Update button tiles

	for (uint i = 0; i < registry.buttons.size(); i++){
		Entity e = registry.buttons.entities[i];
		Tile* tile = registry.tiles.get(e);
		ButtonTile* b = (ButtonTile*) cube.getTile(tile->coords);
		RenderRequest& r = registry.renderRequests.get(e);

		r.used_texture = (TEXTURE_ASSET_ID)((int) TEXTURE_ASSET_ID::BUTTON_START + b->button_id);
		printf("%d\n", (int) r.used_texture);
	}

	// Update constantly moving tiles
	for (uint i = 0; i < registry.oscillations.size(); i++){
		Entity e = registry.oscillations.entities[i];
		Oscillate& o = registry.oscillations.components[i];
		if (registry.tiles.has(e)){
			Tile* tile = registry.tiles.get(e);
			ConstMovingTile* t = (ConstMovingTile*) cube.getTile(tile->coords);
			if (t->endCoords.f == 0){
				o.center = (vec3({t->endCoords.c, t->endCoords.r, 0}) - vec3({t->startCoords.c, t->startCoords.r, 0})) / vec3(2.0f);
			}
			else if (t->endCoords.f == 1){
				o.center = (vec3({0, -t->endCoords.r, t->endCoords.c}) - vec3({0, -t->startCoords.r, t->startCoords.c})) / vec3(2.0f);
			}
			else if (t->endCoords.f == 2){
				o.center = (vec3({0, -t->endCoords.r, -t->endCoords.c}) - vec3({0, -t->startCoords.r, -t->startCoords.c})) / vec3(2.0f);
			}
			else if (t->endCoords.f == 3){
				o.center = (vec3({t->endCoords.c, 0, t->endCoords.r}) - vec3({t->startCoords.c, 0, t->startCoords.r})) / vec3(2.0f);
			}
			else if (t->endCoords.f == 4){
				o.center = (vec3({t->endCoords.c, 0, -t->endCoords.r}) - vec3({t->startCoords.c, 0, -t->startCoords.r})) / vec3(2.0f);
			}
			else if (t->endCoords.f == 5){
				o.center = (vec3({-t->endCoords.c, -t->endCoords.r, 0}) - vec3({-t->startCoords.c, -t->startCoords.r, 0})) / vec3(2.0f);
			}

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
	gameState = GameState::IDLE;
}

// Compute collisions between entities
void WorldSystem::handle_collisions() {
	// Loop over all collisions detected by the physics system
	auto& collisionsRegistry = registry.collisions; // TODO: @Tim, is the reference here needed?
	for (uint i = 0; i < collisionsRegistry.components.size(); i++) {
		// The entity and its collider
		Entity entity = collisionsRegistry.entities[i];
		Entity entity_other = collisionsRegistry.components[i].other;

		if (registry.fire.has(entity) && registry.objects.has(entity_other)){
			Object& object = registry.objects.get(entity_other);
			if (object.burnable) {
				Burn(entity_other);				
			}
		}
		else if (registry.fire.has(entity) && registry.tiles.has(entity_other)){			
			Tile* tile = registry.tiles.get(entity_other);
			if (tile->tileState == TileState::O){
				Interact(tile); 
			}
		}
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

	if (gameState != GameState::IDLE && gameState != GameState::TITLE_SCREEN) {
		return;
	}

	// Menu page esc
	if (action == GLFW_RELEASE && key == GLFW_KEY_ESCAPE) {
		if (gameState != GameState::MENU) {
			gameState = GameState::MENU;
			menu = createMenu(renderer);
		}
	}

	switch (gameState) {
	case GameState::MENU:

		if (action == GLFW_RELEASE) {
			switch (key)
			{
			case GLFW_KEY_W:
				changeMenu(0);
				break;
			case GLFW_KEY_S:
				changeMenu(1);
				break;
			case GLFW_KEY_D:
				changeMenu(2);
				break;
			case GLFW_KEY_A:
				changeMenu(3);
				break;
			case GLFW_KEY_ENTER:
				changeMenu(4);
				break;
			default:
				break;
			}
		}
		break;

	default:

		Direction dir = currDirection;

		Tile* tile = cube.getTile(registry.players.get(player_explorer).playerPos);

		if (action == GLFW_RELEASE && rot.status == BOX_ANIMATION::STILL) {
			switch (key)
			{
			case GLFW_KEY_W:
				dir = Direction::UP;
				if (tile->tileState == TileState::B) { 
					Mix_PlayChannel(-1, move_fail_sound, 0);
					break; }
				if (tile->tileState == TileState::C) {
					ControlTile* c_tile = (ControlTile*)tile;
					if (c_tile->controled == 1) {
						tile_move(dir, c_tile->targetTile, c_tile);
						break;
					}
				}
				player_move(vec3({ 0, 1, 0 }), dir);
				break;
			case GLFW_KEY_S:
				dir = Direction::DOWN;
				if (tile->tileState == TileState::B) {
					Mix_PlayChannel(-1, move_fail_sound, 0); 
					break;
				}
				if (tile->tileState == TileState::C) {
					ControlTile* c_tile = (ControlTile*)tile;
					if (c_tile->controled == 1) {
						tile_move(dir, c_tile->targetTile, c_tile);
						break;
					}
				}
				player_move(vec3({ 0, -1, 0 }), dir);
				break;
			case GLFW_KEY_A:
				dir = Direction::LEFT;
				if (tile->tileState == TileState::B) {
					Mix_PlayChannel(-1, move_fail_sound, 0); 
					break; }
				if (tile->tileState == TileState::C) {
					ControlTile* c_tile = (ControlTile*)tile;
					if (c_tile->controled == 1) {
						tile_move(dir, c_tile->targetTile, c_tile);
						break;
					}
				}
				player_move(vec3({ -1, 0, 0 }), dir);
				break;
			case GLFW_KEY_D:
				dir = Direction::RIGHT;
				if (tile->tileState == TileState::B) {
					Mix_PlayChannel(-1, move_fail_sound, 0);
					break; }
				if (tile->tileState == TileState::C) {
					ControlTile* c_tile = (ControlTile*)tile;
					if (c_tile->controled == 1) {
						tile_move(dir, c_tile->targetTile, c_tile);
						break;
					}
				}
				player_move(vec3({ 1, 0, 0 }), dir);
				break;
			case GLFW_KEY_I:
				if (tile->tileState == TileState::C) {
					ControlTile* c_tile = (ControlTile*)tile;
					Mix_PlayChannel(-1, switch_sound, 0);
					c_tile->controled = !c_tile->controled;
					c_tile->targetTile->highlighted = !c_tile->targetTile->highlighted;
					break;
				}
				if (tile->tileState == TileState::B) { break; }
				Mix_PlayChannel(-1, switch_sound, 0);
				Interact(tile);
				break;
			default:
				break;
			}
		}

		if (action == GLFW_RELEASE && key == GLFW_KEY_ENTER && tile->tileState == TileState::G){
			ButtonTile* bTile = (ButtonTile*) tile;
			switch (bTile->button_id)
			{
				case 0:
					next_level();
					break;
				default:
					break;
			}

		}

		// Fire release
		if (action == GLFW_PRESS && key == GLFW_KEY_ENTER) {
			if (!registry.fire.has(fire)) {
				return;
			}
			Fire& fire_component = registry.fire.get(fire);
			if (!(fire_component.inUse) && fire_component.active) {
				// If fire is picked up and has yet to be shot, add to holdTimer
				Player& player = registry.players.get(player_explorer);
				fire_gauge = createFireGauge(renderer, player.playerPos, player.model);
			}
			Mix_PlayChannel(-1, fire_sound, 0);
		}

		if (action == GLFW_RELEASE && key == GLFW_KEY_ENTER && registry.holdTimers.has(fire_gauge) && gameState != GameState::MENU) {
			HoldTimer& holdTimer = registry.holdTimers.get(fire_gauge);
			float power = holdTimer.counter_ms / holdTimer.max_ms;
			registry.remove_all_components_of(fire_gauge);
			UsePower(currDirection, power);
		}

		if (action == GLFW_PRESS && key == GLFW_KEY_R) {
			if (!registry.restartTimer.has(player_explorer)) {
				gameState = GameState::RESTARTING;
				registry.restartTimer.emplace(player_explorer);
				Mix_PlayChannel(-1, restart_sound, 0);
			}				
		}
		SetSprite(dir);
	}
}

void WorldSystem::on_mouse_move(vec2 mouse_position) 
{
	if (gameState != GameState::TITLE_SCREEN) {
		return;
	}

	Player& player = registry.players.get(player_explorer);
	Entity& startText = registry.text.entities.at(1);
	RenderRequest& request = registry.renderRequests.get(startText);

	if (player.playerPos.f == 3) {

		if (mouse_position.x > 682 || mouse_position.x < 408) {

			request.used_texture = TEXTURE_ASSET_ID::TITLE;
			return;
		}

		float z = ((682 - 408) * mouse_position.y - (526 - 436) * mouse_position.x)/(682 - 408);
		if (z <= 350 && z >= 302) {
			request.used_texture = TEXTURE_ASSET_ID::TITLE_START_MUSIC_SOUND;
		}
		else if (z <= 421 && z >= 374) {
			request.used_texture = TEXTURE_ASSET_ID::TITLE_LEVELS_MUSIC_SOUND;
		}
		else if (z <= 492 && z >= 445) {
			request.used_texture = TEXTURE_ASSET_ID::TITLE_SOUND_MUSIC;
		}
		else if (z < 563 && z >= 516) {
			request.used_texture = TEXTURE_ASSET_ID::TITLE_MUSIC_SOUND;
		}
		else {
			request.used_texture = TEXTURE_ASSET_ID::TITLE;
		}
	}

	(vec2)mouse_position; // dummy to avoid compiler warning
}

void WorldSystem::on_mouse_click(int button, int action, int mods) {

	if (gameState != GameState::TITLE_SCREEN) {
		return;
	}

	Entity& startText = registry.text.entities.at(1);
	RenderRequest& request = registry.renderRequests.get(startText);
	int texID = (int)request.used_texture;

	Player& player = registry.players.get(player_explorer);

	if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT && player.playerPos.f == 3) {
		if (texID >= 48 && texID <= 51) {
			next_level();
		}
	}
}

void WorldSystem::tile_move(Direction direction, Tile* tile, ControlTile* ctile) {
	int dir = static_cast<int>(faceDirection) * -1;
	Direction trueDirection = mod(direction, dir);
	Coordinates newCoords = searchForTile(trueDirection, tile->coords);
	Tile* ntile = cube.getTile(newCoords);
	if (ntile->direction != tile->direction)
	{
		return;
	}
	if (ntile->tileState == TileState::E)
	{
		Entity next_tile_entity = getTileFromRegistry(newCoords);
		RenderRequest& next_request = registry.renderRequests.get(next_tile_entity);
		ntile->tileState = TileState::M;
		ntile->highlighted = true;
		next_request.used_texture = TEXTURE_ASSET_ID::MOVE_TILE;

		Entity cur_tile_entity = getTileFromRegistry(tile->coords);
		RenderRequest& cur_request = registry.renderRequests.get(cur_tile_entity);
		cur_request.used_texture = TEXTURE_ASSET_ID::EMPTY;
		tile->tileState = TileState::E;
		tile->highlighted = false;

		ctile->targetTile = ntile;
	}
	return;
}

void WorldSystem::player_move(vec3 movement, Direction direction) 
{
	Player& player = registry.players.get(player_explorer);
	Motion& motion = registry.motions.get(player_explorer);
	if (motion.position != motion.destination){
		Mix_PlayChannel(-1, move_fail_sound, 0);
		return;
	}

	int dir = static_cast<int>(faceDirection) * -1;
	Direction trueDirection = mod(direction, dir);

	Coordinates newCoords = searchForTile(trueDirection, player.playerPos);
	Tile* tile = cube.getTile(newCoords);

	// No movement
	if (tile->tileState == TileState::B || tile->tileState == TileState::E	|| tile->tileState == TileState::I || 
		tile->tileState == TileState::N || tile->tileState == TileState::O) {
		Mix_PlayChannel(-1, move_fail_sound, 0);
		return;
	}

	if (tile->tileState == TileState::R || tile->tileState == TileState::U || tile->tileState == TileState::L || tile->tileState == TileState::D) {
		UpTile* uptile = (UpTile*)tile;
		if (direction != Direction::UP || uptile->dir != trueDirection)
		{
			Mix_PlayChannel(-1, move_fail_sound, 0);
			return;
		}
	}
	
	// Move off current tile
	// Do any updates of previous tile here
	Tile* currtile = cube.getTile(registry.players.get(player_explorer).playerPos);
	if (currtile->tileState == TileState::G){
		currtile->highlighted = false;
	}

	// Updates based on new tile
	// Button
	if (tile->tileState == TileState::G){
		tile->highlighted = true;
	}
	// Fire
	if (tile->tileState == TileState::F){
		Object& fire_object = registry.objects.get(fire);
		Motion& fire_motion = registry.motions.get(fire);
		Fire& fire_component = registry.fire.get(fire);
		fire_component.active = true;
		fire_object.model = player.model;
		fire_motion.scale = {0.4f, 0.4f, 0.4f};
	}


	if (gameState != GameState::TITLE_SCREEN) {

		gameState = GameState::MOVING;
	}
	
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

	if (tile->tileState == TileState::Z) {
		rot.status = BOX_ANIMATION::STILL;
		Mix_PlayChannel(-1, finish_sound, 0);
		next_level();
	}
	else
	{
		Mix_PlayChannel(-1, move_success_sound, 0);
	}
}

void WorldSystem::button_select(ButtonTile* b) {
	switch(b->button_id)
	{
		case BUTTON::START:
			next_level();
		default:
			return;
	}
}

void WorldSystem::changeMenu(int dir){
	Menu& curr = registry.menus.get(menu);
	
	if ((dir == 4) && (curr.option == 0 || curr.option == 4)) {
		gameState = GameState::IDLE;
		cube.reset();
		faceDirection = Direction::UP;
		rot.status = BOX_ANIMATION::STILL;
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

	if (tile->tileState != TileState::W && tile->tileState != TileState::O) {
		return;
	}
	SwitchTile* s_tile = (SwitchTile*)tile;

	if (s_tile->toggled) {
		gameState = GameState::IDLE;
		return;
	}

	gameState = GameState::INTERACTING;

	Entity successTile = getTileFromRegistry(tile->coords);

	RenderRequest& switchRequest = registry.renderRequests.get(successTile);

	if (s_tile->targetTile->tileState == TileState::I) {

		Entity t = getTileFromRegistry(s_tile->targetTile->coords);
		RenderRequest& request = registry.renderRequests.get(t);
		request.used_texture = TEXTURE_ASSET_ID::TILE;
		switchRequest.used_texture = TEXTURE_ASSET_ID::SWITCH_TILE_SUCCESS;
	}
	else {

		if (s_tile->targetTile->tileState == TileState::E) {

			gameState = GameState::IDLE;
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

		switchRequest.used_texture = TEXTURE_ASSET_ID::SWITCH_TILE_SUCCESS;
	}

	s_tile->action();

	gameState = GameState::IDLE;
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

void WorldSystem::Burn(Entity entity) {
	gameState = GameState::BURNING;
	currBurnable = &registry.objects.get(entity);
	if (currBurnable->burning == false)
	{
		Mix_PlayChannel(-1, burn_sound, 0);
	}
	currBurnable->burning = true;
}

void WorldSystem::SetSprite(Direction direction) {

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

Coordinates WorldSystem::searchForTile(Direction direction, Coordinates coords) {

	switch (direction)
	{
	case Direction::UP:
		if (coords.r == 0) {
			return cube.getTile(coords)->adjList[0].first;
		}
		else {
			coords.r--;
		}
		break;
	case Direction::RIGHT:
		if (coords.c == cube.size - 1) {
			return cube.getTile(coords)->adjList[1].first;
		}
		else {
			coords.c++;
		}
		break;
	case Direction::DOWN:
		if (coords.r == cube.size - 1) {
			return cube.getTile(coords)->adjList[2].first;
		}
		else {
			coords.r++;
		}
		break;
	case Direction::LEFT:
		if (coords.c == 0) {
			return cube.getTile(coords)->adjList[3].first;
		}
		else {
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

	int index = pow(cube.size, 2) * coordinates.f + cube.size * coordinates.r + coordinates.c;
	Tile* tile = registry.tiles.components.at(index);
	return registry.tiles.entities.at(index);
}

void WorldSystem::next_level() {
	
		int w, h;
		glfwGetWindowSize(window, &w, &h);
		cube.reset();
		level++;
		faceDirection = Direction::UP;
		gameState = GameState::IDLE;
		rot.status = BOX_ANIMATION::STILL;
		restart_game();
}
