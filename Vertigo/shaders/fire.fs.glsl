#version 330

// From Vertex Shader
in vec3 vcolor;
in vec2 vpos; // Distance from local origin

// Application data
uniform sampler2D sampler0;
uniform vec3 fcolor;
uniform int index;

// Output color
layout(location = 0) out vec4 color;

void main()
{
	int row = index / 7;
	int column = index % 9;

	vec2 texCoord = vec2((vpos.x + column - 0.5)/9, (vpos.y + row - 0.5)/7);
	color = texture(sampler0, texCoord) * vec4(fcolor * vcolor, 1.0);
}