#pragma once

#include <Core.h>
#include <Types.h>
#include <Utility.h>
#include <HotRef.h>
#include <Handle.h>
#include <Resource.h>
#include <Module.h>
#include <ModuleProxy.h>
#include <Actor.h>
#include <ActorProxy.h>
#include <Component.h>
#include <CollisionComponent.h>
#include <LightComponent.h>
#include <Renderer.h>
#include <Delegates.h>
#include <Window.h>

#include <Globals/Registry.h>
#include <Globals/World.h>

#include <Components/Collider/BoxCollider.h>
#include <Components/Collider/SphereCollider.h>

#include <Components/Lights/AmbientLight.h>
#include <Components/Lights/DirectionLight.h>
#include <Components/Lights/PointLight.h>
#include <Components/Lights/SpotLight.h>

#include <Components/Camera.h>
#include <Components/Renderable.h>
#include <Components/Rigidbody.h>
#include <Components/Transform.h>

#include <Handles/GenericBuffer.h>
#include <Handles/GenericFrameBuffer.h>
#include <Handles/GenericMesh.h>
#include <Handles/GenericProgram.h>
#include <Handles/GenericShader.h>
#include <Handles/GenericTexture.h>

#include <Renderer/DeferredRenderer.h>
#include <Renderer/DeferredRenderTask.h>

#include <Resources/ShaderResource.h>
#include <Resources/TextureResource.h>
#include <Resources/MeshResource.h>

#include <Uniforms/CameraUniform.h>
#include <Uniforms/LightUniform.h>
#include <Uniforms/ModelUniform.h>