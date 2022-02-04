#include "world_init.hpp"
#include "tiny_ecs_registry.hpp"

Entity createBox(RenderSystem* renderer)
{
	auto entity = Entity();

	MeshBox& meshbox = renderer->getMeshBox(GEOMETRY_BUFFER_ID::CUBE);
	registry.meshPtrs.emplace(entity, &meshbox);
	registry.box.emplace(entity);

	return entity;
}

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
			TEXTURE_ASSET_ID::EXPLORER_DOWN,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE
		}
	);
	return entity;
}

void createTile(RenderSystem* renderer, vec2 pos, TileState state) {
	auto entity = Entity();

	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.velocity = { 0.f, 0.f };
	motion.scale = vec2({ TILE_BB_WIDTH, TILE_BB_HEIGHT });

	registry.tiles.emplace(entity);
    Tile * t1 =&registry.tiles.get(entity);
    t1->tileState = state;

    if(state != TileState::E){
        registry.renderRequests.insert(
                entity,
                {
                        TEXTURE_ASSET_ID::TILE,
                        EFFECT_ASSET_ID::TEXTURED,
                        GEOMETRY_BUFFER_ID::SPRITE
                }
        );
    }

}