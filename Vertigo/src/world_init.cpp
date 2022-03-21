#include "world_init.hpp"
#include "tiny_ecs_registry.hpp"

Entity createExplorer(RenderSystem* renderer, Coordinates pos, glm::mat4 translateMatrix) {
	auto entity = Entity();

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.interpolate = true;
	motion.position = vec3(0, 0, 0);
	motion.destination = vec3(0, 0, 0);
	motion.velocity = { 0.f , 0.f , 0.f };

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
	EFFECT_ASSET_ID eid = EFFECT_ASSET_ID::TILE;

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
	case TileState::U:
		id = TEXTURE_ASSET_ID::UP_TILE;
		break;
	case TileState::B:
		id = TEXTURE_ASSET_ID::BUSH_SHEET;
		registry.animated.emplace(entity);
		break;
	case TileState::O:
		id = TEXTURE_ASSET_ID::CONST_MOV_TILE;
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
		eid,
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

	createObject(entity, pos, translateMatrix, true);

	registry.fire.emplace(entity);

	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::FIRE);
	registry.meshPtrs.emplace(entity, &mesh);

	registry.renderRequests.insert(
		entity,
		{
			TEXTURE_ASSET_ID::FIRE,
			EFFECT_ASSET_ID::FIRE,
			GEOMETRY_BUFFER_ID::SPRITE
		}
	);

	return entity;
}

Entity createFireGauge(RenderSystem* renderer, Coordinates pos, glm::mat4 translateMatrix) {
	auto entity = Entity();

	createObject(entity, pos, translateMatrix, true);

	registry.holdTimers.emplace(entity);

	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::COLUMN);
	registry.meshPtrs.emplace(entity, &mesh);

	registry.renderRequests.insert(
		entity,
		{
			TEXTURE_ASSET_ID::TEXTURE_COUNT,
			EFFECT_ASSET_ID::OBJECT,
			GEOMETRY_BUFFER_ID::COLUMN
		}
	);

	return entity;
}

void createColumn(RenderSystem* renderer, Coordinates pos, glm::mat4 translateMatrix) {
	auto entity = Entity();

	createObject(entity, pos, translateMatrix, false);

	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::COLUMN);
	registry.meshPtrs.emplace(entity, &mesh);

	registry.renderRequests.insert(
		entity,
		{
			TEXTURE_ASSET_ID::TEXTURE_COUNT,
			EFFECT_ASSET_ID::OBJECT,
			GEOMETRY_BUFFER_ID::COLUMN
		}
	);
}

void createConstMovingTile(Entity entity, Coordinates pos, glm::mat4 translateMatrix) {
	
	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.interpolate = false;
	motion.position = vec3(0, 0, 0);
	motion.destination = vec3(0, 0, 0);
	motion.velocity = { 0.f , 0.f , 0.f };
	motion.scale = {1.0f, 1.0f, 1.0f};
	
	Oscillate& oscillate = registry.oscillations.emplace(entity);
}

void createObject(Entity entity, Coordinates pos, glm::mat4 translateMatrix, bool hasMotion){
	if (hasMotion){
		// Setting initial motion values
		Motion& motion = registry.motions.emplace(entity);
		motion.interpolate = false;
		motion.position = vec3(0, 0, 0);
		motion.destination = vec3(0, 0, 0);
		motion.velocity = { 0.f , 0.f , 0.f };
		motion.scale = {1.0f, 1.0f, 1.0f};
	}

	Object& object = registry.objects.emplace(entity);
	object.objectPos = pos;
	object.model = rotate(glm::mat4(1.0f), (float)radians(90.0f), vec3(0.0f, 1.0f, 0.0f)) * object.model;
	object.model = rotate(glm::mat4(1.0f), (float)radians(90.0f), vec3(1.0f, 0.0f, 0.0f)) * object.model;
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
}

Entity createMenu(RenderSystem* renderer) {
	Entity entity = Entity();
	Menu& menu = registry.menus.emplace(entity);
	Object& object = registry.objects.emplace(entity);
	
	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::ON_LEVELS,
		 EFFECT_ASSET_ID::MENU,
		 GEOMETRY_BUFFER_ID::SPRITE 
		}
	);

	return entity;
}