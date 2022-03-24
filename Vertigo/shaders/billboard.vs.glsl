#version 330 core

// Positions/Coordinates
layout (location = 0) in vec3 aPos;
// Texture Coordinates
layout (location = 1) in vec2 aTex;

// Outputs the texture coordinates to the fragment shader
out vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform float scale;

void main()
{
    texCoord = aTex;
    mat4 mv = view * model;
    mv[0][0] = scale;
    mv[1][1] = scale;
    gl_Position = proj * mv * vec4(aPos, 1.0);
}