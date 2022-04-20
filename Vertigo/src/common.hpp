#pragma once

// stlib
#include <fstream> // stdout, stderr..
#include <string>
#include <tuple>
#include <vector>
#include <array>

// glfw (OpenGL)
#define NOMINMAX
#include <gl3w.h>
#include <GLFW/glfw3.h>

// The glm library provides vector and matrix operations as in GLSL
#include <glm/vec2.hpp>				// vec2
#include <glm/ext/vector_int2.hpp>  // ivec2
#include <glm/vec3.hpp>             // vec3
#include <glm/mat3x3.hpp>           // mat3
#include <glm/vec4.hpp>				// vec4
#include <glm/gtc/matrix_transform.hpp>

#ifdef _MSC_VER
#pragma warning(disable:4201)
#endif

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>
using namespace glm;

#include "tiny_ecs.hpp"

// Simple utility functions to avoid mistyping directory name
// audio_path("audio.ogg") -> data/audio/audio.ogg
// Get defintion of PROJECT_SOURCE_DIR from:
#include "../ext/project_path.hpp"
inline std::string data_path() { return std::string(PROJECT_SOURCE_DIR) + "data"; };
inline std::string shader_path(const std::string& name) { return std::string(PROJECT_SOURCE_DIR) + "shaders/" + std::string(name); };
inline std::string textures_path(const std::string& name) { return data_path() + "/textures/" + std::string(name); };
inline std::string audio_path(const std::string& name) { return data_path() + "/audio/" + std::string(name); };
inline std::string mesh_path(const std::string& name) { return data_path() + "/meshes/" + std::string(name); };
inline std::string level_path() { return data_path() + "/levels"; };
inline std::string tile_path(const std::string& name) { return level_path() + "/tiles/" + std::string(name); }
inline std::string text_path(const std::string& name) { return level_path() + "/text/" + std::string(name); }
inline std::string modifications_path(const std::string& name) { return level_path() + "/modifications/" + std::string(name); }

const int window_width_px = 900;
const int window_height_px = 900;

const float restart_time = 900.f;
const float popup_height = 0.2f;
const float radius_scale = 0.5f;	// for trackball

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

// The 'Transform' component handles transformations passed to the Vertex shader
// (similar to the gl Immediate mode equivalent, e.g., glTranslate()...)
// We recomment making all components non-copyable by derving from ComponentNonCopyable
struct Transform {
	mat3 mat = { { 1.f, 0.f, 0.f }, { 0.f, 1.f, 0.f}, { 0.f, 0.f, 1.f} }; // start with the identity
	void scale(vec2 scale);
	void rotate(float radians);
	void translate(vec2 offset);

	mat4 mat3D = { { 1.f, 0.f, 0.f, 0.f },
				  { 0.f, 1.f, 0.f, 0.f },
				  { 0.f, 0.f, 1.f, 0.f },
				  { 0.f, 0.f, 0.f, 1.f } };
};

bool gl_has_errors();

enum class Direction
{
	UP = 0,
	RIGHT = 1,
	DOWN = 2,
	LEFT = 3
};

Direction mod(Direction a, int b);

quat RotationBetweenVectors(vec3 start, vec3 dest);

typedef float (*TransFuncPtr)(float);

// green - cyan - white - black
const std::array<vec3, 5> controlTileColors = { 
	vec3(0.f, 1.f, 0.f),
	vec3(0.f, 1.f, 1.f),
	vec3(1.f, 1.f, 1.f),
	vec3(1.f, 0.5f, 0.f), 
	vec3(0.f, 0.f, 0.f)
};