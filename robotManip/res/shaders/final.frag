//Created by IDz on 20240116
#version 410

in vec2 fragTexCoord;
uniform sampler2D textureSampler;
out vec4 fragColor;

void main() 
{
    fragColor = texture(textureSampler, fragTexCoord)*vec4(1.0, 1.0, 1.0, 0.0)+vec4(0.0, 0.0, 0.0, 1.0);
}