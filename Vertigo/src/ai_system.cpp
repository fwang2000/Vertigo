// internal
#include "ai_system.hpp"

// setup animation moving to another tile on the same face
void enemyTranslate(Tile* current, Tile* destination, Enemy& enemy) {
	enemy.changingFaces = false;
	
	vec3 diff = current->model[3] - destination->model[3];

	// the largest diff is the axis we need to move along
	int idx = 0;
	for (int i = 1; i < 3; i++) {
		if (abs(diff[i]) > abs(diff[idx])) idx = i;
	}

	enemy.translateX = idx == 0 ? (diff[idx] > 0.f ? &oneDimensionNegative : &oneDimension) : &defaultTranslate;
	enemy.translateY = idx == 1 ? (diff[idx] > 0.f ? &oneDimensionNegative : &oneDimension) : &defaultTranslate;
	enemy.translateZ = idx == 2 ? (diff[idx] > 0.f ? &oneDimensionNegative : &oneDimension) : &defaultTranslate;
}

// setup animation going from one face to another
void enemyTranslateFace(Tile* current, Tile* destination, Enemy& enemy) {
	enemy.changingFaces = true;
	int currentFace = static_cast<int>(current->direction);
	int destinationFace = static_cast<int>(destination->direction);
	switch (currentFace) {
		case 0:
			if (destinationFace == 1 || destinationFace == 2) {
				enemy.axis = destinationFace == 2 ? vec3(0.f, 1.f, 0.f) : vec3(0.f, -1.f, 0.f);
				enemy.translateX = destinationFace == 2 ? &cosine : &flipCosine;
				enemy.translateY = &defaultTranslate;
				enemy.translateZ = &sine;
			} else {
				enemy.axis = destinationFace == 4 ? vec3(1.f, 0.f, 0.f) : vec3(-1.f, 0.f, 0.f);
				enemy.translateX = &defaultTranslate;
				enemy.translateY = destinationFace == 3 ? &cosine : &flipCosine;
				enemy.translateZ = &sine;
			}
			break;
		case 1:
			if (destinationFace == 5 || destinationFace == 0) {
				enemy.axis = destinationFace == 0 ? vec3(0.f, 1.f, 0.f) : vec3(0.f, -1.f, 0.f);
				enemy.translateX = &flipSine;
				enemy.translateY = &defaultTranslate;
				enemy.translateZ = destinationFace == 0 ? &cosine : &flipCosine;
			} else {
				enemy.axis = destinationFace == 4 ? vec3(0.f, 0.f, 1.f) : vec3(0.f, 0.f, -1.f);
				enemy.translateX = &flipSine;
				enemy.translateY = destinationFace == 3 ? &cosine : &flipCosine;
				enemy.translateZ = &defaultTranslate;
			}
			break;
		case 2:
			if (destinationFace == 0 || destinationFace == 5) {
				enemy.axis = destinationFace == 5 ? vec3(0.f, 1.f, 0.f) : vec3(0.f, -1.f, 0.f);
				enemy.translateX = &sine;
				enemy.translateY = &defaultTranslate;
				enemy.translateZ = destinationFace == 0 ? &cosine : &flipCosine;
			} else {
				enemy.axis = destinationFace == 3 ? vec3(0.f, 0.f, 1.f) : vec3(0.f, 0.f, -1.f);
				enemy.translateX = &sine;
				enemy.translateY = destinationFace == 3 ? &cosine : &flipCosine;
				enemy.translateZ = &defaultTranslate;
			}
			break;
		case 3:
			if (destinationFace == 1 || destinationFace == 2) {
				enemy.axis = destinationFace == 1 ? vec3(0.f, 0.f, 1.f) : vec3(0.f, 0.f, -1.f);
				enemy.translateX = destinationFace == 2 ? &cosine : &flipCosine;
				enemy.translateY = &sine;
				enemy.translateZ = &defaultTranslate;
			} else {
				enemy.axis = destinationFace == 0 ? vec3(1.f, 0.f, 0.f) : vec3(-1.f, 0.f, 0.f);
				enemy.translateX = &defaultTranslate;
				enemy.translateY = &sine;
				enemy.translateZ = destinationFace == 0 ? &cosine : &flipCosine;
			}
			break;
		case 4:
			if (destinationFace == 1 || destinationFace == 2) {
				enemy.axis = destinationFace == 2 ? vec3(0.f, 0.f, 1.f) : vec3(0.f, 0.f, -1.f);
				enemy.translateX = destinationFace == 2 ? &cosine : &flipCosine;
				enemy.translateY = &flipSine;
				enemy.translateZ = &defaultTranslate;
			} else {
				enemy.axis = destinationFace == 5 ? vec3(1.f, 0.f, 0.f) : vec3(-1.f, 0.f, 0.f);
				enemy.translateX = &defaultTranslate;
				enemy.translateY = &flipSine;
				enemy.translateZ = destinationFace == 0 ? &cosine : &flipCosine;
			}
			break;
		case 5:
			if (destinationFace == 2 || destinationFace == 1) {
				enemy.axis = destinationFace == 1 ? vec3(0.f, 1.f, 0.f) : vec3(0.f, -1.f, 0.f);
				enemy.translateX = destinationFace == 2 ? &cosine : &flipCosine;
				enemy.translateY = &defaultTranslate;
				enemy.translateZ = &flipSine;
			} else {
				enemy.axis = destinationFace == 3 ? vec3(1.f, 0.f, 0.f) : vec3(-1.f, 0.f, 0.f);
				enemy.translateX = &defaultTranslate;
				enemy.translateY = destinationFace == 3 ? &cosine : &flipCosine;
				enemy.translateZ = &flipSine;
			}
			break;
	}
}

// BFS that finds the shortest path from player to enemy
bool AISystem::BFS(Entity entity)
{
	Enemy& enemy = registry.enemies.get(entity);
	Player& player = registry.players.components[0]; // assuming only one player
	Object& object = registry.objects.get(entity);
	Cube& cube = worldSystem.cube;

	std::vector<std::vector<std::vector<bool>>> visited(6, std::vector<std::vector<bool>>(cube.size, std::vector<bool>(cube.size, false)));
	std::queue<Coordinates> q;
	q.push(player.playerPos);
	while(!q.empty()) {
		Coordinates cur = q.front();
		visited[cur.f][cur.r][cur.c] = true;
		q.pop();

		// visit neighbours
		for (int i = 0; i < 4; i++) {
			Coordinates newCoord = worldSystem.searchForTile(static_cast<Direction>(i), cur);
			if (visited[newCoord.f][newCoord.r][newCoord.c]) continue;

			Tile* newTile = cube.getTile(newCoord);
			if (newCoord.equal(object.objectPos)) {
				// since we start the BFS from the player, curTile is the tile we want the enemy to move towards
				Tile* curTile = cube.getTile(cur);
				if (newCoord.f == cur.f) {
					enemyTranslate(newTile, curTile, enemy);
					Mix_PlayChannel(-1, worldSystem.rook_slide, 0);
				} else {
					enemyTranslateFace(newTile, curTile, enemy);
					Mix_PlayChannel(-1, worldSystem.rook_jump, 0);
				}
				// round startingPos because we have numerical instability
				enemy.startingPos = object.model[3];
				for (int j = 0; j < 3; j++) {
					float d = enemy.startingPos[j] * 2.0f;
					d = round(d);
					enemy.startingPos[j] = d / 2.0f;
				}

				enemy.moving = true;
				object.objectPos = cur;
				return true;
			} else {
				if (newTile->tileState != TileState::B && newTile->tileState != TileState::E && newTile->tileState != TileState::I &&
					newTile->tileState != TileState::N && newTile->tileState != TileState::O && !worldSystem.enemyOnTile(newTile->coords)) { // TODO: figure out all tiles that cannot be moved to
						q.push(newCoord);
					}
			}
		}
	}
	return false;
}

void AISystem::step()
{
	if (worldSystem.gameState != GameState::ENEMY_SEARCH) return;
	for (Entity entity : registry.enemies.entities) {
		// BFS that finds the shortest path from player to enemy
		if (BFS(entity))
			worldSystem.moving_enemies.insert(int(entity));
	}
	if (worldSystem.moving_enemies.empty()) {
		worldSystem.gameState = GameState::IDLE;
	} else {
		worldSystem.gameState = GameState::ENEMY_MOVE;
	}
}