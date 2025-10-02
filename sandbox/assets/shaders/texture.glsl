#type vertex
#version 330 core
layout(location=0)in vec3 a_Position;
layout(location=1)in vec4 a_Color;
layout(location=2)in vec2 a_TexCoord;

out vec2 v_TexCoord;
out vec4 v_Color;

uniform mat4 u_ProjectionView;
// uniform mat4 u_Transform;

void main()
{
    v_Color=a_Color;
    v_TexCoord=a_TexCoord;
    // gl_Position=u_ProjectionView*u_Transform*vec4(a_Position,1.);
    gl_Position=u_ProjectionView*vec4(a_Position,1.);
}

#type fragment
#version 330 core
layout(location=0)out vec4 color;

in vec4 v_Color;
in vec2 v_TexCoord;

uniform sampler2D u_Texture;
// uniform vec4 u_Color;
uniform float u_TilingFactor;

void main()
{
    // color=texture(u_Texture,v_TexCoord*u_TilingFactor)*u_Color;// 采样器 -> (纹理坐标 * 平铺因子) * 混合颜色
    color=v_Color;
}