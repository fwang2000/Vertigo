#include "world_init.hpp"
#include "tiny_ecs_registry.hpp"

Entity createExplorer(RenderSystem* renderer, Coordinates pos, glm::mat4 translateMatrix) {
	auto entity = Entity();

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.interpolate = true;
	motion.position = vec3(0, 0, 0.5f);
	motion.destination = vec3(0, 0, 0.5f);
	motion.velocity = { 0.f , 0.f , 0.f };

	Player& explorer = registry.players.emplace(entity);
	explorer.playerPos = pos;
	explorer.model = rotate(glm::mat4(1.0f), (float)radians(90.0f), vec3(0.0f, 1.0f, 0.0f)) * explorer.model;
	explorer.model = rotate(glm::mat4(1.0f), (float)radians(-90.0f), vec3(1.0f, 0.0f, 0.0f)) * explorer.model;
	explorer.model = translateMatrix * explorer.model;
	explorer.model = translate(glm::mat4(1.0f), vec3(0.f, 0.f, 0.f)) * explorer.model;

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
	GEOMETRY_BUFFER_ID gid = GEOMETRY_BUFFER_ID::LIGHTING;
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
		id = TEXTURE_ASSET_ID::TILE;
		break;
	case TileState::O:
		id = TEXTURE_ASSET_ID::CONST_MOV_TILE;
		break;
	case TileState::H:
		id = TEXTURE_ASSET_ID::BURN_TARGET_TILE;
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

void createRestartText(RenderSystem* renderer, vec2 position) {
	
	auto entity = Entity();

	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	Menu& menu = registry.menus.emplace(entity);

	// Initialize the motion
	Motion& motion = registry.motions.emplace(entity);
	motion.position = vec3(0.8, 0.8, 1);
	motion.scale = vec3(0.4, -0.4, 1);

	registry.renderRequests.insert(
		entity,
		{
			TEXTURE_ASSET_ID::RESTART_TEXT,
			EFFECT_ASSET_ID::MENU,
			GEOMETRY_BUFFER_ID::SPRITE
		}
	);
}

Entity createFire(RenderSystem* renderer, Coordinates pos, glm::mat4 translateMatrix)
{
	auto entity = Entity();

	createObject(entity, pos, translateMatrix, true, vec3(1), -1);

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

	createObject(entity, pos, translateMatrix, true, vec3(1.5, 0.5, 0.5), 1);

	Object& gauge = registry.objects.get(entity);
	gauge.alpha = 1.0;

	registry.holdTimers.emplace(entity);

	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::GAUGE);
  
	registry.meshPtrs.emplace(entity, &mesh);

	registry.renderRequests.insert(
		entity,
		{
			TEXTURE_ASSET_ID::TEXTURE_COUNT,
			EFFECT_ASSET_ID::OBJECT,
			GEOMETRY_BUFFER_ID::GAUGE
		}
	);

	return entity;
}

void createColumn(RenderSystem* renderer, Coordinates pos, glm::mat4 translateMatrix) {
	auto entity = Entity();

	createObject(entity, pos, translateMatrix, false, vec3(0.5f, 0.5f, 1.f), 1);

	Object& column = registry.objects.get(entity);
	column.alpha = 1.0;

	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::COLUMN);
	registry.meshPtrs.emplace(entity, &mesh);

	registry.renderRequests.insert(
		entity,
		{
			TEXTURE_ASSET_ID::MARBLE,
			EFFECT_ASSET_ID::OBJECT,
			GEOMETRY_BUFFER_ID::COLUMN
		}
	);
}

void createDevice(RenderSystem* renderer, Coordinates pos, glm::mat4 translateMatrix) {
	auto entity = Entity();

	createObject(entity, pos, translateMatrix, true, vec3(0.3f), 1);

	Object& device = registry.objects.get(entity);
	device.alpha = 0.5;
	 
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::DEVICE);
	registry.meshPtrs.emplace(entity, &mesh);

	registry.renderRequests.insert(
		entity,
		{
			TEXTURE_ASSET_ID::MARBLE,
			EFFECT_ASSET_ID::OBJECT,
			GEOMETRY_BUFFER_ID::DEVICE
		}
	);

	Oscillate& o = registry.oscillations.emplace(entity);

	float h = 0.1;

	if (pos.f == 0) {
		o.center = vec3({ 0, 0, h });
	}
	else if (pos.f == 1) {
		o.center = vec3({ -h, 0, 0 });
	}
	else if (pos.f == 2) {
		o.center = vec3({ h, 0, 0 });
	}
	else if (pos.f == 3) {
		o.center = vec3({ 0, h, 0 });
	}
	else if (pos.f == 4) {
		o.center = vec3({ 0, -h, 0});
	}
	else if (pos.f == 5) {
		o.center = vec3({ 0, 0, -h });
	}

	o.amplitude = o.center;

	Motion& motion = registry.motions.get(entity);
	motion.rotation = rand() % 10 + 1;

}

Entity createBurnable(RenderSystem* renderer, Coordinates pos, glm::mat4 translateMatrix) {

	auto entity = Entity();

	createObject(entity, pos, translateMatrix, false, vec3(1.5), 1);

	Object& burnable = registry.objects.get(entity);
	burnable.burnable = true;

	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::TREE);
	registry.meshPtrs.emplace(entity, &mesh);

	registry.renderRequests.insert(
		entity,
		{
			TEXTURE_ASSET_ID::WOOD,
			EFFECT_ASSET_ID::OBJECT,
			GEOMETRY_BUFFER_ID::TREE
		}
	);

	return entity;
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

void createThrowTile(Entity entity, Coordinates pos, glm::mat4 translateMatrix) {

	Motion& motion = registry.motions.emplace(entity);
	motion.interpolate = false;
	motion.position = vec3(0, 0, 0);
	motion.destination = vec3(0, 0, 0);
	motion.velocity = { 0.f , 0.f , 0.f };
	motion.scale = { 1.0f, 1.0f, 1.0f };
}

void createButtonTile(Entity entity, float length){
	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.interpolate = false;
	motion.position = vec3(0, 0, 0);
	motion.destination = vec3(0, 0, 0);
	motion.velocity = { 0.f , 0.f , 0.f };
	motion.scale = {length, -1.0f, 1.0f};

	registry.buttons.emplace(entity);
}

void createObject(Entity entity, Coordinates pos, glm::mat4 translateMatrix, bool hasMotion, vec3 scaleVec, int reflect){

	Object& object = registry.objects.emplace(entity);
	object.objectPos = pos;
	object.model = rotate(glm::mat4(1.0f), (float)radians(90.0f), vec3(0.0f, 1.0f, 0.0f)) * object.model;
	object.model = rotate(glm::mat4(1.0f), (float)radians(reflect * 90.0f), vec3(1.0f, 0.0f, 0.0f)) * object.model;
	object.model = scale(glm::mat4(1.0f), scaleVec) * object.model;
	object.model = translateMatrix * object.model;

	switch (pos.f) {
	case 0:
		object.model = translate(glm::mat4(1.0f), vec3(0.f, 0.f, scaleVec.z / 2)) * object.model;
		break;
	case 1:
		object.model = translate(glm::mat4(1.0f), vec3(-scaleVec.z / 2, 0.f, 0.f)) * object.model;
		break;
	case 2:
		object.model = translate(glm::mat4(1.0f), vec3(scaleVec.z / 2, 0.f, 0.f)) * object.model;
		break;
	case 3:
		object.model = translate(glm::mat4(1.0f), vec3(0.f, scaleVec.z / 2, 0.f)) * object.model;
		break;
	case 4:
		object.model = translate(glm::mat4(1.0f), vec3(0.f, -scaleVec.z / 2, 0.f)) * object.model;
		break;
	case 5:
		object.model = translate(glm::mat4(1.0f), vec3(0.f, 0.f, -scaleVec.z / 2)) * object.model;
		break;
	default:
		break;
	}


	if (hasMotion) {
		// Setting initial motion values
		Motion& motion = registry.motions.emplace(entity);
		motion.interpolate = false;
		motion.position = vec3(object.model[0][3], object.model[1][3], object.model[2][3]);
		motion.destination = vec3(0, 0, 0);
		motion.velocity = { 0.f , 0.f , 0.f };
		motion.scale = { 1.0f, 1.0f, 1.0f };
	}
}

void createLight(RenderSystem* renderer, Coordinates pos, glm::mat4 translateMatrix) {
	auto entity = Entity();

	Billboard& billboard = registry.billboards.emplace(entity);
	billboard.model = translateMatrix * billboard.model;
	switch (pos.f) {
		case 0:
			billboard.model = translate(glm::mat4(1.0f), vec3(0.f, 0.f, 1.5f)) * billboard.model;
			break;
		case 1:
			billboard.model = translate(glm::mat4(1.0f), vec3(-1.5f, 0.f, 0.f)) * billboard.model;
			break;
		case 2:
			billboard.model = translate(glm::mat4(1.0f), vec3(1.5f, 0.f, 0.f)) * billboard.model;
			break;
		case 3:
			billboard.model = translate(glm::mat4(1.0f), vec3(0.f, 1.5f, 0.f)) * billboard.model;
			break;
		case 4:
			billboard.model = translate(glm::mat4(1.0f), vec3(0.f, -1.5f, 0.f)) * billboard.model;
			break;
		case 5:
			billboard.model = translate(glm::mat4(1.0f), vec3(0.f, 0.f, -1.5f)) * billboard.model;
			break;
	}

	registry.lightSources.emplace(entity);

	registry.renderRequests.insert(
	 	entity,
	 	{
	 		TEXTURE_ASSET_ID::TEXTURE_COUNT,
	 		EFFECT_ASSET_ID::BILLBOARD,
	 		GEOMETRY_BUFFER_ID::POINT_LIGHT
	 	}
	 );
}

void createEnemy(RenderSystem* renderer, Coordinates pos, glm::mat4 translateMatrix) {
	auto entity = Entity();

	createObject(entity, pos, translateMatrix, false, vec3(0.75f, 0.75f, 1.2f), 1);

	Object& enemy = registry.objects.get(entity);
	enemy.alpha = 1.0;

	registry.enemies.emplace(entity);

	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::ENEMY);
	registry.meshPtrs.emplace(entity, &mesh);

	registry.renderRequests.insert(
		entity,
		{
			TEXTURE_ASSET_ID::TEXTURE_COUNT,
			EFFECT_ASSET_ID::OBJECT,
			GEOMETRY_BUFFER_ID::ENEMY
		}
	);
}