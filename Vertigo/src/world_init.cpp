#include "world_init.hpp"
#include "tiny_ecs_registry.hpp"

Entity createExplorer(RenderSystem* renderer, Coordinates pos, glm::mat4 translateMatrix) {
	auto entity = Entity();

	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.velocity = { 0.f, 0.f };
	motion.scale = vec2({ EXPLORER_BB_WIDTH, EXPLORER_BB_HEIGHT });

	Player& explorer = registry.players.emplace(entity);
	explorer.playerPos = pos;
	explorer.model = rotate(glm::mat4(1.0f), (float)radians(90.0f), vec3(0.0f, 1.0f, 0.0f)) * explorer.model;
	explorer.model = rotate(glm::mat4(1.0f), (float)radians(-90.0f), vec3(1.0f, 0.0f, 0.0f)) * explorer.model;
	explorer.model = translateMatrix * explorer.model;
	explorer.model = translate(glm::mat4(1.0f), vec3(0.f, 0.f, 0.5f)) * explorer.model;

	registry.renderRequests.insert(
		entity,
		{
			TEXTURE_ASSET_ID::EXPLORER_DOWN,
			EFFECT_ASSET_ID::PLAYER,
			GEOMETRY_BUFFER_ID::SPRITE
		}
	);
	return entity;
}

Entity createTile(Tile tile)
{
	Entity entity = Entity();

	TEXTURE_ASSET_ID id = TEXTURE_ASSET_ID::TILE;

	switch (tile.tileState) {
	case TileState::V:
		id = TEXTURE_ASSET_ID::TILE;
		break;
	case TileState::I:
		id = TEXTURE_ASSET_ID::INVISIBLE_TILE;
		break;
	case TileState::W:
		id = TEXTURE_ASSET_ID::SWITCH_TILE;
		break;
	case TileState::U:
		id = TEXTURE_ASSET_ID::UP_TILE;
		break;
	case TileState::Z:
		id = TEXTURE_ASSET_ID::END_TILE;
		break;
	default:
		id = TEXTURE_ASSET_ID::TILE;
	}

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	registry.tiles.insert(entity, tile);
	registry.renderRequests.insert(
		entity,
		{ id,
		 EFFECT_ASSET_ID::TILE,
		 GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createFire(RenderSystem* renderer, vec3 pos)
{
	auto entity = Entity();

	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);
	int distance = window_height_px / 3;

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = vec2(
		window_width_px / 2 + pos.x * distance,
		window_height_px / 2 + pos.y * distance
	);
	motion.velocity = { 0.f, 0.f };
	motion.scale = vec2({ FIRE_BB_WIDTH, FIRE_BB_HEIGHT });

	Oscillate& oscillate = registry.oscillations.emplace(entity);

	Fire& fire = registry.fire.emplace(entity);
	fire.firePos = vec3(pos.x + 1, pos.y + 1, pos.z + 1) ;

	// registry.renderRequests.insert(
	// 	entity,
	// 	{
	// 		TEXTURE_ASSET_ID::FIRE,
	// 		EFFECT_ASSET_ID::TEXTURED,
	// 		GEOMETRY_BUFFER_ID::SPRITE
	// 	}
	// );
	return entity;
}

void createObject(RenderSystem* renderer, vec3 pos) {
	// auto entity = Entity();

	// Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	// registry.meshPtrs.emplace(entity, &mesh);

	// // Setting initial motion values
	// int distance = window_height_px / 3;

	// Motion& motion = registry.motions.emplace(entity);
	// motion.position = vec2(
	// 	window_width_px / 2 + pos.coordinates.x * distance,
	// 	window_height_px / 2 + pos.coordinates.y * distance
	// );
	// motion.velocity = { 0.f, 0.f };
	// motion.scale = vec2({ OBJECT_BB_WIDTH, OBJECT_BB_HEIGHT });

	// registry.objects.emplace(entity);

	// registry.renderRequests.insert(
	// 	entity,
	// 	{
	// 		TEXTURE_ASSET_ID::OBJECT,
	// 		EFFECT_ASSET_ID::TEXTURED,
	// 		GEOMETRY_BUFFER_ID::SPRITE
	// 	}
	// );
}