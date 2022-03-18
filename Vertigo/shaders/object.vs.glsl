#version 330

// Input attributes
in vec3 in_position;
in vec3 in_color;

out vec3 vcolor;
out vec2 vpos;

// Inputs the matrices needed for 3D viewing with perspective
uniform mat4 model;
uniform mat4 translate;
uniform mat4 view;
uniform mat4 proj;

void main()
{
	vpos = in_position.xy; // local coordinated before transform
	vcolor = in_color;
	gl_Position = proj * view * translate * model * vec4(in_position.xyz, 1.0);
}