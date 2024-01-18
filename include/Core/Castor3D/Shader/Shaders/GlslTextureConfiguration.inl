#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"

#include "Castor3D/Limits.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslPassShaders.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureAnimation.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

namespace castor3d::shader
{
	namespace shdtex
	{
		inline TextureCombine getTextureFlags( PipelineFlags const & flags )
		{
			return flags.textures;
		}

		inline TextureCombine getTextureFlags( TextureCombine const & flags )
		{
			return flags;
		}
	}

	//*********************************************************************************************

	template< typename TexcoordT >
	TexcoordT TextureConfigurations::computeTexcoordsT( PassShaders const & passShaders
		, TextureConfigData const & config
		, TextureTransformData const & anim
		, BlendComponents const & components )const
	{
		if constexpr ( std::is_same_v< TexcoordT, DerivTex > )
		{
			return computeTexcoordsDerivTex( passShaders, config, anim, components );
		}
		else if constexpr ( std::is_same_v< TexcoordT, sdw::Vec3 > )
		{
			return computeTexcoordsVec3( passShaders, config, anim, components );
		}
		else
		{
			return computeTexcoordsVec2( passShaders, config, anim, components );
		}
	}

	template< typename TexcoordT, typename FlagsT >
	void TextureConfigurations::computeMapsContributionsT( PassShaders const & passShaders
		, FlagsT const & flags
		, TextureAnimations const & textureAnims
		, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, Material const & pmaterial
		, BlendComponents & pcomponents )const
	{
		if ( isEnabled()
			&& pcomponents.hasMember( "texture0" ) )
		{
			if ( !this->m_sampleTexture )
			{
				this->m_sampleTexture = m_writer.implementFunction< sdw::Vec4 >( "c3d_sampleTexture"
					, [&]( sdw::UInt const & id
						, TextureConfigData const & config
						, BlendComponents components )
					{
						auto texCoords = components.getMember< TexcoordT >( "texCoords" );
						auto map = maps[nonuniform( id - 1_u )];
						m_writer.returnStmt( passShaders.sampleMap( flags, map, texCoords, components ) );
					}
					, sdw::InUInt{ m_writer, "id" }
					, InTextureConfigData{ m_writer, "config" }
					, InOutBlendComponents{ m_writer, "components", pcomponents } );
			}

			passShaders.applyTextures( *this, textureAnims, maps, pmaterial, pcomponents, m_sampleTexture );
		}

		passShaders.updateComponents( flags
			, maps
			, pmaterial
			, pcomponents );
	}

	template< typename TexcoordT >
	TexcoordT TextureConfigurations::getTexcoord( TextureConfigData const & data
		, TexcoordT const & texCoords0
		, TexcoordT const & texCoords1
		, TexcoordT const & texCoords2
		, TexcoordT const & texCoords3 )
	{
		auto & writer = *data.getWriter();

		if ( texCoords3.isEnabled() )
		{
			return writer.ternary( data.texSet() >= 3u
				, texCoords3
				, writer.ternary( data.texSet() >= 2u
					, texCoords2
					, writer.ternary( data.texSet() >= 1u
						, texCoords1
						, texCoords0 ) ) );
		}

		if ( texCoords2.isEnabled() )
		{
			return writer.ternary( data.texSet() >= 2u
				, texCoords2
				, writer.ternary( data.texSet() >= 1u
					, texCoords1
					, texCoords0 ) );
		}

		if ( texCoords1.isEnabled() )
		{
			return writer.ternary( data.texSet() >= 1u
				, texCoords1
				, texCoords0 );
		}

		return texCoords0;
	}

	template< typename TexcoordT >
	void TextureConfigurations::setTexcoord( TextureConfigData const & data
		, TexcoordT const & value
		, TexcoordT & texCoords0
		, TexcoordT & texCoords1
		, TexcoordT & texCoords2
		, TexcoordT & texCoords3 )
	{
		auto & writer = *data.getWriter();

		if ( texCoords3.isEnabled() )
		{
			IF( writer, data.texSet() >= 3u )
			{
				texCoords3 = value;
			}
			ELSEIF( data.texSet() >= 2u )
			{
				texCoords2 = value;
			}
			ELSEIF( data.texSet() >= 1u )
			{
				texCoords1 = value;
			}
			ELSE
			{
				texCoords0 = value;
			}
			FI;
		}
		else if ( texCoords2.isEnabled() )
		{
			IF( writer, data.texSet() >= 2u )
			{
				texCoords2 = value;
			}
			ELSEIF( data.texSet() >= 1u )
			{
				texCoords1 = value;
			}
			ELSE
			{
				texCoords0 = value;
			}
			FI;
		}
		else if ( texCoords1.isEnabled() )
		{
			IF( writer, data.texSet() >= 1u )
			{
				texCoords1 = value;
			}
			ELSE
			{
				texCoords0 = value;
			}
			FI;
		}
		else
		{
			texCoords0 = value;
		}
	}

	//*********************************************************************************************
}
