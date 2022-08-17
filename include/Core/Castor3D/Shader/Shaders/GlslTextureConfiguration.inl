#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"

#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureAnimation.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

namespace castor3d
{
	namespace shader
	{
		template< typename TexcoordT >
		void TextureConfigData::computeGeometryMapContribution( Utils & utils
			, PassFlags const & passFlags
			, TextureFlags const & textureFlags
			, shader::TextureAnimData const & anim
			, sdw::CombinedImage2DRgba32 const & map
			, TexcoordT & texCoords
			, sdw::Float & opacity
			, sdw::Vec3 & tangentSpaceViewPosition
			, sdw::Vec3 & tangentSpaceFragPosition )
		{
			auto & writer = findWriterMandat( *this );
			auto texCoord = writer.declLocale( "c3d_texCoord"
				, toUv( texCoords ) );
			transformUV( utils, anim, texCoord );

			if ( checkFlag( textureFlags, TextureFlag::eHeight )
				&& ( checkFlag( passFlags, PassFlag::eParallaxOcclusionMappingOne )
					|| checkFlag( passFlags, PassFlag::eParallaxOcclusionMappingRepeat ) ) )
			{
				IF( writer, isHeight() )
				{
					utils.parallaxMapping( texCoord
						, normalize( tangentSpaceViewPosition - tangentSpaceFragPosition )
						, map
						, *this );
					setUv( texCoords, texCoord );

					if ( checkFlag( passFlags, PassFlag::eParallaxOcclusionMappingOne ) )
					{
						IF( writer, getUv( texCoords ).x() > 1.0_f
							|| getUv( texCoords ).y() > 1.0_f
							|| getUv( texCoords ).x() < 0.0_f
							|| getUv( texCoords ).y() < 0.0_f )
						{
							writer.demote();
						}
						FI;
					}
				}
				FI;
			}

			if ( checkFlag( textureFlags, TextureFlag::eOpacity ) )
			{
				IF( *getWriter(), isOpacity() )
				{
					auto sampled = writer.declLocale( "c3d_sampled"
						, utils.sampleMap( passFlags, map, texCoord ) );
					opacity = opacity * getFloat( sampled, opaMask );
				}
				FI;
			}
		}

		template< typename TexcoordT >
		void TextureConfigData::computeGeometryMapContribution( Utils & utils
			, PassFlags const & passFlags
			, TextureFlags const & textureFlags
			, shader::TextureAnimData const & anim
			, sdw::CombinedImage2DRgba32 const & map
			, TexcoordT & texCoords
			, sdw::Float & opacity
			, sdw::Vec3 & normal
			, sdw::Vec3 & tangent
			, sdw::Vec3 & bitangent
			, sdw::Vec3 & tangentSpaceViewPosition
			, sdw::Vec3 & tangentSpaceFragPosition )
		{
			auto & writer = findWriterMandat( *this );
			auto texCoord = writer.declLocale( "c3d_texCoord"
				, toUv( texCoords ) );
			transformUV( utils, anim, texCoord );

			if ( checkFlag( textureFlags, TextureFlag::eHeight )
				&& ( checkFlag( passFlags, PassFlag::eParallaxOcclusionMappingOne )
					|| checkFlag( passFlags, PassFlag::eParallaxOcclusionMappingRepeat ) ) )
			{
				IF( writer, isHeight() )
				{
					utils.parallaxMapping( texCoord
						, normalize( tangentSpaceViewPosition - tangentSpaceFragPosition )
						, map
						, *this );
					setUv( texCoords, texCoord );

					if ( checkFlag( passFlags, PassFlag::eParallaxOcclusionMappingOne ) )
					{
						IF( writer, getUv( texCoords ).x() > 1.0_f
							|| getUv( texCoords ).y() > 1.0_f
							|| getUv( texCoords ).x() < 0.0_f
							|| getUv( texCoords ).y() < 0.0_f )
						{
							writer.demote();
						}
						FI;
					}
				}
				FI;
			}

			auto sampled = writer.declLocale( "c3d_sampled"
				, utils.sampleMap( passFlags, map, texCoord ) );
			applyNormal( textureFlags, sampled, normal, tangent, bitangent, normal );
			applyOpacity( textureFlags, sampled, opacity );
		}

		template< typename TexcoordT >
		sdw::Vec4 TextureConfigData::computeCommonMapContribution( Utils & utils
			, PassFlags const & passFlags
			, TextureFlags const & textureFlags
			, shader::TextureAnimData const & anim
			, sdw::CombinedImage2DRgba32 const & map
			, TexcoordT & texCoords
			, sdw::Vec3 & emissive
			, sdw::Float & opacity
			, sdw::Float & occlusion
			, sdw::Float & transmittance
			, sdw::Vec3 & normal
			, sdw::Vec3 & tangent
			, sdw::Vec3 & bitangent
			, sdw::Vec3 & tangentSpaceViewPosition
			, sdw::Vec3 & tangentSpaceFragPosition )
		{
			auto & writer = findWriterMandat( *this );
			auto texCoord = writer.declLocale( "c3d_texCoord"
				, toUv( texCoords ) );
			transformUV( utils, anim, texCoord );

			if ( checkFlag( textureFlags, TextureFlag::eHeight )
				&& ( checkFlag( passFlags, PassFlag::eParallaxOcclusionMappingOne )
					|| checkFlag( passFlags, PassFlag::eParallaxOcclusionMappingRepeat ) ) )
			{
				IF( writer, isHeight() )
				{
					utils.parallaxMapping( texCoord
						, normalize( tangentSpaceViewPosition - tangentSpaceFragPosition )
						, map
						, *this );
					setUv( texCoords, texCoord );

					if ( checkFlag( passFlags, PassFlag::eParallaxOcclusionMappingOne ) )
					{
						IF( writer, getUv( texCoords ).x() > 1.0_f
							|| getUv( texCoords ).y() > 1.0_f
							|| getUv( texCoords ).x() < 0.0_f
							|| getUv( texCoords ).y() < 0.0_f )
						{
							writer.demote();
						}
						FI;
					}
				}
				FI;
			}

			auto result = writer.declLocale( "c3d_sampled"
				, utils.sampleMap( passFlags, map, texCoord ) );
			applyNormal( textureFlags, result, normal, tangent, bitangent, normal );
			applyOpacity( textureFlags, result, opacity );
			applyEmissive( textureFlags, result, emissive );
			applyOcclusion( textureFlags, result, occlusion );
			applyTransmittance( textureFlags, result, transmittance );
			return result;
		}

		template< typename TexcoordT >
		sdw::Vec4 TextureConfigData::computeCommonMapVoxelContribution( Utils & utils
			, PassFlags const & passFlags
			, TextureFlags const & textureFlags
			, shader::TextureAnimData const & anim
			, sdw::CombinedImage2DRgba32 const & map
			, TexcoordT & texCoords
			, sdw::Vec3 & emissive
			, sdw::Float & opacity
			, sdw::Float & occlusion )
		{
			auto & writer = findWriterMandat( *this );
			auto texCoord = writer.declLocale( "c3d_texCoord"
				, toUv( texCoords ) );
			transformUV( utils, anim, texCoord );
			auto result = writer.declLocale( "c3d_sampled"
				, utils.sampleMap( passFlags, map, texCoord ) );
			applyOpacity( textureFlags, result, opacity );
			applyEmissive( textureFlags, result, emissive );
			applyOcclusion( textureFlags, result, occlusion );
			return result;
		}

		//*********************************************************************************************

		template< typename TexcoordT >
		void TextureConfigurations::computeGeometryMapContributions( Utils & utils
			, PassFlags const & passFlags
			, TextureFlags const & textureFlags
			, TextureAnimations const & textureAnims
			, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
			, shader::Material const & material
			, TexcoordT & texCoords0
			, TexcoordT & texCoords1
			, TexcoordT & texCoords2
			, TexcoordT & texCoords3
			, sdw::Float & opacity
			, sdw::Vec3 & tangentSpaceViewPosition
			, sdw::Vec3 & tangentSpaceFragPosition )const
		{
			if ( !isEnabled() )
			{
				return;
			}

			if ( ( checkFlag( textureFlags, TextureFlag::eHeight )
				&& ( checkFlag( passFlags, PassFlag::eParallaxOcclusionMappingOne )
					|| checkFlag( passFlags, PassFlag::eParallaxOcclusionMappingRepeat ) ) )
				|| checkFlag( textureFlags, TextureFlag::eOpacity ) )
			{
				FOR( m_writer, sdw::UInt, index, 0u, index < material.texturesCount(), ++index )
				{
					auto id = m_writer.declLocale( "c3d_id"
						, material.getTexture( index ) );

					IF( m_writer, id > 0_u )
					{
						auto config = m_writer.declLocale( "config"
							, getTextureConfiguration( id ) );
						auto anim = m_writer.declLocale( "anim"
							, textureAnims.getTextureAnimation( id ) );
						auto texcoordi = m_writer.declLocale( "tex"
							, getTexcoord( config
								, texCoords0
								, texCoords1
								, texCoords2
								, texCoords3 ) );
						config.computeGeometryMapContribution( utils
							, passFlags
							, textureFlags
							, anim
							, maps[id - 1_u]
							, texcoordi
							, opacity
							, tangentSpaceViewPosition
							, tangentSpaceFragPosition );
					}
					FI;
				}
				ROF;
			}
		}

		template< typename TexcoordT >
		TexcoordT TextureConfigurations::getTexcoord( TextureConfigData const & data
			, TexcoordT const & texCoords0
			, TexcoordT const & texCoords1
			, TexcoordT const & texCoords2
			, TexcoordT const & texCoords3 )const
		{
			if ( texCoords3.isEnabled() )
			{
				return m_writer.ternary( data.texSet >= 3u
					, texCoords3
					, m_writer.ternary( data.texSet >= 2u
						, texCoords2
						, m_writer.ternary( data.texSet >= 1u
							, texCoords1
							, texCoords0 ) ) );
			}

			if ( texCoords2.isEnabled() )
			{
				return m_writer.ternary( data.texSet >= 2u
					, texCoords2
					, m_writer.ternary( data.texSet >= 1u
						, texCoords1
						, texCoords0 ) );
			}

			if ( texCoords1.isEnabled() )
			{
				return m_writer.ternary( data.texSet >= 1u
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
			, TexcoordT & texCoords3 )const
		{
			if ( texCoords3.isEnabled() )
			{
				IF( m_writer, data.texSet >= 3u )
				{
					texCoords3 = value;
				}
				ELSEIF( data.texSet >= 2u )
				{
					texCoords2 = value;
				}
				ELSEIF( data.texSet >= 1u )
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
				IF( m_writer, data.texSet >= 2u )
				{
					texCoords2 = value;
				}
				ELSEIF( data.texSet >= 1u )
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
				IF( m_writer, data.texSet >= 1u )
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
