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
uniform mat4 translate;
uniform mat4 scale;
uniform mat4 view;
uniform mat4 proj;
uniform vec3 objColor;

void main()
{
	mat4 trueModel = translate * model * scale;
	fragPos = in_position; // local coordinated before transform
	vcolor = in_color;
	normal = mat3(transpose(inverse(trueModel))) * in_normal;
	gl_Position = proj * view * trueModel * vec4(in_position.xyz, 1.0);
}