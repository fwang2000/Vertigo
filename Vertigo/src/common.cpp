#include "common.hpp"

// Note, we could also use the functions from GLM but we write the transformations here to show the uderlying math
void Transform::scale(vec2 scale)
{
	mat3 S = { { scale.x, 0.f, 0.f },{ 0.f, scale.y, 0.f },{ 0.f, 0.f, 1.f } };
	mat = mat * S;
}

void Transform::rotate(float radians)
{
	float c = cosf(radians);
	float s = sinf(radians);
	mat3 R = { { c, s, 0.f },{ -s, c, 0.f },{ 0.f, 0.f, 1.f } };
	mat = mat * R;
}

void Transform::translate(vec2 offset)
{
	mat3 T = { { 1.f, 0.f, 0.f },{ 0.f, 1.f, 0.f },{ offset.x, offset.y, 1.f } };
	mat = mat * T;
}

void Transform::rotate3D(ROTATION_AXIS axis, bool negative) {

	float s = (negative) ? -sinf(M_PI / 2) : sinf(M_PI / 2);

	switch (axis)
	{
	case ROTATION_AXIS::X_AXIS:
		mat4 r_x = { { 1.f, 0.f, 0.f, 0.f },
					 { 0.f, 0.f, s, 0.f } ,
					 { 0.f, -s, 0.f, 0.f } ,
					 { 0.f, 0.f, 0.f, 1.f } };
		mat3D = mat3D * r_x;
		break;
	case ROTATION_AXIS::Y_AXIS:
		mat4 r_y = { { 0.f, 0.f, -s, 0.f },
					 { 0.f, 1.f, 0.f, 0.f } ,
					 { s, 0.f, 0.f, 0.f } ,
					 { 0.f, 0.f, 0.f, 1.f } };
		mat3D = mat3D * r_y;
		break;
	case ROTATION_AXIS::Z_AXIS:
		mat4 r_z = { { 0.f, -s, 0.f, 0.f },
					 { s, 0.f, 0.f, 0.f } ,
					 { 0.f, 0.f, 1.f, 0.f } ,
					 { 0.f, 0.f, 0.f, 1.f } };
		mat3D = mat3D * r_z;
		break;
	default:
		break;
	}
}

bool gl_has_errors()
{
	GLenum error = glGetError();

	if (error == GL_NO_ERROR) return false;

	while (error != GL_NO_ERROR)
	{
		const char* error_str = "";
		switch (error)
		{
		case GL_INVALID_OPERATION:
			error_str = "INVALID_OPERATION";
			break;
		case GL_INVALID_ENUM:
			error_str = "INVALID_ENUM";
			break;
		case GL_INVALID_VALUE:
			error_str = "INVALID_VALUE";
			break;
		case GL_OUT_OF_MEMORY:
			error_str = "OUT_OF_MEMORY";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			error_str = "INVALID_FRAMEBUFFER_OPERATION";
			break;
		}

		fprintf(stderr, "OpenGL: %s", error_str);
		error = glGetError();
		assert(false);
	}

	return true;
}

Direction mod(Direction dir, int b)
{
	int a = static_cast<int>(dir) + b;
	return static_cast<Direction>((4 + (a % 4)) % 4);
}