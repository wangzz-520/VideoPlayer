#version 330 core

out vec4 color;

uniform sampler2D tex_y;
uniform sampler2D tex_u;
uniform sampler2D tex_v;

in vec2 outTexCoord;

void main()
{
    vec3 yuv;
    vec3 rgb;
    yuv.x = texture2D(tex_y, outTexCoord).g;
    yuv.y = texture2D(tex_u, outTexCoord).g - 0.5;
    yuv.z = texture2D(tex_v, outTexCoord).g - 0.5;
    rgb = mat3( 1,       1,         1,
                0,       -0.39465,  2.03211,
                1.13983, -0.58060,  0) * yuv; 

    color = vec4(rgb, 1);
}
