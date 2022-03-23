#version 330

// Input attributes
in vec3 in_position;
in vec3 in_color;
in vec3 in_normal;

out vec3 vcolor;
out vec3 fragPos;
out vec3 normal;

// Inputs the matrices needed for 3D viewing with perspective
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
	fragPos = in_position; // local coordinated before transform
	vcolor = in_color;
	normal = mat3(transpose(inverse(model))) * in_normal;
	gl_Position = proj * view * model * vec4(in_position.xyz, 1.0);
}