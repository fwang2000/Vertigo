#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "render_system.hpp"

// These are hard coded to the dimensions of the entity texture
// TODO: Later, change / 3 to / dimension of cube
const float EXPLORER_BB_WIDTH = 0.4f * window_height_px / 3;
const float EXPLORER_BB_HEIGHT = 0.4f * window_height_px / 3;
const float TILE_BB_WIDTH = 0.6 * window_height_px / 3;
const float TILE_BB_HEIGHT = 0.6 * window_height_px / 3;
const float FIRE_BB_WIDTH = 0.3 * window_height_px / 3;
const float FIRE_BB_HEIGHT = 0.3 * window_height_px / 3;
const float OBJECT_BB_WIDTH = 0.4 * window_height_px / 3;
const float OBJECT_BB_HEIGHT = 0.4 * window_height_px / 3;

// the cube
Entity createCube(RenderSystem* renderer);
// the explorer
Entity createExplorer(RenderSystem* renderer, Coordinates pos, glm::mat4 translateMatrix);
// the tiles
Entity createTile(Tile tile);
// the fire
Entity createFire(RenderSystem* renderer, vec3 pos);
// the object
void createObject(RenderSystem* renderer, vec3 pos);