#include "Castor3D/Shader/Shaders/GlslTextureAnimation.hpp"

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

		TextureAnimData::TextureAnimData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: sdw::StructInstance{ writer, std::move( expr ), enabled }
			, texTrn{ getMember< sdw::Vec4 >( "texTrn" ) }
			, texRot{ getMember< sdw::Vec4 >( "texRot" ) }
			, texScl{ getMember< sdw::Vec4 >( "texScl" ) }
			, tleSet{ getMember< sdw::Vec4 >( "tleSet" ) }
		{
		}

		std::unique_ptr< sdw::Struct > TextureAnimData::declare( sdw::ShaderWriter & writer )
		{
			return std::make_unique< sdw::Struct >( writer, makeType( writer.getTypesCache() ) );
		}

		ast::type::BaseStructPtr TextureAnimData::makeType( ast::type::TypesCache & cache )
		{
			auto result = cache.getStruct( ast::type::MemoryLayout::eStd140, "C3D_TextureAnimData" );

			if ( result->empty() )
			{
				result->declMember( "texTrn", ast::type::Kind::eVec4F );
				result->declMember( "texRot", ast::type::Kind::eVec4F );
				result->declMember( "texScl", ast::type::Kind::eVec4F );
				result->declMember( "tleSet", ast::type::Kind::eVec4F );
			}

			return result;
		}

		void TextureAnimData::animUV( TextureConfigData const & config
			, sdw::Vec2 & uv )const
		{
			config.convertUV( uv );

			IF( *m_writer, config.isTrnfAnim )
			{
				uv = vec2( uv.x()
					, mix( uv.y(), 1.0_f - uv.y(), config.fneedYI ) );
				uv = scaleUV( texScl.xy(), uv );
				uv = rotateUV( texRot.xy(), uv );
				uv = translateUV( texTrn.xy(), uv );
			}
			FI;

			config.convertToTile( uv );

			IF( *m_writer, config.isTileAnim )
			{
				uv.x() += tleSet.x() / tleSet.z();
				uv.y() += tleSet.y() / tleSet.w();
			}
			FI;
		}

		void TextureAnimData::animUVW( TextureConfigData const & config
			, sdw::Vec3 & uvw )const
		{
			config.convertUVW( uvw );

			IF( *m_writer, config.isTrnfAnim )
			{
				uvw = vec3( uvw.x()
					, mix( uvw.y(), 1.0_f - uvw.y(), config.fneedYI )
					, uvw.z() );
				uvw = scaleUV( texScl.xyz(), uvw );
				uvw = rotateUV( texRot.xyz(), uvw );
				uvw = translateUV( texTrn.xyz(), uvw );
			}
			FI;
		}

		//*********************************************************************************************

		TextureAnimations::TextureAnimations( sdw::ShaderWriter & writer )
			: m_writer{ writer }
		{
		}

		void TextureAnimations::declare( bool hasSsbo
			, uint32_t binding
			, uint32_t set )
		{

			if ( hasSsbo )
			{
				m_ssbo = std::make_unique< sdw::ArraySsboT< TextureAnimData > >( m_writer
					, TextureAnimationBufferName
					, binding
					, set
					, true );
			}
			else
			{
				auto c3d_textureAnimations = m_writer.declSampledImage< FImgBufferRgba32 >( "c3d_textureAnimations"
					, binding
					, set );
				m_getTextureAnimation = m_writer.implementFunction< TextureAnimData >( "c3d_getTextureAnimation"
					, [this, &c3d_textureAnimations]( sdw::UInt const & index )
					{
						auto result = m_writer.declLocale< TextureAnimData >( "result" );
						auto offset = m_writer.declLocale( cuT( "offset" )
							, m_writer.cast< sdw::Int >( index ) * sdw::Int( shader::MaxTextureAnimationComponentsCount ) );
						result.texTrn = c3d_textureAnimations.fetch( sdw::Int{ offset++ } );
						result.texRot = c3d_textureAnimations.fetch( sdw::Int{ offset++ } );
						result.texScl = c3d_textureAnimations.fetch( sdw::Int{ offset++ } );
						m_writer.returnStmt( result );
					}
					, sdw::InUInt{ m_writer, cuT( "index" ) } );
			}
		}

		TextureAnimData TextureAnimations::getTextureAnimation( sdw::UInt const & index )const
		{
			if ( m_ssbo )
			{
				return ( *m_ssbo )[index - 1_u];
			}

			return m_getTextureAnimation( index - 1_u );
		}

		//*********************************************************************************************
	}
}
