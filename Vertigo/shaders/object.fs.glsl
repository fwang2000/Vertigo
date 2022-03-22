#version 330

// From Vertex Shader
in vec3 vcolor;
in vec2 vpos; // Distance from local origin

// Application data
uniform sampler2D mainTexture;
uniform vec3 fcolor;
uniform float alpha;

// Output color
layout(location = 0) out vec4 color;

void main()
{
	color = vec4(fcolor * vcolor, alpha);
}