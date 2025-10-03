#type vertex
#version 330 core
layout(location=0)in vec3 a_Position;
layout(location=1)in vec4 a_Color;
layout(location=2)in vec2 a_TexCoord;
layout(location=3)in float a_TexId;

out vec4 v_Color;
out vec2 v_TexCoord;
out float v_TexId;

uniform mat4 u_ProjectionView;

void main()
{
    v_Color=a_Color;
    v_TexCoord=a_TexCoord;
    v_TexId=a_TexId;
    gl_Position=u_ProjectionView*vec4(a_Position,1.);
}

#type fragment
#version 330 core
layout(location=0)out vec4 color;

in vec4 v_Color;
in vec2 v_TexCoord;
in float v_TexId;

uniform sampler2D u_Textures[32];

void main()
{
    // color=texture(u_Texture,v_TexCoord*u_TilingFactor)*u_Color;// 采样器 -> (纹理坐标 * 平铺因子) * 混合颜色
    color=texture(u_Textures[int(v_TexId)],v_TexCoord)*v_Color;
}