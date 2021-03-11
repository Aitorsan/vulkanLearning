#version 450 core

layout (location = 0) out vec4 fragColor;
layout (location = 0) in vec2 position;
void main()
{
  fragColor = vec4(position.x,position.y,position.x,1.0);
}