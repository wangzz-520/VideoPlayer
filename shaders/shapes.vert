#version 330 core

layout (location = 0) in vec3 position; //位置坐标
layout (location = 1) in vec2 texCoord; //纹理坐标

out vec2 outTexCoord;

void main()
{
    gl_Position = vec4(position,1.0);
    vec2 tc = vec2(texCoord.x, 1.0 - texCoord.y);
    outTexCoord = tc;
}
