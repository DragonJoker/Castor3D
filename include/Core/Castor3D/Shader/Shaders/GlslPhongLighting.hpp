/*
See LICENSE file in root folder
*/
#ifndef ___GLSL_PhongLighting_H___
#define ___GLSL_PhongLighting_H___

#include "Castor3D/Shader/Shaders/GlslLighting.hpp"

namespace castor3d
{
	namespace shader
	{
		class TextureConfigurations;

		class PhongLightingModel
			: public LightingModel
		{
		public:
			C3D_API PhongLightingModel( sdw::ShaderWriter & writer
				, Utils & utils
				, bool isOpaqueProgram );
			C3D_API void computeCombined( sdw::Vec3 const & worldEye
				, sdw::Float const & shininess
				, sdw::Int const & receivesShadows
				, FragmentInput const & fragmentIn
				, OutputComponents & output )const;
			C3D_API void compute( DirectionalLight const & light
				, sdw::Vec3 const & worldEye
				, sdw::Float const & shininess
				, sdw::Int const & receivesShadows
				, FragmentInput const & fragmentIn
				, OutputComponents & output )const;
			C3D_API void compute( PointLight const & light
				, sdw::Vec3 const & worldEye
				, sdw::Float const & shininess
				, sdw::Int const & receivesShadows
				, FragmentInput const & fragmentIn
				, OutputComponents & output )const;
			C3D_API void compute( SpotLight const & light
				, sdw::Vec3 const & worldEye
				, sdw::Float const & shininess
				, sdw::Int const & receivesShadows
				, FragmentInput const & fragmentIn
				, OutputComponents & output )const;
			C3D_API static std::shared_ptr< PhongLightingModel > createModel( sdw::ShaderWriter & writer
				, Utils & utils
				, uint32_t & index
				, uint32_t maxCascades
				, bool isOpaqueProgram );
			C3D_API static std::shared_ptr< PhongLightingModel > createModel( sdw::ShaderWriter & writer
				, Utils & utils
				, ShadowType shadows
				, bool volumetric
				, uint32_t & index
				, uint32_t maxCascades );
			C3D_API static std::shared_ptr< PhongLightingModel > createModel( sdw::ShaderWriter & writer
				, Utils & utils
				, LightType light
				, ShadowType shadows
				, bool volumetric
				, uint32_t & index );
			C3D_API static void computeMapContributions( sdw::ShaderWriter & writer
				, shader::Utils const & utils
				, PipelineFlags const & flags
				, sdw::Float const & gamma
				, TextureConfigurations const & textureConfigs
				, sdw::Array< sdw::UVec4 > const & textureConfig
				, sdw::Vec3 const & tangent
				, sdw::Vec3 const & bitangent
				, sdw::Array< sdw::SampledImage2DRgba32 > const & maps
				, sdw::Vec3 const & texCoords
				, sdw::Vec3 & normal
				, sdw::Vec3 & diffuse
				, sdw::Vec3 & specular
				, sdw::Vec3 & emissive
				, sdw::Float & shininess
				, sdw::Float & opacity
				, sdw::Float & occlusion
				, sdw::Float & transmittance );

		protected:
			void doDeclareModel()override;
			void doDeclareComputeDirectionalLight()override;
			void doDeclareComputePointLight()override;
			void doDeclareComputeSpotLight()override;
			void doDeclareComputeOneDirectionalLight( ShadowType shadowType
				, bool volumetric )override;
			void doDeclareComputeOnePointLight( ShadowType shadowType
				, bool volumetric )override;
			void doDeclareComputeOneSpotLight( ShadowType shadowType
				, bool volumetric )override;

			void doComputeLight( Light const & light
				, sdw::Vec3 const & worldEye
				, sdw::Vec3 const & lightDirection
				, sdw::Float const & shininess
				, sdw::Float const & shadowFactor
				, FragmentInput const & fragmentIn
				, OutputComponents & output );
			void doDeclareComputeLight();

		public:
			C3D_API static const castor::String Name;
			sdw::Function< sdw::Void
				, InLight
				, sdw::InVec3
				, sdw::InVec3
				, sdw::InFloat
				, sdw::InFloat
				, FragmentInput
				, OutputComponents & > m_computeLight;
			sdw::Function< sdw::Void
				, InDirectionalLight
				, sdw::InVec3
				, sdw::InFloat
				, sdw::InInt
				, FragmentInput
				, OutputComponents & > m_computeDirectional;
			sdw::Function< sdw::Void
				, InPointLight
				, sdw::InVec3
				, sdw::InFloat
				, sdw::InInt
				, FragmentInput
				, OutputComponents & > m_computePoint;
			sdw::Function< sdw::Void
				, InSpotLight
				, sdw::InVec3
				, sdw::InFloat
				, sdw::InInt
				, FragmentInput
				, OutputComponents & > m_computeSpot;
			sdw::Function< sdw::Void
				, InPointLight
				, sdw::InVec3
				, sdw::InFloat
				, sdw::InInt
				, FragmentInput
				, OutputComponents & > m_computeOnePoint;
			sdw::Function< sdw::Void
				, InSpotLight
				, sdw::InVec3
				, sdw::InFloat
				, sdw::InInt
				, FragmentInput
				, OutputComponents & > m_computeOneSpot;
		};
	}
}

#endif
