#version 450 core
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) out vec4 fragColor;
layout (location = 0) in vec2 colors;

void main()
{
  fragColor = vec4(colors,colors.x,1.0f);
}