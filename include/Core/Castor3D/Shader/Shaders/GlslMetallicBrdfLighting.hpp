/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslMetallicBrdfLightingModel_H___
#define ___C3D_GlslMetallicBrdfLightingModel_H___

#include "Castor3D/Render/RenderModule.hpp"

#include "Castor3D/Shader/Shaders/GlslCookTorranceBRDF.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"

namespace castor3d
{
	namespace shader
	{
		class MetallicBrdfLightingModel
			: public LightingModel
		{
		public:
			C3D_API MetallicBrdfLightingModel( sdw::ShaderWriter & writer
				, Utils & utils
				, ShadowOptions shadowOptions
				, bool isOpaqueProgram );
			C3D_API void computeCombined( sdw::Vec3 const & worldEye
				, sdw::Vec3 const & albedo
				, sdw::Float const & metallic
				, sdw::Float const & roughness
				, sdw::Int const & receivesShadows
				, FragmentInput const & fragmentIn
				, OutputComponents & output )const;
			C3D_API void compute( DirectionalLight const & light
				, sdw::Vec3 const & worldEye
				, sdw::Vec3 const & albedo
				, sdw::Float const & metallic
				, sdw::Float const & roughness
				, sdw::Int const & receivesShadows
				, FragmentInput const & fragmentIn
				, OutputComponents & output )const;
			C3D_API void compute( PointLight const & light
				, sdw::Vec3 const & worldEye
				, sdw::Vec3 const & albedo
				, sdw::Float const & metallic
				, sdw::Float const & roughness
				, sdw::Int const & receivesShadows
				, FragmentInput const & fragmentIn
				, OutputComponents & output )const;
			C3D_API void compute( SpotLight const & light
				, sdw::Vec3 const & worldEye
				, sdw::Vec3 const & albedo
				, sdw::Float const & metallic
				, sdw::Float const & roughness
				, sdw::Int const & receivesShadows
				, FragmentInput const & fragmentIn
				, OutputComponents & output )const;
			C3D_API sdw::Vec3 computeCombinedDiffuse( sdw::Vec3 const & worldEye
				, sdw::Vec3 const & albedo
				, sdw::Float const & metallic
				, sdw::Float const & roughness
				, sdw::Int const & receivesShadows
				, FragmentInput const & fragmentIn )const;
			C3D_API sdw::Vec3 computeDiffuse( DirectionalLight const & light
				, sdw::Vec3 const & worldEye
				, sdw::Vec3 const & albedo
				, sdw::Float const & metallic
				, sdw::Float const & roughness
				, sdw::Int const & receivesShadows
				, FragmentInput const & fragmentIn )const;
			C3D_API sdw::Vec3 computeDiffuse( PointLight const & light
				, sdw::Vec3 const & worldEye
				, sdw::Vec3 const & albedo
				, sdw::Float const & metallic
				, sdw::Float const & roughness
				, sdw::Int const & receivesShadows
				, FragmentInput const & fragmentIn )const;
			C3D_API sdw::Vec3 computeDiffuse( SpotLight const & light
				, sdw::Vec3 const & worldEye
				, sdw::Vec3 const & albedo
				, sdw::Float const & metallic
				, sdw::Float const & roughness
				, sdw::Int const & receivesShadows
				, FragmentInput const & fragmentIn )const;
			C3D_API static std::shared_ptr< MetallicBrdfLightingModel > createModel( sdw::ShaderWriter & writer
				, Utils & utils
				, SceneFlags sceneFlags
				, bool rsm
				, uint32_t & index
				, bool isOpaqueProgram );
			C3D_API static std::shared_ptr< MetallicBrdfLightingModel > createModel( sdw::ShaderWriter & writer
				, Utils & utils
				, LightType light
				, bool lightUbo
				, bool shadows
				, bool rsm
				, uint32_t & index );
			C3D_API static std::shared_ptr< MetallicBrdfLightingModel > createDiffuseModel( sdw::ShaderWriter & writer
				, Utils & utils
				, SceneFlags sceneFlags
				, bool rsm
				, uint32_t & index
				, bool isOpaqueProgram );
			C3D_API void computeMapContributions( PipelineFlags const & flags
				, sdw::Float const & gamma
				, TextureConfigurations const & textureConfigs
				, sdw::Array< sdw::UVec4 > const & textureConfig
				, sdw::Array< sdw::SampledImage2DRgba32 > const & maps
				, sdw::Vec3 const & texCoords
				, sdw::Vec3 & normal
				, sdw::Vec3 & tangent
				, sdw::Vec3 & bitangent
				, sdw::Vec3 & emissive
				, sdw::Float & opacity
				, sdw::Float & occlusion
				, sdw::Float & transmittance
				, sdw::Vec3 & albedo
				, sdw::Float & metallic
				, sdw::Float & roughness
				, sdw::Vec3 & tangentSpaceViewPosition
				, sdw::Vec3 & tangentSpaceFragPosition );
			C3D_API void computeMapVoxelContributions( PipelineFlags const & flags
				, sdw::Float const & gamma
				, TextureConfigurations const & textureConfigs
				, sdw::Array< sdw::UVec4 > const & textureConfig
				, sdw::Array< sdw::SampledImage2DRgba32 > const & maps
				, sdw::Vec3 const & texCoords
				, sdw::Vec3 & emissive
				, sdw::Float & opacity
				, sdw::Float & occlusion
				, sdw::Vec3 & albedo
				, sdw::Float & metallic
				, sdw::Float & roughness );

		protected:
			void doDeclareModel()override;
			void doDeclareComputeDirectionalLight()override;
			void doDeclareComputePointLight()override;
			void doDeclareComputeSpotLight()override;
			void doDeclareDiffuseModel()override;
			void doDeclareComputeDirectionalLightDiffuse()override;
			void doDeclareComputePointLightDiffuse()override;
			void doDeclareComputeSpotLightDiffuse()override;

		public:
			C3D_API static const castor::String Name;
			CookTorranceBRDF m_cookTorrance;
			sdw::Function< sdw::Void
				, InDirectionalLight
				, sdw::InVec3
				, sdw::InVec3
				, sdw::InFloat
				, sdw::InFloat
				, sdw::InInt
				, FragmentInput
				, OutputComponents & > m_computeDirectional;
			sdw::Function< sdw::Void
				, InPointLight
				, sdw::InVec3
				, sdw::InVec3
				, sdw::InFloat
				, sdw::InFloat
				, sdw::InInt
				, FragmentInput
				, OutputComponents & > m_computePoint;
			sdw::Function< sdw::Void
				, InSpotLight
				, sdw::InVec3
				, sdw::InVec3
				, sdw::InFloat
				, sdw::InFloat
				, sdw::InInt
				, FragmentInput
				, OutputComponents & > m_computeSpot;
			sdw::Function< sdw::Vec3
				, InDirectionalLight
				, sdw::InVec3
				, sdw::InVec3
				, sdw::InFloat
				, sdw::InFloat
				, sdw::InInt
				, FragmentInput > m_computeDirectionalDiffuse;
			sdw::Function< sdw::Vec3
				, InPointLight
				, sdw::InVec3
				, sdw::InVec3
				, sdw::InFloat
				, sdw::InFloat
				, sdw::InInt
				, FragmentInput > m_computePointDiffuse;
			sdw::Function< sdw::Vec3
				, InSpotLight
				, sdw::InVec3
				, sdw::InVec3
				, sdw::InFloat
				, sdw::InFloat
				, sdw::InInt
				, FragmentInput > m_computeSpotDiffuse;
		};
	}
}

#endif
