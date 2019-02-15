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
		class PhongLightingModel
			: public LightingModel
		{
		public:
			C3D_API PhongLightingModel( sdw::ShaderWriter & writer );
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
				, uint32_t & index
				, uint32_t maxCascades );
			C3D_API static std::shared_ptr< PhongLightingModel > createModel( sdw::ShaderWriter & writer
				, ShadowType shadows
				, bool volumetric
				, uint32_t & index
				, uint32_t maxCascades );
			C3D_API static std::shared_ptr< PhongLightingModel > createModel( sdw::ShaderWriter & writer
				, LightType light
				, ShadowType shadows
				, bool volumetric
				, uint32_t & index );
			C3D_API static void computeMapContributions( sdw::ShaderWriter & writer
				, shader::Utils const & utils
				, sdw::Vec3 & normal
				, sdw::Vec3 & diffuse
				, sdw::Vec3 & specular
				, sdw::Vec3 & emissive
				, sdw::Float & shininess
				, sdw::Float const & gamma
				, TextureChannels const & textureFlags
				, ProgramFlags const & programFlags
				, SceneFlags const & sceneFlags
				, PassFlags const & passFlags );

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
