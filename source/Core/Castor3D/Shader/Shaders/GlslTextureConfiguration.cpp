#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/CompositeTypes/StructuredSsbo.hpp>

using namespace castor;

namespace castor3d
{
	namespace shader
	{
		//*****************************************************************************************

		TextureConfigData::TextureConfigData( sdw::Shader * shader
			, ast::expr::ExprPtr expr )
			: sdw::StructInstance{ shader, std::move( expr ) }
			, colrSpec{ getMember< sdw::Vec4 >( "colrSpec" ) }
			, glossOpa{ getMember< sdw::Vec4 >( "glossOpa" ) }
			, emisOccl{ getMember< sdw::Vec4 >( "emisOccl" ) }
			, trnsDumm{ getMember< sdw::Vec4 >( "trnsDumm" ) }
			, normalFc{ getMember< sdw::Vec4 >( "normalFc" ) }
			, heightFc{ getMember< sdw::Vec4 >( "heightFc" ) }
			, miscVals{ getMember< sdw::Vec4 >( "miscVals" ) }
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
			, environment{ shader, sdw::makeCast( shader->getTypesCache().getUInt(), makeExpr( *shader, miscVals.x() ) ) }
			, needsGammaCorrection{ shader, sdw::makeCast( shader->getTypesCache().getUInt(), makeExpr( *shader, miscVals.y() ) ) }
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
			}

			return result;
		}

		sdw::Vec3 TextureConfigData::getDiffuse( sdw::ShaderWriter & writer
			, sdw::Vec4 const & sampled
			, sdw::Vec3 const & diffuse
			, sdw::Float gamma )const
		{
			return mix( diffuse
				, diffuse * removeGamma( writer, getVec3( writer, sampled, colourMask ), gamma )
				, vec3( getMix( writer, colourMask ) ) );
		}

		sdw::Vec3 TextureConfigData::getAlbedo( sdw::ShaderWriter & writer
			, sdw::Vec4 const & sampled
			, sdw::Vec3 const & albedo
			, sdw::Float gamma )const
		{
			return mix( albedo
				, albedo * removeGamma( writer, getVec3( writer, sampled, colourMask ), gamma )
				, vec3( getMix( writer, colourMask ) ) );
		}

		sdw::Vec3 TextureConfigData::getEmissive( sdw::ShaderWriter & writer
			, sdw::Vec4 const & sampled
			, sdw::Vec3 const & emissive
			, sdw::Float gamma )const
		{
			return mix( emissive
				, emissive * removeGamma( writer, getVec3( writer, sampled, emissiveMask ), gamma )
				, vec3( getMix( writer, emissiveMask ) ) );
		}

		sdw::Vec3 TextureConfigData::getSpecular( sdw::ShaderWriter & writer
			, sdw::Vec4 const & sampled
			, sdw::Vec3 const & specular )const
		{
			return mix( specular
				, specular * getVec3( writer, sampled, specularMask )
				, vec3( getMix( writer, specularMask ) ) );
		}

		sdw::Float TextureConfigData::getMetalness( sdw::ShaderWriter & writer
			, sdw::Vec4 const & sampled
			, sdw::Float const & metalness )const
		{
			return mix( metalness
				, metalness * getFloat( writer, sampled, specularMask )
				, getMix( writer, specularMask ) );
		}

		sdw::Float TextureConfigData::getShininess( sdw::ShaderWriter & writer
			, sdw::Vec4 const & sampled
			, sdw::Float const & shininess )const
		{
			return mix( shininess
				, shininess * getFloat( writer, sampled, 255.0_f, glossinessMask )
				, getMix( writer, glossinessMask ) );
		}

		sdw::Float TextureConfigData::getGlossiness( sdw::ShaderWriter & writer
			, sdw::Vec4 const & sampled
			, sdw::Float const & glossiness )const
		{
			return mix( glossiness
				, glossiness * getFloat( writer, sampled, glossinessMask )
				, getMix( writer, glossinessMask ) );
		}

		sdw::Float TextureConfigData::getRoughness( sdw::ShaderWriter & writer
			, sdw::Vec4 const & sampled
			, sdw::Float const & roughness )const
		{
			return mix( roughness
				, roughness * getFloat( writer, sampled, glossinessMask )
				, getMix( writer, glossinessMask ) );
		}

		sdw::Float TextureConfigData::getOpacity( sdw::ShaderWriter & writer
			, sdw::Vec4 const & sampled
			, sdw::Float const & opacity )const
		{
			return mix( opacity
				, opacity * getFloat( writer, sampled, opacity, opacityMask )
				, getMix( writer, opacityMask ) );
		}

		sdw::Vec3 TextureConfigData::getNormal( sdw::ShaderWriter & writer
			, sdw::Vec4 const & sampled
			, sdw::Vec3 const & normal
			, sdw::Vec3 const & tangent
			, sdw::Vec3 const & bitangent )const
		{
			return mix( normal
				, normalize( mat3( normalize( tangent ), normalize( bitangent ), normal )
					* fma( getVec3( writer, sampled, normalMask )
						, vec3( 2.0_f )
						, -vec3( 1.0_f ) ) )
				, vec3( getMix( writer, normalMask ) ) );
		}

		sdw::Float TextureConfigData::getHeight( sdw::ShaderWriter & writer
			, sdw::Vec4 const & sampled
			, sdw::Float const & height )const
		{
			return mix( height
				, getFloat( writer, sampled, heightFactor, heightMask )
				, getMix( writer, heightMask ) );
		}

		sdw::Float TextureConfigData::getOcclusion( sdw::ShaderWriter & writer
			, sdw::Vec4 const & sampled
			, sdw::Float const & occlusion )const
		{
			return mix( occlusion
				, getFloat( writer, sampled, occlusionMask )
				, getMix( writer, occlusionMask ) );
		}

		sdw::Float TextureConfigData::getTransmittance( sdw::ShaderWriter & writer
			, sdw::Vec4 const & sampled
			, sdw::Float const & transmittance )const
		{
			return mix( transmittance
				, getFloat( writer, sampled, transmittanceMask )
				, getMix( writer, transmittanceMask ) );
		}

		sdw::Float TextureConfigData::getMix( sdw::ShaderWriter & writer
			, sdw::Vec2 const & mask )const
		{
			return ( 1.0_f - step( mask[0], 0.0_f ) );
		}

		sdw::Float TextureConfigData::getFloat( sdw::ShaderWriter & writer
			, sdw::Vec4 const & sampled
			, sdw::Vec2 const & mask )const
		{
			return sampled[writer.cast< sdw::UInt >( mask[1] )];
		}

		sdw::Float TextureConfigData::getFloat( sdw::ShaderWriter & writer
			, sdw::Vec4 const & sampled
			, sdw::Float const & factor
			, sdw::Vec2 const & mask )const
		{
			return factor * getFloat( writer, sampled, mask );
		}

		sdw::Vec3 TextureConfigData::getVec3( sdw::ShaderWriter & writer
			, sdw::Vec4 const & sampled
			, sdw::Vec2 const & mask )const
		{
			return ( sampled.rgb() * ( 1.0_f - writer.cast< sdw::Float >( mask[1] ) ) )
				+ ( sampled.gba() * writer.cast< sdw::Float >( mask[1] ) );
		}

		sdw::Vec3 TextureConfigData::removeGamma( sdw::ShaderWriter & writer
			, sdw::Vec3 const & srgb
			, sdw::Float const & gamma )const
		{
			return mix( srgb
				, pow( max( srgb, vec3( 0.0_f, 0.0_f, 0.0_f ) ), vec3( gamma ) )
				, vec3( writer.cast< sdw::Float >( needsGammaCorrection ) ) );
		}

		//*********************************************************************************************

		TextureConfigurations::TextureConfigurations( sdw::ShaderWriter & writer )
			: m_writer{ writer }
		{
		}

		void TextureConfigurations::declare( bool hasSsbo )
		{
			m_type = TextureConfigData::declare( m_writer );

			if ( hasSsbo )
			{
				m_ssbo = std::make_unique< sdw::ArraySsboT< TextureConfigData > >( m_writer
					, TextureConfigurationBufferName
					, m_type->getType()
					, TexturesBufferIndex
					, 0u );
			}
			else
			{
				auto c3d_textureConfigurations = m_writer.declSampledImage< FImgBufferRgba32 >( "c3d_textureConfigurations", TexturesBufferIndex, 0u );
				m_getTextureConfiguration = m_writer.implementFunction< TextureConfigData >( "getTextureConfiguration"
					, [this, &c3d_textureConfigurations]( sdw::UInt const & index )
					{
						auto result = m_writer.declLocale< TextureConfigData >( "result"
							, *m_type );
						auto offset = m_writer.declLocale( cuT( "offset" )
							, m_writer.cast< sdw::Int >( index ) * sdw::Int( shader::MaxTextureConfigurationComponentsCount ) );
						result.colrSpec = texelFetch( c3d_textureConfigurations, offset++ );
						result.glossOpa = texelFetch( c3d_textureConfigurations, offset++ );
						result.emisOccl = texelFetch( c3d_textureConfigurations, offset++ );
						result.trnsDumm = texelFetch( c3d_textureConfigurations, offset++ );
						result.normalFc = texelFetch( c3d_textureConfigurations, offset++ );
						result.heightFc = texelFetch( c3d_textureConfigurations, offset++ );
						result.miscVals = texelFetch( c3d_textureConfigurations, offset++ );
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
			else
			{
				return m_getTextureConfiguration( index - 1_u );
			}
		}

		//*********************************************************************************************
	}
}
