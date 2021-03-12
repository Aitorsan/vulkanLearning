#version 450 core

layout (location = 0) out vec4 fragColor;
layout (location = 0) in vec3 col;

void main()
{
  fragColor = vec4(col,1.0);
}