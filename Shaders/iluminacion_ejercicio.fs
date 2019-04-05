#version 330 core

// Cuadros oscuros
in vec3 our_color;
in vec3 our_normal;
in vec3 fragPos;
// out vec3 color;

struct PositionalLight
{
        vec3 ambient;
        vec3 diffuse;
        vec3 specular;
        vec3 position;
};

out vec4 color;

uniform PositionalLight light;
uniform vec3 viewPos;


void main()
{
    vec3 ambient = light.ambient * our_color;

    //Difusa
    vec3 normal = normalize(our_normal);
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * light.diffuse * our_color;

    // Vector de reflexión
    vec3 r = reflect(-lightDir, normal);
    vec3 viewDir = normalize(viewPos - fragPos);
    //Factor de brillo (potencia)
    float spec = pow(max(dot(r, viewDir), 0.0), 64.0);
    vec3 specular = spec * light.specular * our_color;

    //Especular

    //Agrega las componentes
    vec3 result = ambient + diffuse + specular;

	color = vec4(result, 1.0);
}