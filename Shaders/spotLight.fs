#version 330 core

struct Light {
    vec3 position;
	vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	
	float cutOff;
	float outerCutOff;

    float constant;
    float linear;
    float quadratic;
};

out vec4 color;

in vec3 fragPos;  
in vec3 our_normal;
in vec2 our_uv;
 
uniform vec3 viewPos;
uniform Light light;  
uniform sampler2D texture1;

void main()
{
	vec3 lightDir = normalize(light.position - fragPos);
    float theta = dot(lightDir, normalize(-light.direction));
        
    float epsilon   = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);   
    
	// Ambient
    vec3 ambient  = light.ambient * vec3(texture(texture1, our_uv));
  	
    // Diffuse 
    vec3 normal = normalize(our_normal);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse  = light.diffuse * (diff * vec3(texture(texture1, our_uv)));
    
    // Specular
    float specularStrength = 0.5f;
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = light.specular * (spec * vec3(texture(texture1, our_uv)));  
	
	diffuse *= intensity;
	specular *= intensity;
        
    float distance = length(light.position - fragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * distance * distance);

    //ambient *= attenuation;
    //diffuse *= attenuation;
    //specular *= attenuation;

    vec3 result = ambient + diffuse + specular;
    color = vec4(result, 1.0f);
}