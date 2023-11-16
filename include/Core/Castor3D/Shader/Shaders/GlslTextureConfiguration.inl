#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"

#include "Castor3D/Limits.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslPassShaders.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureAnimation.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

namespace castor3d
{
	namespace shader
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
				if ( !this->m_applyTexture )
				{
					this->m_applyTexture = m_writer.implementFunction< sdw::Void >( "c3d_applyTexture"
						, [&]( sdw::UInt const & id
							, BlendComponents components )
						{
							auto texCoords0 = components.getMember< TexcoordT >( "texture0" );
							auto texCoords1 = components.getMember< TexcoordT >( "texture1", true );
							auto texCoords2 = components.getMember< TexcoordT >( "texture2", true );
							auto texCoords3 = components.getMember< TexcoordT >( "texture3", true );

							IF( m_writer, id > 0_u )
							{
								auto config = m_writer.declLocale( "c3d_config"
									, getTextureConfiguration( nonuniform( id ) ) );
								auto anim = m_writer.declLocale( "c3d_anim"
									, textureAnims.getTextureAnimation( nonuniform( id ) ) );
								auto texCoords = m_writer.declLocale( "c3d_tex"
									, TextureConfigurations::getTexcoord( config
										, texCoords0
										, texCoords1
										, texCoords2
										, texCoords3 ) );
								auto map = maps[nonuniform( id - 1_u )];
								auto & writer = findWriterMandat( config, anim, maps );
								auto texCoord = writer.declLocale( "c3d_texCoord"
									, config.toUv( texCoords ) );
								config.transformUV( passShaders.getUtils(), anim, texCoord );
								config.setUv( texCoords, texCoord );
								std::map< uint32_t, PassComponentTextureFlag > texcoordModifs = passShaders.getTexcoordModifs( flags );

								if ( !texcoordModifs.empty() )
								{
									FOR( m_writer, sdw::UInt, comp, 0u, comp < config.componentCount(), ++comp )
									{
										auto component = writer.declLocale( "c3d_component"
											, config.component( comp ) );

										IF( writer, component.y() != 0_u )
										{
											passShaders.computeTexcoord( flags
												, config
												, component
												, map
												, texCoords
												, texCoord
												, id
												, components );
										}
										FI;
									}
									ROF;
								}

								auto sampled = writer.declLocale( "c3d_sampled"
									, passShaders.sampleMap( flags, map, texCoords, components ) );

								FOR( m_writer, sdw::UInt, comp, 0u, comp < config.componentCount(), ++comp )
								{
									auto component = writer.declLocale( "c3d_component"
										, config.component( comp ) );

									IF( writer, component.y() != 0_u )
									{
										passShaders.applyComponents( flags
											, config
											, component
											, sampled
											, config.getUv( texCoords )
											, components );
									}
									FI;
								}
								ROF;
							}
							FI;
						}
						, sdw::InUInt{ m_writer, "id" }
						, InOutBlendComponents{ m_writer, "components", pcomponents } );
				}

				auto count = shdtex::getTextureFlags( flags ).configCount;

				for ( uint32_t index = 0u; index < count; ++index )
				{
					this->m_applyTexture( pmaterial.getTexture( index ), pcomponents );
				}
			}

			passShaders.updateComponents( flags
				, maps
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
}
