#version 460 core

layout (binding = 0) uniform sampler2D uAlbedo;
layout (binding = 1) uniform sampler2D uNormal;
layout (binding = 2) uniform sampler2D uSpecular;
layout (binding = 3) uniform sampler2D uMetallic;
layout (binding = 4) uniform sampler2D uRoughness;

layout (location = 0) in VertOut
{
  vec3 Position;
  vec3 Normal;
  vec2 UV;
  vec4 Color;
  vec3 Tangent;
} fragIn;

layout (location = 0) out vec4 oPosition;
layout (location = 1) out vec4 oAlbedo;
layout (location = 2) out vec4 oNormal;
layout (location = 3) out vec4 oSpecular;
layout (location = 4) out vec4 oMetallic;
layout (location = 5) out vec4 oRoughness;

void main()
{
  oPosition = vec4(fragIn.UV, 0.0f, 1.0f);
  oAlbedo = texture(uAlbedo, fragIn.UV);
  oNormal = texture(uNormal, fragIn.UV);
  oSpecular = texture(uSpecular, fragIn.UV);
  oMetallic = texture(uMetallic, fragIn.UV);
  oRoughness = texture(uRoughness, fragIn.UV);
}