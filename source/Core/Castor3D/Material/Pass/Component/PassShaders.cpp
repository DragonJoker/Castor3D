#include "Castor3D/Material/Pass/Component/PassShaders.hpp"

#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"

#include <ShaderWriter/Writer.hpp>

namespace castor3d::shader
{
	//************************************************************************************************

	PassShaders::PassShaders( PassComponentRegister const & compRegister
		, TextureFlagsArray const & texturesFlags
		, ComponentModeFlags filter
		, Utils & utils )
		: m_utils{ utils }
		, m_compRegister{ compRegister }
		, m_shaders{ m_compRegister.getComponentsShaders( texturesFlags, filter, m_updateComponents ) }
		, m_filter{ filter }
		, m_opacity{ checkFlags( texturesFlags, m_compRegister.getOpacityFlags() ) != texturesFlags.end() }
	{
	}

	PassShaders::PassShaders( PassComponentRegister const & compRegister
		, PipelineFlags const & flags
		, ComponentModeFlags filter
		, Utils & utils )
		: m_utils{ utils }
		, m_compRegister{ compRegister }
		, m_shaders{ m_compRegister.getComponentsShaders( flags, filter, m_updateComponents ) }
		, m_filter{ filter }
		, m_opacity{ ( flags.usesOpacity()
			&& flags.hasMap( m_compRegister.getOpacityFlags() )
			&& flags.hasOpacity() ) }
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
			shader->fillComponentsInits( components, materials, nullptr, nullptr, inits );
		}
	}

	void PassShaders::fillComponents( sdw::type::BaseStruct & components
		, Materials const & materials
		, Material const & material
		, sdw::StructInstance const & surface
		, sdw::expr::ExprList & inits )const
	{
		for ( auto & shader : m_shaders )
		{
			shader->fillComponents( components, materials, &surface );
			shader->fillComponentsInits( components, materials, &material, &surface, inits );
		}
	}

	void PassShaders::fillComponentsInits( sdw::type::BaseStruct const & components
		, Materials const & materials
		, sdw::expr::ExprList & inits )const
	{
		for ( auto & shader : m_shaders )
		{
			shader->fillComponentsInits( components, materials, nullptr, nullptr, inits );
		}
	}

	void PassShaders::fillComponentsInits( sdw::type::BaseStruct const & components
		, Materials const & materials
		, Material const & material
		, sdw::StructInstance const & surface
		, sdw::expr::ExprList & inits )const
	{
		for ( auto & shader : m_shaders )
		{
			shader->fillComponentsInits( components, materials, &material, &surface, inits );
		}
	}

	void PassShaders::applyComponents( TextureFlagsArray const & texturesFlags
		, TextureConfigData const & config
		, sdw::U32Vec3 const & imgCompConfig
		, sdw::Vec4 const & sampled
		, BlendComponents & components )const
	{
		auto & writer = findWriterMandat( config, imgCompConfig, sampled, components );

		for ( auto & shader : m_shaders )
		{
			auto & plugin = m_compRegister.getPlugin( shader->getId() );

			if ( checkFlags( texturesFlags, plugin.getTextureFlags() ) != texturesFlags.end() )
			{
				IF( writer, imgCompConfig.x() == sdw::UInt{ plugin.getTextureFlags() } )
				{
					shader->applyComponents( texturesFlags, nullptr, config, imgCompConfig, sampled, components );
				}
				FI;
			}
		}
	}

	void PassShaders::applyComponents( PipelineFlags const & flags
		, TextureConfigData const & config
		, sdw::U32Vec3 const & imgCompConfig
		, sdw::Vec4 const & sampled
		, BlendComponents & components )const
	{
		auto & writer = findWriterMandat( config, imgCompConfig, sampled, components );
		auto texturesFlags = flags.makeTexturesFlags();

		for ( auto & shader : m_shaders )
		{
			auto & plugin = m_compRegister.getPlugin( shader->getId() );

			if ( checkFlags( texturesFlags, plugin.getTextureFlags() ) != texturesFlags.end() )
			{
				IF( writer, imgCompConfig.x() == sdw::UInt{ plugin.getTextureFlags() } )
				{
					shader->applyComponents( flags.makeTexturesFlags(), &flags, config, imgCompConfig, sampled, components );
				}
				FI;
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
		, Material & material )const
	{
		for ( auto & shader : m_compRegister.getMaterialShaders() )
		{
			shader->updateMaterial( albedo
				, spcRgh
				, colMtl
				, material );
		}
	}

	void PassShaders::updateOutputs( Material const & material
		, SurfaceBase const & surface
		, sdw::Vec4 & spcRgh
		, sdw::Vec4 & colMtl )const
	{
		for ( auto & shader : m_compRegister.getMaterialShaders() )
		{
			shader->updateOutputs( material
				, surface
				, spcRgh
				, colMtl );
		}
	}

	void PassShaders::updateOutputs( BlendComponents const & components
		, SurfaceBase const & surface
		, sdw::Vec4 & spcRgh
		, sdw::Vec4 & colMtl )const
	{
		for ( auto & shader : m_shaders )
		{
			shader->updateOutputs( components
				, surface
				, spcRgh
				, colMtl );
		}
	}

	void PassShaders::updateComponents( TextureFlagsArray const & texturesFlags
		, BlendComponents & components )const
	{
		for ( auto & update : m_updateComponents )
		{
			update( m_compRegister, texturesFlags, components );
		}
	}

	void PassShaders::updateComponents( PipelineFlags const & flags
		, BlendComponents & components )const
	{
		updateComponents( flags.makeTexturesFlags(), components );
	}

	bool PassShaders::hasTexcoordModif( PipelineFlags const & flags )const
	{
		return m_compRegister.hasTexcoordModif( flags );
	}

	bool PassShaders::hasTexcoordModif( TextureFlagsArray const & flags )const
	{
		return m_compRegister.hasTexcoordModif( flags );
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
		auto textures = flags.makeTexturesFlags();

		for ( auto & shader : m_shaders )
		{
			auto & plugin = m_compRegister.getPlugin( shader->getId() );

			if ( checkFlags( textures, plugin.getTextureFlags() ) != textures.end() )
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
		auto textures = flags.makeTexturesFlags();

		for ( auto & shader : m_shaders )
		{
			auto & plugin = m_compRegister.getPlugin( shader->getId() );

			if ( checkFlags( textures, plugin.getTextureFlags() ) != textures.end() )
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

	void PassShaders::computeTexcoord( TextureFlagsArray const & flags
		, TextureConfigData const & config
		, sdw::U32Vec3 const & imgCompConfig
		, sdw::CombinedImage2DRgba32 const & map
		, sdw::Vec3 & texCoords
		, sdw::Vec2 & texCoord
		, BlendComponents & components )const
	{
	}

	void PassShaders::computeTexcoord( TextureFlagsArray const & flags
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
