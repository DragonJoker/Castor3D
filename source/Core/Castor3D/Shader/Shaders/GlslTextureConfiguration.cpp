#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"

#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/CompositeTypes/ArraySsbo.hpp>

using namespace castor;

namespace castor3d
{
	namespace shader
	{
		namespace
		{
			template< typename LhsT, typename RhsT >
			LhsT translateUV( LhsT const & translate
				, RhsT const & uv )
			{
				return translate + uv;
			}

			sdw::Vec2 rotateUV( sdw::Vec2 const & rotate
				, sdw::Vec2 const & uv )
			{
				auto mid = 0.5_f;
				return vec2( rotate.x() * ( uv.x() - mid ) + rotate.y() * ( uv.y() - mid ) + mid
					, rotate.x() * ( uv.y() - mid ) - rotate.y() * ( uv.x() - mid ) + mid );
			}

			sdw::Vec3 rotateUV( sdw::Vec3 const & rotate
				, sdw::Vec3 const & uv )
			{
				return ( ( uv - vec3( 0.5_f, 0.5f, 0.5f ) ) * rotate ) + vec3( 0.5_f, 0.5f, 0.5f );
			}

			template< typename LhsT, typename RhsT >
			LhsT scaleUV( LhsT const & scale
				, RhsT const & uv )
			{
				return scale * uv;
			}
		}

		//*****************************************************************************************

		TextureConfigData::TextureConfigData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: sdw::StructInstance{ writer, std::move( expr ), enabled }
			, colOpa{ getMember< sdw::Vec4 >( "colOpa" ) }
			, spcShn{ getMember< sdw::Vec4 >( "spcShn" ) }
			, emsOcc{ getMember< sdw::Vec4 >( "emsOcc" ) }
			, trsDum{ getMember< sdw::Vec4 >( "trsDum" ) }
			, nmlFcr{ getMember< sdw::Vec4 >( "nmlFcr" ) }
			, hgtFcr{ getMember< sdw::Vec4 >( "hgtFcr" ) }
			, mscVls{ getMember< sdw::Vec4 >( "mscVls" ) }
			, texTrn{ getMember< sdw::Vec4 >( "texTrn" ) }
			, texRot{ getMember< sdw::Vec4 >( "texRot" ) }
			, texScl{ getMember< sdw::Vec4 >( "texScl" ) }
			, colEnbl{ colOpa.x() }
			, colMask{ colOpa.y() }
			, opaEnbl{ colOpa.z() }
			, opaMask{ colOpa.w() }
			, spcEnbl{ spcShn.x() }
			, spcMask{ spcShn.y() }
			, shnEnbl{ spcShn.z() }
			, shnMask{ spcShn.w() }
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
			, needsGC{ writer.cast< sdw::UInt >( mscVls.x() ) }
			, fneedYI{ mscVls.y() }
			, needsYI{ writer.cast< sdw::UInt >( mscVls.y() ) }
		{
		}

		std::unique_ptr< sdw::Struct > TextureConfigData::declare( sdw::ShaderWriter & writer )
		{
			return std::make_unique< sdw::Struct >( writer, makeType( writer.getTypesCache() ) );
		}

		ast::type::StructPtr TextureConfigData::makeType( ast::type::TypesCache & cache )
		{
			auto result = cache.getStruct( ast::type::MemoryLayout::eStd140, "TextureConfigData" );

			if ( result->empty() )
			{
				result->declMember( "colOpa", ast::type::Kind::eVec4F );
				result->declMember( "spcShn", ast::type::Kind::eVec4F );
				result->declMember( "emsOcc", ast::type::Kind::eVec4F );
				result->declMember( "trsDum", ast::type::Kind::eVec4F );
				result->declMember( "nmlFcr", ast::type::Kind::eVec4F );
				result->declMember( "hgtFcr", ast::type::Kind::eVec4F );
				result->declMember( "mscVls", ast::type::Kind::eVec4F );
				result->declMember( "texTrn", ast::type::Kind::eVec4F );
				result->declMember( "texRot", ast::type::Kind::eVec4F );
				result->declMember( "texScl", ast::type::Kind::eVec4F );
			}

			return result;
		}

		sdw::Vec3 TextureConfigData::getDiffuse( sdw::ShaderWriter & writer
			, sdw::Vec4 const & sampled
			, sdw::Vec3 const & diffuse
			, sdw::Float gamma )const
		{
			return diffuse * removeGamma( writer
				, getVec3( writer, sampled, colMask )
				, gamma );
		}

		sdw::Vec3 TextureConfigData::getAlbedo( sdw::ShaderWriter & writer
			, sdw::Vec4 const & sampled
			, sdw::Vec3 const & albedo
			, sdw::Float gamma )const
		{
			return albedo * removeGamma( writer
				, getVec3( writer, sampled, colMask )
				, gamma );
		}

		sdw::Vec3 TextureConfigData::getEmissive( sdw::ShaderWriter & writer
			, sdw::Vec4 const & sampled
			, sdw::Vec3 const & emissive
			, sdw::Float gamma )const
		{
			return emissive * removeGamma( writer
				, getVec3( writer, sampled, emsMask )
				, gamma );
		}

		sdw::Vec3 TextureConfigData::getSpecular( sdw::ShaderWriter & writer
			, sdw::Vec4 const & sampled
			, sdw::Vec3 const & specular )const
		{
			return specular * getVec3( writer, sampled, spcMask );
		}

		sdw::Float TextureConfigData::getMetalness( sdw::ShaderWriter & writer
			, sdw::Vec4 const & sampled
			, sdw::Float const & metalness )const
		{
			return metalness * getFloat( writer, sampled, spcMask );
		}

		sdw::Float TextureConfigData::getShininess( sdw::ShaderWriter & writer
			, sdw::Vec4 const & sampled
			, sdw::Float const & shininess )const
		{
			return shininess
				* clamp( getFloat( writer, sampled, shnMask )
					, 0.00390625_f // 1 / 256
					, 1.0_f );
		}

		sdw::Float TextureConfigData::getGlossiness( sdw::ShaderWriter & writer
			, sdw::Vec4 const & sampled
			, sdw::Float const & glossiness )const
		{
			return glossiness * getFloat( writer, sampled, shnMask );
		}

		sdw::Float TextureConfigData::getRoughness( sdw::ShaderWriter & writer
			, sdw::Vec4 const & sampled
			, sdw::Float const & roughness )const
		{
			return roughness * getFloat( writer, sampled, shnMask );
		}

		sdw::Float TextureConfigData::getOpacity( sdw::ShaderWriter & writer
			, sdw::Vec4 const & sampled
			, sdw::Float const & opacity )const
		{
			return opacity * getFloat( writer, sampled, opaMask );
		}

		sdw::Vec3 TextureConfigData::getNormal( sdw::ShaderWriter & writer
			, sdw::Vec4 const & sampled
			, sdw::Mat3 const & tbn )const
		{
			return normalize( tbn
				* fma( getVec3( writer, sampled, nmlMask )
					, vec3( 2.0_f )
					, -vec3( 1.0_f ) ) );
		}

		sdw::Vec3 TextureConfigData::getNormal( sdw::ShaderWriter & writer
			, sdw::Vec4 const & sampled
			, sdw::Vec3 const & normal
			, sdw::Vec3 const & tangent
			, sdw::Vec3 const & bitangent )const
		{
			return getNormal( writer
				, sampled
				, shader::Utils::getTBN( normal, tangent, bitangent ) );
		}

		sdw::Float TextureConfigData::getHeight( sdw::ShaderWriter & writer
			, sdw::Vec4 const & sampled )const
		{
			return hgtFact * getFloat( writer, sampled, hgtMask );
		}

		sdw::Float TextureConfigData::getOcclusion( sdw::ShaderWriter & writer
			, sdw::Vec4 const & sampled
			, sdw::Float const & occlusion )const
		{
			return getFloat( writer, sampled, occMask );
		}

		sdw::Float TextureConfigData::getTransmittance( sdw::ShaderWriter & writer
			, sdw::Vec4 const & sampled
			, sdw::Float const & transmittance )const
		{
			return getFloat( writer, sampled, trsMask );
		}

		sdw::Float TextureConfigData::getFloat( sdw::ShaderWriter & writer
			, sdw::Vec4 const & sampled
			, sdw::Float const & mask )const
		{
			return sampled[writer.cast< sdw::UInt >( mask )];
		}

		sdw::Vec3 TextureConfigData::getVec3( sdw::ShaderWriter & writer
			, sdw::Vec4 const & sampled
			, sdw::Float const & mask )const
		{
			return writer.ternary( mask == 0.0_f
				, sampled.rgb()
				, sampled.gba() );
		}

		sdw::Vec3 TextureConfigData::removeGamma( sdw::ShaderWriter & writer
			, sdw::Vec3 const & srgb
			, sdw::Float const & gamma )const
		{
			return mix( srgb
				, pow( max( srgb, vec3( 0.0_f, 0.0_f, 0.0_f ) ), vec3( gamma ) )
				, vec3( writer.cast< sdw::Float >( needsGC ) ) );
		}

		void TextureConfigData::convertUV( sdw::ShaderWriter & writer
			, sdw::Vec2 & uv )const
		{
			uv = vec2( uv.x()
				, mix( uv.y(), 1.0_f - uv.y(), fneedYI ) );
			uv = scaleUV( texScl.xy(), uv );
			uv = rotateUV( texRot.xy(), uv );
			uv = translateUV( texTrn.xy(), uv );
		}

		void TextureConfigData::convertUVW( sdw::ShaderWriter & writer
			, sdw::Vec3 & uvw )const
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
						result.emsOcc = c3d_textureConfigurations.fetch( sdw::Int{ offset++ } );
						result.trsDum = c3d_textureConfigurations.fetch( sdw::Int{ offset++ } );
						result.nmlFcr = c3d_textureConfigurations.fetch( sdw::Int{ offset++ } );
						result.hgtFcr = c3d_textureConfigurations.fetch( sdw::Int{ offset++ } );
						result.mscVls = c3d_textureConfigurations.fetch( sdw::Int{ offset++ } );
						result.texTrn = c3d_textureConfigurations.fetch( sdw::Int{ offset++ } );
						result.texRot = c3d_textureConfigurations.fetch( sdw::Int{ offset++ } );
						result.texScl = c3d_textureConfigurations.fetch( sdw::Int{ offset++ } );
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
