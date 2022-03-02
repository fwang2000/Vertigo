// internal
#include "render_system.hpp"
#include <SDL.h>

#include "tiny_ecs_registry.hpp"
#include "world_system.hpp"

void RenderSystem::drawTexturedMesh(Entity entity,
	const mat4& projection3D, const mat3& projection2D, const mat4& view)
{
	assert(registry.renderRequests.has(entity));
	const RenderRequest& render_request = registry.renderRequests.get(entity);

	const GLuint used_effect_enum = (GLuint)render_request.used_effect;
	assert(used_effect_enum != (GLuint)EFFECT_ASSET_ID::EFFECT_COUNT);
	const GLuint program = (GLuint)effects[used_effect_enum];

	// Setting shaders
	glUseProgram(program);
	gl_has_errors();

	assert(render_request.used_geometry != GEOMETRY_BUFFER_ID::GEOMETRY_COUNT);
	const GLuint vbo = vertex_buffers[(GLuint)render_request.used_geometry];
	const GLuint ibo = index_buffers[(GLuint)render_request.used_geometry];

	// Setting vertex and index buffers
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	gl_has_errors();

	glm::mat4 model = glm::mat4(1.f);

	// Input data location as in the vertex buffer
	if (render_request.used_effect == EFFECT_ASSET_ID::TEXTURED)
	{
		Motion& motion = registry.motions.get(entity);

		// Transformation code, see Rendering and Transformation in the template
		// specification for more info Incrementally updates transformation matrix,
		// thus ORDER IS IMPORTANT
		Transform transform;

		transform.translate( motion.position[0] * motion.x_vector
						   + motion.position[1] * motion.y_vector
						   + motion.position[2] * motion.z_vector
						   + motion.origin);
		transform.scale(motion.scale);
		transform.rotate(-acos(dot(motion.x_vector, vec2({0, 1}))));
		GLint in_position_loc = glGetAttribLocation(program, "in_position");
		GLint in_texcoord_loc = glGetAttribLocation(program, "in_texcoord");
		gl_has_errors();
		assert(in_texcoord_loc >= 0);

		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
			sizeof(TexturedVertex), (void*)0);
		gl_has_errors();

		glEnableVertexAttribArray(in_texcoord_loc);
		glVertexAttribPointer(
			in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex),
			(void*)sizeof(
				vec3)); // note the stride to skip the preceeding vertex position

		// Enabling and binding texture to slot 0
		glActiveTexture(GL_TEXTURE0);
		gl_has_errors();

		assert(registry.renderRequests.has(entity));
		GLuint texture_id =
			texture_gl_handles[(GLuint)registry.renderRequests.get(entity).used_texture];

		glBindTexture(GL_TEXTURE_2D, texture_id);
		gl_has_errors();

		// Getting uniform locations for glUniform* calls
		GLint color_uloc = glGetUniformLocation(program, "fcolor");
		const vec3 color = registry.colors.has(entity) ? registry.colors.get(entity) : vec3(1);
		glUniform3fv(color_uloc, 1, (float*)&color);
		gl_has_errors();

		// Get number of indices from index buffer, which has elements uint16_t
		GLint size = 0;
		glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
		gl_has_errors();

		GLsizei num_indices = size / sizeof(uint16_t);
		// GLsizei num_triangles = num_indices / 3;

		GLint currProgram;
		glGetIntegerv(GL_CURRENT_PROGRAM, &currProgram);
		// Setting uniform values to the currently bound program
		GLuint transform_loc = glGetUniformLocation(currProgram, "transform");
		glUniformMatrix3fv(transform_loc, 1, GL_FALSE, (float*)&transform.mat);
		GLuint projection_loc = glGetUniformLocation(currProgram, "projection");
		glUniformMatrix3fv(projection_loc, 1, GL_FALSE, (float*)&projection2D);
		gl_has_errors();
		// Drawing of num_indices/3 triangles specified in the index buffer
		glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_SHORT, nullptr);
		gl_has_errors();
	}
	else if (render_request.used_effect == EFFECT_ASSET_ID::TILE)
	{
		GLint in_position_loc = glGetAttribLocation(program, "aPos");
		GLint in_texcoord_loc = glGetAttribLocation(program, "aTex");
		gl_has_errors();
		assert(in_texcoord_loc >= 0);

		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
							  sizeof(TexturedVertex), (void *)0);
		gl_has_errors();

		glEnableVertexAttribArray(in_texcoord_loc);
		// remember to change this if tex0's type changes vec2/vec3
		glVertexAttribPointer(
			in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex),
			(void *)sizeof(
				vec3)); // note the stride to skip the preceeding vertex position

		// Enabling and binding texture to slot 0
		glActiveTexture(GL_TEXTURE0);
		gl_has_errors();

		assert(registry.renderRequests.has(entity));
		GLuint texture_id =
			texture_gl_handles[(GLuint)registry.renderRequests.get(entity).used_texture];

		// use 2d
		glBindTexture(GL_TEXTURE_2D, texture_id);
		gl_has_errors();

		Tile* boxRotate = registry.tiles.get(entity);
		if (boxRotate->tileState == TileState::E) {

			model = mat4(0);
		}
		else
		{
			model = boxRotate->model;
		}

		// Get number of indices from index buffer, which has elements uint16_t
		GLint size = 0;
		glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
		gl_has_errors();

		GLsizei num_indices = size / sizeof(uint16_t);
		// GLsizei num_triangles = num_indices / 3;

		GLint currProgram;
		glGetIntegerv(GL_CURRENT_PROGRAM, &currProgram);
		// Setting uniform values to the currently bound program
		GLuint model_loc = glGetUniformLocation(currProgram, "model");
		glUniformMatrix4fv(model_loc, 1, GL_FALSE, (float *)&model);
		GLuint view_loc = glGetUniformLocation(currProgram, "view");
		glUniformMatrix4fv(view_loc, 1, GL_FALSE, (float *)&view);
		GLuint projection_loc = glGetUniformLocation(currProgram, "proj");

		glUniformMatrix4fv(projection_loc, 1, GL_FALSE, (float *)&projection3D);
		gl_has_errors();
		// Drawing of num_indices/3 triangles specified in the index buffer
		glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_SHORT, nullptr);
		gl_has_errors();
	}
	else if (render_request.used_effect == EFFECT_ASSET_ID::PLAYER)
	{
		GLint in_position_loc = glGetAttribLocation(program, "aPos");
		GLint in_texcoord_loc = glGetAttribLocation(program, "aTex");
		gl_has_errors();
		assert(in_texcoord_loc >= 0);

		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
							  sizeof(TexturedVertex), (void *)0);
		gl_has_errors();

		glEnableVertexAttribArray(in_texcoord_loc);
		// remember to change this if tex0's type changes vec2/vec3
		glVertexAttribPointer(
			in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex),
			(void *)sizeof(
				vec3)); // note the stride to skip the preceeding vertex position

		// Enabling and binding texture to slot 0
		glActiveTexture(GL_TEXTURE0);
		gl_has_errors();

		assert(registry.renderRequests.has(entity));
		GLuint texture_id =
			texture_gl_handles[(GLuint)registry.renderRequests.get(entity).used_texture];

		// use 2d
		glBindTexture(GL_TEXTURE_2D, texture_id);
		gl_has_errors();

		// Get number of indices from index buffer, which has elements uint16_t
		GLint size = 0;
		glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
		gl_has_errors();

		GLsizei num_indices = size / sizeof(uint16_t);
		// GLsizei num_triangles = num_indices / 3;

		Player& player = registry.players.get(entity);
		model = player.model;

		GLint currProgram;
		glGetIntegerv(GL_CURRENT_PROGRAM, &currProgram);
		// Setting uniform values to the currently bound program
		GLuint model_loc = glGetUniformLocation(currProgram, "model");
		glUniformMatrix4fv(model_loc, 1, GL_FALSE, (float *)&model);
		GLuint view_loc = glGetUniformLocation(currProgram, "view");
		glUniformMatrix4fv(view_loc, 1, GL_FALSE, (float *)&view);
		GLuint projection_loc = glGetUniformLocation(currProgram, "proj");
		glUniformMatrix4fv(projection_loc, 1, GL_FALSE, (float *)&projection3D);
		gl_has_errors();
		// Drawing of num_indices/3 triangles specified in the index buffer
		glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_SHORT, nullptr);
		gl_has_errors();
	}
	else if (render_request.used_effect == EFFECT_ASSET_ID::TEXT)
	{
		GLint in_position_loc = glGetAttribLocation(program, "aPos");
		GLint in_texcoord_loc = glGetAttribLocation(program, "aTex");
		gl_has_errors();
		assert(in_texcoord_loc >= 0);

		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
			sizeof(TexturedVertex), (void*)0);
		gl_has_errors();

		glEnableVertexAttribArray(in_texcoord_loc);
		// remember to change this if tex0's type changes vec2/vec3
		glVertexAttribPointer(
			in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex),
			(void*)sizeof(
				vec3)); // note the stride to skip the preceeding vertex position

		// Enabling and binding texture to slot 0
		glActiveTexture(GL_TEXTURE0);
		gl_has_errors();

		assert(registry.renderRequests.has(entity));
		GLuint texture_id =
			texture_gl_handles[(GLuint)registry.renderRequests.get(entity).used_texture];

		// use 2d
		glBindTexture(GL_TEXTURE_2D, texture_id);
		gl_has_errors();

		Text& boxRotate = registry.text.get(entity);
		model = boxRotate.model;

		// Get number of indices from index buffer, which has elements uint16_t
		GLint size = 0;
		glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
		gl_has_errors();

		GLsizei num_indices = size / sizeof(uint16_t);
		// GLsizei num_triangles = num_indices / 3;

		GLint currProgram;
		glGetIntegerv(GL_CURRENT_PROGRAM, &currProgram);
		// Setting uniform values to the currently bound program
		GLuint model_loc = glGetUniformLocation(currProgram, "model");
		glUniformMatrix4fv(model_loc, 1, GL_FALSE, (float*)&model);
		GLuint view_loc = glGetUniformLocation(currProgram, "view");
		glUniformMatrix4fv(view_loc, 1, GL_FALSE, (float*)&view);
		GLuint projection_loc = glGetUniformLocation(currProgram, "proj");
		glUniformMatrix4fv(projection_loc, 1, GL_FALSE, (float*)&projection3D);
		gl_has_errors();
		// Drawing of num_indices/3 triangles specified in the index buffer
		glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_SHORT, nullptr);
		gl_has_errors();
	}
	else
	{
		assert(false && "Type of render request not supported");
	}
}

void RenderSystem::drawToScreen()
{
	glUseProgram(effects[(GLuint)EFFECT_ASSET_ID::FADE]);
	gl_has_errors();
	
	// Clearing backbuffer
	int w, h;
	glfwGetFramebufferSize(window, &w, &h); // Note, this will be 2x the resolution given to glfwCreateWindow on retina displays
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, w, h);
	glDepthRange(0, 10);
	glClearColor(1.f, 0, 0, 1.0);
	glClearDepth(1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	gl_has_errors();
	// Enabling alpha channel for textures
	glDisable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);

	// Draw the screen texture on the quad geometry
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers[(GLuint)GEOMETRY_BUFFER_ID::SCREEN_TRIANGLE]);
	glBindBuffer(
		GL_ELEMENT_ARRAY_BUFFER,
		index_buffers[(GLuint)GEOMETRY_BUFFER_ID::SCREEN_TRIANGLE]); // Note, GL_ELEMENT_ARRAY_BUFFER associates
																	 // indices to the bound GL_ARRAY_BUFFER
	gl_has_errors();

	const GLuint fade_program = effects[(GLuint)EFFECT_ASSET_ID::FADE];
	// Set clock
	GLuint time_uloc = glGetUniformLocation(fade_program, "time");
	GLuint dead_timer_uloc = glGetUniformLocation(fade_program, "darken_screen_factor");
	glUniform1f(time_uloc, (float)(glfwGetTime() * 10.0f));
	ScreenState& screen = registry.screenStates.get(screen_state_entity);
	glUniform1f(dead_timer_uloc, screen.darken_screen_factor);
	printf("%f", screen.darken_screen_factor);
	gl_has_errors();
	// Set the vertex position and vertex texture coordinates (both stored in the
	// same VBO)
	GLint in_position_loc = glGetAttribLocation(fade_program, "in_position");
	glEnableVertexAttribArray(in_position_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);
	gl_has_errors();

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_2D, off_screen_render_buffer_color);
	gl_has_errors();
	// Draw
	glDrawElements(
		GL_TRIANGLES, 3, GL_UNSIGNED_SHORT,
		nullptr); // one triangle = 3 vertices; nullptr indicates that there is
				  // no offset from the bound index buffer
	gl_has_errors();
}

// Render our game world
// http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-14-render-to-texture/
void RenderSystem::draw()
{
	// Getting size of window
	int w, h;
	glfwGetFramebufferSize(window, &w, &h); // Note, this will be 2x the resolution given to glfwCreateWindow on retina displays

	// First render to the custom framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0); // have to set this variable to 0 instead of frame_buffer in order for it to render
	gl_has_errors();
	// Clearing backbuffer
	glViewport(0, 0, w, h);
	glDepthRange(0.00001, 10);
	// glClearColor(0.674, 0.847, 1.0 , 1.0);
	glClearColor(0, 0, 0, 1.0);
	glClearDepth(10.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST); // native OpenGL does not work with a depth buffer
							  // and alpha blending, one would have to sort
							  // sprites back to front
	gl_has_errors();

	
	mat4 projection_3D = create3DProjectionMatrix(w, h);
	mat3 projection_2D = create2DProjectionMatrix();
	mat4 view = createViewMatrix();

	// Draw all textured meshes that have a position and size component
	for (Entity entity : registry.renderRequests.entities)
	{
		// if (!registry.motions.has(entity))
		// 	continue;
		// Note, its not very efficient to access elements indirectly via the entity
		// albeit iterating through all Sprites in sequence. A good point to optimize
		drawTexturedMesh(entity, projection_3D, projection_2D, view);
	}

	// Truely render to the screen
	// drawToScreen();

	// flicker-free display with a double buffer
	glfwSwapBuffers(window);
	gl_has_errors();
}

mat4 RenderSystem::createViewMatrix()
{
	// Not recommended to change as it is hard-coded to match with the 2d projection
	// See motion
    mat4 view = lookAt(vec3(6.0f, 3.0f, 6.0f),
                       vec3(0.0f, 0.0f, 0.0f),
                       vec3(0.0f, 1.0f, 0.0f));
    return view;
}

mat4 RenderSystem::create3DProjectionMatrix(int width, int height)
{
    mat4 proj = mat4(1.0f);

    float const aspect = (float)width / (float)height;
    float const view_distance = 3.5f; // this number should match the dimension of our box - 0.5;
    proj = ortho(-aspect * view_distance, aspect * view_distance, -view_distance, view_distance, -1000.f, 1000.f);
    return proj;
}

mat3 RenderSystem::create2DProjectionMatrix()
{
	// Fake projection matrix, scales with respect to window coordinates
	float left = 0.f;
	float top = 0.f;

	gl_has_errors();
	float right = (float) window_width_px;
	float bottom = (float) window_height_px;

	float sx = 2.f / (right - left);
	float sy = 2.f / (top - bottom);
	float tx = -(right + left) / (right - left);
	float ty = -(top + bottom) / (top - bottom);
	return {{sx, 0.f, 0.f}, {0.f, sy, 0.f}, {tx, ty, 1.f}};
}