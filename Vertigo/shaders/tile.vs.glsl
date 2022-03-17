#version 330 core

// Positions/Coordinates
layout (location = 0) in vec3 aPos;
// Texture Coordinates
layout (location = 1) in vec2 aTex;
// Normal
layout (location = 2) in vec3 aNormal;

// Outputs the texture coordinates to the fragment shader
out vec3 fragPos;
out vec2 texCoord;
out vec3 normal;

// Controls the scale of the vertices
uniform float scale;

// Inputs the matrices needed for 3D viewing with perspective
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

// Flag for burnable animations
// uniform int burned = 0;
// uniform float width = 375.0;
// uniform float height = 375.0;
// varying int counter;

void main()
{
	fragPos = vec3(model * vec4(aPos, 1.0));
	// Assigns the texture coordinates from the Vertex Data to "texCoord"
	texCoord = aTex;
	normal = mat3(transpose(inverse(model))) * aNormal;
	// if (burned == 1) {
	// 	if (counter == 5) {
	// 		counter = 0;
	// 	}
	// 	texCoord.x = texCoord.x + (width*counter);
	// 	counter += 1;
	// }

	// Outputs the positions/coordinates of all vertices
	gl_Position = proj * view * model * vec4(aPos, 1.0);
}