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
    motion.angle = 0.f;
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
Entity createFire(RenderSystem* renderer, vec2 pos) {
    auto entity = Entity();
	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = vec2({ EXPLORER_BB_WIDTH, EXPLORER_BB_HEIGHT });

	registry.fire.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::FIRE, // TEXTURE_COUNT indicates that no txture is needed
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

//Entity createBug(RenderSystem* renderer, vec2 position)
//{
//	// Reserve en entity
//	auto entity = Entity();
//
//	// Store a reference to the potentially re-used mesh object
//	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
//	registry.meshPtrs.emplace(entity, &mesh);
//
//	// Initialize the position, scale, and physics components
//	auto& motion = registry.motions.emplace(entity);
//	motion.angle = 0.f;
//	motion.velocity = { 0, 50 };
//	motion.position = position;
//
//	// Setting initial values, scale is negative to make it face the opposite way
//	motion.scale = vec2({ -BUG_BB_WIDTH, BUG_BB_HEIGHT });
//
//	// Create an (empty) Bug component to be able to refer to all bug
//	registry.eatables.emplace(entity);
//	registry.renderRequests.insert(
//		entity,
//		{ TEXTURE_ASSET_ID::BUG,
//			EFFECT_ASSET_ID::TEXTURED,
//			GEOMETRY_BUFFER_ID::SPRITE });
//
//	return entity;
//}
//
//Entity createEagle(RenderSystem* renderer, vec2 position)
//{
//	auto entity = Entity();
//
//	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
//	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
//	registry.meshPtrs.emplace(entity, &mesh);
//
//	// Initialize the motion
//	auto& motion = registry.motions.emplace(entity);
//	motion.angle = 0.f;
//	motion.velocity = { 0, 100.f };
//	motion.position = position;
//
//	// Setting initial values, scale is negative to make it face the opposite way
//	motion.scale = vec2({ -EAGLE_BB_WIDTH, EAGLE_BB_HEIGHT });
//
//	// Create and (empty) Eagle component to be able to refer to all eagles
//	registry.deadlys.emplace(entity);
//	registry.renderRequests.insert(
//		entity,
//		{ TEXTURE_ASSET_ID::EAGLE,
//		 EFFECT_ASSET_ID::TEXTURED,
//		 GEOMETRY_BUFFER_ID::SPRITE });
//
//	return entity;
//}
//

//Entity createEgg(vec2 pos, vec2 size)
//{
//	auto entity = Entity();
//
//	// Setting initial motion values
//	Motion& motion = registry.motions.emplace(entity);
//	motion.position = pos;
//	motion.angle = 0.f;
//	motion.velocity = { 0.f, 0.f };
//	motion.scale = size;
//
//	// Create and (empty) Chicken component to be able to refer to all eagles
//	registry.deadlys.emplace(entity);
//	registry.renderRequests.insert(
//		entity,
//		{ TEXTURE_ASSET_ID::TEXTURE_COUNT, // TEXTURE_COUNT indicates that no txture is needed
//			EFFECT_ASSET_ID::EGG,
//			GEOMETRY_BUFFER_ID::EGG });
//
//	return entity;
//}