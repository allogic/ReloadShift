#version 460 core

////////////////////////////////////////////////////////
// Constants
////////////////////////////////////////////////////////

#define MAX_AMBIENT_LIGHTS 32
#define MAX_DIRECTION_LIGHTS 32
#define MAX_POINT_LIGHTS 32
#define MAX_SPOT_LIGHTS 32

////////////////////////////////////////////////////////
// Light types
////////////////////////////////////////////////////////

struct AmbientLight
{
  float Intensity;
  vec3 Color;
  uint Enabled;
};
struct DirectionLight
{
  float Intensity;
  vec3 Color;
  vec3 Direction;
  uint Enabled;
};
struct PointLight
{
  float Intensity;
  vec3 Color;
  vec3 Position;
  uint Enabled;
};
struct SpotLight
{
  float Intensity;
  float CutOff;
  vec3 Color;
  vec3 Position;
  vec3 Direction;
  uint Enabled;
};

////////////////////////////////////////////////////////
// Uniform sampler
////////////////////////////////////////////////////////

layout (binding = 0) uniform sampler2D uAlbedo;
layout (binding = 1) uniform sampler2D uNormal;
layout (binding = 2) uniform sampler2D uSpecular;
layout (binding = 3) uniform sampler2D uMetallic;
layout (binding = 4) uniform sampler2D uRoughness;
layout (binding = 5) uniform sampler2D uAmbientOcclusion;

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
layout (binding = 2) uniform AmbientLightUniform
{
  AmbientLight uAmbientLights[MAX_AMBIENT_LIGHTS];
};
layout (binding = 3) uniform DirectionLightUniform
{
  DirectionLight uDirectionLights[MAX_DIRECTION_LIGHTS];
};
layout (binding = 4) uniform PointLightUniform
{
  PointLight uPointLights[MAX_POINT_LIGHTS];
};
layout (binding = 5) uniform SpotLightUniform
{
  SpotLight uSpotLights[MAX_SPOT_LIGHTS];
};

////////////////////////////////////////////////////////
// Fragment inputs
////////////////////////////////////////////////////////

layout (location = 0) in VertOut
{
  vec3 Position;
  vec3 Normal;
  vec2 UV;
  vec4 Color;
  vec3 Tangent;
} fragIn;

////////////////////////////////////////////////////////
// Fragment outputs
////////////////////////////////////////////////////////

layout (location = 0) out vec4 oLit;
layout (location = 1) out vec4 oUnlit;

layout (location = 2) out vec4 oNormal;
layout (location = 3) out vec4 oSpecular;
layout (location = 4) out vec4 oMetallic;
layout (location = 5) out vec4 oRoughness;
layout (location = 6) out vec4 oAmbientOcclusion;
layout (location = 7) out vec4 oWorldTangent;
layout (location = 8) out vec4 oWorldNormal;
layout (location = 9) out vec4 oDepth;

////////////////////////////////////////////////////////
// Entry point
////////////////////////////////////////////////////////

void main()
{
  vec3 worldPosition = fragIn.Position;
  vec3 worldNormal = normalize(fragIn.Normal);

  vec4 texAlbedo = texture(uAlbedo, fragIn.UV);
  vec4 texNormal = texture(uNormal, fragIn.UV);
  vec4 texSpecular = texture(uSpecular, fragIn.UV);
  vec4 texMetallic = texture(uMetallic, fragIn.UV);
  vec4 texRoughness = texture(uRoughness, fragIn.UV);
  vec4 texAmbientOcclusion = texture(uAmbientOcclusion, fragIn.UV);

  vec3 lit = texAlbedo.xyz;
  vec3 unlit = texAlbedo.xyz;

  for (uint i = 0; i < MAX_POINT_LIGHTS; ++i)
  {
    if (uPointLights[i].Enabled == 0) continue;

    vec3 lightDelta = uPointLights[i].Position - worldPosition;
    vec3 lightDir = normalize(lightDelta);

    float lightDistance = length(lightDelta);
    float lightIntensity = max(dot(worldNormal, lightDir), 0.0f);
    float lightAttenuation = 1.0f / (1.0f + (0.7f * lightDistance) + (1.8f * lightDistance * lightDistance));

    lit = uPointLights[i].Color; // * lightIntensity * lightAttenuation;
  }

  oLit = vec4(lit, 1.0f);
  oUnlit = vec4(unlit, 1.0f);

  oNormal = vec4(worldNormal, 1.0f);
  oSpecular = texSpecular;
  oMetallic = texMetallic;
  oRoughness = texRoughness;
  oAmbientOcclusion = texAmbientOcclusion;
  oWorldTangent = vec4(0.0f, 1.0f, 0.0f, 1.0f);
  oWorldNormal = vec4(0.0f, 1.0f, 0.0f, 1.0f);
  oDepth = vec4(0.0f, 1.0f, 0.0f, 1.0f);
}