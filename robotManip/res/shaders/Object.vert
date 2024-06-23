#version 460
layout(std140, binding = 0) uniform Matrices {
    mat4 MVPmat[100];
};



in vec3 position;
flat out uint instanceID;



void main() 
{
    gl_Position = MVPmat[gl_BaseInstance] * vec4(position, 1.0);
    instanceID = gl_BaseInstance;
}

