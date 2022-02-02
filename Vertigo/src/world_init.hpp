#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "render_system.hpp"

// These are hard coded to the dimensions of the entity texture
const float EXPLORER_BB_WIDTH = 0.6f * 328.f;
const float EXPLORER_BB_HEIGHT = 0.6f * 488.f;

// the explorer
Entity createExplorer(RenderSystem* renderer, vec2 pos);


