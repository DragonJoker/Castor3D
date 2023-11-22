#include "Castor3D/Shader/Shaders/GlslPassShaders.hpp"

#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include <ShaderWriter/Writer.hpp>

namespace castor3d::shader
{
	//************************************************************************************************

	PassShaders::PassShaders( PassComponentRegister const & compRegister
		, TextureCombine const & combine
		, ComponentModeFlags filter
		, Utils & utils
		, bool forceLod0 )
		: m_utils{ utils }
		, m_compRegister{ compRegister }
		, m_shaders{ m_compRegister.getComponentsShaders( combine, filter, m_updateComponents, m_finishComponents ) }
		, m_reflRefr{ nullptr }
		, m_filter{ filter }
		, m_opacity{ hasAny( combine, m_compRegister.getOpacityMapFlags() ) }
		, m_frontCulled{ false }
		, m_forceLod0{ forceLod0 }
	{
	}

	PassShaders::PassShaders( PassComponentRegister const & compRegister
		, PipelineFlags const & flags
		, ComponentModeFlags filter
		, Utils & utils
		, bool forceLod0 )
		: m_utils{ utils }
		, m_compRegister{ compRegister }
		, m_shaders{ m_compRegister.getComponentsShaders( flags, filter, m_updateComponents, m_finishComponents ) }
		, m_reflRefr{ m_compRegister.getReflRefrShader( flags.pass ) }
		, m_filter{ filter }
		, m_opacity{ ( flags.usesOpacity()
			&& flags.hasMap( m_compRegister.getOpacityMapFlags() )
			&& m_compRegister.hasOpacity( flags ) ) }
		, m_frontCulled{ flags.isFrontCulled() }
		, m_forceLod0{ forceLod0 }
	{
	}

	void PassShaders::fillMaterial( sdw::type::BaseStruct & material
		, sdw::expr::ExprList & inits )const
	{
		if ( material.empty() )
		{
			m_compRegister.fillMaterial( material, inits, 0u );
		}
	}

	void PassShaders::fillComponents( sdw::type::BaseStruct & components
		, Materials const & materials
		, sdw::expr::ExprList & inits )const
	{
		for ( auto & shader : m_shaders )
		{
			shader->fillComponents( getFilter(), components, materials, nullptr );
			shader->fillComponentsInits( components, materials, nullptr, nullptr, nullptr, inits );
		}
	}

	void PassShaders::fillComponents( sdw::type::BaseStruct & components
		, Materials const & materials
		, Material const & material
		, sdw::StructInstance const & surface
		, sdw::Vec4 const * clrCot
		, sdw::expr::ExprList & inits )const
	{
		for ( auto & shader : m_shaders )
		{
			shader->fillComponents( getFilter(), components, materials, &surface );
			shader->fillComponentsInits( components, materials, &material, &surface, clrCot, inits );
		}
	}

	void PassShaders::fillComponentsInits( sdw::type::BaseStruct const & components
		, Materials const & materials
		, sdw::expr::ExprList & inits )const
	{
		for ( auto & shader : m_shaders )
		{
			shader->fillComponentsInits( components, materials, nullptr, nullptr, nullptr, inits );
		}
	}

	void PassShaders::fillComponentsInits( sdw::type::BaseStruct const & components
		, Materials const & materials
		, Material const & material
		, sdw::StructInstance const & surface
		, sdw::Vec4 const * clrCot
		, sdw::expr::ExprList & inits )const
	{
		for ( auto & shader : m_shaders )
		{
			shader->fillComponentsInits( components, materials, &material, &surface, clrCot, inits );
		}
	}

	void PassShaders::applyComponents( TextureCombine const & combine
		, TextureConfigData const & config
		, sdw::U32Vec3 const & imgCompConfig
		, sdw::Vec4 const & sampled
		, sdw::Vec2 const & uv
		, BlendComponents & components )const
	{
		for ( auto & shader : m_shaders )
		{
			auto & plugin = m_compRegister.getPlugin( shader->getId() );

			if ( hasAny( combine, plugin.getTextureFlags() ) )
			{
				shader->applyComponents( combine, nullptr, config, imgCompConfig, sampled, uv, components );
			}
		}
	}

	void PassShaders::applyComponents( PipelineFlags const & flags
		, TextureConfigData const & config
		, sdw::U32Vec3 const & imgCompConfig
		, sdw::Vec4 const & sampled
		, sdw::Vec2 const & uv
		, BlendComponents & components )const
	{
		auto & combine = flags.textures;

		for ( auto & shader : m_shaders )
		{
			auto & plugin = m_compRegister.getPlugin( shader->getId() );

			if ( hasAny( combine, plugin.getTextureFlags() ) )
			{
				shader->applyComponents( combine, &flags, config, imgCompConfig, sampled, uv, components );
			}
		}
	}

	void PassShaders::blendComponents( shader::Materials const & materials
		, sdw::Float const & passMultiplier
		, BlendComponents & res
		, BlendComponents const & src )const
	{
		for ( auto & shader : m_shaders )
		{
			shader->blendComponents( materials, passMultiplier, res, src );
		}
	}

	void PassShaders::updateComponents( TextureCombine const & combine
		, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, BlendComponents & components
		, bool isFrontCulled )const
	{
		for ( auto & shader : m_shaders )
		{
			shader->updateComponent( combine, maps, components, isFrontCulled );
		}

		for ( auto & update : m_updateComponents )
		{
			update( m_compRegister, combine, components, isFrontCulled );
		}
	}

	void PassShaders::updateComponents( PipelineFlags const & flags
		, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, BlendComponents & components )const
	{
		updateComponents( flags.textures, maps, components, flags.isFrontCulled() );
	}

	void PassShaders::finishComponents( SurfaceBase const & surface
		, sdw::Vec3 const worldEye
		, Utils & utils
		, BlendComponents & components )const
	{
		for ( auto & finish : m_finishComponents )
		{
			finish( surface, worldEye, utils, components );
		}
	}

	std::map< uint32_t, PassComponentTextureFlag > PassShaders::getTexcoordModifs( PipelineFlags const & flags )const
	{
		return m_compRegister.getTexcoordModifs( flags );
	}

	std::map< uint32_t, PassComponentTextureFlag > PassShaders::getTexcoordModifs( TextureCombine const & combine )const
	{
		return m_compRegister.getTexcoordModifs( combine );
	}

	sdw::Vec4 PassShaders::sampleMap( PipelineFlags const & flags
		, sdw::CombinedImage2DRgba32 const map
		, sdw::Vec3 const texCoords
		, shader::BlendComponents const & components )const
	{
		auto it = std::find_if( m_shaders.begin()
			, m_shaders.end()
			, [&flags]( PassComponentsShaderPtr const & shader )
			{
				return shader->isMapSampling( flags );
			} );

		return it != m_shaders.end()
			? ( *it )->sampleMap( map, texCoords, components )
			: ( m_forceLod0
				? m_utils.sampleMap( map, texCoords, 0.0_f )
				: m_utils.sampleMap( map, texCoords ) );
	}

	sdw::Vec4 PassShaders::sampleMap( PipelineFlags const & flags
		, sdw::CombinedImage2DRgba32 const map
		, DerivTex const texCoords
		, shader::BlendComponents const & components )const
	{
		auto it = std::find_if( m_shaders.begin()
			, m_shaders.end()
			, [&flags]( PassComponentsShaderPtr const & shader )
			{
				return shader->isMapSampling( flags );
			} );

		return it != m_shaders.end()
			? ( *it )->sampleMap( map, texCoords, components )
			: ( m_forceLod0
				? m_utils.sampleMap( map, texCoords.uv(), 0.0_f )
				: m_utils.sampleMap( map, texCoords ) );
	}

	sdw::Vec4 PassShaders::sampleMap( TextureCombine const & flags
		, sdw::CombinedImage2DRgba32 const map
		, sdw::Vec3 const texCoords
		, shader::BlendComponents const & components )const
	{
		return m_utils.sampleMap( map, texCoords );
	}

	sdw::Vec4 PassShaders::sampleMap( TextureCombine const & flags
		, sdw::CombinedImage2DRgba32 const map
		, DerivTex const texCoords
		, shader::BlendComponents const & components )const
	{
		return ( m_forceLod0
			? m_utils.sampleMap( map, texCoords.uv(), 0.0_f )
			: m_utils.sampleMap( map, texCoords ) );
	}

	void PassShaders::computeTexcoord( PipelineFlags const & flags
		, TextureConfigData const & config
		, sdw::U32Vec3 const & imgCompConfig
		, sdw::CombinedImage2DRgba32 const & map
		, sdw::Vec3 & texCoords
		, sdw::Vec2 & texCoord
		, sdw::UInt const & mapId
		, BlendComponents & components )const
	{
		auto & writer = findWriterMandat( config, map, texCoords, texCoord, components );
		auto & combine = flags.textures;

		for ( auto & shader : m_shaders )
		{
			auto & plugin = m_compRegister.getPlugin( shader->getId() );

			if ( hasAny( combine, plugin.getTextureFlags() )
				&& plugin.hasTexcoordModif( m_compRegister, &flags ) )
			{
				IF( writer, imgCompConfig.x() == sdw::UInt{ plugin.getTextureFlags() } )
				{
					shader->computeTexcoord( flags
						, config
						, imgCompConfig
						, map
						, texCoords
						, texCoord
						, mapId
						, components );
				}
				FI;
			}
		}
	}

	void PassShaders::computeTexcoord( PipelineFlags const & flags
		, TextureConfigData const & config
		, sdw::U32Vec3 const & imgCompConfig
		, sdw::CombinedImage2DRgba32 const & map
		, DerivTex & texCoords
		, DerivTex & texCoord
		, sdw::UInt const & mapId
		, BlendComponents & components )const
	{
		auto & writer = findWriterMandat( config, map, texCoords, texCoord, components );
		auto & combine = flags.textures;

		for ( auto & shader : m_shaders )
		{
			auto & plugin = m_compRegister.getPlugin( shader->getId() );

			if ( hasAny( combine, plugin.getTextureFlags() )
				&& plugin.hasTexcoordModif( m_compRegister, &flags ) )
			{
				IF( writer, imgCompConfig.x() == sdw::UInt{ plugin.getTextureFlags() } )
				{
					shader->computeTexcoord( flags
						, config
						, imgCompConfig
						, map
						, texCoords
						, texCoord
						, mapId
						, components );
				}
				FI;
			}
		}
	}

	void PassShaders::computeTexcoord( TextureCombine const & combine
		, TextureConfigData const & config
		, sdw::U32Vec3 const & imgCompConfig
		, sdw::CombinedImage2DRgba32 const & map
		, sdw::Vec3 & texCoords
		, sdw::Vec2 & texCoord
		, sdw::UInt const & mapId
		, BlendComponents & components )const
	{
	}

	void PassShaders::computeTexcoord( TextureCombine const & combine
		, TextureConfigData const & config
		, sdw::U32Vec3 const & imgCompConfig
		, sdw::CombinedImage2DRgba32 const & map
		, DerivTex & texCoords
		, DerivTex & texCoord
		, sdw::UInt const & mapId
		, BlendComponents & components )const
	{
	}

	bool PassShaders::enableParallaxOcclusionMapping( PipelineFlags const & flags )const
	{
		return flags.enableParallaxOcclusionMapping( m_compRegister);
	}

	bool PassShaders::enableParallaxOcclusionMappingOne( PipelineFlags const & flags )const
	{
		return flags.enableParallaxOcclusionMappingOne( m_compRegister );
	}

	void PassShaders::computeReflRefr( ReflectionModel & reflections
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
		, DebugOutput & debugOutput )const
	{
		if ( !m_reflRefr )
		{
			return;
		}

		m_reflRefr->computeReflRefr( reflections
			, components
			, lightSurface
			, position
			, background
			, mippedScene
			, camera
			, lighting
			, indirect
			, sceneUv
			, envMapIndex
			, incident
			, hasReflection
			, hasRefraction
			, refractionRatio
			, reflectedDiffuse
			, reflectedSpecular
			, refracted
			, coatReflected
			, sheenReflected
			, debugOutput );
	}

	void PassShaders::computeReflRefr( ReflectionModel & reflections
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
		, DebugOutput & debugOutput )const
	{
		if ( !m_reflRefr )
		{
			return;
		}

		m_reflRefr->computeReflRefr( reflections
			, components
			, lightSurface
			, background
			, camera
			, lighting
			, indirect
			, sceneUv
			, envMapIndex
			, incident
			, hasReflection
			, hasRefraction
			, refractionRatio
			, reflectedDiffuse
			, reflectedSpecular
			, refracted
			, coatReflected
			, sheenReflected
			, debugOutput );
	}
}
