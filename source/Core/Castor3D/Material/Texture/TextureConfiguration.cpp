#include "Castor3D/Material/Texture/TextureConfiguration.hpp"

#include <CastorUtils/Graphics/PixelFormat.hpp>

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

		void updateStartIndex( castor::PixelFormat format
			, castor::Point2ui & mask )
		{
			if ( mask[0] )
			{
				auto components = getPixelComponents( mask[0] );

				switch ( components.size() )
				{
				case 1:
					mask[1] = castor::getComponentIndex( *components.begin(), format );
					break;
				case 3:
					if ( mask[0] & 0xFF000000 )
					{
						mask[1] = 1;
					}
					else
					{
						mask[1] = 0;
					}
					break;
				default:
					CU_Failure( "Invalid component count for a texture component flag" );
					mask[1] = 0;
					break;
				}
			}
		}
	}

	//*********************************************************************************************

	bool operator==( TextureTransform const & lhs
		, TextureTransform const & rhs )noexcept
	{
		return lhs.translate == rhs.translate
			&& lhs.rotate == rhs.rotate
			&& lhs.scale == rhs.scale;
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

	bool shallowEqual( TextureConfiguration const & lhs, TextureConfiguration const & rhs )
	{
		return lhs.colourMask[0] == rhs.colourMask[0]
			&& lhs.specularMask[0] == rhs.specularMask[0]
			&& lhs.metalnessMask[0] == rhs.metalnessMask[0]
			&& lhs.glossinessMask[0] == rhs.glossinessMask[0]
			&& lhs.roughnessMask[0] == rhs.roughnessMask[0]
			&& lhs.opacityMask[0] == rhs.opacityMask[0]
			&& lhs.emissiveMask[0] == rhs.emissiveMask[0]
			&& lhs.normalMask[0] == rhs.normalMask[0]
			&& lhs.heightMask[0] == rhs.heightMask[0]
			&& lhs.occlusionMask[0] == rhs.occlusionMask[0]
			&& lhs.transmittanceMask[0] == rhs.transmittanceMask[0];
	}

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

	void updateIndices( castor::PixelFormat format
		, TextureConfiguration & config )
	{
		updateStartIndex( format, config.colourMask );
		updateStartIndex( format, config.specularMask );
		updateStartIndex( format, config.metalnessMask );
		updateStartIndex( format, config.glossinessMask );
		updateStartIndex( format, config.roughnessMask );
		updateStartIndex( format, config.opacityMask );
		updateStartIndex( format, config.emissiveMask );
		updateStartIndex( format, config.normalMask );
		updateStartIndex( format, config.heightMask );
		updateStartIndex( format, config.occlusionMask );
		updateStartIndex( format, config.transmittanceMask );
	}
}
