#version 330

// From Vertex Shader
in vec3 vcolor;
in vec3 vpos; // Distance from local origin
in vec3 vnormal;

// Application data
uniform vec3 fcolor;

// Output color
layout(location = 0) out vec4 color;

void main()
{
	// ambient
	vec3 lightColor = vec3(1.0, 1.0, 1.0);
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
  	
    // diffuse 
    vec3 norm = normalize(vnormal);
    vec3 lightDir = normalize(vec3(0.0, 0.0, 2.5) - vpos); // TODO: replace the vec3 with the actual light position
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

	vec3 result = (ambient + diffuse) * vcolor;

	color = vec4(result, 1.0);
}