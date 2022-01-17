#version 460 core

////////////////////////////////////////////////////////
// Uniform buffer
////////////////////////////////////////////////////////

layout (binding = 0) uniform CameraUniform
{
  mat4 uProjection;
  mat4 uView;
};
layout (binding = 1) uniform ModelUniform
{
  mat4 uModel;
};

////////////////////////////////////////////////////////
// Vertex inputs
////////////////////////////////////////////////////////

layout (location = 0) in vec3 iPosition;
layout (location = 1) in vec3 iNormal;
layout (location = 2) in vec2 iUV;
layout (location = 3) in vec4 iColor;
layout (location = 4) in vec3 iTangent;

////////////////////////////////////////////////////////
// Vertex outputs
////////////////////////////////////////////////////////

layout (location = 0) out VertOut
{
  vec3 Position;
  vec3 Normal;
  vec2 UV;
  vec4 Color;
  vec3 Tangent;
} vertOut;

////////////////////////////////////////////////////////
// Entry point
////////////////////////////////////////////////////////

void main()
{
  mat4 mvp = uProjection * uView * uModel;

  vertOut.Position = vec4(uModel * vec4(iPosition, 1.0f)).xyz;
  vertOut.Normal = iNormal;
  vertOut.UV = iUV;
  vertOut.Color = iColor;
  vertOut.Tangent = iTangent;

  gl_Position = mvp * vec4(iPosition, 1.0f);
}