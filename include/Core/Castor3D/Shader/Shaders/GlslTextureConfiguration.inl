#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"

#include "Castor3D/Limits.hpp"
#include "Castor3D/Material/Pass/Component/PassShaders.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureAnimation.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

namespace castor3d
{
	namespace shader
	{
		//*********************************************************************************************

		template< typename TexcoordT, typename FlagsT >
		void TextureConfigurations::computeMapsContributionsT( PassShaders const & passShaders
			, FlagsT const & flags
			, TextureAnimations const & textureAnims
			, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
			, Material const & material
			, BlendComponents & components )const
		{
			if ( isEnabled()
				&& components.hasMember( "texture0" ) )
			{
				auto texCoords0 = components.getMember< TexcoordT >( "texture0" );
				auto texCoords1 = components.getMember< TexcoordT >( "texture1", true );
				auto texCoords2 = components.getMember< TexcoordT >( "texture2", true );
				auto texCoords3 = components.getMember< TexcoordT >( "texture3", true );

				FOR( m_writer, sdw::UInt, index, 0u, index < material.texturesCount && index < MaxPassTextures, ++index )
				{
					auto id = m_writer.declLocale( "c3d_id"
						, material.getTexture( index ) );

					IF( m_writer, id > 0_u )
					{
						auto config = m_writer.declLocale( "c3d_config"
							, getTextureConfiguration( id ) );
						auto anim = m_writer.declLocale( "c3d_anim"
							, textureAnims.getTextureAnimation( id ) );
						auto texcoord = m_writer.declLocale( "c3d_tex"
							, TextureConfigurations::getTexcoord( config
								, texCoords0
								, texCoords1
								, texCoords2
								, texCoords3 ) );
						passShaders.computeMapContribution< TexcoordT >( flags
							, config
							, anim
							, maps[id - 1_u]
							, texcoord
							, components );
					}
					FI;
				}
				ROF;
			}

			passShaders.updateComponents( flags
				, components );
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
}
