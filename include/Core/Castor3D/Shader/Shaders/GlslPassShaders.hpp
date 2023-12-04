/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslPassShaders_H___
#define ___C3D_GlslPassShaders_H___

#include "Castor3D/Shader/Shaders/SdwModule.hpp"

#include "Castor3D/Material/MaterialModule.hpp"
#include "Castor3D/Material/Pass/PassModule.hpp"
#include "Castor3D/Material/Pass/Component/ComponentModule.hpp"
#include "Castor3D/Scene/Animation/AnimationModule.hpp"
#include "Castor3D/Shader/ShaderModule.hpp"
#include "Castor3D/Shader/ShaderBuffers/ShaderBuffersModule.hpp"

#include "Castor3D/Material/Pass/Component/PassComponent.hpp"

namespace castor3d::shader
{
	class PassShaders
	{
	public:
		C3D_API PassShaders( PassComponentRegister const & compRegister
			, TextureCombine const & textures
			, ComponentModeFlags filter
			, Utils & utils
			, bool forceLod0 = false );
		C3D_API PassShaders( PassComponentRegister const & compRegister
			, PipelineFlags const & flags
			, ComponentModeFlags filter
			, Utils & utils
			, bool forceLod0 = false );

		C3D_API void fillMaterial( sdw::type::BaseStruct & material
			, sdw::expr::ExprList & inits )const;
		C3D_API void fillComponents( sdw::type::BaseStruct & components
			, Materials const & materials
			, sdw::expr::ExprList & inits )const;
		C3D_API void fillComponents( sdw::type::BaseStruct & components
			, Materials const & materials
			, Material const & material
			, sdw::StructInstance const & surface
			, sdw::Vec4 const * clrCot
			, sdw::expr::ExprList & inits )const;
		C3D_API void fillComponentsInits( sdw::type::BaseStruct const & components
			, Materials const & materials
			, sdw::expr::ExprList & inits )const;
		C3D_API void fillComponentsInits( sdw::type::BaseStruct const & components
			, Materials const & materials
			, Material const & material
			, sdw::StructInstance const & surface
			, sdw::Vec4 const * clrCot
			, sdw::expr::ExprList & inits )const;
		C3D_API void computeTexcoords( TextureConfigurations const & textureConfigs
			, TextureConfigData const & config
			, TextureTransformData const & anim
			, BlendComponents & components )const;
		C3D_API void applyTextures( TextureConfigurations const & textureConfigs
			, TextureAnimations const & textureAnims
			, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
			, Material const & material
			, BlendComponents & components
			, SampleTexture const & sampleTexture )const;
		C3D_API void applyComponents( TextureCombine const & combine
			, TextureConfigData const & config
			, sdw::U32Vec3 const & imgCompConfig
			, sdw::Vec4 const & sampled
			, sdw::Vec2 const & uv
			, BlendComponents & components )const;
		C3D_API void applyComponents( PipelineFlags const & flags
			, TextureConfigData const & config
			, sdw::U32Vec3 const & imgCompConfig
			, sdw::Vec4 const & sampled
			, sdw::Vec2 const & uv
			, BlendComponents & components )const;
		C3D_API void blendComponents( Materials const & materials
			, sdw::Float const & passMultiplier
			, BlendComponents & res
			, BlendComponents const & src )const;
		C3D_API void updateComponents( PipelineFlags const & flags
			, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
			, Material const & material
			, BlendComponents & components )const;
		C3D_API void updateComponents( TextureCombine const & combine
			, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
			, Material const & material
			, BlendComponents & components
			, bool isFrontCulled = false )const;
		C3D_API void finishComponents( SurfaceBase const & surface
			, sdw::Vec3 const worldEye
			, Utils & utils
			, BlendComponents & components )const;
		C3D_API std::map< uint32_t, PassComponentTextureFlag > getTexcoordModifs( PipelineFlags const & flags )const;
		C3D_API std::map< uint32_t, PassComponentTextureFlag > getTexcoordModifs( TextureCombine const & combine )const;
		C3D_API bool enableParallaxOcclusionMapping( PipelineFlags const & flags )const;
		C3D_API bool enableParallaxOcclusionMappingOne( PipelineFlags const & flags )const;
		C3D_API sdw::Vec4 sampleMap( PipelineFlags const & flags
			, sdw::CombinedImage2DRgba32 const map
			, sdw::Vec3 const texCoords
			, shader::BlendComponents const & components )const;
		C3D_API sdw::Vec4 sampleMap( PipelineFlags const & flags
			, sdw::CombinedImage2DRgba32 const map
			, DerivTex const texCoords
			, shader::BlendComponents const & components )const;
		C3D_API sdw::Vec4 sampleMap( TextureCombine const & flags
			, sdw::CombinedImage2DRgba32 const map
			, sdw::Vec3 const texCoords
			, shader::BlendComponents const & components )const;
		C3D_API sdw::Vec4 sampleMap( TextureCombine const & flags
			, sdw::CombinedImage2DRgba32 const map
			, sdw::Vec2 const texCoords
			, shader::BlendComponents const & components )const;
		C3D_API sdw::Vec4 sampleMap( TextureCombine const & flags
			, sdw::CombinedImage2DRgba32 const map
			, DerivTex const texCoords
			, shader::BlendComponents const & components )const;
		C3D_API void computeReflRefr( ReflectionModel & reflections
			, BlendComponents & components
			, LightSurface const & lightSurface
			, sdw::Vec4 const & position
			, BackgroundModel & background
			, sdw::CombinedImage2DRgba32 const & mippedScene
			, CameraData const & camera
			, DirectLighting & lighting
			, IndirectLighting & indirect
			, sdw::Vec2 const & sceneUv
			, sdw::UInt const & envMapIndex
			, sdw::Vec3 const & incident
			, sdw::UInt const & hasReflection
			, sdw::UInt const & hasRefraction
			, sdw::Float const & refractionRatio
			, sdw::Vec3 & reflectedDiffuse
			, sdw::Vec3 & reflectedSpecular
			, sdw::Vec3 & refracted
			, sdw::Vec3 & coatReflected
			, sdw::Vec3 & sheenReflected
			, DebugOutput & debugOutput )const;
		C3D_API void computeReflRefr( ReflectionModel & reflections
			, BlendComponents & components
			, LightSurface const & lightSurface
			, BackgroundModel & background
			, CameraData const & camera
			, DirectLighting & lighting
			, IndirectLighting & indirect
			, sdw::Vec2 const & sceneUv
			, sdw::UInt const & envMapIndex
			, sdw::Vec3 const & incident
			, sdw::UInt const & hasReflection
			, sdw::UInt const & hasRefraction
			, sdw::Float const & refractionRatio
			, sdw::Vec3 & reflectedDiffuse
			, sdw::Vec3 & reflectedSpecular
			, sdw::Vec3 & refracted
			, sdw::Vec3 & coatReflected
			, sdw::Vec3 & sheenReflected
			, DebugOutput & debugOutput )const;

		auto getFilter()const
		{
			return m_filter;
		}

		bool enableOpacity()const
		{
			return m_opacity;
		}

		bool isFrontCulled()const
		{
			return m_frontCulled;
		}

		Utils & getUtils()const
		{
			return m_utils;
		}

		PassComponentCombine const & getPassCombine()const
		{
			return m_passCombine;
		}

		TextureCombine const & getTexturesCombine()const
		{
			return m_texturesCombine;
		}

	private:
		PassComponentCombine m_passCombine;
		TextureCombine m_texturesCombine;
		Utils & m_utils;
		PassComponentRegister const & m_compRegister;
		std::vector< UpdateComponent > m_updateComponents;
		std::vector< FinishComponent > m_finishComponents;
		std::vector< PassComponentsShaderPtr > m_shaders;
		std::vector< PassMapComponentsShader * > m_mapShaders;
		PassReflRefrShaderPtr m_reflRefr;
		ComponentModeFlags m_filter;
		bool m_opacity{};
		bool m_frontCulled{};
		bool m_forceLod0{};
	};
}

#endif
