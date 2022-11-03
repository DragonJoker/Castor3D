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
		, Utils & utils )
		: m_utils{ utils }
		, m_compRegister{ compRegister }
		, m_shaders{ m_compRegister.getComponentsShaders( combine, filter, m_updateComponents, m_finishComponents ) }
		, m_filter{ filter }
		, m_opacity{ hasAny( combine, m_compRegister.getOpacityMapFlags() ) }
	{
	}

	PassShaders::PassShaders( PassComponentRegister const & compRegister
		, PipelineFlags const & flags
		, ComponentModeFlags filter
		, Utils & utils )
		: m_utils{ utils }
		, m_compRegister{ compRegister }
		, m_shaders{ m_compRegister.getComponentsShaders( flags, filter, m_updateComponents, m_finishComponents ) }
		, m_filter{ filter }
		, m_opacity{ ( flags.usesOpacity()
			&& flags.hasMap( m_compRegister.getOpacityMapFlags() )
			&& m_compRegister.hasOpacity( flags ) ) }
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
			shader->fillComponents( components, materials, nullptr );
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
			shader->fillComponents( components, materials, &surface );
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
		, BlendComponents & components )const
	{
		for ( auto & shader : m_shaders )
		{
			auto & plugin = m_compRegister.getPlugin( shader->getId() );

			if ( hasAny( combine, plugin.getTextureFlags() ) )
			{
				shader->applyComponents( combine, nullptr, config, imgCompConfig, sampled, components );
			}
		}
	}

	void PassShaders::applyComponents( PipelineFlags const & flags
		, TextureConfigData const & config
		, sdw::U32Vec3 const & imgCompConfig
		, sdw::Vec4 const & sampled
		, BlendComponents & components )const
	{
		auto & combine = flags.textures;

		for ( auto & shader : m_shaders )
		{
			auto & plugin = m_compRegister.getPlugin( shader->getId() );

			if ( hasAny( combine, plugin.getTextureFlags() ) )
			{
				shader->applyComponents( combine, &flags, config, imgCompConfig, sampled, components );
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

	void PassShaders::updateMaterial( sdw::Vec3 const & albedo
		, sdw::Vec4 const & spcRgh
		, sdw::Vec4 const & colMtl
		, sdw::Vec4 const & crTsIr
		, sdw::Vec4 const & sheen
		, Material & material )const
	{
		for ( auto & shader : m_compRegister.getMaterialShaders() )
		{
			shader->updateMaterial( albedo
				, spcRgh
				, colMtl
				, crTsIr
				, sheen
				, material );
		}
	}

	void PassShaders::updateOutputs( Material const & material
		, SurfaceBase const & surface
		, sdw::Vec4 & spcRgh
		, sdw::Vec4 & colMtl
		, sdw::Vec4 & sheen )const
	{
		for ( auto & shader : m_compRegister.getMaterialShaders() )
		{
			shader->updateOutputs( material
				, surface
				, spcRgh
				, colMtl
				, sheen );
		}
	}

	void PassShaders::updateOutputs( BlendComponents const & components
		, SurfaceBase const & surface
		, sdw::Vec4 & spcRgh
		, sdw::Vec4 & colMtl
		, sdw::Vec4 & sheen )const
	{
		for ( auto & shader : m_shaders )
		{
			shader->updateOutputs( components
				, surface
				, spcRgh
				, colMtl
				, sheen );
		}
	}

	void PassShaders::updateComponents( TextureCombine const & combine
		, BlendComponents & components )const
	{
		for ( auto & update : m_updateComponents )
		{
			update( m_compRegister, combine, components );
		}
	}

	void PassShaders::updateComponents( PipelineFlags const & flags
		, BlendComponents & components )const
	{
		updateComponents( flags.textures, components );
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
			: m_utils.sampleMap( map, texCoords );
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
			: m_utils.sampleMap( map, texCoords );
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
		return m_utils.sampleMap( map, texCoords );
	}

	void PassShaders::computeTexcoord( PipelineFlags const & flags
		, TextureConfigData const & config
		, sdw::U32Vec3 const & imgCompConfig
		, sdw::CombinedImage2DRgba32 const & map
		, sdw::Vec3 & texCoords
		, sdw::Vec2 & texCoord
		, BlendComponents & components )const
	{
		auto & writer = findWriterMandat( config, map, texCoords, texCoord, components );
		auto & combine = flags.textures;

		for ( auto & shader : m_shaders )
		{
			auto & plugin = m_compRegister.getPlugin( shader->getId() );

			if ( hasAny( combine, plugin.getTextureFlags() ) )
			{
				IF( writer, imgCompConfig.x() == sdw::UInt{ plugin.getTextureFlags() } )
				{
					shader->computeTexcoord( flags
						, config
						, imgCompConfig
						, map
						, texCoords
						, texCoord
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
		, BlendComponents & components )const
	{
		auto & writer = findWriterMandat( config, map, texCoords, texCoord, components );
		auto & combine = flags.textures;

		for ( auto & shader : m_shaders )
		{
			auto & plugin = m_compRegister.getPlugin( shader->getId() );

			if ( hasAny( combine, plugin.getTextureFlags() ) )
			{
				IF( writer, imgCompConfig.x() == sdw::UInt{ plugin.getTextureFlags() } )
				{
					shader->computeTexcoord( flags
						, config
						, imgCompConfig
						, map
						, texCoords
						, texCoord
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
		, BlendComponents & components )const
	{
	}

	void PassShaders::computeTexcoord( TextureCombine const & combine
		, TextureConfigData const & config
		, sdw::U32Vec3 const & imgCompConfig
		, sdw::CombinedImage2DRgba32 const & map
		, DerivTex & texCoords
		, DerivTex & texCoord
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
}
