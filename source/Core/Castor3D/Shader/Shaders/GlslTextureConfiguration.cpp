#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"

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
			, isAnim{ mscVls.y() != 0.0_f }
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

		sdw::Vec3 TextureConfigData::getDiffuse( sdw::Vec4 const & sampled
			, sdw::Vec3 const & diffuse )const
		{
			return diffuse * getVec3( sampled, colMask );
		}

		sdw::Vec3 TextureConfigData::getAlbedo( sdw::Vec4 const & sampled
			, sdw::Vec3 const & albedo )const
		{
			return albedo * getVec3( sampled, colMask );
		}

		sdw::Vec3 TextureConfigData::getEmissive( sdw::Vec4 const & sampled
			, sdw::Vec3 const & emissive )const
		{
			return emissive * getVec3( sampled, emsMask );
		}

		sdw::Vec3 TextureConfigData::getSpecular( sdw::Vec4 const & sampled
			, sdw::Vec3 const & specular )const
		{
			return specular * getVec3( sampled, spcMask );
		}

		sdw::Float TextureConfigData::getMetalness( sdw::Vec4 const & sampled
			, sdw::Float const & metalness )const
		{
			return metalness * getFloat( sampled, metMask );
		}

		sdw::Float TextureConfigData::getShininess( sdw::Vec4 const & sampled
			, sdw::Float const & shininess )const
		{
			return shininess
				* clamp( getFloat( sampled, shnMask )
					, 0.00390625_f // 1 / 256
					, 1.0_f );
		}

		sdw::Float TextureConfigData::getGlossiness( sdw::Vec4 const & sampled
			, sdw::Float const & glossiness )const
		{
			return glossiness * getFloat( sampled, shnMask );
		}

		sdw::Float TextureConfigData::getRoughness( sdw::Vec4 const & sampled
			, sdw::Float const & roughness )const
		{
			return roughness * getFloat( sampled, rghMask );
		}

		sdw::Float TextureConfigData::getOpacity( sdw::Vec4 const & sampled
			, sdw::Float const & opacity )const
		{
			return opacity * getFloat( sampled, opaMask );
		}

		sdw::Vec3 TextureConfigData::getNormal( sdw::Vec4 const & sampled
			, sdw::Mat3 const & tbn )const
		{
			return normalize( tbn
				* fma( getVec3( sampled, nmlMask )
					, vec3( 2.0_f )
					, -vec3( 1.0_f ) ) );
		}

		sdw::Vec3 TextureConfigData::getNormal( sdw::Vec4 const & sampled
			, sdw::Vec3 const & normal
			, sdw::Vec3 const & tangent
			, sdw::Vec3 const & bitangent )const
		{
			return getNormal( sampled
				, shader::Utils::getTBN( normal, tangent, bitangent ) );
		}

		sdw::Float TextureConfigData::getHeight( sdw::Vec4 const & sampled )const
		{
			return hgtFact * getFloat( sampled, hgtMask );
		}

		sdw::Float TextureConfigData::getOcclusion( sdw::Vec4 const & sampled
			, sdw::Float const & occlusion )const
		{
			return getFloat( sampled, occMask );
		}

		sdw::Float TextureConfigData::getTransmittance( sdw::Vec4 const & sampled
			, sdw::Float const & transmittance )const
		{
			return getFloat( sampled, trsMask );
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

			IF( *m_writer, tleSet.z() > 1.0_f )
			{
				uv.x() /= tleSet.z();
				uv.x() += tleSet.x() / tleSet.z();
			}
			FI;

			IF( *m_writer, tleSet.w() > 1.0_f )
			{
				uv.x() /= tleSet.w();
				uv.x() += tleSet.y() / tleSet.w();
			}
			FI;
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

		//*********************************************************************************************

		TextureConfigurations::TextureConfigurations( sdw::ShaderWriter & writer )
			: m_writer{ writer }
		{
		}

		void TextureConfigurations::declare( bool hasSsbo
			, uint32_t binding
			, uint32_t set )
		{

			if ( hasSsbo )
			{
				m_ssbo = std::make_unique< sdw::ArraySsboT< TextureConfigData > >( m_writer
					, TextureConfigurationBufferName
					, binding
					, set
					, true );
			}
			else
			{
				auto c3d_textureConfigurations = m_writer.declSampledImage< FImgBufferRgba32 >( "c3d_textureConfigurations"
					, binding
					, set );
				m_getTextureConfiguration = m_writer.implementFunction< TextureConfigData >( "getTextureConfiguration"
					, [this, &c3d_textureConfigurations]( sdw::UInt const & index )
					{
						auto result = m_writer.declLocale< TextureConfigData >( "result" );
						auto offset = m_writer.declLocale( cuT( "offset" )
							, m_writer.cast< sdw::Int >( index ) * sdw::Int( shader::MaxTextureConfigurationComponentsCount ) );
						result.colOpa = c3d_textureConfigurations.fetch( sdw::Int{ offset++ } );
						result.spcShn = c3d_textureConfigurations.fetch( sdw::Int{ offset++ } );
						result.metRgh = c3d_textureConfigurations.fetch( sdw::Int{ offset++ } );
						result.emsOcc = c3d_textureConfigurations.fetch( sdw::Int{ offset++ } );
						result.trsDum = c3d_textureConfigurations.fetch( sdw::Int{ offset++ } );
						result.nmlFcr = c3d_textureConfigurations.fetch( sdw::Int{ offset++ } );
						result.hgtFcr = c3d_textureConfigurations.fetch( sdw::Int{ offset++ } );
						result.mscVls = c3d_textureConfigurations.fetch( sdw::Int{ offset++ } );
						result.texTrn = c3d_textureConfigurations.fetch( sdw::Int{ offset++ } );
						result.texRot = c3d_textureConfigurations.fetch( sdw::Int{ offset++ } );
						result.texScl = c3d_textureConfigurations.fetch( sdw::Int{ offset++ } );
						result.tleSet = c3d_textureConfigurations.fetch( sdw::Int{ offset++ } );
						m_writer.returnStmt( result );
					}
					, sdw::InUInt{ m_writer, cuT( "index" ) } );
			}
		}

		TextureConfigData TextureConfigurations::getTextureConfiguration( sdw::UInt const & index )const
		{
			if ( m_ssbo )
			{
				return ( *m_ssbo )[index - 1_u];
			}

			return m_getTextureConfiguration( index - 1_u );
		}

		//*********************************************************************************************
	}
}
