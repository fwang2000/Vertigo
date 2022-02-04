#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "render_system.hpp"

// These are hard coded to the dimensions of the entity texture
const float EXPLORER_BB_WIDTH = 0.4f * window_height_px / 3;
const float EXPLORER_BB_HEIGHT = 0.4f * window_height_px / 3;
const float TILE_BB_WIDTH = 0.6 * window_height_px / 3;
const float TILE_BB_HEIGHT = 0.6 * window_height_px / 3;

// the cube
Entity createCube(RenderSystem* renderer);
// the explorer
Entity createExplorer(RenderSystem* renderer, vec2 pos);
// the tiles
void createTile(RenderSystem* renderer, TilePosition pos, TileState state);