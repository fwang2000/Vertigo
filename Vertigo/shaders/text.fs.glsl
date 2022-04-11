#version 330 core

// Outputs colors in RGBA
out vec4 FragColor;

// Inputs the texture coordinates from the Vertex Shader
in vec2 texCoord;
uniform vec3 fcolor;

// Gets the Texture Unit from the main function
uniform sampler2D tex0;

void main()
{
	vec4 texColor = texture(tex0, texCoord);
	if (texColor.a < 0.1) {
		discard;
	}

	FragColor = texture(tex0, texCoord);
}