#version 330 core

// Positions/Coordinates
layout (location = 0) in vec3 aPos;
// Texture Coordinates
layout (location = 1) in vec2 aTex;

// Outputs the texture coordinates to the fragment shader
out vec2 texCoord;

// Inputs the matrices needed for 3D viewing with perspective
uniform mat4 model;
uniform mat4 translate;
uniform mat4 scale;
uniform mat4 view;
uniform mat4 proj;

// Flag for animations
uniform bool animated = false;
uniform int sheet_length = 1;
uniform int index = 0;

void main()
{
	// Outputs the positions/coordinates of all vertices
	gl_Position = proj * view * translate * model * scale * vec4(aPos, 1.0);
	// Assigns the texture coordinates from the Vertex Data to "texCoord"
	if (animated) {
		texCoord = vec2((aTex.x + index)/ sheet_length, aTex.y);
	}
	else {
		texCoord = vec2(aTex.x, aTex.y);
	}
}