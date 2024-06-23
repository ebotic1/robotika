#version 460

layout(std140, binding = 1) uniform Colors {
    vec4 color[100];
};

layout(std140, binding = 0) uniform Matrices {
    mat4 MVPmat[100];
};

in vec3 FragPos;  
in vec3 Fnormal;
out vec4 fragColor;
flat in uint instanceID;

// Fixed material properties
const vec3 materialAmbient = vec3(1.0, 0.5, 0.31);
const vec3 materialDiffuse = vec3(1.0, 0.5, 0.31);
const vec3 materialSpecular = vec3(0.5, 0.5, 0.5);
const float materialShininess = 4.0;

// Fixed light properties and direction
const vec3 lightPosition = vec3(1.2f, 1.0f, 2.0f);
const vec3 lightAmbient = vec3(0.2, 0.2, 0.2);
const vec3 lightDiffuse = vec3(0.5, 0.5, 0.5);
const vec3 lightSpecular = vec3(1.0, 1.0, 1.0);


void main() 
{
    

    // Ambient
    vec3 ambient = lightAmbient * materialAmbient;

    // Diffuse 
    vec3 norm = normalize(Fnormal);
    vec3 lightDir = normalize(lightPosition - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = lightDiffuse * (diff * materialDiffuse);

    // Specular
    vec3 viewDir = normalize(vec3(inverse((MVPmat[0]))[3]) - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), materialShininess);
    vec3 specular = lightSpecular * (spec * materialSpecular);

    vec3 result = (ambient + diffuse + specular) * vec3(color[instanceID]);
    fragColor = vec4(result, 1.0);

}

