/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Castor3DPrerequisites_H___
#define ___C3D_Castor3DPrerequisites_H___

#include "Castor3D/Animation/AnimationModule.hpp"
#include "Castor3D/Binary/BinaryModule.hpp"
#include "Castor3D/Buffer/BufferModule.hpp"
#include "Castor3D/Cache/CacheModule.hpp"
#include "Castor3D/Event/Frame/FrameEventModule.hpp"
#include "Castor3D/Event/UserInput/UserInputEventModule.hpp"
#include "Castor3D/Material/MaterialModule.hpp"
#include "Castor3D/Material/Pass/PassModule.hpp"
#include "Castor3D/Material/Texture/TextureModule.hpp"
#include "Castor3D/Material/Texture/Animation/TextureAnimationModule.hpp"
#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Model/ModelModule.hpp"
#include "Castor3D/Model/Mesh/MeshModule.hpp"
#include "Castor3D/Model/Mesh/Animation/MeshAnimationModule.hpp"
#include "Castor3D/Model/Mesh/Generator/MeshGeneratorModule.hpp"
#include "Castor3D/Model/Mesh/Submesh/SubmeshModule.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/ComponentModule.hpp"
#include "Castor3D/Model/Skeleton/SkeletonModule.hpp"
#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationModule.hpp"
#include "Castor3D/Overlay/OverlayModule.hpp"
#include "Castor3D/Plugin/PluginModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/Culling/CullingModule.hpp"
#include "Castor3D/Render/EnvironmentMap/EnvironmentMapModule.hpp"
#include "Castor3D/Render/GlobalIllumination/GlobalIlluminationModule.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightPropagationVolumesModule.hpp"
#include "Castor3D/Render/Node/RenderNodeModule.hpp"
#include "Castor3D/Render/Passes/PassesModule.hpp"
#include "Castor3D/Render/PBR/PbrModule.hpp"
#include "Castor3D/Render/PostEffect/PostEffectModule.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapModule.hpp"
#include "Castor3D/Render/Ssao/SsaoModule.hpp"
#include "Castor3D/Render/Technique/TechniqueModule.hpp"
#include "Castor3D/Render/Technique/Opaque/OpaqueModule.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/LightingModule.hpp"
#include "Castor3D/Render/Technique/Transparent/TransparentModule.hpp"
#include "Castor3D/Render/GlobalIllumination/VoxelConeTracing/VoxelizeModule.hpp"
#include "Castor3D/Render/ToneMapping/ToneMappingModule.hpp"
#include "Castor3D/Render/ToTexture/RenderToTextureModule.hpp"
#include "Castor3D/Scene/SceneModule.hpp"
#include "Castor3D/Scene/Animation/AnimationModule.hpp"
#include "Castor3D/Scene/Animation/Mesh/MeshAnimationModule.hpp"
#include "Castor3D/Scene/Animation/Skeleton/SkeletonAnimationModule.hpp"
#include "Castor3D/Scene/Animation/Texture/TextureAnimationModule.hpp"
#include "Castor3D/Scene/Background/BackgroundModule.hpp"
#include "Castor3D/Scene/Light/LightModule.hpp"
#include "Castor3D/Scene/ParticleSystem/ParticleModule.hpp"
#include "Castor3D/Shader/ShaderModule.hpp"
#include "Castor3D/Shader/ShaderBuffers/ShaderBuffersModule.hpp"
#include "Castor3D/Shader/Shaders/SdwModule.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include "Castor3D/Event/Frame/CpuFrameEvent.hpp"
#include "Castor3D/Event/Frame/GpuFrameEvent.hpp"
#include "Castor3D/Event/UserInput/UserInputEvent.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"

#include <CastorUtils/Design/Collection.hpp>
#include <CastorUtils/Graphics/Size.hpp>
#include <CastorUtils/Math/Quaternion.hpp>
#include <CastorUtils/Math/SquareMatrix.hpp>
#include <CastorUtils/Miscellaneous/StringUtils.hpp>

#include <ShaderWriter/Source.hpp>

#include <ashespp/Core/Device.hpp>

#include <RenderGraph/FrameGraphPrerequisites.hpp>

#endif
