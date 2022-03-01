// Header
#include "world_system.hpp"
#include "world_init.hpp"

// stlib
#include <cassert>
#include <sstream>

#include "physics_system.hpp"
#include <iostream>

// Game configuration

// Create the world
WorldSystem::WorldSystem()
	: level(0) {
	// Seeding rng with random device
	rng = std::default_random_engine(std::random_device()());
}

WorldSystem::~WorldSystem() {

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

	return window;
}

void WorldSystem::init(RenderSystem* renderer_arg) {
	this->renderer = renderer_arg;

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

	// Remove entities that leave the screen on the left side
	// Iterate backwards to be able to remove without unterfering with the next object to visit
	// (the containers exchange the last element with the current)
	// for (int i = (int)motions_registry.components.size() - 1; i >= 0; --i) {
	// 	Motion& motion = motions_registry.components[i];
	// 	if (motion.position.x + abs(motion.scale.x) < 0.f) {
	// 		if (!registry.players.has(motions_registry.entities[i])) // don't remove the player
	// 			registry.remove_all_components_of(motions_registry.entities[i]);
	// 	}
	// }

	// Processing the explorer state
	assert(registry.screenStates.components.size() <= 1);
	ScreenState& screen = registry.screenStates.components[0];

	Motion& player_motion = motions_registry.get(player_explorer);
	Motion& fire_motion = motions_registry.get(fire);

	if (!registry.shootTimers.has(fire_shadow)){
		fire_motion.origin = player_motion.origin;
		fire_motion.position = player_motion.position + vec3(-40, 40, 10);
		fire_motion.acceleration = vec3(0, 0, 0);
	}
	
	// Update timers
	for (Entity entity : registry.shootTimers.entities) {
		// progress timer
		ShootTimer& counter = registry.shootTimers.get(entity);
		counter.counter_ms -= elapsed_ms_since_last_update;
		// restart the game once the death timer expired
		if (counter.counter_ms < 0 || fire_motion.position[2] < 0){
			registry.remove_all_components_of(entity);
		}
	}
	
	for (Entity entity : registry.holdTimers.entities) {
		// progress timer
		HoldTimer& counter = registry.holdTimers.get(entity);
		Motion& timer_motion = registry.motions.get(entity);
		timer_motion.origin = player_motion.origin;
		timer_motion.position = player_motion.position + vec3(-40, -0, 0);;

		if (counter.increasing){
			counter.counter_ms += elapsed_ms_since_last_update;
			// restart the game once the death timer expired
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
			// restart the game once the death timer expired
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

		timer_motion.scale[1] = 100 * counter.counter_ms / counter.max_ms;
	}

	return true;
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
		registry.remove_all_components_of(registry.renderRequests.entities.back());

	// Create a new fire
	fire = createFire(renderer, vec3(-1, -1, -1));
	fire_spot = vec2(
		window_width_px / 2 + -1 * window_height_px / 3,
		window_height_px / 2 + -1 * window_height_px / 3
	);
	registry.colors.insert(fire, vec3{ 1, 0, 0 });

	obtainedFire = false;

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
				if (!(cube.faces[i][j][k]->tileState == TileState::E))
					createTile(cube.faces[i][j][k]);
				if (cube.faces[i][j][k]->tileState == TileState::S) {
					startingpos.f = i;
					startingpos.r = j;
					startingpos.c = k;
					translateMatrix = cube.faces[i][j][k]->model;
				}
			}
		}
	}
	
	cube.loadTextFromExcelFile(text_path("text" + std::to_string(level) + ".csv"));
	for (int i = 0; i < cube.text.size(); i++) {
		createText(cube.text[i]);
	}

	// Create a new explorer
	player_explorer = createExplorer(renderer, startingpos, cube.size);
	registry.colors.insert(player_explorer, {1, 1, 1});
}

// Compute collisions between entities
void WorldSystem::handle_collisions() {
	// Loop over all collisions detected by the physics system
	auto& collisionsRegistry = registry.collisions; // TODO: @Tim, is the reference here needed?
	for (uint i = 0; i < collisionsRegistry.components.size(); i++) {
		// The entity and its collider
		Entity entity = collisionsRegistry.entities[i];
		Entity entity_other = collisionsRegistry.components[i].other;

		// if (registry.fire.has(entity) && registry.objects.has(entity_other))
		// {
		// 	Motion& motion = registry.motions.get(entity);
		// 	motion.velocity = vec2(0);
		// 	motion.position = player_destination + vec2(40, -40);
		// 	interacting = false;

		// 	currentObject = entity_other;
		// 	activated = true;
        // }

		// if (registry.players.has(entity) && registry.fire.has(entity_other)) 
		// {
		// 	//Player& player = registry.players.get(entity);
		// 	if (!obtainedFire) 
		// 	{
		// 		obtainedFire = true;
		// 		Motion& motion = registry.motions.get(entity_other);
		// 		motion.scale = { 0.5 * FIRE_BB_WIDTH, 0.5 * FIRE_BB_HEIGHT };
		// 	}
		// }
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

	if (moving || interacting) {
		return;
	}

	Direction dir = currDirection;

	Entity& tileEntity = registry.tiles.entities[0];
	Tile* tile = &registry.tiles.get(tileEntity);

	printf("%d, %d, %d\n", tile->coords.f, tile->coords.r, tile->coords.c);

	if (action == GLFW_RELEASE && tile->status == BOX_ANIMATION::STILL) {
		switch (key)
		{
		case GLFW_KEY_W:
			dir = Direction::UP;
			player_move(vec3({0, -1, 0}), dir);
			break;
		case GLFW_KEY_S:
			dir = Direction::DOWN;
			player_move(vec3({0, 1, 0}), dir);
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
			// BURN
			break;
		default:
			break;
		}
	}

	// Fire release
	if (action == GLFW_PRESS && key == GLFW_KEY_ENTER) {
		if (!registry.shootTimers.has(fire_shadow)){
			// If fire has yet to be shot, add to holdTimer
			fire_gauge = createFireGauge(renderer);
		}
	}

	if (action == GLFW_RELEASE && key == GLFW_KEY_ENTER && registry.holdTimers.has(fire_gauge)) {
		HoldTimer& holdTimer = registry.holdTimers.get(fire_gauge);
		float power = holdTimer.counter_ms/holdTimer.max_ms;
		registry.remove_all_components_of(fire_gauge);
		UsePower(currDirection, power);
	}

	SetSprite(dir);
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
	if (tile->tileState == TileState::E || tile->tileState == TileState::O || tile->tileState == TileState::I) {
		return;
	}
	Player& player = registry.players.get(player_explorer);
	Motion& motion = registry.motions.get(player_explorer);

	if (player.playerPos.f != newCoords.f) {
		// play cube rotation animation based on DIRECTION, not trueDirection
		float multiplier = cube.size-1.f;
		Tile* curTile = cube.getTile(player.playerPos);
		faceDirection = mod(faceDirection, curTile->adjList[static_cast<int>(trueDirection)].second);
		motion.remaining_time = 1000;

		switch (direction) {
			case Direction::UP:
				
				motion.destination[1] += multiplier * TILE_BB_HEIGHT;

				for (Tile& t : registry.tiles.components) {
					if (t.status == BOX_ANIMATION::STILL)
						t.status = BOX_ANIMATION::DOWN;
				}
				for (Text& t : registry.text.components) {
					if (t.status == BOX_ANIMATION::STILL)
						t.status = BOX_ANIMATION::DOWN;
				}
				player.model = translate(glm::mat4(1.f), vec3(0.f, (multiplier * -1.f), 0.f)) * player.model;
				break;
			case Direction::RIGHT:
				
				motion.destination[0] -= multiplier * TILE_BB_WIDTH;

				for (Tile& t : registry.tiles.components) {
					if (t.status == BOX_ANIMATION::STILL)
						t.status = BOX_ANIMATION::LEFT;
				}
				for (Text& t : registry.text.components) {
					if (t.status == BOX_ANIMATION::STILL)
						t.status = BOX_ANIMATION::LEFT;
				}
				player.model = translate(glm::mat4(1.f), vec3((multiplier * -1.f), 0.f, 0.f)) * player.model;
				break;
			case Direction::LEFT:
				
				motion.destination[0] += multiplier * TILE_BB_WIDTH;

				for (Tile& t : registry.tiles.components) {
					if (t.status == BOX_ANIMATION::STILL)
						t.status = BOX_ANIMATION::RIGHT;
				}
				for (Text& t : registry.text.components) {
					if (t.status == BOX_ANIMATION::STILL)
						t.status = BOX_ANIMATION::RIGHT;
				}
				player.model = translate(glm::mat4(1.f), vec3((multiplier * 1.f), 0.f, 0.f)) * player.model;
				break;
			case Direction::DOWN:
				
				motion.destination[1] -= multiplier * TILE_BB_HEIGHT;

				for (Tile& t : registry.tiles.components) {
					if (t.status == BOX_ANIMATION::STILL)
						t.status = BOX_ANIMATION::UP;
				}
				for (Text& t : registry.text.components) {
					if (t.status == BOX_ANIMATION::STILL)
						t.status = BOX_ANIMATION::UP;
				}
				player.model = translate(glm::mat4(1.f), vec3(0.f, (multiplier * 1.f), 0.f)) * player.model;
				break;
		}
	} 
	else {
		motion.remaining_time = 500;
		motion.destination = motion.position + vec3({TILE_BB_WIDTH, TILE_BB_HEIGHT, 0}) * movement;

		switch (direction) {
			case Direction::UP:
				player.model = translate(glm::mat4(1.f), vec3(0.f, 1.f, 0.f)) * player.model;
				break;
			case Direction::RIGHT:
				player.model = translate(glm::mat4(1.f), vec3(1.f, 0.f, 0.f)) * player.model;
				break;
			case Direction::LEFT:
				player.model = translate(glm::mat4(1.f), vec3(-1.f, 0.f, 0.f)) * player.model;
				break;
			case Direction::DOWN:
				player.model = translate(glm::mat4(1.f), vec3(0.f, -1.f, 0.f)) * player.model;
				break;
		}
	}

	player.playerPos = newCoords; // same as UpdatePlayerCoordinates

	if (tile->tileState == TileState::Z) {
		next_level();
	}

	// UpdatePlayerCoordinates(direction);
	// Motion& motion = registry.motions.get(player_explorer);
	// motion.velocity = velocity;
	// player_destination = motion.position + distanceTo;
	// moving = true;

	// if (obtainedFire) 
	// {
	// 	fire_move(velocity);
	// }
}

void WorldSystem::fire_move(vec2 velocity)
{
	// Motion& motion = registry.motions.get(fire);
	// motion.velocity = velocity;
}

void WorldSystem::UpdatePlayerCoordinates(Direction direction) {
	// TODO: rework this
	// vec2 playerPosShift;

	// switch (direction)
	// {
	// case Direction::DOWN:
	// 	playerPosShift = vec2(0, 1);
	// 	break;
	// case Direction::UP:
	// 	playerPosShift = vec2(0, -1);
	// 	break;
	// case Direction::LEFT:
	// 	playerPosShift = vec2(-1, 0);
	// 	break;
	// case Direction::RIGHT:
	// 	playerPosShift = vec2(1, 0);
	// 	break;
	// default:
	// 	break;
	// }

	// Player& player = registry.players.get(player_explorer);
	// player.playerPos.coordinates += playerPosShift;
}

// bool WorldSystem::checkForTile(Direction direction) 
// {
// 	Tile* tile = searchForTile(direction);

// 	if (tile->tileState == TileState::E || tile->tileState == TileState::O) {
// 		return false;
// 	}
	
// 	return true;
// }

void WorldSystem::Interact(Tile* tile) 
{
	if (tile->tileState != TileState::W) {
		return;
	}

	SwitchTile* s_tile = (SwitchTile*)tile;
	s_tile->action();

	if (s_tile->targetTile->tileState == TileState::I) {
		
	}
}

void WorldSystem::UsePower(Direction direction, float power) 
{
	Player& player = registry.players.get(player_explorer);

	// If fire is already shot, do not reshoot
	if (registry.shootTimers.has(fire_shadow)){
		return;
	}

	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(fire_shadow, &mesh);
	registry.renderRequests.insert(
		fire_shadow,
		{
			TEXTURE_ASSET_ID::FIRE_SHADOW,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE
		}
	);
	ShootTimer& shootTimer = registry.shootTimers.emplace(fire_shadow);
	shootTimer.counter_ms = 10000;
	Motion& shot_motion = registry.motions.emplace(fire_shadow);

	Motion& motion = registry.motions.get(fire);
	float power_factor = 3.0f;
	motion.acceleration = vec3(0, 0, -TILE_BB_WIDTH * power_factor);

	switch (direction) 
	{
	case Direction::DOWN:
		motion.velocity = vec3(0, TILE_BB_HEIGHT, TILE_BB_WIDTH * power_factor) * (power + 0.2f);
		break;
	case Direction::UP:
		motion.velocity = vec3(0, -TILE_BB_HEIGHT, TILE_BB_WIDTH * power_factor) * (power + 0.2f);
		break;
	case Direction::LEFT:
		motion.velocity = vec3(-TILE_BB_WIDTH, 0, TILE_BB_HEIGHT * power_factor) * (power + 0.2f);
		break;
	case Direction::RIGHT:
		motion.velocity = vec3(TILE_BB_WIDTH, 0, TILE_BB_HEIGHT * power_factor) * (power + 0.2f);
		break;
	default:
		motion.velocity = vec3(0, 0, 0);
	}
	
	shot_motion.position = motion.position + vec3(40, -40, 0);
	shot_motion.scale = motion.scale;
	shot_motion.velocity = motion.velocity;
	shot_motion.velocity[2] = 0;
	shot_motion.origin = motion.origin;
}

void WorldSystem::SetSprite(Direction direction) {

	if (direction == currDirection) {
		return;
	}

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

void WorldSystem::next_level() {
	
		int w, h;
		glfwGetWindowSize(window, &w, &h);
		cube.reset();
		level++;
		faceDirection = Direction::UP;
		restart_game();
}
