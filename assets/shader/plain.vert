#version 330
in vec3 vPosition;
layout(location = 3) in vec4 Color;

smooth out vec4 theColor;

uniform mat4 MVP;

void main()
{
    //gl_Position = vec4(position, 1.0);
    gl_Position = MVP * vec4(vPosition, 1.0);
    theColor = Color;
}