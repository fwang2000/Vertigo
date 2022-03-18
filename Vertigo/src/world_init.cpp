#include "world_init.hpp"
#include "tiny_ecs_registry.hpp"

Entity createExplorer(RenderSystem* renderer, Coordinates pos, glm::mat4 translateMatrix) {
	auto entity = Entity();

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.interpolate = true;
	motion.origin = vec2{ pos.c, pos.r };
	motion.position = vec3(0, 0, 0);
	motion.destination = vec3(0, 0, 0);
	motion.velocity = { 0.f , 0.f , 0.f };
	motion.scale = vec2( EXPLORER_BB_WIDTH, EXPLORER_BB_HEIGHT );

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

Entity createTile(Tile* tile)
{
	Entity entity = Entity();

	TEXTURE_ASSET_ID id = TEXTURE_ASSET_ID::TILE;
	GEOMETRY_BUFFER_ID gid = GEOMETRY_BUFFER_ID::SPRITE;

	switch (tile->tileState) {
	case TileState::E:
		id = TEXTURE_ASSET_ID::EMPTY;
		break;
	case TileState::V:
		id = TEXTURE_ASSET_ID::TILE;
		break;
	case TileState::I:
		id = TEXTURE_ASSET_ID::INVISIBLE_TILE;
		break;
	case TileState::W:
		id = TEXTURE_ASSET_ID::SWITCH_TILE;
		break;
	case TileState::R:
		id = TEXTURE_ASSET_ID::RIGHT_TILE;
		break;
	case TileState::L:
		id = TEXTURE_ASSET_ID::LEFT_TILE;
		break;
	case TileState::D:
		id = TEXTURE_ASSET_ID::DOWN_TILE;
		break;
	case TileState::U:
		id = TEXTURE_ASSET_ID::UP_TILE;
		break;
	case TileState::C:
		id = TEXTURE_ASSET_ID::CONTROL_TILE;
		break;
	case TileState::M:
		id = TEXTURE_ASSET_ID::MOVE_TILE;
		break;
	case TileState::B:
		id = TEXTURE_ASSET_ID::BUSH_SHEET;
		registry.burnables.emplace(entity);
		gid = GEOMETRY_BUFFER_ID::ANIMATED;
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
		 gid });

	return entity;
}

Entity createText(Text text) {

	Entity entity = Entity();

	TEXTURE_ASSET_ID id = (TEXTURE_ASSET_ID)text.texture_id;

	registry.text.insert(entity, text);
	registry.renderRequests.insert(
		entity,
		{ id,
		 EFFECT_ASSET_ID::TEXT,
		 GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createFire(RenderSystem* renderer, Coordinates pos, glm::mat4 translateMatrix)
{
	auto entity = Entity();

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.interpolate = true;
	motion.origin = vec2{ pos.c, pos.r };
	motion.position = vec3(0, 0, 0);
	motion.destination = vec3(0, 0, 0);
	motion.velocity = { 0.f , 0.f , 0.f };
	motion.scale = vec2(FIRE_BB_WIDTH, FIRE_BB_HEIGHT);

	Fire& fire = registry.fire.emplace(entity);
	fire.firePos = pos;
	fire.model = rotate(glm::mat4(1.0f), (float)radians(90.0f), vec3(0.0f, 1.0f, 0.0f)) * fire.model;
	fire.model = rotate(glm::mat4(1.0f), (float)radians(90.0f), vec3(1.0f, 0.0f, 0.0f)) * fire.model;
	fire.model = scale(glm::mat4(1.0f), vec3(0.5f, 0.5f, 1.f)) * fire.model;
	fire.model = translateMatrix * fire.model;

	switch (pos.f) {
	case 0:
		fire.model = translate(glm::mat4(1.0f), vec3(0.f, 0.f, 0.5f)) * fire.model;
		break;
	case 1:
		fire.model = translate(glm::mat4(1.0f), vec3(-0.5f, 0.f, 0.f)) * fire.model;
		break;
	case 2:
		fire.model = translate(glm::mat4(1.0f), vec3(0.5f, 0.f, 0.f)) * fire.model;
		break;
	case 3:
		fire.model = translate(glm::mat4(1.0f), vec3(0.f, 0.5f, 0.f)) * fire.model;
		break;
	case 4:
		fire.model = translate(glm::mat4(1.0f), vec3(0.f, -0.5f, 0.f)) * fire.model;
		break;
	case 5:
		fire.model = translate(glm::mat4(1.0f), vec3(0.f, 0.f, -0.5f)) * fire.model;
		break;
	default:
		break;
	}

	registry.renderRequests.insert(
		entity,
		{
			TEXTURE_ASSET_ID::FIRE_SHEET,
			EFFECT_ASSET_ID::FIRE,
			GEOMETRY_BUFFER_ID::SPRITE
		}
	);

	return entity;
}

Entity createFireGauge(RenderSystem* renderer)
{
	auto entity = Entity();

	registry.holdTimers.emplace(entity);

	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	Motion& motion = registry.motions.emplace(entity);
	motion.scale = vec2({ 50, 0});
	motion.origin = vec2(100, 100);
	motion.x_vector = vec2(0, 1);

	registry.renderRequests.insert(
		entity,
		{
			TEXTURE_ASSET_ID::FIRE_GAUGE,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE
		}
	);
	
	return entity;
}

void createObject(RenderSystem* renderer, Coordinates pos, glm::mat4 translateMatrix) {
	 auto entity = Entity();

	 Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::COLUMN);
	 registry.meshPtrs.emplace(entity, &mesh);

	 // Setting initial motion values
	 Motion& motion = registry.motions.emplace(entity);
	 motion.interpolate = true;
	 motion.origin = vec2{ pos.c, pos.r };
	 motion.position = vec3(0, 0, 0);
	 motion.destination = vec3(0, 0, 0);
	 motion.velocity = { 0.f , 0.f , 0.f };
	 motion.scale = vec2(OBJECT_BB_WIDTH, OBJECT_BB_HEIGHT);

	 Object& object = registry.objects.emplace(entity);
	 object.objectPos = pos;
	 object.model = rotate(glm::mat4(1.0f), (float)radians(90.0f), vec3(0.0f, 1.0f, 0.0f)) * object.model;
	 object.model = rotate(glm::mat4(1.0f), (float)radians(90.0f), vec3(1.0f, 0.0f, 0.0f)) * object.model;
	 object.model = scale(glm::mat4(1.0f), vec3(0.5f, 0.5f, 1.f)) * object.model;
	 object.model = translateMatrix * object.model;

	 switch (pos.f) {
	 case 0:
		 object.model = translate(glm::mat4(1.0f), vec3(0.f, 0.f, 0.5f)) * object.model;
		 break;
	 case 1:
		 object.model = translate(glm::mat4(1.0f), vec3(-0.5f, 0.f, 0.f)) * object.model;
		 break;
	 case 2:
		 object.model = translate(glm::mat4(1.0f), vec3(0.5f, 0.f, 0.f)) * object.model;
		 break;
	 case 3:
		 object.model = translate(glm::mat4(1.0f), vec3(0.f, 0.5f, 0.f)) * object.model;
		 break;
	 case 4:
		 object.model = translate(glm::mat4(1.0f), vec3(0.f, -0.5f, 0.f)) * object.model;
		 break;
	 case 5:
		 object.model = translate(glm::mat4(1.0f), vec3(0.f, 0.f, -0.5f)) * object.model;
		 break;
	 default:
		 break;
	 }

	 registry.renderRequests.insert(
	 	entity,
	 	{
	 		TEXTURE_ASSET_ID::TEXTURE_COUNT,
	 		EFFECT_ASSET_ID::OBJECT,
	 		GEOMETRY_BUFFER_ID::COLUMN
	 	}
	 );
}