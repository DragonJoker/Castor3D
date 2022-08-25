#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"

#include "Castor3D/Limits.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureAnimation.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

namespace castor3d
{
	namespace shader
	{
		template< typename TexcoordT >
		void TextureConfigData::computeGeometryMapContribution( Utils & utils
			, PipelineFlags const & flags
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
			applyParallax( utils,  flags, map, texCoords, texCoord, tangentSpaceViewPosition, tangentSpaceFragPosition );

			auto sampled = writer.declLocale( "c3d_sampled"
				, utils.sampleMap( flags, map, texCoord ) );
			applyOpacity( flags, sampled, opacity );
		}

		template< typename TexcoordT >
		void TextureConfigData::computeGeometryMapContribution( Utils & utils
			, PipelineFlags const & flags
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
			applyParallax( utils, flags, map, texCoords, texCoord, tangentSpaceViewPosition, tangentSpaceFragPosition );

			auto sampled = writer.declLocale( "c3d_sampled"
				, utils.sampleMap( flags, map, texCoord ) );
			applyNormal( flags, sampled, normal, tangent, bitangent, normal );
			applyOpacity( flags, sampled, opacity );
		}

		template< typename TexcoordT >
		sdw::Vec4 TextureConfigData::computeCommonMapContribution( Utils & utils
			, PipelineFlags const & flags
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
			applyParallax( utils, flags, map, texCoords, texCoord, tangentSpaceViewPosition, tangentSpaceFragPosition );

			auto result = writer.declLocale( "c3d_sampled"
				, utils.sampleMap( flags, map, texCoord ) );
			applyNormal( flags, result, normal, tangent, bitangent, normal );
			applyOpacity( flags, result, opacity );
			applyEmissive( flags, result, emissive );
			applyOcclusion( flags, result, occlusion );
			applyTransmittance( flags, result, transmittance );
			return result;
		}

		template< typename TexcoordT >
		sdw::Vec4 TextureConfigData::computeCommonMapVoxelContribution( Utils & utils
			, PipelineFlags const & flags
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
				, utils.sampleMap( flags, map, texCoord ) );
			applyOpacity( flags, result, opacity );
			applyEmissive( flags, result, emissive );
			applyOcclusion( flags, result, occlusion );
			return result;
		}

		template< typename TexcoordsT, typename TexcoordT >
		void TextureConfigData::applyParallax( Utils & utils
			, PipelineFlags const & flags
			, sdw::CombinedImage2DRgba32 const & map
			, TexcoordsT & texCoords
			, TexcoordT & texCoord
			, sdw::Vec3 & tangentSpaceViewPosition
			, sdw::Vec3 & tangentSpaceFragPosition )
		{
			if ( flags.enableParallaxOcclusionMapping() )
			{
				auto & writer = findWriterMandat( *this );

				IF( writer, isHeight() )
				{
					utils.parallaxMapping( texCoord
						, normalize( tangentSpaceViewPosition - tangentSpaceFragPosition )
						, map
						, *this );
					setUv( texCoords, texCoord );

					if ( flags.enableParallaxOcclusionMappingOne() )
					{
						IF( writer, this->getUv( texCoords ).x() > 1.0_f
							|| this->getUv( texCoords ).y() > 1.0_f
							|| this->getUv( texCoords ).x() < 0.0_f
							|| this->getUv( texCoords ).y() < 0.0_f )
						{
							writer.demote();
						}
						FI;
					}
				}
				FI;
			}
		}

		//*********************************************************************************************

		template< typename TexcoordT >
		void TextureConfigurations::computeGeometryMapContributions( Utils & utils
			, PipelineFlags const & flags
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

			if ( flags.enableParallaxOcclusionMapping()
				|| flags.enableOpacity() )
			{
				FOR( m_writer, sdw::UInt, index, 0u, index < material.texturesCount() && index < MaxPassTextures, ++index )
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
							, flags
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
