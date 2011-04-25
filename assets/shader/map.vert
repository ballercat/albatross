#version 330

in vec3 vPosition;
in vec4 vColor;
layout(location = 4) in vec2 vUV;

smooth out vec2 vTexCoord;
smooth out vec4 theColor;

uniform mat4 MVP;

void main()
{
    //gl_Position = vec4(position, 1.0);
    gl_Position = MVP * vec4(vPosition, 1.0);
    theColor = vColor;
    vTexCoord = vUV;
}