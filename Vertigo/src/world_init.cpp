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

	// Add oscillation
	Oscillate& oscillate = registry.oscillations.emplace(entity);

	Player& explorer = registry.players.emplace(entity);
	explorer.playerPos.coordinates = vec2(1, 1);

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

void createTile(RenderSystem* renderer, TilePosition pos, TileState state) {
	auto entity = Entity();

	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	int distance = window_height_px / 3;

	Motion& motion = registry.motions.emplace(entity);
	motion.position = vec2(
		window_width_px / 2 + pos.coordinates.x * distance,
		window_height_px / 2 + pos.coordinates.y * distance
	);
	motion.velocity = { 0.f, 0.f };
	motion.scale = vec2({ TILE_BB_WIDTH, TILE_BB_HEIGHT });

	Tile& tile = registry.tiles.emplace(entity);
	tile.tileState = state;
	tile.tilePos = TilePosition{ vec2(pos.coordinates.x + 1, pos.coordinates.y + 1) };

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

void createTileShadow(RenderSystem* renderer, TilePosition pos) {
	auto entity = Entity();

	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	int distance = window_height_px / 3;

	Motion& motion = registry.motions.emplace(entity);
	motion.position = vec2(
		window_width_px / 2 + pos.coordinates.x * distance,
		window_height_px / 2 + pos.coordinates.y * distance
	);
	motion.velocity = { 0.f, 0.f };
	motion.scale = vec2({ TILE_BB_WIDTH * 1.1, TILE_BB_HEIGHT * 1.1 });

	Parallax& parallax = registry.parallax.emplace(entity);
	parallax.position = motion.position;

	registry.renderRequests.insert(
		entity,
		{
			TEXTURE_ASSET_ID::TILE_SHADOW,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE
		}
	);
}

Entity createFire(RenderSystem* renderer, TilePosition pos)
{
	auto entity = Entity();

	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);
	int distance = window_height_px / 3;

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = vec2(
		window_width_px / 2 + pos.coordinates.x * distance,
		window_height_px / 2 + pos.coordinates.y * distance
	);
	motion.velocity = { 0.f, 0.f };
	motion.scale = vec2({ FIRE_BB_WIDTH, FIRE_BB_HEIGHT });

	Oscillate& oscillate = registry.oscillations.emplace(entity);

	Fire& fire = registry.fire.emplace(entity);
	fire.firePos = TilePosition{ vec2(pos.coordinates.x + 1, pos.coordinates.y + 1) };

	registry.renderRequests.insert(
		entity,
		{
			TEXTURE_ASSET_ID::FIRE,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE
		}
	);
	return entity;
}

void createObject(RenderSystem* renderer, TilePosition pos) {
	auto entity = Entity();

	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	int distance = window_height_px / 3;

	Motion& motion = registry.motions.emplace(entity);
	motion.position = vec2(
		window_width_px / 2 + pos.coordinates.x * distance,
		window_height_px / 2 + pos.coordinates.y * distance
	);
	motion.velocity = { 0.f, 0.f };
	motion.scale = vec2({ OBJECT_BB_WIDTH, OBJECT_BB_HEIGHT });

	registry.objects.emplace(entity);

	registry.renderRequests.insert(
		entity,
		{
			TEXTURE_ASSET_ID::OBJECT,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE
		}
	);
}