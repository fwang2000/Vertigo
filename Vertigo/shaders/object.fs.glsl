#version 330

struct Material {
	// Gets the Texture Unit from the main function
    sampler2D diffuse;
    vec3 specular;    
    float shininess;
}; 

struct DirLight {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define NR_POINT_LIGHTS 5

// From Vertex Shader
in vec3 vcolor;
in vec3 fragPos; // Distance from local origin
in vec3 normal;

uniform vec3 viewPos;
uniform Material material;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform int numLights;
uniform float alpha;
uniform vec3 objColor;

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

// Output color
layout(location = 0) out vec4 color;

void main()
{
	// ambient
    vec3 ambient = dirLight.ambient * vcolor * objColor;
  	
    // diffuse 
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(dirLight.position - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = dirLight.diffuse * diff * vcolor;

    // specular
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = dirLight.specular * (spec * material.specular);  

	vec3 result = ambient + diffuse + specular;

    for(int i = 0; i < numLights; i++)
        result += CalcPointLight(pointLights[i], norm, fragPos, viewDir);

	color = vec4(result, alpha);
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient = light.ambient * vcolor;
    vec3 diffuse = light.diffuse * diff * vcolor;
    vec3 specular = light.specular * spec * material.specular;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}