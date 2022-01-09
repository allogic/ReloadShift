#version 460 core

layout (binding = 0) uniform ProjectionUniform
{
  mat4 uProjection;
  mat4 uView;
  mat4 uModel;
};

layout (location = 0) in vec3 iPosition;
layout (location = 1) in vec3 iNormal;
layout (location = 2) in vec2 iUv;
layout (location = 3) in vec4 iColor;
layout (location = 4) in vec3 iTangent;

layout (location = 0) out VertOut
{
  vec3 position;
  vec3 normal;
  vec2 uv;
  vec4 color;
  vec3 tangent;
} vertOut;

void main()
{
  mat4 mvp = uProjection * uView * uModel;

  vertOut.position = vec4(uModel * vec4(iPosition, 1.0f)).xyz;
  vertOut.normal = iNormal;
  vertOut.uv = iUv;
  vertOut.color = iColor;
  vertOut.tangent = iTangent;

  gl_Position = mvp * vec4(iPosition, 1.0f);
}