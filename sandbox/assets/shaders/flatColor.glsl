// 四边形的基础shader
#type vertex
#version 330 core
layout(location=0)in vec3 a_Position;
out vec3 v_Position;

uniform mat4 u_ProjectionView;
uniform mat4 u_Transform;

void main()
{
    v_Position=a_Position;
    gl_Position=u_ProjectionView*u_Transform*vec4(a_Position,1.);
}

#type fragment
#version 330 core
layout(location=0)out vec4 color;
in vec3 v_Position;

uniform vec4 u_Color;

void main()
{
    color=u_Color;
}