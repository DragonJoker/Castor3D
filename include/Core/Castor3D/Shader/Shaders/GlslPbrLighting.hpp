/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslPbrLightingModel_H___
#define ___C3D_GlslPbrLightingModel_H___

#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include "Castor3D/Shader/Shaders/GlslCookTorranceBRDF.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"

namespace castor3d
{
	namespace shader
	{
		class PbrLightingModel
			: public LightingModel
		{
		public:
			C3D_API explicit PbrLightingModel( sdw::ShaderWriter & writer
				, Utils & utils
				, ShadowOptions shadowOptions
				, bool isOpaqueProgram );
			C3D_API void computeCombined( sdw::Vec3 const & worldEye
				, PbrLightMaterial const & material
				, sdw::Int const & receivesShadows
				, SceneData const & sceneData
				, Surface surface
				, OutputComponents & output )const;
			C3D_API void compute( TiledDirectionalLight const & light
				, sdw::Vec3 const & worldEye
				, PbrLightMaterial const & material
				, sdw::Int const & receivesShadows
				, Surface surface
				, OutputComponents & output )const;
			C3D_API void compute( DirectionalLight const & light
				, sdw::Vec3 const & worldEye
				, PbrLightMaterial const & material
				, sdw::Int const & receivesShadows
				, Surface surface
				, OutputComponents & output )const;
			C3D_API void compute( PointLight const & light
				, sdw::Vec3 const & worldEye
				, PbrLightMaterial const & material
				, sdw::Int const & receivesShadows
				, Surface surface
				, OutputComponents & output )const;
			C3D_API void compute( SpotLight const & light
				, sdw::Vec3 const & worldEye
				, PbrLightMaterial const & material
				, sdw::Int const & receivesShadows
				, Surface surface
				, OutputComponents & output )const;
			C3D_API static sdw::Vec3 combine( sdw::Vec3 const & directDiffuse
				, sdw::Vec3 const & indirectDiffuse
				, sdw::Vec3 const & directSpecular
				, sdw::Vec3 const & indirectSpecular
				, sdw::Vec3 const & ambient
				, sdw::Vec3 const & indirectAmbient
				, sdw::Float const & ambientOcclusion
				, sdw::Vec3 const & emissive
				, sdw::Vec3 const & reflected
				, sdw::Vec3 const & refracted
				, sdw::Vec3 const & materialAlbedo );
			C3D_API sdw::Vec3 computeCombinedDiffuse( sdw::Vec3 const & worldEye
				, PbrLightMaterial const & material
				, sdw::Int const & receivesShadows
				, SceneData const & sceneData
				, Surface surface )const;
			C3D_API sdw::Vec3 computeDiffuse( TiledDirectionalLight const & light
				, sdw::Vec3 const & worldEye
				, PbrLightMaterial const & material
				, sdw::Int const & receivesShadows
				, Surface surface )const;
			C3D_API sdw::Vec3 computeDiffuse( DirectionalLight const & light
				, sdw::Vec3 const & worldEye
				, PbrLightMaterial const & material
				, sdw::Int const & receivesShadows
				, Surface surface )const;
			C3D_API sdw::Vec3 computeDiffuse( PointLight const & light
				, sdw::Vec3 const & worldEye
				, PbrLightMaterial const & material
				, sdw::Int const & receivesShadows
				, Surface surface )const;
			C3D_API sdw::Vec3 computeDiffuse( SpotLight const & light
				, sdw::Vec3 const & worldEye
				, PbrLightMaterial const & material
				, sdw::Int const & receivesShadows
				, Surface surface )const;
			C3D_API static std::shared_ptr< PbrLightingModel > createModel( sdw::ShaderWriter & writer
				, Utils & utils
				, uint32_t lightsBufBinding
				, uint32_t lightsBufSet
				, ShadowOptions const & shadows
				, uint32_t & shadowMapBinding
				, uint32_t shadowMapSet
				, bool isOpaqueProgram );
			template< typename LightsBufBindingT >
			static std::shared_ptr< PbrLightingModel > createModelT( sdw::ShaderWriter & writer
				, Utils & utils
				, LightsBufBindingT lightsBufBinding
				, uint32_t lightsBufSet
				, ShadowOptions const & shadows
				, uint32_t & shadowMapBinding
				, uint32_t shadowMapSet
				, bool isOpaqueProgram )
			{
				return createModel( writer
					, utils
					, uint32_t( lightsBufBinding )
					, lightsBufSet
					, shadows
					, shadowMapBinding
					, shadowMapSet
					, isOpaqueProgram );
			}
			C3D_API static std::shared_ptr< PbrLightingModel > createDiffuseModel( sdw::ShaderWriter & writer
				, Utils & utils
				, uint32_t lightsBufBinding
				, uint32_t lightsBufSet
				, ShadowOptions const & shadows
				, uint32_t & shadowMapBinding
				, uint32_t shadowMapSet
				, bool isOpaqueProgram );
			template< typename LightsBufBindingT >
			static std::shared_ptr< PbrLightingModel > createDiffuseModelT( sdw::ShaderWriter & writer
				, Utils & utils
				, LightsBufBindingT lightsBufBinding
				, uint32_t lightsBufSet
				, ShadowOptions const & shadows
				, uint32_t & shadowMapBinding
				, uint32_t shadowMapSet
				, bool isOpaqueProgram )
			{
				return createDiffuseModel( writer
					, utils
					, uint32_t( lightsBufBinding )
					, lightsBufSet
					, shadows
					, shadowMapBinding
					, shadowMapSet
					, isOpaqueProgram );
			}
			template< typename LightBindingT >
			static std::shared_ptr< PbrLightingModel > createModel( sdw::ShaderWriter & writer
				, Utils & utils
				, LightType light
				, LightBindingT lightBinding
				, uint32_t lightSet
				, bool lightUbo
				, ShadowOptions const & shadows
				, uint32_t & shadowMapBinding
				, uint32_t shadowMapSet )
			{
				return createModel( writer
					, utils
					, light
					, lightUbo
					, uint32_t( lightBinding )
					, lightSet
					, shadows
					, shadowMapBinding
					, shadowMapSet );
			}

			C3D_API void computeMapContributions( PassFlags const & passFlags
				, FilteredTextureFlags const & textures
				, sdw::Float const & gamma
				, TextureConfigurations const & textureConfigs
				, sdw::Array< sdw::SampledImage2DRgba32 > const & maps
				, sdw::Vec3 & texCoords
				, sdw::Vec3 & normal
				, sdw::Vec3 & tangent
				, sdw::Vec3 & bitangent
				, sdw::Vec3 & emissive
				, sdw::Float & opacity
				, sdw::Float & occlusion
				, sdw::Float & transmittanc
				, PbrLightMaterial & lightMat
				, sdw::Vec3 & tangentSpaceViewPosition
				, sdw::Vec3 & tangentSpaceFragPosition );
			C3D_API void computeMapVoxelContributions( PassFlags const & passFlags
				, FilteredTextureFlags const & textures
				, sdw::Float const & gamma
				, TextureConfigurations const & textureConfigs
				, sdw::Array< sdw::SampledImage2DRgba32 > const & maps
				, sdw::Vec3 const & texCoords
				, sdw::Vec3 & emissive
				, sdw::Float & opacity
				, sdw::Float & occlusion
				, PbrLightMaterial & lightMat );

		protected:
			void doDeclareModel()override;
			void doDeclareComputeDirectionalLight()override;
			void doDeclareComputePointLight()override;
			void doDeclareComputeSpotLight()override;
			void doDeclareDiffuseModel()override;
			void doDeclareComputeDirectionalLightDiffuse()override;
			void doDeclareComputePointLightDiffuse()override;
			void doDeclareComputeSpotLightDiffuse()override;

		private:
			C3D_API static std::shared_ptr< PbrLightingModel > createModel( sdw::ShaderWriter & writer
				, Utils & utils
				, LightType light
				, bool lightUbo
				, uint32_t lightBinding
				, uint32_t lightSet
				, ShadowOptions const & shadows
				, uint32_t & shadowMapBinding
				, uint32_t shadowMapSet );

		public:
			C3D_API static const castor::String Name;
			CookTorranceBRDF m_cookTorrance;
			sdw::Function< sdw::Void
				, InTiledDirectionalLight
				, sdw::InVec3
				, InPbrLightMaterial
				, sdw::InInt
				, InSurface
				, OutputComponents & > m_computeTiledDirectional;
			sdw::Function< sdw::Void
				, InDirectionalLight
				, sdw::InVec3
				, InPbrLightMaterial
				, sdw::InInt
				, InSurface
				, OutputComponents & > m_computeDirectional;
			sdw::Function< sdw::Void
				, InPointLight
				, sdw::InVec3
				, InPbrLightMaterial
				, sdw::InInt
				, InSurface
				, OutputComponents & > m_computePoint;
			sdw::Function< sdw::Void
				, InSpotLight
				, sdw::InVec3
				, InPbrLightMaterial
				, sdw::InInt
				, InSurface
				, OutputComponents & > m_computeSpot;
			sdw::Function< sdw::Vec3
				, InTiledDirectionalLight
				, sdw::InVec3
				, InPbrLightMaterial
				, sdw::InInt
				, InSurface > m_computeTiledDirectionalDiffuse;
			sdw::Function< sdw::Vec3
				, InDirectionalLight
				, sdw::InVec3
				, InPbrLightMaterial
				, sdw::InInt
				, InSurface > m_computeDirectionalDiffuse;
			sdw::Function< sdw::Vec3
				, InPointLight
				, sdw::InVec3
				, InPbrLightMaterial
				, sdw::InInt
				, InSurface > m_computePointDiffuse;
			sdw::Function< sdw::Vec3
				, InSpotLight
				, sdw::InVec3
				, InPbrLightMaterial
				, sdw::InInt
				, InSurface > m_computeSpotDiffuse;
		};
	}
}

#endif
