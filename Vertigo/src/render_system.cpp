// internal
#include "render_system.hpp"
#include <SDL.h>

#include "tiny_ecs_registry.hpp"
#include "world_system.hpp"
#include <SDL_opengl.h>

void RenderSystem::setLighting(GLint currProgram)
{
	// light properties
	glUniform3fv(glGetUniformLocation(currProgram, "dirLight.position"), 1, (float *)&viewPos);
	glUniform3f(glGetUniformLocation(currProgram, "dirLight.ambient"), 0.3f, 0.3f, 0.3f);
	glUniform3f(glGetUniformLocation(currProgram, "dirLight.diffuse"), 0.8f, 0.8f, 0.8f);
	glUniform3f(glGetUniformLocation(currProgram, "dirLight.specular"), 0.5f, 0.5f, 0.5f);

	// material properties
	glUniform1i(glGetUniformLocation(currProgram, "material.diffuse"), 0);
	glUniform3f(glGetUniformLocation(currProgram, "material.specular"), 0.5f, 0.5f, 0.5f);
	glUniform1f(glGetUniformLocation(currProgram, "material.shininess"), 30.f);

	std::string pointLightStr = "pointLights[0]";
	for (unsigned int i = 0; i < registry.lightSources.entities.size(); i++) {
		Entity light = registry.lightSources.entities[i];
		pointLightStr[12] = '0' + i;
		if (registry.fire.has(light)) {
			Motion& motion = registry.motions.get(light);
			Object& object = registry.objects.get(light);
			mat4 model = translate(mat4(1.f), motion.position) * object.model * scale(mat4(1.f), motion.scale);
			glUniform3f(glGetUniformLocation(currProgram, (pointLightStr + ".position").c_str()), model[3].x, model[3].y, model[3].z);
		} else {
			Billboard& billboard = registry.billboards.get(light);
			glUniform3f(glGetUniformLocation(currProgram, (pointLightStr + ".position").c_str()), billboard.model[3].x, billboard.model[3].y, billboard.model[3].z);
		}
		glUniform3f(glGetUniformLocation(currProgram, (pointLightStr + ".ambient").c_str()), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(currProgram, (pointLightStr + ".diffuse").c_str()), 0.8f, 0.8f, 0.8f);
		glUniform3f(glGetUniformLocation(currProgram, (pointLightStr + ".specular").c_str()), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(currProgram, (pointLightStr + ".constant").c_str()), 1.f);
		glUniform1f(glGetUniformLocation(currProgram, (pointLightStr + ".linear").c_str()), 0.09f);
		glUniform1f(glGetUniformLocation(currProgram, (pointLightStr + ".quadratic").c_str()), 0.032f);
	}
}

void RenderSystem::drawTexturedMesh(Entity entity, const mat4& projection3D, const mat4& view)
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
	GLint currProgram;
	glGetIntegerv(GL_CURRENT_PROGRAM, &currProgram);
	// Input data location as in the vertex buffer
	if (render_request.used_effect == EFFECT_ASSET_ID::TILE)
	{
		GLint in_position_loc = glGetAttribLocation(program, "aPos");
		GLint in_texcoord_loc = glGetAttribLocation(program, "aTex");
		GLint in_normal_loc = glGetAttribLocation(program, "aNormal");
		gl_has_errors();
		assert(in_texcoord_loc >= 0);

		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
							  sizeof(LightedVertex), (void *)0);
		gl_has_errors();

		glEnableVertexAttribArray(in_texcoord_loc);

		glVertexAttribPointer(
			in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(LightedVertex),
			(void *)sizeof(
				vec3)); // note the stride to skip the preceeding vertex position
		gl_has_errors();

		glEnableVertexAttribArray(in_normal_loc);
		glVertexAttribPointer(
			in_normal_loc, 3, GL_FLOAT, GL_FALSE, sizeof(LightedVertex),
			(void *)(5 * sizeof(float))); // note the stride to skip the preceeding vertex position
		gl_has_errors();

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
		mat4 trans = mat4(1.f);
		mat4 sca = mat4(1.f);
		if (registry.motions.has(entity)){
			Motion& motion = registry.motions.get(entity);
			trans = translate(mat4(1.f), motion.position);
			sca = scale(mat4(1.0f), motion.scale);
		}
		if (boxRotate->popup) {
			switch (boxRotate->direction) {
				case FACE_DIRECTION::FRONT:
					trans = translate(mat4(1.f), vec3(0.f, 0.f, popup_height)) * trans;
					break;
				case FACE_DIRECTION::LEFT:
					trans = translate(mat4(1.f), vec3(-popup_height, 0.f, 0.f)) * trans;
					break;
				case FACE_DIRECTION::RIGHT:
					trans = translate(mat4(1.f), vec3(popup_height, 0.f, 0.f)) * trans;
					break;
				case FACE_DIRECTION::TOP:
					trans = translate(mat4(1.f), vec3(0.f, popup_height, 0.f)) * trans;
					break;
				case FACE_DIRECTION::BOTTOM:
					trans = translate(mat4(1.f), vec3(0.f, -popup_height, 0.f)) * trans;
					break;
				case FACE_DIRECTION::BACK:
					trans = translate(mat4(1.f), vec3(0.f, 0.f, -popup_height)) * trans;
					break;
			}
		}
		model = model * sca;
		model = trans * model;
		TrackBallInfo& trackball = registry.trackBall.components[0];
		mat4 mouseRotation = toMat4(trackball.rotation);
		model = mouseRotation * model;

		// Setting uniform values to the currently bound program
		setLighting(currProgram);
		glUniform1i(glGetUniformLocation(currProgram, "numLights"), (int)registry.lightSources.entities.size());
		glUniform1i(glGetUniformLocation(currProgram, "highlighted"), boxRotate->highlighted);
		if (boxRotate->color != -1)
			glUniform3fv(glGetUniformLocation(currProgram, "color"), 1, (float *)&controlTileColors[boxRotate->color]);
		else
			glUniform3f(glGetUniformLocation(currProgram, "color"), 0.f, 0.f, 0.f);
		GLuint viewPos_loc = glGetUniformLocation(currProgram, "viewPos");
		glUniform3fv(viewPos_loc, 1, (float *)&viewPos);
		gl_has_errors();
		
		if (registry.animated.has(entity)){
			Animated& animated = registry.animated.get(entity);
			GLuint animated_loc = glGetUniformLocation(currProgram, "animated");
			glUniform1i(animated_loc, true);
			GLuint length_loc = glGetUniformLocation(currProgram, "sheet_length");
			glUniform1i(length_loc, animated.num_intervals);
			GLuint index_loc = glGetUniformLocation(currProgram, "index");
			glUniform1i(index_loc, floor(animated.counter_ms * animated.num_intervals / animated.max_ms));
		}
		else{
			GLuint animated_loc = glGetUniformLocation(currProgram, "animated");
			glUniform1i(animated_loc, false);
		}
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

		Player& player = registry.players.get(entity);
		model = player.model;

		Motion& motion = registry.motions.get(entity);
		model = translate(mat4(1.f), motion.position) * model;

		TrackBallInfo& trackball = registry.trackBall.components[0];
		mat4 mouseRotation = toMat4(trackball.rotation);
		model = mouseRotation * model;
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
		glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE,
			sizeof(TexturedVertex), (void*)sizeof(vec3));
		gl_has_errors();

		assert(registry.renderRequests.has(entity));
		GLuint texture_id =
			texture_gl_handles[(GLuint)registry.renderRequests.get(entity).used_texture];

		// use 2d
		glBindTexture(GL_TEXTURE_2D, texture_id);
		gl_has_errors();

		Text& boxRotate = registry.text.get(entity);
		model = boxRotate.model;

		TrackBallInfo& trackball = registry.trackBall.components[0];
		mat4 mouseRotation = toMat4(trackball.rotation);
		model = mouseRotation * model;
	}
	else if (render_request.used_effect == EFFECT_ASSET_ID::BILLBOARD)
	{
		GLint in_position_loc = glGetAttribLocation(program, "in_position");
		GLint in_color_loc = glGetAttribLocation(program, "in_color");
		gl_has_errors();
		assert(in_color_loc >= 0);

		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
			sizeof(ColoredVertex), (void*)0);
		gl_has_errors();

		glEnableVertexAttribArray(in_color_loc);
		// remember to change this if tex0's type changes vec2/vec3
		glVertexAttribPointer(
			in_color_loc, 3, GL_FLOAT, GL_FALSE, sizeof(ColoredVertex),
			(void*)sizeof(
				vec3)); // note the stride to skip the preceeding vertex position

		Billboard& obj = registry.billboards.get(entity);
		model = obj.model;

		TrackBallInfo& trackball = registry.trackBall.components[0];
		mat4 mouseRotation = toMat4(trackball.rotation);
		model = mouseRotation * model;

		// set the upper 3x3 matrix to identity matrix OR the transpose of the view matrix
		model[0][0] = view[0][0];
		model[0][1] = view[1][0];
		model[0][2] = view[2][0];

		model[1][0] = view[0][1];
		model[1][1] = view[1][1];
		model[1][2] = view[2][1];

		model[2][0] = view[0][2];
		model[2][1] = view[1][2];
		model[2][2] = view[2][2];

		// Setting uniform values to the currently bound program
		GLuint scale_loc = glGetUniformLocation(currProgram, "scale");
		glUniform1f(scale_loc, 0.5f);
		gl_has_errors();
	}
	else
	{
		assert(false && "Type of render request not supported");
	}

	// Get number of indices from index buffer, which has elements uint16_t
	GLint size = 0;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	gl_has_errors();

	GLsizei num_indices = size / sizeof(uint16_t);

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

void RenderSystem::drawFire(Entity entity, const mat4& projection3D, const mat4& view) {

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
	GLint in_position_loc = glGetAttribLocation(program, "aPos");
	GLint in_texcoord_loc = glGetAttribLocation(program, "aTex");
	gl_has_errors();
	assert(in_texcoord_loc >= 0);

	glEnableVertexAttribArray(in_position_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
		sizeof(TexturedVertex), (void*)0);
	gl_has_errors();

	glEnableVertexAttribArray(in_texcoord_loc);
	glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE,
		sizeof(TexturedVertex), (void*)sizeof(vec3));
	gl_has_errors();

	// Get number of indices from index buffer, which has elements uint16_t
	GLint size = 0;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	gl_has_errors();

	GLsizei num_indices = size / sizeof(uint16_t);

	Motion& motion = registry.motions.get(entity);

	Object& object = registry.objects.get(entity);
	model = object.model;
	model = scale(model, motion.scale);
	model = translate(mat4(1.f), motion.position) * model;

	TrackBallInfo& trackball = registry.trackBall.components[0];
	mat4 mouseRotation = toMat4(trackball.rotation);
	model = mouseRotation * model;

	Fire& fire = registry.fire.get(entity);
	int index = (int)floor(fire.index);

	fire.index += 1;
	if (fire.index >= fire.maxIndex - 1) { fire.index = 0; }

	GLint currProgram;
	glGetIntegerv(GL_CURRENT_PROGRAM, &currProgram);
	// Setting uniform values to the currently bound program
	GLuint index_loc = glGetUniformLocation(currProgram, "index");
	glUniform1i(index_loc, index);
	GLuint model_loc = glGetUniformLocation(currProgram, "model");
	glUniformMatrix4fv(model_loc, 1, GL_FALSE, (float*)&model);
	GLuint view_loc = glGetUniformLocation(currProgram, "view");
	glUniformMatrix4fv(view_loc, 1, GL_FALSE, (float*)&view);
	GLuint projection_loc = glGetUniformLocation(currProgram, "proj");
	glUniformMatrix4fv(projection_loc, 1, GL_FALSE, (float*)&projection3D);
	GLuint mainTexture = glGetUniformLocation(currProgram, "tex0");
	glUniform1i(mainTexture, 0);
	gl_has_errors();

	GLuint texture_id =
		texture_gl_handles[(GLuint)registry.renderRequests.get(entity).used_texture];

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);

	// use 2d
	glBindTexture(GL_TEXTURE_2D, texture_id);
	gl_has_errors();

	// Drawing of num_indices/3 triangles specified in the index buffer
	glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_SHORT, nullptr);
	gl_has_errors();
}

void RenderSystem::drawObject(Entity entity, const mat4& projection3D, const mat4& view) 
{
	if (registry.fire.has(entity) || registry.lightSources.has(entity)) {
		return;
	}

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
	GLint in_position_loc = glGetAttribLocation(program, "in_position");
	GLint in_color_loc = glGetAttribLocation(program, "in_color");
	GLint in_normal_loc = glGetAttribLocation(program, "in_normal");
	gl_has_errors();

	glEnableVertexAttribArray(in_position_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
		sizeof(ColoredVertex), (void*)0);
	gl_has_errors();

	glEnableVertexAttribArray(in_color_loc);
	glVertexAttribPointer(in_color_loc, 3, GL_FLOAT, GL_FALSE,
		sizeof(ColoredVertex), (void*)sizeof(vec3));
	gl_has_errors();

	glEnableVertexAttribArray(in_normal_loc);
	glVertexAttribPointer(in_normal_loc, 3, GL_FLOAT, GL_FALSE,
		sizeof(ColoredVertex), (void*)(6*sizeof(float)));
	gl_has_errors();

	// Get number of indices from index buffer, which has elements uint16_t
	GLint size = 0;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	gl_has_errors();

	GLsizei num_indices = size / sizeof(uint16_t);
	// GLsizei num_triangles = num_indices / 3;

	Object& object = registry.objects.get(entity);
	model = object.model;

	TrackBallInfo& trackball = registry.trackBall.components[0];
	mat4 mouseRotation = toMat4(trackball.rotation);
	model = mouseRotation * model;

	mat4 trans = mat4(1.f);
	mat4 sca = mat4(1.f);
	if (registry.motions.has(entity)) {
		Motion& motion = registry.motions.get(entity);
		trans = translate(mat4(1.f), motion.position);
		sca = scale(mat4(1.0f), motion.scale);
	}

	if (object.alpha > 0.0f && object.burning) {
		object.alpha -= 0.005f;
		if (object.alpha <= 0.0f) {
			object.alpha = 0;
			object.burning = false;
		}
	}

	GLint currProgram;
	glGetIntegerv(GL_CURRENT_PROGRAM, &currProgram);
	// Setting uniform values to the currently bound program

	setLighting(currProgram);
	glUniform1i(glGetUniformLocation(currProgram, "numLights"), (int)registry.lightSources.entities.size());

	GLuint alpha_loc = glGetUniformLocation(currProgram, "alpha");
	glUniform1f(alpha_loc, object.alpha);
	GLuint model_loc = glGetUniformLocation(currProgram, "model");
	glUniformMatrix4fv(model_loc, 1, GL_FALSE, (float*)&model);
	GLuint translate_loc = glGetUniformLocation(currProgram, "translate");
	glUniformMatrix4fv(translate_loc, 1, GL_FALSE, (float*)&trans);
	GLuint scale_loc = glGetUniformLocation(currProgram, "scale");
	glUniformMatrix4fv(scale_loc, 1, GL_FALSE, (float*)&sca);
	GLuint view_loc = glGetUniformLocation(currProgram, "view");
	glUniformMatrix4fv(view_loc, 1, GL_FALSE, (float*)&view);
	GLuint projection_loc = glGetUniformLocation(currProgram, "proj");
	glUniformMatrix4fv(projection_loc, 1, GL_FALSE, (float*)&projection3D);
	GLuint mainTexture = glGetUniformLocation(currProgram, "mainTexture");
	glUniform1i(mainTexture, 0);
	GLuint objColor = glGetUniformLocation(currProgram, "objColor");
	glUniform3fv(objColor, 1, (float*)&object.color);
	GLuint viewPos_loc = glGetUniformLocation(currProgram, "viewPos");
	glUniform3fv(viewPos_loc, 1, (float *)&viewPos);
	gl_has_errors();

	/*GLuint texture_id =
		texture_gl_handles[(GLuint)registry.renderRequests.get(entity).used_texture];*/

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);

	// use 2d
	glBindTexture(GL_TEXTURE_2D, (GLuint)TEXTURE_ASSET_ID::WOOD);
	gl_has_errors();

	// Drawing of num_indices/3 triangles specified in the index buffer
	glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_SHORT, nullptr);
	gl_has_errors();
}

void RenderSystem::drawMenu(Entity entity, const mat3 &projection)
{
	assert(registry.renderRequests.has(entity));
	const RenderRequest& render_request = registry.renderRequests.get(entity);

	assert(registry.menus.has(entity));
	const Menu& menu = registry.menus.get(entity);

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

	RenderRequest& r = registry.renderRequests.get(entity);
	GLuint texture_id;

	if (menu.auto_texture_id){
		texture_id = (GLuint)r.used_texture;
	}
	else{
		texture_id = texture_gl_handles[(GLuint)r.used_texture];
	}

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

	Transform transform;

	if (registry.motions.has(entity)) {
		Motion& motion = registry.motions.get(entity);
		transform.translate(vec2(motion.position.x, motion.position.y));
		transform.scale(vec2(motion.scale.x, motion.scale.y));
	}

	GLuint transform_loc = glGetUniformLocation(program, "transform");
	glUniformMatrix3fv(transform_loc, 1, GL_FALSE, (float*)&transform.mat);
	gl_has_errors();

	// Drawing of num_indices/3 triangles specified in the index buffer
	glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_SHORT, nullptr);
	gl_has_errors();
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
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer); // have to set this variable to 0 instead of frame_buffer in order for it to render
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
	mat4 view = createViewMatrix();
	mat3 projection = createProjectionMatrix();

	if (registry.menuButtons.entities.size() == 0){
		// Draw all textured meshes that have a position and size component
		for (Entity entity : registry.renderRequests.entities)
		{
			RenderRequest& request = registry.renderRequests.get(entity);

			if (request.used_effect == EFFECT_ASSET_ID::OBJECT) {
				continue;
			}

			if (request.used_effect == EFFECT_ASSET_ID::FIRE) {
				continue;
			}

			if (request.used_effect == EFFECT_ASSET_ID::MENU) {
				continue;
			}
			// Note, its not very efficient to access elements indirectly via the entity
			// albeit iterating through all Sprites in sequence. A good point to optimize
			drawTexturedMesh(entity, projection_3D, view);
		}

		for (Entity entity : registry.objects.entities) {

			RenderRequest& request = registry.renderRequests.get(entity);

			if (request.used_effect == EFFECT_ASSET_ID::OBJECT) {
				drawObject(entity, projection_3D, view);
			}
		}

		if (registry.fire.entities.size() != 0) {
			drawFire(registry.fire.entities.at(0), projection_3D, view);
		}
		for (Entity entity : registry.menus.entities)
		{
			drawMenu(entity, projection);
		}
	}
	else{
		for (Entity entity : registry.menuButtons.entities)
		{
			drawTexturedMesh(entity, create3DProjectionMatrixPerspective(w, h), lookAt(vec3(0.0f, 0.0f, 8.0f),
																				vec3(0.0f, 0.0f, 0.0f),
																				vec3(0.0f, 1.0f, 0.0f)));
		}
		for (Entity entity : registry.menus.entities)
		{
			drawMenu(entity, projection);
		}
	}

	// Truely render to the screen
	drawToScreen();

	// flicker-free display with a double buffer
	glfwSwapBuffers(window);
	gl_has_errors();
}

mat4 RenderSystem::createViewMatrix()
{
    mat4 view = lookAt(vec3(6.0f, 3.0f, 6.0f),
                       vec3(0.0f, 0.0f, 0.0f),
                       vec3(0.0f, 1.0f, 0.0f));
    return view;
}

mat3 RenderSystem::createProjectionMatrix()
{
	// Fake projection matrix, scales with respect to window coordinates
	float left = 0.f;
	float top = 0.f;

	gl_has_errors();
	float right = (float)window_width_px;
	float bottom = (float)window_height_px;

	float sx = 2.f / (right - left);
	float sy = 2.f / (top - bottom);
	float tx = -(right + left) / (right - left);
	float ty = -(top + bottom) / (top - bottom);
	return { {sx, 0.f, 0.f}, {0.f, sy, 0.f}, {tx, ty, 1.f} };
}

mat4 RenderSystem::create3DProjectionMatrix(int width, int height)
{
    mat4 proj = mat4(1.0f);

    float const aspect = (float)width / (float)height;
    float const view_distance = screen_cube.size + 0.5f; // this number should match the dimension of our box - 0.5;
    proj = ortho(-aspect * view_distance, aspect * view_distance, -view_distance, view_distance, -1000.f, 1000.f);
    return proj;
}

mat4 RenderSystem::create3DProjectionMatrixPerspective(int width, int height)
{
    mat4 proj = mat4(1.0f);

    float const aspect = (float)width / (float)height;
    // float const view_distance = screen_cube.size + 0.5; // this number should match the dimension of our box - 0.5;
    proj = perspective(20.f, aspect, 1.f, 20.f);
    return proj;
}

void RenderSystem::setCube(Cube cube) {
	screen_cube = cube;
	viewPos = vec3(screen_cube.size + 0.5f);
}