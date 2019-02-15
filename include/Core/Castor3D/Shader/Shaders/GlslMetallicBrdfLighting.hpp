/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslMetallicBrdfLighting_H___
#define ___C3D_GlslMetallicBrdfLighting_H___

#include "Castor3D/Shader/Shaders/GlslLighting.hpp"

namespace castor3d
{
	namespace shader
	{
		class MetallicBrdfLightingModel
			: public LightingModel
		{
		public:
			C3D_API explicit MetallicBrdfLightingModel( sdw::ShaderWriter & writer );
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
			C3D_API static std::shared_ptr< MetallicBrdfLightingModel > createModel( sdw::ShaderWriter & writer
				, uint32_t & index
				, uint32_t maxCascades );
			C3D_API static std::shared_ptr< MetallicBrdfLightingModel > createModel( sdw::ShaderWriter & writer
				, ShadowType shadows
				, bool volumetric
				, uint32_t & index
				, uint32_t maxCascades );
			C3D_API static std::shared_ptr< MetallicBrdfLightingModel > createModel( sdw::ShaderWriter & writer
				, LightType light
				, ShadowType shadows
				, bool volumetric
				, uint32_t & index );
			C3D_API static void computeMapContributions( sdw::ShaderWriter & writer
				, shader::Utils const & utils
				, sdw::Vec3 & normal
				, sdw::Vec3 & albedo
				, sdw::Float & metallic
				, sdw::Vec3 & emissive
				, sdw::Float & roughness
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
				, sdw::Vec3 const & direction
				, sdw::Vec3 const & albedo
				, sdw::Float const & metallic
				, sdw::Float const & roughness
				, sdw::Float const & shadowFactor
				, FragmentInput const & fragmentIn
				, OutputComponents & output )const;

			void doDeclareDistribution();
			void doDeclareGeometry();
			void doDeclareFresnelShlick();
			void doDeclareComputeLight();

		public:
			C3D_API static const castor::String Name;
			sdw::Function< sdw::Float
				, sdw::InFloat
				, sdw::InFloat > m_distributionGGX;
			sdw::Function< sdw::Float
				, sdw::InFloat
				, sdw::InFloat > m_geometrySchlickGGX;
			sdw::Function< sdw::Float
				, sdw::InFloat
				, sdw::InFloat
				, sdw::InFloat > m_geometrySmith;
			sdw::Function< sdw::Float
				, sdw::InVec3
				, sdw::InVec3
				, sdw::InVec3
				, sdw::InFloat > m_smith;
			sdw::Function< sdw::Vec3
				, sdw::InFloat
				, sdw::InVec3 > m_schlickFresnel;
			sdw::Function< sdw::Vec3
				, sdw::InFloat
				, sdw::InFloat
				, sdw::InFloat
				, sdw::InFloat
				, sdw::InVec3 > m_cookTorrance;
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
			sdw::Function< sdw::Void
				, InPointLight
				, sdw::InVec3
				, sdw::InVec3
				, sdw::InFloat
				, sdw::InFloat
				, sdw::InInt
				, FragmentInput
				, OutputComponents & > m_computeOnePoint;
			sdw::Function< sdw::Void
				, InSpotLight
				, sdw::InVec3
				, sdw::InVec3
				, sdw::InFloat
				, sdw::InFloat
				, sdw::InInt
				, FragmentInput
				, OutputComponents & > m_computeOneSpot;
			sdw::Function< sdw::Void
				, InLight
				, sdw::InVec3
				, sdw::InVec3
				, sdw::InVec3
				, sdw::InFloat
				, sdw::InFloat
				, sdw::InFloat
				, FragmentInput
				, OutputComponents & > m_computeLight;
		};
	}
}

#endif
