#version 330 core

// Positions/Coordinates
layout (location = 0) in vec3 in_position;
// Color Coordinates
layout (location = 1) in vec3 in_color;

// Outputs the texture coordinates to the fragment shader
out vec3 vcolor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform float scale;

void main()
{
    vcolor = in_color;
    mat4 mv = view * model;
    mv[0][0] = scale;
    mv[1][1] = scale;
    gl_Position = proj * mv * vec4(in_position, 1.0);
}