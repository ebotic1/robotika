#version 460

layout(std140, binding = 1) uniform Colors {
    vec4 color[100];
};

out vec4 fragColor;
flat in uint instanceID;

void main() 
{
    fragColor = color[instanceID];
}

