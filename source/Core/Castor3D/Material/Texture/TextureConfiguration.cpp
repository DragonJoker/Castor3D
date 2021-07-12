#include "Castor3D/Material/Texture/TextureConfiguration.hpp"

namespace castor3d
{
	namespace
	{
		void mergeMasks( uint32_t toMerge
			, TextureFlag flag
			, TextureFlags & result )
		{
			result |= toMerge
				? flag
				: TextureFlag::eNone;
		}
	}

	//*********************************************************************************************

	TextureConfiguration const TextureConfiguration::DiffuseTexture = []()
	{
		TextureConfiguration result;
		result.colourMask[0] = 0x00FFFFFF;
		return result;
	}();

	TextureConfiguration const TextureConfiguration::AlbedoTexture = []()
	{
		TextureConfiguration result;
		result.colourMask[0] = 0x00FFFFFF;
		return result;
	}();

	TextureConfiguration const TextureConfiguration::SpecularTexture = []()
	{
		TextureConfiguration result;
		result.specularMask[0] = 0x00FFFFFF;
		return result;
	}();

	TextureConfiguration const TextureConfiguration::MetalnessTexture = []()
	{
		TextureConfiguration result;
		result.metalnessMask[0] = 0x00FF0000;
		return result;
	}();

	TextureConfiguration const TextureConfiguration::ShininessTexture = []()
	{
		TextureConfiguration result;
		result.glossinessMask[0] = 0x00FF0000;
		return result;
	}();

	TextureConfiguration const TextureConfiguration::GlossinessTexture = []()
	{
		TextureConfiguration result;
		result.glossinessMask[0] = 0x00FF0000;
		return result;
	}();

	TextureConfiguration const TextureConfiguration::RoughnessTexture = []()
	{
		TextureConfiguration result;
		result.roughnessMask[0] = 0x00FF0000;
		return result;
	}();

	TextureConfiguration const TextureConfiguration::OpacityTexture = []()
	{
		TextureConfiguration result;
		result.opacityMask[0] = 0xFF000000;
		return result;
	}();

	TextureConfiguration const TextureConfiguration::EmissiveTexture = []()
	{
		TextureConfiguration result;
		result.emissiveMask[0] = 0x00FFFFFF;
		return result;
	}();

	TextureConfiguration const TextureConfiguration::NormalTexture = []()
	{
		TextureConfiguration result;
		result.normalMask[0] = 0x00FFFFFF;
		return result;
	}();

	TextureConfiguration const TextureConfiguration::HeightTexture = []()
	{
		TextureConfiguration result;
		result.heightMask[0] = 0x00FF0000;
		return result;
	}();

	TextureConfiguration const TextureConfiguration::OcclusionTexture = []()
	{
		TextureConfiguration result;
		result.occlusionMask[0] = 0x00FF0000;
		return result;
	}();

	TextureConfiguration const TextureConfiguration::TransmittanceTexture = []()
	{
		TextureConfiguration result;
		result.transmittanceMask[0] = 0xFF000000;
		return result;
	}();

	bool operator==( TextureConfiguration const & lhs, TextureConfiguration const & rhs )
	{
		return lhs.colourMask == rhs.colourMask
			&& lhs.specularMask == rhs.specularMask
			&& lhs.metalnessMask == rhs.metalnessMask
			&& lhs.glossinessMask == rhs.glossinessMask
			&& lhs.roughnessMask == rhs.roughnessMask
			&& lhs.opacityMask == rhs.opacityMask
			&& lhs.emissiveMask == rhs.emissiveMask
			&& lhs.normalMask == rhs.normalMask
			&& lhs.heightMask == rhs.heightMask
			&& lhs.occlusionMask == rhs.occlusionMask
			&& lhs.transmittanceMask == rhs.transmittanceMask
			&& lhs.normalFactor == rhs.normalFactor
			&& lhs.heightFactor == rhs.heightFactor
			&& lhs.normalGMultiplier == rhs.normalGMultiplier
			&& lhs.needsGammaCorrection == rhs.needsGammaCorrection
			&& lhs.needsYInversion == rhs.needsYInversion
			&& lhs.transform.translate == rhs.transform.translate
			&& lhs.transform.rotate == rhs.transform.rotate
			&& lhs.transform.scale == rhs.transform.scale;
	}

	bool operator!=( TextureConfiguration const & lhs, TextureConfiguration const & rhs )
	{
		return !( lhs == rhs );
	}

	TextureFlags getFlags( TextureConfiguration const & config )
	{
		TextureFlags result = TextureFlag::eNone;
		mergeMasks( config.colourMask[0], TextureFlag::eDiffuse, result );
		mergeMasks( config.specularMask[0], TextureFlag::eSpecular, result );
		mergeMasks( config.metalnessMask[0], TextureFlag::eMetalness, result );
		mergeMasks( config.glossinessMask[0], TextureFlag::eGlossiness, result );
		mergeMasks( config.roughnessMask[0], TextureFlag::eRoughness, result );
		mergeMasks( config.opacityMask[0], TextureFlag::eOpacity, result );
		mergeMasks( config.emissiveMask[0], TextureFlag::eEmissive, result );
		mergeMasks( config.normalMask[0], TextureFlag::eNormal, result );
		mergeMasks( config.heightMask[0], TextureFlag::eHeight, result );
		mergeMasks( config.occlusionMask[0], TextureFlag::eOcclusion, result );
		mergeMasks( config.transmittanceMask[0], TextureFlag::eTransmittance, result );
		return result;
	}

	castor::PixelComponents getPixelComponents( uint32_t mask )
	{
		castor::PixelComponents result;

		if ( mask & 0xFF000000 )
		{
			result |= castor::PixelComponent::eAlpha;
		}

		if ( mask & 0x00FF0000 )
		{
			result |= castor::PixelComponent::eRed;
		}

		if ( mask & 0x0000FF00 )
		{
			result |= castor::PixelComponent::eGreen;
		}

		if ( mask & 0x000000FF )
		{
			result |= castor::PixelComponent::eBlue;
		}

		return result;
	}
}
