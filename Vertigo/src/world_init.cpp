#include "world_init.hpp"
#include "tiny_ecs_registry.hpp"

Entity createCube(RenderSystem* renderer) {

	auto entity = Entity();

	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	registry.cube.emplace(entity);

	return entity;
}

Entity createExplorer(RenderSystem* renderer, vec2 pos) {
	auto entity = Entity();

	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.velocity = { 0.f, 0.f };
	motion.scale = vec2({ EXPLORER_BB_WIDTH, EXPLORER_BB_HEIGHT });


	registry.players.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{
			TEXTURE_ASSET_ID::EXPLORER,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE
		}
	);
	return entity;
}