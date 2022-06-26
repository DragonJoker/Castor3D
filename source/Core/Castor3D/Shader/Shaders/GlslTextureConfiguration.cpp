#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"

#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureAnimation.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/ModelDataUbo.hpp"

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/CompositeTypes/ArraySsbo.hpp>

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
			, texSet{ m_writer->cast< sdw::UInt >( mscVls.w() ) }
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

			if ( checkFlag( textureFlags, TextureFlag::eOpacity ) )
			{
				IF( *getWriter(), isOpacity() )
				{
					auto sampled = writer.declLocale( "c3d_sampled" + name
						, utils.sampleMap( passFlags, map, texCoord ) );
					opacity = opacity * getFloat( sampled, opaMask );
				}
				FI;
			}
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
				, utils.sampleMap( passFlags, map, texCoord ) );
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
				, utils.sampleMap( passFlags, map, texCoord ) );
			applyNormal( textureFlags, result, normal, tangent, bitangent, normal );
			applyOpacity( textureFlags, result, opacity );
			applyEmissive( textureFlags, result, emissive );
			applyOcclusion( textureFlags, result, occlusion );
			applyTransmittance( textureFlags, result, transmittance );
			return result;
		}

		sdw::Vec4 TextureConfigData::computeCommonMapVoxelContribution( Utils & utils
			, PassFlags const & passFlags
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
			auto result = writer.declLocale( "c3d_result" + name
				, utils.sampleMap( passFlags, map, texCoord ) );
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
					diffuse *= getVec3( sampled, colMask );
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
					albedo *= getVec3( sampled, colMask );
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
					emissive *= getVec3( sampled, emsMask );
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
					specular *= getVec3( sampled, spcMask );
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
					metalness *= getFloat( sampled, metMask );
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
					shininess *= clamp( getFloat( sampled, shnMask )
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
					roughness *= getFloat( sampled, rghMask );
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
					opacity *= getFloat( sampled, opaMask );
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

		TextureConfigurations::TextureConfigurations( sdw::ShaderWriter & writer
			, bool enable )
			: m_writer{ writer }
			, m_enable{ enable }
		{
		}

		TextureConfigurations::TextureConfigurations( sdw::ShaderWriter & writer
			, uint32_t binding
			, uint32_t set
			, bool enable )
			: TextureConfigurations{ writer, enable }
		{
			if ( m_enable )
			{
				declare( binding, set );
			}
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

		void TextureConfigurations::computeGeometryMapContributions( Utils & utils
			, PassFlags const & passFlags
			, TextureFlagsArray const & textures
			, TextureAnimations const & textureAnims
			, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
			, shader::Material const & material
			, sdw::Vec3 & texCoords0
			, sdw::Vec3 & texCoords1
			, sdw::Vec3 & texCoords2
			, sdw::Vec3 & texCoords3
			, sdw::Float & opacity
			, sdw::Vec3 & tangentSpaceViewPosition
			, sdw::Vec3 & tangentSpaceFragPosition )const
		{
			if ( !isEnabled() )
			{
				return;
			}

			auto textureFlags = merge( textures );

			if ( ( checkFlag( textureFlags, TextureFlag::eHeight )
				&& ( checkFlag( passFlags, PassFlag::eParallaxOcclusionMappingOne )
					|| checkFlag( passFlags, PassFlag::eParallaxOcclusionMappingRepeat ) ) )
				|| checkFlag( textureFlags, TextureFlag::eOpacity ) )
			{
				for ( uint32_t index = 0u; index < textures.size(); ++index )
				{
					auto name = castor::string::stringCast< char >( castor::string::toString( index ) );
					auto id = m_writer.declLocale( "c3d_id" + name
						, material.getTexture( index ) );

					IF( m_writer, id > 0_u )
					{
						auto config = m_writer.declLocale( "config" + name
							, getTextureConfiguration( id ) );
						auto anim = m_writer.declLocale( "anim" + name
							, textureAnims.getTextureAnimation( id ) );
						auto texcoordi = m_writer.declLocale( "tex" + name
							, getTexcoord( config
								, texCoords0
								, texCoords1
								, texCoords2
								, texCoords3 ) );
						config.computeGeometryMapContribution( utils
							, passFlags
							, textureFlags
							, name
							, anim
							, maps[id - 1_u]
							, texcoordi
							, opacity
							, tangentSpaceViewPosition
							, tangentSpaceFragPosition );
					}
					FI;
				}
			}
		}

		sdw::Vec3 TextureConfigurations::getTexcoord( TextureConfigData const & data
			, sdw::Vec3 const & texCoords0
			, sdw::Vec3 const & texCoords1
			, sdw::Vec3 const & texCoords2
			, sdw::Vec3 const & texCoords3 )const
		{
			if ( texCoords3.isEnabled() )
			{
				if ( !m_getTexcoord4 )
				{
					m_getTexcoord4 = m_writer.implementFunction< sdw::Vec3 >( "c3d_getTexCoord4"
						, [&]( sdw::UInt const & texSet
							, sdw::Vec3 const & texCoord0
							, sdw::Vec3 const & texCoord1
							, sdw::Vec3 const & texCoord2
							, sdw::Vec3 const & texCoord3 )
						{
							m_writer.returnStmt( m_writer.ternary( texSet >= 3u
								, texCoord3
								, m_writer.ternary( texSet >= 2u
									, texCoord2
									, m_writer.ternary( texSet >= 1u
										, texCoord1
										, texCoord0 ) ) ) );
						}
						, sdw::InUInt{ m_writer, "texSet" }
						, sdw::InVec3{ m_writer, "texcoord0" }
						, sdw::InVec3{ m_writer, "texcoord1" }
						, sdw::InVec3{ m_writer, "texcoord2" }
						, sdw::InVec3{ m_writer, "texcoord3" } );
				}

				return m_getTexcoord4( data.texSet
					, texCoords0
					, texCoords1
					, texCoords2
					, texCoords3 );
			}
			else if ( texCoords2.isEnabled() )
			{
				if ( !m_getTexcoord3 )
				{
					m_getTexcoord3 = m_writer.implementFunction< sdw::Vec3 >( "c3d_getTexCoord3"
						, [&]( sdw::UInt const & texSet
							, sdw::Vec3 const & texCoord0
							, sdw::Vec3 const & texCoord1
							, sdw::Vec3 const & texCoord2 )
						{
							m_writer.returnStmt( m_writer.ternary( texSet >= 2u
								, texCoord2
								, m_writer.ternary( texSet >= 1u
									, texCoord1
									, texCoord0 ) ) );
						}
						, sdw::InUInt{ m_writer, "texSet" }
						, sdw::InVec3{ m_writer, "texcoord0" }
						, sdw::InVec3{ m_writer, "texcoord1" }
						, sdw::InVec3{ m_writer, "texcoord2" } );
				}

				return m_getTexcoord3( data.texSet
					, texCoords0
					, texCoords1
					, texCoords2 );
			}
			else if ( texCoords1.isEnabled() )
			{
				if ( !m_getTexcoord2 )
				{
					m_getTexcoord2 = m_writer.implementFunction< sdw::Vec3 >( "c3d_getTexCoord2"
						, [&]( sdw::UInt const & texSet
							, sdw::Vec3 const & texCoord0
							, sdw::Vec3 const & texCoord1 )
						{
							m_writer.returnStmt( m_writer.ternary( texSet >= 1u
								, texCoord1
								, texCoord0 ) );
						}
						, sdw::InUInt{ m_writer, "texSet" }
						, sdw::InVec3{ m_writer, "texcoord0" }
						, sdw::InVec3{ m_writer, "texcoord1" } );
				}

				return m_getTexcoord2( data.texSet
					, texCoords0
					, texCoords1 );
			}
			else
			{
				return texCoords0;
			}
		}

		void TextureConfigurations::setTexcoord( TextureConfigData const & data
			, sdw::Vec3 const & value
			, sdw::Vec3 & texCoords0
			, sdw::Vec3 & texCoords1
			, sdw::Vec3 & texCoords2
			, sdw::Vec3 & texCoords3 )const
		{
			if ( texCoords3.isEnabled() )
			{
				if ( !m_setTexcoord4 )
				{
					m_setTexcoord4 = m_writer.implementFunction< sdw::Void >( "c3d_setTexCoord4"
						, [&]( sdw::UInt const & texSet
							, sdw::Vec3 const & texCoord
							, sdw::Vec3 texCoord0
							, sdw::Vec3 texCoord1
							, sdw::Vec3 texCoord2
							, sdw::Vec3 texCoord3 )
						{
							IF( m_writer, texSet >= 3u )
							{
								texCoord3 = texCoord;
							}
							ELSEIF( texSet >= 2u )
							{
								texCoord2 = texCoord;
							}
							ELSEIF( texSet >= 1u )
							{
								texCoord1 = texCoord;
							}
							ELSE
							{
								texCoord0 = texCoord;
							}
							FI;
						}
						, sdw::InUInt{ m_writer, "texSet" }
						, sdw::InVec3{ m_writer, "texCoord" }
						, sdw::InOutVec3{ m_writer, "texcoord0" }
						, sdw::InOutVec3{ m_writer, "texcoord1" } 
						, sdw::InOutVec3{ m_writer, "texcoord2" }
						, sdw::InOutVec3{ m_writer, "texcoord3" } );
				}

				m_setTexcoord4( data.texSet
					, value
					, texCoords0
					, texCoords1
					, texCoords2
					, texCoords3 );
			}
			else if ( texCoords2.isEnabled() )
			{
				if ( !m_setTexcoord3 )
				{
					m_setTexcoord3 = m_writer.implementFunction< sdw::Void >( "c3d_setTexCoord3"
						, [&]( sdw::UInt const & texSet
							, sdw::Vec3 const & texCoord
							, sdw::Vec3 texCoord0
							, sdw::Vec3 texCoord1
							, sdw::Vec3 texCoord2 )
						{
							IF( m_writer, texSet >= 2u )
							{
								texCoord2 = texCoord;
							}
							ELSEIF( texSet >= 1u )
							{
								texCoord1 = texCoord;
							}
							ELSE
							{
								texCoord0 = texCoord;
							}
							FI;
						}
						, sdw::InUInt{ m_writer, "texSet" }
						, sdw::InVec3{ m_writer, "texCoord" }
						, sdw::InOutVec3{ m_writer, "texcoord0" }
						, sdw::InOutVec3{ m_writer, "texcoord1" } 
						, sdw::InOutVec3{ m_writer, "texcoord2" } );
				}

				m_setTexcoord3( data.texSet
					, value
					, texCoords0
					, texCoords1
					, texCoords2 );
			}
			else if ( texCoords1.isEnabled() )
			{
				if ( !m_setTexcoord2 )
				{
					m_setTexcoord2 = m_writer.implementFunction< sdw::Void >( "c3d_setTexCoord2"
						, [&]( sdw::UInt const & texSet
							, sdw::Vec3 const & texCoord
							, sdw::Vec3 texCoord0
							, sdw::Vec3 texCoord1 )
						{
							IF( m_writer, texSet >= 1u )
							{
								texCoord1 = texCoord;
							}
							ELSE
							{
								texCoord0 = texCoord;
							}
							FI;
						}
						, sdw::InUInt{ m_writer, "texSet" }
						, sdw::InVec3{ m_writer, "texCoord" }
						, sdw::InOutVec3{ m_writer, "texcoord0" }
						, sdw::InOutVec3{ m_writer, "texcoord1" } );
				}

				m_setTexcoord2( data.texSet
					, value
					, texCoords0
					, texCoords1 );
			}
			else
			{
				texCoords0 = value;
			}
		}

		//*********************************************************************************************
	}
}
