/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslPhongLightingModel_H___
#define ___C3D_GlslPhongLightingModel_H___

#include "Castor3D/Shader/Shaders/GlslLighting.hpp"

#include "Castor3D/Render/RenderModule.hpp"

namespace castor3d
{
	namespace shader
	{
		class PhongLightingModel
			: public LightingModel
		{
		public:
			C3D_API PhongLightingModel( sdw::ShaderWriter & writer
				, Utils & utils
				, ShadowOptions shadowOptions
				, bool isOpaqueProgram );
			C3D_API void computeCombined( sdw::Vec3 const & worldEye
				, sdw::Float const & shininess
				, sdw::Int const & receivesShadows
				, Surface surface
				, OutputComponents & output )const;
			C3D_API void compute( DirectionalLight const & light
				, sdw::Vec3 const & worldEye
				, sdw::Float const & shininess
				, sdw::Int const & receivesShadows
				, Surface surface
				, OutputComponents & output )const;
			C3D_API void compute( PointLight const & light
				, sdw::Vec3 const & worldEye
				, sdw::Float const & shininess
				, sdw::Int const & receivesShadows
				, Surface surface
				, OutputComponents & output )const;
			C3D_API void compute( SpotLight const & light
				, sdw::Vec3 const & worldEye
				, sdw::Float const & shininess
				, sdw::Int const & receivesShadows
				, Surface surface
				, OutputComponents & output )const;
			C3D_API sdw::Vec3 computeCombinedDiffuse( sdw::Vec3 const & worldEye
				, sdw::Float const & shininess
				, sdw::Int const & receivesShadows
				, Surface surface )const;
			C3D_API sdw::Vec3 computeDiffuse( DirectionalLight const & light
				, sdw::Vec3 const & worldEye
				, sdw::Float const & shininess
				, sdw::Int const & receivesShadows
				, Surface surface )const;
			C3D_API sdw::Vec3 computeDiffuse( PointLight const & light
				, sdw::Vec3 const & worldEye
				, sdw::Float const & shininess
				, sdw::Int const & receivesShadows
				, Surface surface )const;
			C3D_API sdw::Vec3 computeDiffuse( SpotLight const & light
				, sdw::Vec3 const & worldEye
				, sdw::Float const & shininess
				, sdw::Int const & receivesShadows
				, Surface surface )const;
			C3D_API static std::shared_ptr< PhongLightingModel > createModel( sdw::ShaderWriter & writer
				, Utils & utils
				, ShadowOptions const & shadows
				, uint32_t & index
				, bool isOpaqueProgram );
			C3D_API static std::shared_ptr< PhongLightingModel > createModel( sdw::ShaderWriter & writer
				, Utils & utils
				, LightType light
				, bool lightUbo
				, ShadowOptions const & shadows
				, uint32_t & index );
			C3D_API static std::shared_ptr< PhongLightingModel > createDiffuseModel( sdw::ShaderWriter & writer
				, Utils & utils
				, ShadowOptions const & shadows
				, uint32_t & index
				, bool isOpaqueProgram );
			C3D_API void computeMapContributions( PassFlags const & passFlags
				, FilteredTextureFlags const & textures
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
				, sdw::Vec3 & diffuse
				, sdw::Vec3 & specular
				, sdw::Float & shininess
				, sdw::Vec3 & tangentSpaceViewPosition
				, sdw::Vec3 & tangentSpaceFragPosition );
			C3D_API void computeMapVoxelContributions( PassFlags const & passFlags
				, FilteredTextureFlags const & textures
				, sdw::Float const & gamma
				, TextureConfigurations const & textureConfigs
				, sdw::Array< sdw::UVec4 > const & textureConfig
				, sdw::Array< sdw::SampledImage2DRgba32 > const & maps
				, sdw::Vec3 const & texCoords
				, sdw::Vec3 & emissive
				, sdw::Float & opacity
				, sdw::Float & occlusion
				, sdw::Vec3 & diffuse
				, sdw::Vec3 & specular
				, sdw::Float & shininess );

		protected:
			void doDeclareModel()override;
			void doDeclareComputeDirectionalLight()override;
			void doDeclareComputePointLight()override;
			void doDeclareComputeSpotLight()override;
			void doDeclareDiffuseModel()override;
			void doDeclareComputeDirectionalLightDiffuse()override;
			void doDeclareComputePointLightDiffuse()override;
			void doDeclareComputeSpotLightDiffuse()override;

			void doComputeLight( Light const & light
				, sdw::Vec3 const & worldEye
				, sdw::Vec3 const & lightDirection
				, sdw::Float const & shininess
				, Surface surface
				, OutputComponents & output );
			void doDeclareComputeLight();
			sdw::Vec3 doComputeLightDiffuse( Light const & light
				, sdw::Vec3 const & worldEye
				, sdw::Vec3 const & lightDirection
				, sdw::Float const & shininess
				, Surface surface );
			void doDeclareComputeLightDiffuse();

		public:
			C3D_API static const castor::String Name;
			sdw::Function< sdw::Void
				, InLight
				, sdw::InVec3
				, sdw::InVec3
				, sdw::InFloat
				, InSurface
				, OutputComponents & > m_computeLight;
			sdw::Function< sdw::Void
				, InDirectionalLight
				, sdw::InVec3
				, sdw::InFloat
				, sdw::InInt
				, InSurface
				, OutputComponents & > m_computeDirectional;
			sdw::Function< sdw::Void
				, InPointLight
				, sdw::InVec3
				, sdw::InFloat
				, sdw::InInt
				, InSurface
				, OutputComponents & > m_computePoint;
			sdw::Function< sdw::Void
				, InSpotLight
				, sdw::InVec3
				, sdw::InFloat
				, sdw::InInt
				, InSurface
				, OutputComponents & > m_computeSpot;
			sdw::Function< sdw::Vec3
				, InLight
				, sdw::InVec3
				, sdw::InVec3
				, sdw::InFloat
				, InSurface > m_computeLightDiffuse;
			sdw::Function< sdw::Vec3
				, InDirectionalLight
				, sdw::InVec3
				, sdw::InFloat
				, sdw::InInt
				, InSurface > m_computeDirectionalDiffuse;
			sdw::Function< sdw::Vec3
				, InPointLight
				, sdw::InVec3
				, sdw::InFloat
				, sdw::InInt
				, InSurface > m_computePointDiffuse;
			sdw::Function< sdw::Vec3
				, InSpotLight
				, sdw::InVec3
				, sdw::InFloat
				, sdw::InInt
				, InSurface > m_computeSpotDiffuse;
		};
	}
}

#endif
