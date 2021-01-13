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
		}

		//*****************************************************************************************

		TextureConfigData::TextureConfigData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr )
			: sdw::StructInstance{ writer, std::move( expr ) }
			, colrSpec{ getMember< sdw::Vec4 >( "colrSpec" ) }
			, glossOpa{ getMember< sdw::Vec4 >( "glossOpa" ) }
			, emisOccl{ getMember< sdw::Vec4 >( "emisOccl" ) }
			, trnsDumm{ getMember< sdw::Vec4 >( "trnsDumm" ) }
			, normalFc{ getMember< sdw::Vec4 >( "normalFc" ) }
			, heightFc{ getMember< sdw::Vec4 >( "heightFc" ) }
			, miscVals{ getMember< sdw::Vec4 >( "miscVals" ) }
			, translate{ getMember< sdw::Vec4 >( "translate" ) }
			, rotate{ getMember< sdw::Vec4 >( "rotate" ) }
			, colourMask{ colrSpec.xy() }
			, specularMask{ colrSpec.zw() }
			, glossinessMask{ glossOpa.xy() }
			, opacityMask{ glossOpa.zw() }
			, normalMask{ normalFc.xy() }
			, normalFactor{ normalFc.z() }
			, normalGMultiplier{ normalFc.w() }
			, heightMask{ heightFc.xy() }
			, heightFactor{ heightFc.z() }
			, emissiveMask{ emisOccl.xy() }
			, occlusionMask{ emisOccl.zw() }
			, transmittanceMask{ trnsDumm.xy() }
			, needsGammaCorrection{ writer.cast< sdw::UInt >( miscVals.x() ) }
			, needsYInversion{ writer.cast< sdw::UInt >( miscVals.y() ) }
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
				result->declMember( "colrSpec", ast::type::Kind::eVec4F );
				result->declMember( "glossOpa", ast::type::Kind::eVec4F );
				result->declMember( "emisOccl", ast::type::Kind::eVec4F );
				result->declMember( "trnsDumm", ast::type::Kind::eVec4F );
				result->declMember( "normalFc", ast::type::Kind::eVec4F );
				result->declMember( "heightFc", ast::type::Kind::eVec4F );
				result->declMember( "miscVals", ast::type::Kind::eVec4F );
				result->declMember( "translate", ast::type::Kind::eVec4F );
				result->declMember( "rotate", ast::type::Kind::eVec4F );
			}

			return result;
		}

		sdw::Vec3 TextureConfigData::getDiffuse( sdw::ShaderWriter & writer
			, sdw::Vec4 const & sampled
			, sdw::Vec3 const & diffuse
			, sdw::Float gamma )const
		{
			return diffuse * removeGamma( writer
				, getVec3( writer, sampled, colourMask )
				, gamma );
		}

		sdw::Vec3 TextureConfigData::getAlbedo( sdw::ShaderWriter & writer
			, sdw::Vec4 const & sampled
			, sdw::Vec3 const & albedo
			, sdw::Float gamma )const
		{
			return albedo * removeGamma( writer
				, getVec3( writer, sampled, colourMask )
				, gamma );
		}

		sdw::Vec3 TextureConfigData::getEmissive( sdw::ShaderWriter & writer
			, sdw::Vec4 const & sampled
			, sdw::Vec3 const & emissive
			, sdw::Float gamma )const
		{
			return emissive * removeGamma( writer
				, getVec3( writer, sampled, emissiveMask )
				, gamma );
		}

		sdw::Vec3 TextureConfigData::getSpecular( sdw::ShaderWriter & writer
			, sdw::Vec4 const & sampled
			, sdw::Vec3 const & specular )const
		{
			return specular * getVec3( writer, sampled, specularMask );
		}

		sdw::Float TextureConfigData::getMetalness( sdw::ShaderWriter & writer
			, sdw::Vec4 const & sampled
			, sdw::Float const & metalness )const
		{
			return metalness * getFloat( writer, sampled, specularMask );
		}

		sdw::Float TextureConfigData::getShininess( sdw::ShaderWriter & writer
			, sdw::Vec4 const & sampled
			, sdw::Float const & shininess )const
		{
			return shininess
				* clamp( getFloat( writer, sampled, glossinessMask )
					, 0.00390625_f // 1 / 256
					, 1.0_f );
		}

		sdw::Float TextureConfigData::getGlossiness( sdw::ShaderWriter & writer
			, sdw::Vec4 const & sampled
			, sdw::Float const & glossiness )const
		{
			return glossiness * getFloat( writer, sampled, glossinessMask );
		}

		sdw::Float TextureConfigData::getRoughness( sdw::ShaderWriter & writer
			, sdw::Vec4 const & sampled
			, sdw::Float const & roughness )const
		{
			return roughness * getFloat( writer, sampled, glossinessMask );
		}

		sdw::Float TextureConfigData::getOpacity( sdw::ShaderWriter & writer
			, sdw::Vec4 const & sampled
			, sdw::Float const & opacity )const
		{
			return opacity * getFloat( writer, sampled, opacityMask );
		}

		sdw::Vec3 TextureConfigData::getNormal( sdw::ShaderWriter & writer
			, sdw::Vec4 const & sampled
			, sdw::Mat3 const & tbn )const
		{
			return normalize( tbn
				* fma( getVec3( writer, sampled, normalMask )
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
			, sdw::Vec4 const & sampled
			, sdw::Float const & height )const
		{
			return heightFactor * getFloat( writer, sampled, heightMask );
		}

		sdw::Float TextureConfigData::getOcclusion( sdw::ShaderWriter & writer
			, sdw::Vec4 const & sampled
			, sdw::Float const & occlusion )const
		{
			return getFloat( writer, sampled, occlusionMask );
		}

		sdw::Float TextureConfigData::getTransmittance( sdw::ShaderWriter & writer
			, sdw::Vec4 const & sampled
			, sdw::Float const & transmittance )const
		{
			return getFloat( writer, sampled, transmittanceMask );
		}

		sdw::Float TextureConfigData::getFloat( sdw::ShaderWriter & writer
			, sdw::Vec4 const & sampled
			, sdw::Vec2 const & mask )const
		{
			return sampled[writer.cast< sdw::UInt >( mask.y() )];
		}

		sdw::Vec3 TextureConfigData::getVec3( sdw::ShaderWriter & writer
			, sdw::Vec4 const & sampled
			, sdw::Vec2 const & mask )const
		{
			return writer.ternary( mask.y() == 0.0_f
				, sampled.rgb()
				, sampled.gba() );
		}

		sdw::Vec3 TextureConfigData::removeGamma( sdw::ShaderWriter & writer
			, sdw::Vec3 const & srgb
			, sdw::Float const & gamma )const
		{
			return mix( srgb
				, pow( max( srgb, vec3( 0.0_f, 0.0_f, 0.0_f ) ), vec3( gamma ) )
				, vec3( writer.cast< sdw::Float >( needsGammaCorrection ) ) );
		}

		sdw::Vec2 TextureConfigData::convertUV( sdw::ShaderWriter & writer
			, sdw::Vec2 const & uv )const
		{
			return translateUV( translate.xy()
				, rotateUV( vec2( rotate.xy() )
					, mix( uv
						, vec2( uv.x(), 1.0_f - uv.y() )
						, vec2( writer.cast< sdw::Float >( needsYInversion ) ) ) ) );
		}

		sdw::Vec3 TextureConfigData::convertUVW( sdw::ShaderWriter & writer
			, sdw::Vec3 const & uvw )const
		{
			return translateUV( translate.xyz()
				, rotateUV( vec3( rotate.xy(), 0.0f )
					, mix( uvw
						, vec3( uvw.x(), 1.0_f - uvw.y(), uvw.z() )
						, vec3( writer.cast< sdw::Float >( needsYInversion ) ) ) ) );
		}

		//*********************************************************************************************

		TextureConfigurations::TextureConfigurations( sdw::ShaderWriter & writer )
			: m_writer{ writer }
		{
		}

		void TextureConfigurations::declare( bool hasSsbo )
		{

			if ( hasSsbo )
			{
				m_ssbo = std::make_unique< sdw::ArraySsboT< TextureConfigData > >( m_writer
					, TextureConfigurationBufferName
					, getTexturesBufferIndex()
					, 0u );
			}
			else
			{
				auto c3d_textureConfigurations = m_writer.declSampledImage< FImgBufferRgba32 >( "c3d_textureConfigurations"
					, getTexturesBufferIndex()
					, 0u );
				m_getTextureConfiguration = m_writer.implementFunction< TextureConfigData >( "getTextureConfiguration"
					, [this, &c3d_textureConfigurations]( sdw::UInt const & index )
					{
						auto result = m_writer.declLocale< TextureConfigData >( "result" );
						auto offset = m_writer.declLocale( cuT( "offset" )
							, m_writer.cast< sdw::Int >( index ) * sdw::Int( shader::MaxTextureConfigurationComponentsCount ) );
						result.colrSpec = c3d_textureConfigurations.fetch( sdw::Int{ offset++ } );
						result.glossOpa = c3d_textureConfigurations.fetch( sdw::Int{ offset++ } );
						result.emisOccl = c3d_textureConfigurations.fetch( sdw::Int{ offset++ } );
						result.trnsDumm = c3d_textureConfigurations.fetch( sdw::Int{ offset++ } );
						result.normalFc = c3d_textureConfigurations.fetch( sdw::Int{ offset++ } );
						result.heightFc = c3d_textureConfigurations.fetch( sdw::Int{ offset++ } );
						result.miscVals = c3d_textureConfigurations.fetch( sdw::Int{ offset++ } );
						result.translate = c3d_textureConfigurations.fetch( sdw::Int{ offset++ } );
						result.rotate = c3d_textureConfigurations.fetch( sdw::Int{ offset++ } );
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
