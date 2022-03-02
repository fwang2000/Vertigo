#include "components.hpp"
#include "render_system.hpp" // for gl_has_errors

#define STB_IMAGE_IMPLEMENTATION
#include "../ext/stb_image/stb_image.h"

// stlib
#include <iostream>
#include <sstream>

glm::mat4 tileStartingMatrix(int face, float x, float y, float distance) {
	glm::mat4 matrix = glm::mat4(1.0f);
	// rotate then translate
	switch (face) {
	case 0:
		matrix = scale(glm::mat4(1.0f), vec3(0.85)) * matrix;
		matrix = translate(glm::mat4(1.0f), vec3(x, -y, distance)) * matrix;
		break;
	case 1:
		matrix = scale(glm::mat4(1.0f), vec3(0.85)) * matrix;
		matrix = rotate(glm::mat4(1.0f), (float)radians(-90.0f), vec3(0.0f, 1.0f, 0.0f)) * matrix;
		matrix = translate(glm::mat4(1.0f), vec3(-distance, -x, -y)) * matrix;
		matrix = rotate(glm::mat4(1.0f), (float)radians(-90.0f), vec3(1.0f, 0.0f, 0.0f)) * matrix;
		break;
	case 2:
		matrix = scale(glm::mat4(1.0f), vec3(0.85)) * matrix;
		matrix = rotate(glm::mat4(1.0f), (float)radians(90.0f), vec3(0.0f, 1.0f, 0.0f)) * matrix;
		matrix = translate(glm::mat4(1.0f), vec3(distance, -y, -x)) * matrix;
		break;
	case 3:
		matrix = scale(glm::mat4(1.0f), vec3(0.85)) * matrix;
		matrix = rotate(glm::mat4(1.0f), (float)radians(-90.0f), vec3(1.0f, 0.0f, 0.0f)) * matrix;
		matrix = translate(glm::mat4(1.0f), vec3(x, distance, y)) * matrix;
		break;
	case 4:
		matrix = scale(glm::mat4(1.0f), vec3(0.85)) * matrix;
		matrix = rotate(glm::mat4(1.0f), (float)radians(90.0f), vec3(1.0f, 0.0f, 0.0f)) * matrix;
		matrix = translate(glm::mat4(1.0f), vec3(x, -distance, -y)) * matrix;
		break;
	case 5:
		matrix = scale(glm::mat4(1.0f), vec3(0.85)) * matrix;
		matrix = translate(glm::mat4(1.0f), vec3(x, y, -distance)) * matrix;
		matrix = rotate(glm::mat4(1.0f), (float)radians(180.0f), vec3(0.0f, 0.0f, 1.0f)) * matrix;
		break;
	default:
		break;
	}
	return matrix;
}

glm::mat4 textStartingMatrix(int face, float x, float y, float distance, float scale_x, float scale_y) {
	glm::mat4 matrix = glm::mat4(1.0f);
	// rotate then translate
	switch (face) {
	case 0:
		matrix = scale(glm::mat4(1.0f), vec3(scale_x, scale_y, 0.85)) * matrix;
		matrix = translate(glm::mat4(1.0f), vec3(x, y, distance)) * matrix;
		break;
	case 1:
		matrix = scale(glm::mat4(1.0f), vec3(scale_x, scale_y, 0.85)) * matrix;
		matrix = rotate(glm::mat4(1.0f), (float)radians(-90.0f), vec3(0.0f, 1.0f, 0.0f)) * matrix;
		matrix = translate(glm::mat4(1.0f), vec3(-distance, -x, y)) * matrix;
		matrix = rotate(glm::mat4(1.0f), (float)radians(-90.0f), vec3(1.0f, 0.0f, 0.0f)) * matrix;
		break;
	case 2:
		matrix = scale(glm::mat4(1.0f), vec3(scale_x, scale_y, 0.85)) * matrix;
		matrix = rotate(glm::mat4(1.0f), (float)radians(90.0f), vec3(0.0f, 1.0f, 0.0f)) * matrix;
		matrix = translate(glm::mat4(1.0f), vec3(distance, x, y)) * matrix;
		matrix = rotate(glm::mat4(1.0f), (float)radians(-90.0f), vec3(1.0f, 0.0f, 0.0f)) * matrix;
		break;
	case 3:
		matrix = scale(glm::mat4(1.0f), vec3(scale_x, scale_y, 0.85)) * matrix;
		matrix = rotate(glm::mat4(1.0f), (float)radians(90.0f), vec3(1.0f, 0.0f, 0.0f)) * matrix;
		matrix = translate(glm::mat4(1.0f), vec3(x, distance, y)) * matrix;
		break;
	case 4:
		matrix = scale(glm::mat4(1.0f), vec3(scale_x, scale_y, 0.85)) * matrix;
		matrix = rotate(glm::mat4(1.0f), (float)radians(90.0f), vec3(1.0f, 0.0f, 0.0f)) * matrix;
		matrix = translate(glm::mat4(1.0f), vec3(x, -distance, y)) * matrix;
		break;
	case 5:
		matrix = scale(glm::mat4(1.0f), vec3(scale_x, scale_y, 0.85)) * matrix;
		matrix = translate(glm::mat4(1.0f), vec3(-x, y, -distance)) * matrix;
		break;
	default:
		break;
	}
	return matrix;
}

void Cube::createAdjList() {
	// doing the edges (gonna take 12 for loops lol)
	for (int i = 0; i < size; i++) {
		Tile* left = faces[0][i][size-1];
		Tile* right = faces[2][i][0];
		Coordinates leftCoord = {0, i, size-1};
		Coordinates rightCoord = {2, i, 0};
		left->adjList[1] = std::make_pair(rightCoord, 0);
		right->adjList[3] = std::make_pair(leftCoord, 0);
	}

	for (int i = 0; i < size; i++) {
		Tile* left = faces[2][i][size-1];
		Tile* right = faces[5][i][0];
		Coordinates leftCoord = {2, i, size-1};
		Coordinates rightCoord = {5, i, 0};
		left->adjList[1] = std::make_pair(rightCoord, 0);
		right->adjList[3] = std::make_pair(leftCoord, 0);
	}

	for (int i = 0; i < size; i++) {
		Tile* left = faces[5][i][size-1];
		Tile* right = faces[1][i][0];
		Coordinates leftCoord = {5, i, size-1};
		Coordinates rightCoord = {1, i, 0};
		left->adjList[1] = std::make_pair(rightCoord, 0);
		right->adjList[3] = std::make_pair(leftCoord, 0);
	}

	for (int i = 0; i < size; i++) {
		Tile* left = faces[1][i][size-1];
		Tile* right = faces[0][i][0];
		Coordinates leftCoord = {1, i, size-1};
		Coordinates rightCoord = {0, i, 0};
		left->adjList[1] = std::make_pair(rightCoord, 0);
		right->adjList[3] = std::make_pair(leftCoord, 0);
	}

	// up/down adges
	for (int i = 0; i < size; i++) {
		Tile* down = faces[0][0][i];
		Tile* up = faces[3][size-1][i];
		Coordinates downCoord = {0, 0, i};
		Coordinates upCoord = {3, size-1, i};
		down->adjList[0] = std::make_pair(upCoord, 0);
		up->adjList[2] = std::make_pair(downCoord, 0);
	}

	for (int i = 0; i < size; i++) {
		Tile* down = faces[4][0][i];
		Tile* up = faces[0][size-1][i];
		Coordinates downCoord = {4, 0, i};
		Coordinates upCoord = {0, size-1, i};
		down->adjList[0] = std::make_pair(upCoord, 0);
		up->adjList[2] = std::make_pair(downCoord, 0);
	}

	for (int i = 0; i < size; i++) {
		Tile* up = faces[3][0][i];
		Tile* up2 = faces[5][0][size - (i+1)];
		Coordinates upCoord = {3, 0, i};
		Coordinates up2Coord = {5, 0, size - (i+1)};
		up->adjList[0] = std::make_pair(up2Coord, 2);
		up2->adjList[0] = std::make_pair(upCoord, 2);
	}

	for (int i = 0; i < size; i++) {
		Tile* down = faces[4][size-1][i];
		Tile* down2 = faces[5][size-1][size - (i+1)];
		Coordinates downCoord = {4, size-1, i};
		Coordinates down2Coord = {5, size-1, size - (i+1)};
		down->adjList[2] = std::make_pair(down2Coord, 2);
		down2->adjList[2] = std::make_pair(downCoord, 2);
	}

	// last 4 edges
	for (int i = 0; i < size; i++) {
		Tile* up = faces[3][size-(i+1)][size-1];
		Tile* right = faces[2][0][i];
		Coordinates upCoord = {3, size-(i+1), size-1};
		Coordinates rightCoord = {2, 0, i};
		up->adjList[1] = std::make_pair(rightCoord, -1);
		right->adjList[0] = std::make_pair(upCoord, 1);
	}

	for (int i = 0; i < size; i++) {
		Tile* up = faces[3][i][0];
		Tile* left = faces[1][0][i];
		Coordinates upCoord = {3, i, 0};
		Coordinates leftCoord = {1, 0, i};
		up->adjList[3] = std::make_pair(leftCoord, 1);
		left->adjList[0] = std::make_pair(upCoord, -1);
	}

	for (int i = 0; i < size; i++) {
		Tile* down = faces[4][i][0];
		Tile* left = faces[1][size-1][size-(i+1)];
		Coordinates downCoord = {4, i, 0};
		Coordinates leftCoord = {1, size-1, size-(i+1)};
		down->adjList[3] = std::make_pair(leftCoord, -1);
		left->adjList[2] = std::make_pair(downCoord, 1);
	}

	for (int i = 0; i < size; i++) {
		Tile* down = faces[4][i][size-1];
		Tile* right = faces[2][size-1][i];
		Coordinates downCoord = {4, i, size-1};
		Coordinates rightCoord = {2, size-1, i};
		down->adjList[1] = std::make_pair(rightCoord, 1);
		right->adjList[2] = std::make_pair(downCoord, -1);
	}
}

// load tiles from excel file, also set the model matrix of each tile (rotate -> translate)
// order of faces: front, left, right, top, bottom, back
bool Cube::loadFromExcelFile(std::string filename) {
	std::ifstream file(filename);

	std::string sizeStr;
	std::getline(file, sizeStr);
	size = stoi(sizeStr);
	float distance = size / 2.f;

	float divisor = 1.f;

	switch (size) {
	case 3:
		divisor = 3.f;
		break;
	case 4:
		divisor = 2.f;
		break;
	case 5:
		divisor = 2.5f;
		break;
	default:
		break;
	}

	std::string line;
	for (int i = 0; i < 6; i++) {
		int y = int(-size / divisor); // divide by: size = 3 --> 3, size = 4 --> 2, size = 5 --> 2.5
		if (size % 2 == 0) y += 0.5f;
		int rows = 0;
		while (std::getline(file, line)) {
			int x = int(-size / divisor); // divide by: size = 3 --> 3, size = 4 --> 2, size = 5 --> 2.5
			if (size % 2 == 0) x += 0.5f;
			std::string value;
			std::stringstream ss(line);
			std::vector<Tile*> row;
			row.reserve(size);
			while (std::getline(ss, value, ','))
			{
				switch (static_cast<TileState>(value[0] - 'A')) {
				case TileState::W:
				{
					SwitchTile* s_tile = new SwitchTile();
					s_tile->model = tileStartingMatrix(i, x, y, distance);
					s_tile->tileState = static_cast<TileState>(value[0] - 'A');

					row.push_back(s_tile);
					s_tile->coords = { i, y + 1, x + 1 };
					s_tile->currentPos = s_tile->coords;
					s_tile->direction = static_cast<FACE_DIRECTION>(i);
					break;
				}
				case TileState::U:
				{
					UpTile* u_tile = new UpTile();
					u_tile->model = tileStartingMatrix(i, x, y, distance);
					u_tile->tileState = static_cast<TileState>(value[0] - 'A');

					row.push_back(u_tile);
					u_tile->coords = { i, y + 1, x + 1 };
					u_tile->currentPos = u_tile->coords;
					u_tile->direction = static_cast<FACE_DIRECTION>(i);
					break;
				}
				case TileState::I:
				{
					InvisibleTile* i_tile = new InvisibleTile();
					i_tile->model = tileStartingMatrix(i, x, y, distance);
					i_tile->tileState = static_cast<TileState>(value[0] - 'A');

					row.push_back(i_tile);
					i_tile->coords = { i, y + 1, x + 1 };
					i_tile->currentPos = i_tile->coords;
					i_tile->direction = static_cast<FACE_DIRECTION>(i);
					break;
				}
				default:
				{
					Tile* tile = new Tile();
					tile->model = tileStartingMatrix(i, x, y, distance);
					tile->tileState = static_cast<TileState>(value[0] - 'A');

					row.push_back(tile);
					tile->coords = { i, y + 1, x + 1 };
					tile->currentPos = tile->coords;
					tile->direction = static_cast<FACE_DIRECTION>(i);
					break;
				}
				}

				x += 1.f;
			}
			if (row.size() != size) {
				printf("One of the rows has an incorrect number of tiles\n");
				return false;
			}
			this->faces[i].push_back(row);
			std::vector<Tile*>().swap(row);
			y += 1.f;
			if (++rows == size) break;
		}
	}
	return true;
}

bool Cube::loadTextFromExcelFile(std::string filename) {
	std::ifstream file(filename);

	if (file.peek() == std::ifstream::traits_type::eof()) {
		return true;
	}

	std::string sizeStr;
	std::getline(file, sizeStr);
	size = stoi(sizeStr);
	float distance = size / 2.f;

	std::string line;
	while (std::getline(file, line)) {

		std::string value;
		std::stringstream ss(line);
		std::vector<std::string> textValues;
		textValues.reserve(6);

		while (std::getline(ss, value, ','))
		{
			textValues.push_back(value);
		}

		int i = std::stoi(textValues[0]);
		float x = std::stof(textValues[1]);
		float y = std::stof(textValues[2]);
		float scale_x = std::stof(textValues[3]);
		float scale_y = std::stof(textValues[4]);

		Text t;
		t.model = textStartingMatrix(i, x, y, distance, scale_x, scale_y);
		t.texture_id = std::stoi(textValues[5]);
		this->text.push_back(t);

		std::vector<std::string>().swap(textValues);
	}
	return true;
}

bool Cube::loadModificationsFromExcelFile(std::string filename) {

	std::ifstream file(filename);

	if (file.peek() == std::ifstream::traits_type::eof()) {
		return true;
	}

	std::string line;
	while (std::getline(file, line)) {

		std::string value;
		std::stringstream ss(line);
		std::vector<std::string> modifications;
		modifications.reserve(11);

		while (std::getline(ss, value, ','))
		{
			modifications.push_back(value);
		}

		if (modifications.at(0) == "W") {

			int f = std::stoi(modifications.at(1));
			int r = std::stoi(modifications.at(2));
			int c = std::stoi(modifications.at(3));

			SwitchTile* switch_tile = (SwitchTile*)getTile(Coordinates{ f, r, c });

			int t_f = std::stoi(modifications.at(5));
			int t_r = std::stoi(modifications.at(6));
			int t_c = std::stoi(modifications.at(7));

			if (modifications.at(4) == "I") {

				switch_tile->targetTile = (InvisibleTile*)getTile(Coordinates{ t_f, t_r, t_c });
			}
			else {

				Tile* target = getTile(Coordinates{ t_f, t_r, t_c });
				if (target->tileState == TileState::W) {
					switch_tile->targetTile = (SwitchTile*)target;
				}
				else {
					switch_tile->targetTile = target;
				}
				switch_tile->targetCoords = Coordinates{ std::stoi(modifications.at(8)), std::stoi(modifications.at(9)), std::stoi(modifications.at(10)) } ;
			}
		}

		std::vector<std::string>().swap(modifications);
	}

	return true;
}

void Cube::reset() {
	std::array<std::vector<std::vector<Tile*>>, 6>().swap(this->faces);
	std::vector<Text>().swap(this->text);
}

Tile* Cube::getTile(Coordinates coord) {
	return this->faces[coord.f][coord.r][coord.c];
}

// Very, VERY simple OBJ loader from https://github.com/opengl-tutorials/ogl tutorial 7
// (modified to also read vertex color and omit uv and normals)
bool Mesh::loadFromOBJFile(std::string obj_path, std::vector<ColoredVertex>& out_vertices, std::vector<uint16_t>& out_vertex_indices, vec2& out_size)
{
	// disable warnings about fscanf and fopen on Windows
#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif

	printf("Loading OBJ file %s...\n", obj_path.c_str());
	// Note, normal and UV indices are not loaded/used, but code is commented to do so
	std::vector<uint16_t> out_uv_indices, out_normal_indices;
	std::vector<glm::vec2> out_uvs;
	std::vector<glm::vec3> out_normals;

	FILE* file = fopen(obj_path.c_str(), "r");
	if (file == NULL) {
		printf("Impossible to open the file ! Are you in the right path ? See Tutorial 1 for details\n");
		getchar();
		return false;
	}

	while (1) {
		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		if (strcmp(lineHeader, "v") == 0) {
			ColoredVertex vertex;
			int matches = fscanf(file, "%f %f %f %f %f %f\n", &vertex.position.x, &vertex.position.y, &vertex.position.z,
				&vertex.color.x, &vertex.color.y, &vertex.color.z);
			if (matches == 3)
				vertex.color = { 1,1,1 };
			out_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0) {
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
			out_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0) {
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			out_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0) {
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], normalIndex[3], uvIndex[3];

			int matches = fscanf(file, "%d %d %d\n", &vertexIndex[0], &vertexIndex[1], &vertexIndex[2]);
			if (matches == 1) // try again
			{
				// Note first vertex index is already consumed by the first fscanf call (match ==1) since it aborts on the first error
				matches = fscanf(file, "//%d %d//%d %d//%d\n", &normalIndex[0], &vertexIndex[1], &normalIndex[1], &vertexIndex[2], &normalIndex[2]);
				if (matches != 5) // try again
				{
					matches = fscanf(file, "%d/%d %d/%d/%d %d/%d/%d\n", &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
					if (matches != 8)
					{
						printf("File can't be read by our simple parser :-( Try exporting with other options\n");
						fclose(file);
						return false;
					}
				}
			}

			// -1 since .obj starts counting at 1 and OpenGL starts at 0
			out_vertex_indices.push_back((uint16_t)vertexIndex[0] - 1);
			out_vertex_indices.push_back((uint16_t)vertexIndex[1] - 1);
			out_vertex_indices.push_back((uint16_t)vertexIndex[2] - 1);
			//out_uv_indices.push_back(uvIndex[0] - 1);
			//out_uv_indices.push_back(uvIndex[1] - 1);
			//out_uv_indices.push_back(uvIndex[2] - 1);
			out_normal_indices.push_back((uint16_t)normalIndex[0] - 1);
			out_normal_indices.push_back((uint16_t)normalIndex[1] - 1);
			out_normal_indices.push_back((uint16_t)normalIndex[2] - 1);
		}
		else {
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}
	}
	fclose(file);

	// Compute bounds of the mesh
	vec3 max_position = { -99999,-99999,-99999 };
	vec3 min_position = { 99999,99999,99999 };
	for (ColoredVertex& pos : out_vertices)
	{
		max_position = glm::max(max_position, pos.position);
		min_position = glm::min(min_position, pos.position);
	}
	if (abs(max_position.z - min_position.z) < 0.001)
		max_position.z = min_position.z + 1; // don't scale z direction when everythin is on one plane

	vec3 size3d = max_position - min_position;
	out_size = size3d;

	// Normalize mesh to range -0.5 ... 0.5
	for (ColoredVertex& pos : out_vertices)
		pos.position = ((pos.position - min_position) / size3d) - vec3(0.5f, 0.5f, 0.5f);

	return true;
}

#pragma region TILE_FUNCTIONS

void SwitchTile::action() {

	printf("Switch\n");

	if (toggled) {
		return;
	}

	if (targetTile->tileState == TileState::I) {
		targetTile->tileState = TileState::V;
		targetTile->action();
	}

	toggled = true;
}

void UpTile::action() {
	// if up tile facing up {
	// 	this->tileState = TileState::D;
	// }

	return;
}

void InvisibleTile::action() {

	printf("Invisible\n");

	if (!toggled) {
		toggled = true;
		this->tileState = TileState::V;
	}
}

void BurnableTile::action() {
	printf("Burning\n");

	if (!burned) {
		burned = true;
		this->tileState = TileState::V;
	}
  
void Tile::move(vec2 t, vec2 delta_coord) {

	vec3 translation = vec3(0);

	switch (direction) {
	case FACE_DIRECTION::FRONT:
		translation = vec3(t.x, t.y, 0);
		break;
	case FACE_DIRECTION::LEFT:
		translation = vec3(0, t.y, -t.x);
		break;
	case FACE_DIRECTION::RIGHT:
		translation = vec3(0, t.y, -t.x);
		break;
	case FACE_DIRECTION::TOP:
		translation = vec3(t.x, 0, -t.y);
		break;
	case FACE_DIRECTION::BOTTOM:
		translation = vec3(t.x, 0, t.y);
		break;
	case FACE_DIRECTION::BACK:
		translation = vec3(-t.x, -t.y, 0);
		break;
	default:
		break;
	}

	model = translate(glm::mat4(1.f), translation) * model;
	currentPos = Coordinates{ currentPos.f, currentPos.r - (int)delta_coord.y , currentPos.c + (int)delta_coord.x };
	status = BOX_ANIMATION::STILL;
}

#pragma endregion
