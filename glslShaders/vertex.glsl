#version 450 core

vec2 positions[3] = vec2[](
  vec2(0,-0.5f),
  vec2(0.5,0.5f),
  vec2(-0.5,0.5)
);

layout (location = 0) out vec2 colors;

void main()
{

   gl_Position = vec4(positions[gl_VertexIndex],0.0f,1.0f);

   colors = positions[gl_VertexIndex];
}