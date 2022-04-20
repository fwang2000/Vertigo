#version 330 core

// Positions/Coordinates
layout (location = 0) in vec3 aPos;
// Texture Coordinates
layout (location = 1) in vec2 aTex;

// Outputs the texture coordinates to the fragment shader
out vec2 texCoord;

// Controls the scale of the vertices
uniform int index;

// Inputs the matrices needed for 3D viewing with perspective
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
	int row = index / 7;
	int column = index % 9;

	// Outputs the positions/coordinates of all vertices
	gl_Position = proj * view * model * vec4(aPos.xyz, 1.0);
	// Assigns the texture coordinates from the Vertex Data to "texCoord"
	texCoord = vec2((aTex.x + column)/ 9, (aTex.y + row)/ 7);
}