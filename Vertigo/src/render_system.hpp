#pragma once

#include <array>
#include <utility>

#include "common.hpp"
#include "components.hpp"
#include "tiny_ecs.hpp"

// System responsible for setting up OpenGL and for rendering all the
// visual entities in the game
class RenderSystem {
	/**
	 * The following arrays store the assets the game will use. They are loaded
	 * at initialization and are assumed to not be modified by the render loop.
	 *
	 * Whenever possible, add to these lists instead of creating dynamic state
	 * it is easier to debug and faster to execute for the computer.
	 */
	std::array<GLuint, texture_count> texture_gl_handles;
	std::array<ivec2, texture_count> texture_dimensions;

	// Make sure these paths remain in sync with the associated enumerators.
	// Associated id with .obj path
	const std::vector < std::pair<GEOMETRY_BUFFER_ID, std::string>> mesh_paths =
	{
		// specify meshes of other assets here
	  std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::COLUMN, mesh_path("column.obj")),
	  std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::FIRE, mesh_path("fire.obj")),
	  std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::TREE, mesh_path("burnables/tree.obj")),
	  std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::GAUGE, mesh_path("gauge.obj")),
	  std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::ENEMY, mesh_path("enemy.obj")),
	  std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::DEVICE, mesh_path("device.obj"))
	};

	// Make sure these paths remain in sync with the associated enumerators.
	const std::array<std::string, texture_count> texture_paths = {
			textures_path("text/Vertigo.png"),
			textures_path("text/Level.png"),
			textures_path("text/One.png"),
			textures_path("tutorial/welcome.png"),
			textures_path("tutorial/escape.png"),
			textures_path("tutorial/time.png"),
			textures_path("tutorial/luck.png"),
			textures_path("text/Two.png"),
			textures_path("text/Three.png"),
			textures_path("tutorial/empty.png"),
			textures_path("tutorial/other.png"),
			textures_path("tutorial/device.png"),
			textures_path("tutorial/activate.png"),
			textures_path("tutorial/new.png"),
			textures_path("text/Four.png"),
			textures_path("tutorial/position.png"),
			textures_path("tutorial/function.png"),
			textures_path("tutorial/fire.png"),
			textures_path("tutorial/throw.png"),
			textures_path("text/Five.png"),
			textures_path("tutorial/bush.png"),
			textures_path("text/Six.png"),
			textures_path("text/Seven.png"),
			textures_path("text/Eight.png"),
			textures_path("text/Nine.png"),
			textures_path("tutorial/up.png"),
			textures_path("text/Ten.png"),
			textures_path("text/Eleven.png"),
			textures_path("text/Twelve.png"),
			textures_path("tutorial/different.png"),
			textures_path("tutorial/release.png"),
			textures_path("text/Thirteen.png"),
			textures_path("text/Fourteen.png"),
			textures_path("text/Fifteen.png"),
			textures_path("text/Sixteen.png"),
			textures_path("text/Seventeen.png"),
			textures_path("text/Eighteen.png"),
			textures_path("text/Invisible.png"),
			textures_path("text/Switch.png"),
			textures_path("explorer_down.png"),
			textures_path("explorer_up.png"),
			textures_path("explorer_left.png"),
			textures_path("explorer_right.png"),
			textures_path("tile.png"),
			textures_path("invisible_tile.png"),
			textures_path("switch_tile.png"),
			textures_path("switch_tile_success.png"),
			textures_path("right_tile.png"),
			textures_path("left_tile.png"),
			textures_path("down_tile.png"),
			textures_path("up_tile.png"),
			textures_path("up_tile_lightup.png"),
			textures_path("direction_tile_lightup.png"),
			textures_path("control_tile.png"),
			textures_path("move_tile.png"),
			textures_path("end_tile.png"),
			textures_path("tileshadow.png"),
			textures_path("moving_tile.png"),
			textures_path("moving_tile_success.png"),
			textures_path("empty_tile.png"),
			textures_path("fire/firesheet.png"),
			textures_path("fire_shadow.png"),
			textures_path("fire_gauge.png"),
			textures_path("Spritesheets/bushSheet.png"),
			textures_path("menu/on_levels.png"),
			textures_path("menu/on_sound.png"),
			textures_path("menu/on_tutorial.png"),
			textures_path("menu/on_x.png"),
			textures_path("menu/off_levels.png"),
			textures_path("menu/off_sound.png"),
			textures_path("menu/off_tutorial.png"),
			textures_path("menu/off_x.png"),
			textures_path("wood.png"),
			textures_path("marble.png"),
			textures_path("dissolveTexture.png"),
			textures_path("title_page/title_start.png"),
			textures_path("title_page/title_start_music.png"),
			textures_path("title_page/title_start_music_sound.png"),
			textures_path("title_page/title_start_sound.png"),
			textures_path("title_page/title_levels.png"),
			textures_path("title_page/title_levels_music.png"),
			textures_path("title_page/title_levels_music_sound.png"),
			textures_path("title_page/title_levels_sound.png"),
			textures_path("title_page/title_sound_off.png"),
			textures_path("title_page/title_sound_music.png"),
			textures_path("title_page/title_sound_no_music.png"),
			textures_path("title_page/title_sound_off_music.png"),
			textures_path("title_page/title_music_off.png"),
			textures_path("title_page/title_music_off_sound.png"),
			textures_path("title_page/title_music_sound.png"),
			textures_path("title_page/title_music_no_sound.png"),
			textures_path("title_page/title.png"),
			textures_path("title_page/move_controls.png"),
			textures_path("buttons/button_start.png"),
			textures_path("buttons/button_levels.png"),
			textures_path("buttons/button_sound_off.png"),
			textures_path("buttons/button_sound_on.png"),
			textures_path("buttons/levels/Slide1.PNG"),
			textures_path("buttons/levels/Slide2.PNG"),
			textures_path("buttons/levels/Slide3.PNG"),
			textures_path("buttons/levels/Slide4.PNG"),
			textures_path("buttons/levels/Slide5.PNG"),
			textures_path("buttons/levels/Slide6.PNG"),
			textures_path("buttons/levels/Slide7.PNG"),
			textures_path("buttons/levels/Slide8.PNG"),
			textures_path("buttons/levels/Slide9.PNG"),
			textures_path("buttons/levels/Slide10.PNG"),
			textures_path("buttons/levels/Slide11.PNG"),
			textures_path("buttons/levels/Slide12.PNG"),
			textures_path("buttons/levels/Slide13.PNG"),
			textures_path("buttons/levels/Slide14.PNG"),
			textures_path("buttons/levels/Slide15.PNG"),
			textures_path("buttons/levels/Slide16.PNG"),
			textures_path("buttons/levels/Slide17.PNG"),
			textures_path("buttons/levels/Slide18.PNG"),
			textures_path("buttons/levels/Slide19.PNG"),
			textures_path("buttons/levels/Slide20.PNG"),
			textures_path("buttons/levels/Slide21.PNG"),
			textures_path("buttons/levels/Slide22.PNG"),
			textures_path("buttons/levels/Slide23.PNG"),
			textures_path("buttons/levels/Slide24.PNG"),
			textures_path("buttons/levels/Slide25.PNG"),
			textures_path("buttons/levels/lock.png"),
			textures_path("burn_target_tile.png"),
			textures_path("restart.png"),
			textures_path("tutorial/rook.png"),
			textures_path("tutorial/follows.png"),
			textures_path("tutorial/tile_ai_interaction.png"),
			textures_path("title_page/Enter.png"),
			textures_path("tutorial/target.png"),
			textures_path("instruction.png"),
			textures_path("text/Nineteen.png"),
			textures_path("text/Twenty.png"),
			textures_path("text/Left.png"),
			textures_path("text/Middle.png"),
			textures_path("text/Right.png"),
			textures_path("text/4.png"),
			textures_path("text/5.png"),
			textures_path("text/6.png"),
			textures_path("trackball_rotate.png"),
			textures_path("trackball_reset.png"),
			textures_path("text/TwentyOne.png"),
			textures_path("text/TwentyTwo.png"),
			textures_path("text/TwentyThree.png"),
			textures_path("text/TwentyFour.png"),
			textures_path("text/TwentyFive.png"),
			textures_path("text/Congratulations.png")
	};

	std::array<GLuint, effect_count> effects;
	// Make sure these paths remain in sync with the associated enumerators.
	const std::array<std::string, effect_count> effect_paths = {
		shader_path("coloured"),
		shader_path("tile"),
		shader_path("text"),
		shader_path("player"),
		shader_path("fade"),
		shader_path("object"),
		shader_path("fire"),
		shader_path("menu"),
		shader_path("burnable"),
		shader_path("billboard")
	};

	std::array<GLuint, geometry_count> vertex_buffers;
	std::array<GLuint, geometry_count> index_buffers;
	std::array<Mesh, geometry_count> meshes;
	// std::array<MeshBox, geometry_count> meshboxs;

public:
	// Initialize the window
	bool init(GLFWwindow* window);

	template <class T>
	void bindVBOandIBO(GEOMETRY_BUFFER_ID gid, std::vector<T> vertices, std::vector<uint16_t> indices);

	void initializeGlTextures();

	void initializeGlEffects();

	void initializeGlMeshes();
	Mesh& getMesh(GEOMETRY_BUFFER_ID id) { return meshes[(int)id]; };
	// MeshBox& getMeshBox(GEOMETRY_BUFFER_ID id) { return meshboxs[(int)id]; };

	void initializeGlGeometryBuffers();
	// Initialize the screen texture used as intermediate render target
	// The draw loop first renders to this texture, then it is used for the wind
	// shader
	bool initScreenTexture();

	// Destroy resources associated to one or all entities created by the system
	~RenderSystem();

	// Draw all entities
	void draw();
	void drawFire(Entity entity, const mat4& projection3D, const mat4& view);
	void drawObject(Entity entity, const mat4& projection3D, const mat4& view);
	void drawMenu(Entity entity, const mat3& projection);

	mat4 createViewMatrix();
	mat3 createProjectionMatrix();
	mat4 create3DProjectionMatrix(int width, int height);
	mat4 create3DProjectionMatrixPerspective(int width, int height);
	void setCube(Cube cube);

private:
	// Internal drawing functions for each entity type
	void drawTexturedMesh(Entity entity, const mat4& projection3D, const mat4 &view);
	void drawToScreen();
	void setLighting(GLint currProgram);

	// Window handle
	GLFWwindow* window;

	// Screen texture handles
	GLuint frame_buffer;
	GLuint off_screen_render_buffer_color;
	GLuint off_screen_render_buffer_depth;

	Entity screen_state_entity;
	Cube screen_cube;
	vec3 viewPos;
};

bool loadEffectFromFile(
	const std::string& vs_path, const std::string& fs_path, GLuint& out_program);