#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"

#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
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
			: StructInstanceHelperT{ writer, std::move( expr ), enabled }
			, fneedYI{ writer.cast< sdw::Float >( needsYI() ) }
		{
		}

		void TextureConfigData::transformUV( Utils & utils
			, TextureTransformData const & anim
			, sdw::Vec2 & uv )const
		{
			uv = utils.transformUV( *this, anim, uv );
		}

		void TextureConfigData::transformUVW( Utils & utils
			, TextureTransformData const & anim
			, sdw::Vec3 & uvw )const
		{
			uvw = utils.transformUVW( *this, anim, uvw );
		}

		void TextureConfigData::transformUV( Utils & utils
			, TextureTransformData const & anim
			, DerivTex & uv )const
		{
			uv.uv() = utils.transformUV( *this, anim, uv.uv() );
		}

		sdw::Float TextureConfigData::getFloat( sdw::Vec4 const & sampled
			, sdw::UInt const & mask )
		{
			return sampled[mask];
		}

		sdw::Vec3 TextureConfigData::getVec3( sdw::Vec4 const & sampled
			, sdw::UInt const & mask )
		{
			return vec3( sampled[mask]
				, sampled[mask + 1u]
				, sampled[mask + 2u] );
		}

		//*********************************************************************************************

		TextureConfigurations::TextureConfigurations( sdw::ShaderWriter & writer
			, uint32_t binding
			, uint32_t set
			, bool enable )
			: BufferT< TextureConfigData >{ writer
				, "C3D_TextureConfigurations"
				, "c3d_textureConfigurations"
				, binding
				, set
				, enable }
		{
		}

		void TextureConfigurations::computeMapsContributions( PassShaders const & passShaders
			, PipelineFlags const & flags
			, TextureAnimations const & textureAnims
			, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
			, Material const & material
			, BlendComponents & components )const
		{
			if ( checkFlag( passShaders.getFilter(), ComponentModeFlag::eDerivTex ) )
			{
				computeMapsContributionsT< DerivTex >( passShaders
					, flags
					, textureAnims
					, maps
					, material
					, components );
			}
			else
			{
				computeMapsContributionsT< sdw::Vec3 >( passShaders
					, flags
					, textureAnims
					, maps
					, material
					, components );
			}
		}

		void TextureConfigurations::computeMapsContributions( PassShaders const & passShaders
			, TextureCombine const & combine
			, TextureAnimations const & textureAnims
			, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
			, Material const & material
			, BlendComponents & components )const
		{
			if ( checkFlag( passShaders.getFilter(), ComponentModeFlag::eDerivTex ) )
			{
				computeMapsContributionsT< DerivTex >( passShaders
					, combine
					, textureAnims
					, maps
					, material
					, components );
			}
			else
			{
				computeMapsContributionsT< sdw::Vec2 >( passShaders
					, combine
					, textureAnims
					, maps
					, material
					, components );
			}
		}

		RetDerivTex TextureConfigurations::computeTexcoordsDerivTex( PassShaders const & passShaders
			, TextureConfigData const & pconfig
			, TextureTransformData const & panim
			, BlendComponents const & pcomponents )const
		{
			if ( !m_computeTexcoordsDerivTex )
			{
				m_computeTexcoordsDerivTex = m_writer.implementFunction< DerivTex >( "c3d_computeTexcoords"
					, [&]( TextureConfigData const & config
						, TextureTransformData const & anim
						, BlendComponents const & components )
					{
						auto texCoords0 = components.getMember< DerivTex >( "texture0" );
						auto texCoords1 = components.getMember< DerivTex >( "texture1", true );
						auto texCoords2 = components.getMember< DerivTex >( "texture2", true );
						auto texCoords3 = components.getMember< DerivTex >( "texture3", true );
						auto texCoords = m_writer.declLocale( "c3d_tex"
							, TextureConfigurations::getTexcoord( config
								, texCoords0
								, texCoords1
								, texCoords2
								, texCoords3 ) );
						auto texCoord = m_writer.declLocale( "c3d_texCoord"
							, config.toUv( texCoords ) );
						config.transformUV( passShaders.getUtils(), anim, texCoord );
						config.setUv( texCoords, texCoord );
						m_writer.returnStmt( texCoords );
					}
					, InTextureConfigData{ m_writer, "config" }
					, InTextureTransformData{ m_writer, "anim" }
					, InBlendComponents{ m_writer, "components", pcomponents } );
			}

			return m_computeTexcoordsDerivTex( pconfig, panim, pcomponents );
		}

		sdw::RetVec3 TextureConfigurations::computeTexcoordsVec3( PassShaders const & passShaders
			, TextureConfigData const & pconfig
			, TextureTransformData const & panim
			, BlendComponents const & pcomponents )const
		{
			if ( !m_computeTexcoordsVec3 )
			{
				m_computeTexcoordsVec3 = m_writer.implementFunction< sdw::Vec3 >( "c3d_computeTexcoords"
					, [&]( TextureConfigData const & config
						, TextureTransformData const & anim
						, BlendComponents const & components )
					{
						auto texCoords0 = components.getMember< sdw::Vec3 >( "texture0" );
						auto texCoords1 = components.getMember< sdw::Vec3 >( "texture1", true );
						auto texCoords2 = components.getMember< sdw::Vec3 >( "texture2", true );
						auto texCoords3 = components.getMember< sdw::Vec3 >( "texture3", true );
						auto texCoords = m_writer.declLocale( "c3d_texCoords"
							, TextureConfigurations::getTexcoord( config
								, texCoords0
								, texCoords1
								, texCoords2
								, texCoords3 ) );
						auto texCoord = m_writer.declLocale( "c3d_texCoord"
							, config.toUv( texCoords ) );
						config.transformUV( passShaders.getUtils(), anim, texCoord );
						config.setUv( texCoords, texCoord );
						m_writer.returnStmt( texCoords );
					}
					, InTextureConfigData{ m_writer, "config" }
					, InTextureTransformData{ m_writer, "anim" }
					, InBlendComponents{ m_writer, "components", pcomponents } );
			}

			return m_computeTexcoordsVec3( pconfig, panim, pcomponents );
		}

		sdw::RetVec2 TextureConfigurations::computeTexcoordsVec2( PassShaders const & passShaders
			, TextureConfigData const & pconfig
			, TextureTransformData const & panim
			, BlendComponents const & pcomponents )const
		{
			if ( !m_computeTexcoordsVec2 )
			{
				m_computeTexcoordsVec2 = m_writer.implementFunction< sdw::Vec2 >( "c3d_computeTexcoords"
					, [&]( TextureConfigData const & config
						, TextureTransformData const & anim
						, BlendComponents const & components )
					{
						auto texCoords0 = components.getMember< sdw::Vec2 >( "texture0" );
						auto texCoords1 = components.getMember< sdw::Vec2 >( "texture1", true );
						auto texCoords2 = components.getMember< sdw::Vec2 >( "texture2", true );
						auto texCoords3 = components.getMember< sdw::Vec2 >( "texture3", true );
						auto texCoords = m_writer.declLocale( "c3d_texCoords"
							, TextureConfigurations::getTexcoord( config
								, texCoords0
								, texCoords1
								, texCoords2
								, texCoords3 ) );
						config.transformUV( passShaders.getUtils(), anim, texCoords );
						m_writer.returnStmt( texCoords );
					}
					, InTextureConfigData{ m_writer, "config" }
					, InTextureTransformData{ m_writer, "anim" }
					, InBlendComponents{ m_writer, "components", pcomponents } );
			}

			return m_computeTexcoordsVec2( pconfig, panim, pcomponents );
		}

		//*********************************************************************************************
	}
}
