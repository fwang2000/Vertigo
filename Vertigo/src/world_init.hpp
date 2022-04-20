#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "render_system.hpp"

// These are hard coded to the dimensions of the entity texture
// TODO: Later, change / 3 to / dimension of cube
const float TILE_BB_WIDTH = 1.0f * min(window_height_px, window_width_px) / 14.f * (sqrt(98.f) / 7.f) ;
const float TILE_BB_HEIGHT = 1.0f * min(window_height_px, window_width_px) / 7;
const float EXPLORER_BB_WIDTH = 0.8f * TILE_BB_WIDTH;
const float EXPLORER_BB_HEIGHT = 0.8f * TILE_BB_HEIGHT;
const float FIRE_BB_WIDTH = 0.5f * EXPLORER_BB_WIDTH;
const float FIRE_BB_HEIGHT = FIRE_BB_WIDTH;
const float OBJECT_BB_WIDTH = 0.4f * min(window_height_px, window_width_px) / 6.f;
const float OBJECT_BB_HEIGHT = 0.4f * min(window_height_px, window_width_px) / 4.f;

// the cube
Entity createCube(RenderSystem* renderer);
// the explorer
Entity createExplorer(RenderSystem* renderer, Coordinates pos, glm::mat4 translateMatrix);
// Entity createExplorer(RenderSystem* renderer, Coordinates pos, int size);
// the tiles
Entity createTile(Tile* tile);
// the text
Entity createText(Text text);
// the restart text
void createRestartText(RenderSystem* renderer);
// the trackball instructions text
Entity createTrackBallText(RenderSystem* renderer);
// the instructions text
void createInstructionsText(RenderSystem* renderer);
// the fire
Entity createFire(RenderSystem* renderer, Coordinates pos, glm::mat4 translateMatrix);
// the fire's power gauge
Entity createFireGauge(RenderSystem* renderer, Coordinates pos, glm::mat4 translateMatrix);
// the column
void createColumn(RenderSystem* renderer, Coordinates pos, glm::mat4 translateMatrix);
// the device
Entity createDevice(RenderSystem* renderer, Coordinates pos, glm::mat4 translateMatrix);
// the constantly moving tile
void createConstMovingTile(Entity entity, Coordinates pos, glm::mat4 translateMatrix);
// the throw tile
void createThrowTile(Entity entity, Coordinates pos, glm::mat4 translateMatrix);
// the button tile
void createButtonTile(Entity entity, float length);
// general objects
void createObject(Entity entity, Coordinates pos, glm::mat4 translateMatrix, bool hasMotion, vec3 scale, int reflect);
// the burnable
Entity createBurnable(RenderSystem* renderer, Coordinates pos, glm::mat4 translateMatrix);
void createLight(RenderSystem* renderer, Coordinates pos, glm::mat4 translateMatrix);
void createEnemy(RenderSystem* renderer, Coordinates pos, glm::mat4 translateMatrix);
Entity createCutscene(RenderSystem* renderer, unsigned int texture_id);