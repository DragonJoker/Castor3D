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
			, TextureFlags const & texturesFlags
			, TextureAnimations const & textureAnims
			, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
			, Material const & material
			, BlendComponents & components )const
		{
			if ( checkFlag( passShaders.getFilter(), ComponentModeFlag::eDerivTex ) )
			{
				computeMapsContributionsT< DerivTex >( passShaders
					, texturesFlags
					, textureAnims
					, maps
					, material
					, components );
			}
			else
			{
				computeMapsContributionsT< sdw::Vec3 >( passShaders
					, texturesFlags
					, textureAnims
					, maps
					, material
					, components );
			}
		}

		//*********************************************************************************************
	}
}
