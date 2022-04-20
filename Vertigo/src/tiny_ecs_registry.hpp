#pragma once
#include <vector>

#include "tiny_ecs.hpp"
#include "components.hpp"

class ECSRegistry
{
	// Callbacks to remove a particular or all entities in the system
	std::vector<ContainerInterface*> registry_list;

public:
	// Manually created list of all components this game has
	// TODO: A1 add a LightUp component
	ComponentContainer<HoldTimer> holdTimers;
	ComponentContainer<Motion> motions;
	ComponentContainer<Oscillate> oscillations;
	ComponentContainer<Collision> collisions;
	ComponentContainer<Player> players;
	ComponentContainer<Mesh*> meshPtrs;
	ComponentContainer<RenderRequest> renderRequests;
	ComponentContainer<ScreenState> screenStates;
	ComponentContainer<vec3> colors;

	ComponentContainer<Tile*> tiles;
	ComponentContainer<Text> text;
	ComponentContainer<Fire> fire;
	ComponentContainer<Object> objects;
	ComponentContainer<Burnable> burnables;
	ComponentContainer<Animated> animated;
	ComponentContainer<Menu> menus;
	ComponentContainer<Menu> menuButtons;
	ComponentContainer<Button> buttons;
	ComponentContainer<Billboard> billboards;
	ComponentContainer<LightSource> lightSources;
	ComponentContainer<RestartTimer> restartTimer;
	ComponentContainer<Enemy> enemies;
	ComponentContainer<TrackBallInfo> trackBall;

	// constructor that adds all containers for looping over them
	// IMPORTANT: Don't forget to add any newly added containers!
	ECSRegistry()
	{
		registry_list.push_back(&holdTimers);
		registry_list.push_back(&motions);
		registry_list.push_back(&oscillations);
		registry_list.push_back(&collisions);
		registry_list.push_back(&players);
		registry_list.push_back(&meshPtrs);
		registry_list.push_back(&renderRequests);
		registry_list.push_back(&screenStates);
		registry_list.push_back(&colors);
		registry_list.push_back(&tiles);
		registry_list.push_back(&text);
		registry_list.push_back(&fire);
		registry_list.push_back(&objects);
		registry_list.push_back(&burnables);
		registry_list.push_back(&animated);
		registry_list.push_back(&menus);
		registry_list.push_back(&menuButtons);
		registry_list.push_back(&buttons);
		registry_list.push_back(&billboards);
		registry_list.push_back(&lightSources);
		registry_list.push_back(&restartTimer);
		registry_list.push_back(&enemies);
		registry_list.push_back(&trackBall);
	}

	void clear_all_components() {
		for (ContainerInterface* reg : registry_list)
			reg->clear();
	}

	void list_all_components() {
		printf("Debug info on all registry entries:\n");
		for (ContainerInterface* reg : registry_list)
			if (reg->size() > 0)
				printf("%4d components of type %s\n", (int)reg->size(), typeid(*reg).name());
	}

	void list_all_components_of(Entity e) {
		printf("Debug info on components of entity %u:\n", (unsigned int)e);
		for (ContainerInterface* reg : registry_list)
			if (reg->has(e))
				printf("type %s\n", typeid(*reg).name());
	}

	void remove_all_components_of(Entity e) {
		for (ContainerInterface* reg : registry_list)
			reg->remove(e);
	}
};

extern ECSRegistry registry;