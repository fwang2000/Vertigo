#version 330 core

// Outputs colors in RGBA
out vec4 FragColor;

// Inputs the texture coordinates from the Vertex Shader
in vec3 vcolor;

void main()
{
	FragColor = vec4(vcolor, 1.0);
}