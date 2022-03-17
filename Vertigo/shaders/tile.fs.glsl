#version 330 core

// Outputs colors in RGBA
out vec4 FragColor;

struct Material {
	// Gets the Texture Unit from the main function
    sampler2D diffuse;
    vec3 specular;    
    float shininess;
}; 

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

// Inputs the texture coordinates from the Vertex Shader
in vec3 fragPos;
in vec2 texCoord;
in vec3 normal;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main()
{	
	// ambient
    vec3 ambient = light.ambient * texture(material.diffuse, texCoord).rgb;

	// diffuse 
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, texCoord).rgb;  
    
    // specular
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);  

	vec3 result = ambient + diffuse + specular;
	FragColor = vec4(result, 1.0);
}