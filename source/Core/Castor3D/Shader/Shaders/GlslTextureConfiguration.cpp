#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"

#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureAnimation.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/ModelDataUbo.hpp"

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/CompositeTypes/ArrayStorageBuffer.hpp>

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

		void TextureConfigData::transformUV( Utils & utils
			, TextureAnimData const & anim
			, sdw::Vec2 & uv )const
		{
			uv = utils.transformUV( *this, anim, uv );
		}

		void TextureConfigData::transformUVW( Utils & utils
			, TextureAnimData const & anim
			, sdw::Vec3 & uvw )const
		{
			uvw = utils.transformUVW( *this, anim, uvw );
		}

		void TextureConfigData::transformUV( Utils & utils
			, TextureAnimData const & anim
			, DerivTex & uv )const
		{
			uv.uv() = utils.transformUV( *this, anim, uv.uv() );
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

		//*********************************************************************************************

		TextureConfigurations::TextureConfigurations( sdw::ShaderWriter & writer
			, uint32_t binding
			, uint32_t set
			, bool enable )
			: BufferT< TextureConfigData >{ TextureConfigurationBufferName
				, "texConfigs"
				, writer
				, binding
				, set
				, enable }
		{
		}

		//*********************************************************************************************
	}
}
