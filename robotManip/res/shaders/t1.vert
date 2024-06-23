#version 460
layout(std140, binding = 0) uniform Matrices {
    mat4 MVPmat[100];
};


uniform mat4 VP;

in vec3 position;
in vec3 normal;
out vec3 FragPos;  
out vec3 Fnormal;

out vec3 normalOut;
flat out uint instanceID;



void main() 
{
    gl_Position = VP * MVPmat[gl_BaseInstance] * vec4(position, 1.0);
    FragPos = vec3(MVPmat[gl_BaseInstance] * vec4(position, 1.0));
    Fnormal  = mat3(transpose(inverse(MVPmat[0]))) * normal;
    instanceID = gl_BaseInstance;
}

