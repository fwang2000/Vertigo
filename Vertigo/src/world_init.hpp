#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "render_system.hpp"

// These are hard coded to the dimensions of the entity texture
const float EXPLORER_BB_WIDTH = 0.4f * 387.f;
const float EXPLORER_BB_HEIGHT = 0.4f * 387.f;
const float TILE_BB_WIDTH = 0.6 * 387.f;
const float TILE_BB_HEIGHT = 0.6 * 387.f;

// the cube
Entity createCube(RenderSystem* renderer);
// the explorer
Entity createExplorer(RenderSystem* renderer, vec2 pos);
// the tiles
void createTile(RenderSystem* renderer, vec2 pos);