// Header
#include "world_system.hpp"
#include "world_init.hpp"

// stlib
#include <cassert>
#include <sstream>

#include "physics_system.hpp"

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
		fprintf(stderr, "%d: %s", error, desc);
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

	// Removing out of screen entities
	auto& motions_registry = registry.motions;

	// Remove entities that leave the screen on the left side
	// Iterate backwards to be able to remove without unterfering with the next object to visit
	// (the containers exchange the last element with the current)
	for (int i = (int)motions_registry.components.size() - 1; i >= 0; --i) {
		Motion& motion = motions_registry.components[i];
		if (motion.position.x + abs(motion.scale.x) < 0.f) {
			if (!registry.players.has(motions_registry.entities[i])) // don't remove the player
				registry.remove_all_components_of(motions_registry.entities[i]);
		}
	}
	
	Motion* player_motion = &motions_registry.get(player_explorer);

	// Processing the explorer state
	assert(registry.screenStates.components.size() <= 1);
	ScreenState& screen = registry.screenStates.components[0];

	if (moving) {
		if ((destination.y >= player_motion->position.y && currDirection == Direction::UP) ||
			(destination.y <= player_motion->position.y && currDirection == Direction::DOWN) ||
			(destination.x <= player_motion->position.x && currDirection == Direction::RIGHT) ||
			(destination.x >= player_motion->position.x && currDirection == Direction::LEFT))
		{
			player_motion->velocity = vec2(0, 0);
			player_motion->position = destination;
			moving = false;
		}
	}

	/*
	float min_counter_ms = 3000.f;
	for (Entity entity : registry.deathTimers.entities) {
		// progress timer
		DeathTimer& counter = registry.deathTimers.get(entity);
		counter.counter_ms -= elapsed_ms_since_last_update;
		if (counter.counter_ms < min_counter_ms) {
			min_counter_ms = counter.counter_ms;
		}

		// restart the game once the death timer expired
		if (counter.counter_ms < 0) {
			registry.deathTimers.remove(entity);
			screen.darken_screen_factor = 0;
			restart_game();
			return true;
		}
	}
	// reduce window brightness if any of the present chickens is dying
	screen.darken_screen_factor = 1 - min_counter_ms / 3000;

	*/

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

	// Debugging for memory/component leaks
	registry.list_all_components();

	// Create a new explorer
	player_explorer = createExplorer(renderer, { window_width_px / 2, window_height_px / 2 });
	cube = createCube(renderer);
	registry.colors.insert(player_explorer, { 1, 1, 1 });

	// initTileCreation();
}

// Compute collisions between entities
void WorldSystem::handle_collisions() {
	// Loop over all collisions detected by the physics system
	auto& collisionsRegistry = registry.collisions; // TODO: @Tim, is the reference here needed?
	for (uint i = 0; i < collisionsRegistry.components.size(); i++) {
		// The entity and its collider
		Entity entity = collisionsRegistry.entities[i];
		Entity entity_other = collisionsRegistry.components[i].other;

		// For now, we are only interested in collisions that involve the chicken
		if (registry.players.has(entity)) {
			//Player& player = registry.players.get(entity);

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
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TODO: Control player such that one click will make the player
	//		 to the next tile. 
	// IDEA: boolean to check if movement is allowed. With each step,
	//		 decrease the distance between the expected destination,
	//		 and current position by some velocity. Once it has passed,
	//		 untick the boolean and reset the explorer position to the
	//		 exact tile position to account for any lag.
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	if (moving) {
		return;
	}

	Direction dir = Direction::DOWN;

	switch (key)
	{
		if (action != GLFW_PRESS) {
			break;
		}
	case GLFW_KEY_W:
		dir = Direction::UP;
		move(vec2(0, -250), vec2(0, -100));
		break;
	case GLFW_KEY_S:
		dir = Direction::DOWN;
		move(vec2(0, 250), vec2(0, 100));
		break;
	case GLFW_KEY_A:
		dir = Direction::LEFT;
		move(vec2(-250, 0), vec2(-100, 0));
		break;
	case GLFW_KEY_D:
		dir = Direction::RIGHT;
		move(vec2(250, 0), vec2(100, 0));
		break;
	default:
		break;
	}

	SetSprite(dir);

	/*
	// Resetting game
	if (action == GLFW_RELEASE && key == GLFW_KEY_R) {
		int w, h;
		glfwGetWindowSize(window, &w, &h);

		restart_game();
	}
	*/
}

void WorldSystem::on_mouse_move(vec2 mouse_position) 
{
	(vec2)mouse_position; // dummy to avoid compiler warning
}

void WorldSystem::move(vec2 velocity, vec2 distanceTo) 
{
	Motion* explorer_motion = &registry.motions.get(player_explorer);
	explorer_motion->velocity = velocity;
	destination = vec2(explorer_motion->position.x, explorer_motion->position.y) + distanceTo;
	moving = true;
}

void WorldSystem::SetSprite(Direction direction) {

	if (direction == currDirection) {
		return;
	}

	TEXTURE_ASSET_ID id = TEXTURE_ASSET_ID::EXPLORER_DOWN;
	switch (direction) {
	case Direction::UP:
		id = TEXTURE_ASSET_ID::EXPLORER_UP;
		break;
	case Direction::DOWN:
		id = TEXTURE_ASSET_ID::EXPLORER_DOWN;
		break;
	case Direction::LEFT:
		id = TEXTURE_ASSET_ID::EXPLORER_LEFT;
		break;
	case Direction::RIGHT:
		id = TEXTURE_ASSET_ID::EXPLORER_RIGHT;
		break;
	default:
		break;
	}

	registry.renderRequests.remove(player_explorer);

	registry.renderRequests.insert(
		player_explorer,
		{
			id,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE
		}
	);

	currDirection = direction;
}

void WorldSystem::initTileCreation() {

	int distance = 300;

	for (int i = -1; i < 2; i++) 
	{
		for (int j = -1; j < 2; j++) 
		{
			createTile(renderer, vec2(window_width_px / 2 + i * distance, window_height_px / 2 + j * distance));
		}
	}
}