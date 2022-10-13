/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PassShaders_H___
#define ___C3D_PassShaders_H___

#include "Castor3D/Material/MaterialModule.hpp"
#include "Castor3D/Material/Pass/PassModule.hpp"
#include "Castor3D/Material/Pass/Component/ComponentModule.hpp"
#include "Castor3D/Scene/Animation/AnimationModule.hpp"
#include "Castor3D/Shader/ShaderModule.hpp"
#include "Castor3D/Shader/ShaderBuffers/ShaderBuffersModule.hpp"
#include "Castor3D/Shader/Shaders/SdwModule.hpp"

#include "Castor3D/Material/Pass/Component/PassComponent.hpp"

namespace castor3d::shader
{
	class PassShaders
	{
	public:
		C3D_API PassShaders( PassComponentRegister const & compRegister
			, TextureFlagsArray const & texturesFlags
			, ComponentModeFlags filter
			, Utils & utils );
		C3D_API PassShaders( PassComponentRegister const & compRegister
			, PipelineFlags const & flags
			, ComponentModeFlags filter
			, Utils & utils );

		C3D_API void fillMaterial( sdw::type::BaseStruct & material
			, sdw::expr::ExprList & inits )const;
		C3D_API void fillComponents( sdw::type::BaseStruct & components
			, Materials const & materials
			, sdw::expr::ExprList & inits )const;
		C3D_API void fillComponents( sdw::type::BaseStruct & components
			, Materials const & materials
			, Material const & material
			, sdw::StructInstance const & surface
			, sdw::expr::ExprList & inits )const;
		C3D_API void fillComponentsInits( sdw::type::BaseStruct const & components
			, Materials const & materials
			, sdw::expr::ExprList & inits )const;
		C3D_API void fillComponentsInits( sdw::type::BaseStruct const & components
			, Materials const & materials
			, Material const & material
			, sdw::StructInstance const & surface
			, sdw::expr::ExprList & inits )const;
		C3D_API void applyComponents( TextureFlagsArray const & texturesFlags
			, TextureConfigData const & config
			, sdw::U32Vec3 const & imgCompConfig
			, sdw::Vec4 const & sampled
			, BlendComponents & components )const;
		C3D_API void applyComponents( PipelineFlags const & flags
			, TextureConfigData const & config
			, sdw::U32Vec3 const & imgCompConfig
			, sdw::Vec4 const & sampled
			, BlendComponents & components )const;
		C3D_API void blendComponents( Materials const & materials
			, sdw::Float const & passMultiplier
			, BlendComponents & res
			, BlendComponents const & src )const;
		C3D_API void updateMaterial( sdw::Vec3 const & albedo
			, sdw::Vec4 const & spcRgh
			, sdw::Vec4 const & colMtl
			, Material & material )const;
		C3D_API void updateOutputs( Material const & material
			, SurfaceBase const & surface
			, sdw::Vec4 & spcRgh
			, sdw::Vec4 & colMtl )const;
		C3D_API void updateOutputs( BlendComponents const & components
			, SurfaceBase const & surface
			, sdw::Vec4 & spcRgh
			, sdw::Vec4 & colMtl )const;
		C3D_API void updateComponents( PipelineFlags const & flags
			, BlendComponents & components )const;
		C3D_API void updateComponents( TextureFlagsArray const & texturesFlags
			, BlendComponents & components )const;
		C3D_API bool hasTexcoordModif( PipelineFlags const & flags )const;
		C3D_API bool hasTexcoordModif( TextureFlagsArray const & flags )const;
		C3D_API void computeTexcoord( TextureFlagsArray const & flags
			, TextureConfigData const & config
			, sdw::U32Vec3 const & imgCompConfig
			, sdw::CombinedImage2DRgba32 const & map
			, sdw::Vec3 & texCoords
			, sdw::Vec2 & texCoord
			, BlendComponents & components )const;
		C3D_API void computeTexcoord( TextureFlagsArray const & flags
			, TextureConfigData const & config
			, sdw::U32Vec3 const & imgCompConfig
			, sdw::CombinedImage2DRgba32 const & map
			, DerivTex & texCoords
			, DerivTex & texCoord
			, BlendComponents & components )const;
		C3D_API void computeTexcoord( PipelineFlags const & flags
			, TextureConfigData const & config
			, sdw::U32Vec3 const & imgCompConfig
			, sdw::CombinedImage2DRgba32 const & map
			, sdw::Vec3 & texCoords
			, sdw::Vec2 & texCoord
			, BlendComponents & components )const;
		C3D_API void computeTexcoord( PipelineFlags const & flags
			, TextureConfigData const & config
			, sdw::U32Vec3 const & imgCompConfig
			, sdw::CombinedImage2DRgba32 const & map
			, DerivTex & texCoords
			, DerivTex & texCoord
			, BlendComponents & components )const;
		C3D_API bool enableParallaxOcclusionMapping( PipelineFlags const & flags )const;
		C3D_API bool enableParallaxOcclusionMappingOne( PipelineFlags const & flags )const;

		template< typename TexcoordT >
		void computeMapContribution( TextureFlagsArray const & texturesFlags
			, TextureConfigData const & config
			, sdw::U32Vec3 const & imgCompConfig
			, TextureAnimData const & anim
			, sdw::CombinedImage2DRgba32 const & map
			, TexcoordT & texCoords
			, BlendComponents & components )const;
		template< typename TexcoordT >
		void computeMapContribution( PipelineFlags const & flags
			, TextureConfigData const & config
			, sdw::U32Vec3 const & imgCompConfig
			, TextureAnimData const & anim
			, sdw::CombinedImage2DRgba32 const & map
			, TexcoordT & texCoords
			, BlendComponents & components )const;

		auto getFilter()const
		{
			return m_filter;
		}

		bool enableOpacity()const
		{
			return m_opacity;
		}

		Utils & getUtils()const
		{
			return m_utils;
		}

	private:
		Utils & m_utils;
		PassComponentRegister const & m_compRegister;
		std::vector< UpdateComponent > m_updateComponents;
		std::vector< PassComponentsShaderPtr > m_shaders;
		ComponentModeFlags m_filter;
		bool m_opacity{};
	};
}

#include "PassShaders.inl"

#endif
