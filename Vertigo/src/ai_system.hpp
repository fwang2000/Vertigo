#pragma once

#include <vector>
#include <queue>

#include "tiny_ecs_registry.hpp"
#include "common.hpp"
#include "world_system.hpp"

class AISystem
{
public:
	AISystem(WorldSystem& wrldSystem): worldSystem(wrldSystem) {};

	void step();
	bool BFS(Entity entity);
private:
	WorldSystem& worldSystem;
};