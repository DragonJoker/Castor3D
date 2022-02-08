#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"

#include "Castor3D/Shader/Shaders/GlslTextureAnimation.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/CompositeTypes/ArraySsbo.hpp>

using namespace castor;

namespace castor3d
{
	namespace shader
	{
		//*****************************************************************************************

		TextureConfigData::TextureConfigData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: sdw::StructInstance{ writer, std::move( expr ), enabled }
			, colOpa{ getMember< sdw::Vec4 >( "colOpa" ) }
			, spcShn{ getMember< sdw::Vec4 >( "spcShn" ) }
			, metRgh{ getMember< sdw::Vec4 >( "metRgh" ) }
			, emsOcc{ getMember< sdw::Vec4 >( "emsOcc" ) }
			, trsDum{ getMember< sdw::Vec4 >( "trsDum" ) }
			, nmlFcr{ getMember< sdw::Vec4 >( "nmlFcr" ) }
			, hgtFcr{ getMember< sdw::Vec4 >( "hgtFcr" ) }
			, mscVls{ getMember< sdw::Vec4 >( "mscVls" ) }
			, texTrn{ getMember< sdw::Vec4 >( "texTrn" ) }
			, texRot{ getMember< sdw::Vec4 >( "texRot" ) }
			, texScl{ getMember< sdw::Vec4 >( "texScl" ) }
			, tleSet{ getMember< sdw::Vec4 >( "tleSet" ) }
			, colEnbl{ colOpa.x() }
			, colMask{ colOpa.y() }
			, opaEnbl{ colOpa.z() }
			, opaMask{ colOpa.w() }
			, spcEnbl{ spcShn.x() }
			, spcMask{ spcShn.y() }
			, shnEnbl{ spcShn.z() }
			, shnMask{ spcShn.w() }
			, metEnbl{ metRgh.x() }
			, metMask{ metRgh.y() }
			, rghEnbl{ metRgh.z() }
			, rghMask{ metRgh.w() }
			, emsEnbl{ emsOcc.x() }
			, emsMask{ emsOcc.y() }
			, occEnbl{ emsOcc.z() }
			, occMask{ emsOcc.w() }
			, trsEnbl{ trsDum.x() }
			, trsMask{ trsDum.y() }
			, nmlEnbl{ nmlFcr.x() }
			, nmlMask{ nmlFcr.y() }
			, nmlFact{ nmlFcr.z() }
			, nmlGMul{ nmlFcr.w() }
			, hgtEnbl{ hgtFcr.x() }
			, hgtMask{ hgtFcr.y() }
			, hgtFact{ hgtFcr.z() }
			, fneedYI{ mscVls.x() }
			, needsYI{ m_writer->cast< sdw::UInt >( mscVls.x() ) }
			, isTrnfAnim{ mscVls.y() != 0.0_f }
			, isTileAnim{ mscVls.z() != 0.0_f }
		{
		}

		std::unique_ptr< sdw::Struct > TextureConfigData::declare( sdw::ShaderWriter & writer )
		{
			return std::make_unique< sdw::Struct >( writer, makeType( writer.getTypesCache() ) );
		}

		ast::type::BaseStructPtr TextureConfigData::makeType( ast::type::TypesCache & cache )
		{
			auto result = cache.getStruct( ast::type::MemoryLayout::eStd140, "C3D_TextureConfigData" );

			if ( result->empty() )
			{
				result->declMember( "colOpa", ast::type::Kind::eVec4F );
				result->declMember( "spcShn", ast::type::Kind::eVec4F );
				result->declMember( "metRgh", ast::type::Kind::eVec4F );
				result->declMember( "emsOcc", ast::type::Kind::eVec4F );
				result->declMember( "trsDum", ast::type::Kind::eVec4F );
				result->declMember( "nmlFcr", ast::type::Kind::eVec4F );
				result->declMember( "hgtFcr", ast::type::Kind::eVec4F );
				result->declMember( "mscVls", ast::type::Kind::eVec4F );
				result->declMember( "texTrn", ast::type::Kind::eVec4F );
				result->declMember( "texRot", ast::type::Kind::eVec4F );
				result->declMember( "texScl", ast::type::Kind::eVec4F );
				result->declMember( "tleSet", ast::type::Kind::eVec4F );
			}

			return result;
		}

		void TextureConfigData::computeGeometryMapContribution( Utils & utils
			, PassFlags const & passFlags
			, TextureFlags const & textureFlags
			, std::string const & name
			, shader::TextureAnimData const & anim
			, sdw::CombinedImage2DRgba32 const & map
			, sdw::Vec3 & texCoords
			, sdw::Float & opacity
			, sdw::Vec3 & normal
			, sdw::Vec3 & tangent
			, sdw::Vec3 & bitangent
			, sdw::Vec3 & tangentSpaceViewPosition
			, sdw::Vec3 & tangentSpaceFragPosition )
		{
			auto & writer = findWriterMandat( *this );
			auto texCoord = writer.declLocale( "c3d_texCoord" + name
				, texCoords.xy() );
			transformUV( anim, texCoord );

			if ( checkFlag( textureFlags, TextureFlag::eHeight )
				&& ( checkFlag( passFlags, PassFlag::eParallaxOcclusionMappingOne )
					|| checkFlag( passFlags, PassFlag::eParallaxOcclusionMappingRepeat ) ) )
			{
				IF( writer, isHeight() )
				{
					texCoord = utils.parallaxMapping( texCoord
						, normalize( tangentSpaceViewPosition - tangentSpaceFragPosition )
						, map
						, *this );
					texCoords.xy() = texCoord;

					if ( checkFlag( passFlags, PassFlag::eParallaxOcclusionMappingOne ) )
					{
						IF( writer, texCoords.x() > 1.0_f
							|| texCoords.y() > 1.0_f
							|| texCoords.x() < 0.0_f
							|| texCoords.y() < 0.0_f )
						{
							writer.demote();
						}
						FI;
					}
				}
				FI;
			}

			auto sampled = writer.declLocale( "c3d_sampled" + name
				, map.sample( texCoord ) );
			applyNormal( textureFlags, sampled, normal, tangent, bitangent, normal );
			applyOpacity( textureFlags, sampled, opacity );
		}

		sdw::Vec4 TextureConfigData::computeCommonMapContribution( Utils & utils
			, PassFlags const & passFlags
			, TextureFlags const & textureFlags
			, std::string const & name
			, shader::TextureAnimData const & anim
			, sdw::CombinedImage2DRgba32 const & map
			, sdw::Vec3 const & texCoords
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
			auto texCoord = writer.declLocale( "c3d_texCoord" + name
				, texCoords.xy() );

			if ( checkFlag( textureFlags, TextureFlag::eHeight )
				&& ( checkFlag( passFlags, PassFlag::eParallaxOcclusionMappingOne )
					|| checkFlag( passFlags, PassFlag::eParallaxOcclusionMappingRepeat ) ) )
			{
				IF( writer, isHeight() )
				{
					texCoord = utils.parallaxMapping( texCoord
						, normalize( tangentSpaceViewPosition - tangentSpaceFragPosition )
						, map
						, *this );
					texCoords.xy() = texCoord;

					if ( checkFlag( passFlags, PassFlag::eParallaxOcclusionMappingOne ) )
					{
						IF( writer, texCoords.x() > 1.0_f
							|| texCoords.y() > 1.0_f
							|| texCoords.x() < 0.0_f
							|| texCoords.y() < 0.0_f )
						{
							writer.demote();
						}
						FI;
					}
				}
				FI;
			}

			transformUV( anim, texCoord );
			auto result = writer.declLocale( "c3d_result" + name
				, map.sample( texCoord ) );
			applyNormal( textureFlags, result, normal, tangent, bitangent, normal );
			applyOpacity( textureFlags, result, opacity );
			applyEmissive( textureFlags, result, emissive );
			applyOcclusion( textureFlags, result, occlusion );
			applyTransmittance( textureFlags, result, transmittance );
			return result;
		}

		sdw::Vec4 TextureConfigData::computeCommonMapVoxelContribution( PassFlags const & passFlags
			, TextureFlags const & textureFlags
			, std::string const & name
			, shader::TextureAnimData const & anim
			, sdw::CombinedImage2DRgba32 const & map
			, sdw::Vec3 const & texCoords
			, sdw::Vec3 & emissive
			, sdw::Float & opacity
			, sdw::Float & occlusion )
		{
			auto & writer = findWriterMandat( *this );
			auto texCoord = writer.declLocale( "c3d_texCoord" + name
				, texCoords.xy() );
			transformUV( anim, texCoord );
			auto result = writer.declLocale< sdw::Vec4 >( "c3d_result" + name
				, map.sample( texCoord ) );
			applyOpacity( textureFlags, result, opacity );
			applyEmissive( textureFlags, result, emissive );
			applyOcclusion( textureFlags, result, occlusion );
			return result;
		}

		void TextureConfigData::transformUV( TextureAnimData const & config
			, sdw::Vec2 & uv )const
		{
			convertUV( uv );

			IF( *m_writer, isTrnfAnim )
			{
				uv = vec2( uv.x()
					, mix( uv.y(), 1.0_f - uv.y(), fneedYI ) );
				uv = scaleUV( config.texScl.xy(), uv );
				uv = rotateUV( config.texRot.xy(), uv );
				uv = translateUV( config.texTrn.xy(), uv );
			}
			FI;

			convertToTile( uv );

			IF( *m_writer, isTileAnim )
			{
				uv.x() += config.tleSet.x() / config.tleSet.z();
				uv.y() += config.tleSet.y() / config.tleSet.w();
			}
			FI;
		}

		void TextureConfigData::transformUVW( TextureAnimData const & config
			, sdw::Vec3 & uvw )const
		{
			convertUVW( uvw );

			IF( *m_writer, isTrnfAnim )
			{
				uvw = vec3( uvw.x()
					, mix( uvw.y(), 1.0_f - uvw.y(), fneedYI )
					, uvw.z() );
				uvw = scaleUV( config.texScl.xyz(), uvw );
				uvw = rotateUV( config.texRot.xyz(), uvw );
				uvw = translateUV( config.texTrn.xyz(), uvw );
			}
			FI;
		}

		sdw::Float TextureConfigData::getGlossiness( sdw::Vec4 const & sampled
			, sdw::Float const & glossiness )const
		{
			return glossiness * getFloat( sampled, shnMask );
		}

		sdw::Vec3 TextureConfigData::getColour( sdw::Vec4 const & sampled
			, sdw::Vec3 const & colour )const
		{
			return colour * getVec3( sampled, colMask );
		}

		sdw::Float TextureConfigData::getOpacity( sdw::Vec4 const & sampled
			, sdw::Float const & opacity )const
		{
			return opacity * getFloat( sampled, opaMask );
		}

		void TextureConfigData::applyDiffuse( TextureFlags const & textureFlags
			, sdw::Vec4 const & sampled
			, sdw::Vec3 & diffuse )const
		{
			if ( checkFlag( textureFlags, TextureFlag::eDiffuse ) )
			{
				IF( *getWriter(), isDiffuse() )
				{
					diffuse = diffuse * getVec3( sampled, colMask );
				}
				FI;
			}
		}

		void TextureConfigData::applyAlbedo( TextureFlags const & textureFlags
			, sdw::Vec4 const & sampled
			, sdw::Vec3 & albedo )const
		{
			if ( checkFlag( textureFlags, TextureFlag::eAlbedo ) )
			{
				IF( *getWriter(), isAlbedo() )
				{
					albedo = albedo * getVec3( sampled, colMask );
				}
				FI;
			}
		}

		void TextureConfigData::applyEmissive( TextureFlags const & textureFlags
			, sdw::Vec4 const & sampled
			, sdw::Vec3 & emissive )const
		{
			if ( checkFlag( textureFlags, TextureFlag::eEmissive ) )
			{
				IF( *getWriter(), isEmissive() )
				{
					emissive = emissive * getVec3( sampled, emsMask );
				}
				FI;
			}
		}

		void TextureConfigData::applySpecular( TextureFlags const & textureFlags
			, sdw::Vec4 const & sampled
			, sdw::Vec3 & specular )const
		{
			if ( checkFlag( textureFlags, TextureFlag::eSpecular ) )
			{
				IF( *getWriter(), isSpecular() )
				{
					specular = specular * getVec3( sampled, spcMask );
				}
				FI;
			}
		}

		void TextureConfigData::applyMetalness( TextureFlags const & textureFlags
			, sdw::Vec4 const & sampled
			, sdw::Float & metalness )const
		{
			if ( checkFlag( textureFlags, TextureFlag::eMetalness ) )
			{
				IF( *getWriter(), isMetalness() )
				{
					metalness = metalness * getFloat( sampled, metMask );
				}
				FI;
			}
		}

		void TextureConfigData::applyShininess( TextureFlags const & textureFlags
			, sdw::Vec4 const & sampled
			, sdw::Float & shininess )const
		{
			if ( checkFlag( textureFlags, TextureFlag::eShininess ) )
			{
				IF( *getWriter(), isShininess() )
				{
					shininess = shininess
						* clamp( getFloat( sampled, shnMask )
							, 0.00390625_f // 1 / 256
							, 1.0_f );
				}
				FI;
			}
		}

		void TextureConfigData::applyRoughness( TextureFlags const & textureFlags
			, sdw::Vec4 const & sampled
			, sdw::Float & roughness )const
		{
			if ( checkFlag( textureFlags, TextureFlag::eRoughness ) )
			{
				IF( *getWriter(), isRoughness() )
				{
					roughness = roughness * getFloat( sampled, rghMask );
				}
				FI;
			}
		}

		void TextureConfigData::applyOpacity( TextureFlags const & textureFlags
			, sdw::Vec4 const & sampled
			, sdw::Float & opacity )const
		{
			if ( checkFlag( textureFlags, TextureFlag::eOpacity ) )
			{
				IF( *getWriter(), isOpacity() )
				{
					opacity = opacity * getFloat( sampled, opaMask );
				}
				FI;
			}
		}

		void TextureConfigData::applyNormal( TextureFlags const & textureFlags
			, sdw::Vec4 const & sampled
			, sdw::Mat3 const & tbn
			, sdw::Vec3 & normal )const
		{
			if ( checkFlag( textureFlags, TextureFlag::eNormal ) )
			{
				IF( *getWriter(), isNormal() )
				{
					normal = normalize( tbn
						* fma( getVec3( sampled, nmlMask )
							, vec3( 2.0_f )
							, -vec3( 1.0_f ) ) );
				}
				FI;
			}
		}

		void TextureConfigData::applyNormal( TextureFlags const & textureFlags
			, sdw::Vec4 const & sampled
			, sdw::Vec3 const & normal
			, sdw::Vec3 const & tangent
			, sdw::Vec3 const & bitangent
			, sdw::Vec3 & result )const
		{
			if ( checkFlag( textureFlags, TextureFlag::eNormal ) )
			{
				IF( *getWriter(), isNormal() )
				{
					result = normalize( shader::Utils::getTBN( normal, tangent, bitangent )
						* fma( getVec3( sampled, nmlMask )
							, vec3( 2.0_f )
							, -vec3( 1.0_f ) ) );
				}
				FI;
			}
		}

		void TextureConfigData::applyHeight( TextureFlags const & textureFlags
			, sdw::Vec4 const & sampled
			, sdw::Float & height )const
		{
			if ( checkFlag( textureFlags, TextureFlag::eHeight ) )
			{
				IF( *getWriter(), isHeight() )
				{
					height = hgtFact * getFloat( sampled, hgtMask );
				}
				FI;
			}
		}

		void TextureConfigData::applyOcclusion( TextureFlags const & textureFlags
			, sdw::Vec4 const & sampled
			, sdw::Float & occlusion )const
		{
			if ( checkFlag( textureFlags, TextureFlag::eOcclusion ) )
			{
				IF( *getWriter(), isOcclusion() )
				{
					occlusion = getFloat( sampled, occMask );
				}
				FI;
			}
		}

		void TextureConfigData::applyTransmittance( TextureFlags const & textureFlags
			, sdw::Vec4 const & sampled
			, sdw::Float & transmittance )const
		{
			if ( checkFlag( textureFlags, TextureFlag::eTransmittance ) )
			{
				IF( *getWriter(), isTransmittance() )
				{
					transmittance = getFloat( sampled, trsMask );
				}
				FI;
			}
		}

		sdw::Float TextureConfigData::getFloat( sdw::Vec4 const & sampled
			, sdw::Float const & mask )const
		{
			return sampled[m_writer->cast< sdw::UInt >( mask )];
		}

		sdw::Vec3 TextureConfigData::getVec3( sdw::Vec4 const & sampled
			, sdw::Float const & mask )const
		{
			return m_writer->ternary( mask == 0.0_f
				, sampled.rgb()
				, sampled.gba() );
		}

		void TextureConfigData::convertUV( sdw::Vec2 & uv )const
		{
			uv = vec2( uv.x()
				, mix( uv.y(), 1.0_f - uv.y(), fneedYI ) );
			uv = scaleUV( texScl.xy(), uv );
			uv = rotateUV( texRot.xy(), uv );
			uv = translateUV( texTrn.xy(), uv );
		}

		void TextureConfigData::convertUVW( sdw::Vec3 & uvw )const
		{
			uvw = vec3( uvw.x()
				, mix( uvw.y(), 1.0_f - uvw.y(), fneedYI )
				, uvw.z() );
			uvw = scaleUV( texScl.xyz(), uvw );
			uvw = rotateUV( texRot.xyz(), uvw );
			uvw = translateUV( texTrn.xyz(), uvw );
		}

		void TextureConfigData::convertToTile( sdw::Vec2 & uv )const
		{
			uv.x() = ( uv.x() + tleSet.x() ) / tleSet.z();
			uv.y() = ( uv.y() + tleSet.y() ) / tleSet.w();
		}

		//*********************************************************************************************

		TextureConfigurations::TextureConfigurations( sdw::ShaderWriter & writer )
			: m_writer{ writer }
		{
		}

		void TextureConfigurations::declare( uint32_t binding, uint32_t set )
		{
			m_ssbo = std::make_unique< sdw::ArraySsboT< TextureConfigData > >( m_writer
				, TextureConfigurationBufferName
				, binding
				, set
				, true );
		}

		TextureConfigData TextureConfigurations::getTextureConfiguration( sdw::UInt const & index )const
		{
			return ( *m_ssbo )[index - 1_u];
		}

		//*********************************************************************************************
	}
}
