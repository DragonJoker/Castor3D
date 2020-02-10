#include "Castor3D/Texture/TextureConfiguration.hpp"

namespace castor3d
{
	namespace
	{
	}

	TextureConfiguration::TextWriter::TextWriter( castor::String const & tabs
		, MaterialType type )
		: castor::TextWriter< TextureConfiguration >{ tabs, cuT( "TextureConfiguration" ) }
		, m_type{ type }
	{
	}

	bool TextureConfiguration::TextWriter::operator()( TextureConfiguration const & configuration
		, castor::TextFile & file )
	{
		bool result = true;

		if ( result && configuration.colourMask[0] )
		{
			switch ( m_type )
			{
			case MaterialType::ePhong:
				result = writeMask( cuT( "diffuse_mask" ), configuration.colourMask[0], file );
				break;
			case MaterialType::eMetallicRoughness:
			case MaterialType::eSpecularGlossiness:
				result = writeMask( cuT( "albedo_mask" ), configuration.colourMask[0], file );
				break;
			default:
				break;
			}
		}

		if ( result && configuration.specularMask[0] )
		{
			switch ( m_type )
			{
			case MaterialType::ePhong:
			case MaterialType::eSpecularGlossiness:
				result = writeMask( cuT( "specular_mask" ), configuration.specularMask[0], file );
				break;
			case MaterialType::eMetallicRoughness:
				result = writeMask( cuT( "metalness_mask" ), configuration.specularMask[0], file );
				break;
			default:
				break;
			}
		}

		if ( result && configuration.glossinessMask[0] )
		{
			switch ( m_type )
			{
			case MaterialType::ePhong:
				result = writeMask( cuT( "shininess_mask" ), configuration.glossinessMask[0], file );
				break;
			case MaterialType::eMetallicRoughness:
				result = writeMask( cuT( "roughness_mask" ), configuration.glossinessMask[0], file );
				break;
			case MaterialType::eSpecularGlossiness:
				result = writeMask( cuT( "glossiness_mask" ), configuration.glossinessMask[0], file );
				break;
			default:
				break;
			}

			checkError( result, "gloss mask" );
		}

		if ( result && configuration.opacityMask[0] )
		{
			result = writeMask( cuT( "opacity_mask" ), configuration.opacityMask[0], file );
		}

		if ( result && configuration.emissiveMask[0] )
		{
			result = writeMask( cuT( "emissive_mask" ), configuration.emissiveMask[0], file );
		}

		if ( result && configuration.normalMask[0] )
		{
			result = writeMask( cuT( "normal_mask" ), configuration.normalMask[0], file );

			if ( result && configuration.normalFactor != 1.0f )
			{
				result = write( cuT( "normal_factor" ), configuration.normalFactor, file );
			}

			if ( result )
			{
				result = writeOpt( cuT( "normal_directx" ), configuration.normalGMultiplier != 1.0f, file );
			}
		}

		if ( result && configuration.heightMask[0] )
		{
			result = writeMask( cuT( "height_mask" ), configuration.heightMask[0], file );

			if ( result && configuration.heightFactor != 1.0f )
			{
				result = write( cuT( "height_factor" ), configuration.heightFactor, file );
			}
		}

		if ( result && configuration.occlusionMask[0] )
		{
			result = writeMask( cuT( "occlusion_mask" ), configuration.occlusionMask[0], file );
		}

		if ( result && configuration.transmittanceMask[0] )
		{
			result = writeMask( cuT( "transmittance_mask" ), configuration.transmittanceMask[0], file );
		}

		if ( result )
		{
			result = writeOpt( cuT( "reflection" )
				, castor::checkFlag( configuration.environment, TextureConfiguration::ReflectionMask )
				, file );
		}

		if ( result )
		{
			result = writeOpt( cuT( "refraction" )
				, castor::checkFlag( configuration.environment, TextureConfiguration::RefractionMask ) 
				, file );
		}

		if ( result )
		{
			result = writeOpt( cuT( "invert_y" ), configuration.needsYInversion, file );
		}

		return result;
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
		result.specularMask[0] = 0x00FFFFFF;
		return result;
	}();

	TextureConfiguration const TextureConfiguration::ShininessTexture = []()
	{
		TextureConfiguration result;
		result.glossinessMask[0] = 0x000000FF;
		return result;
	}();

	TextureConfiguration const TextureConfiguration::GlossinessTexture = []()
	{
		TextureConfiguration result;
		result.glossinessMask[0] = 0x000000FF;
		return result;
	}();

	TextureConfiguration const TextureConfiguration::RoughnessTexture = []()
	{
		TextureConfiguration result;
		result.glossinessMask[0] = 0x000000FF;
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
		result.heightMask[0] = 0x000000FF;
		return result;
	}();

	TextureConfiguration const TextureConfiguration::OcclusionTexture = []()
	{
		TextureConfiguration result;
		result.occlusionMask[0] = 0x000000FF;
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
			&& lhs.glossinessMask == rhs.glossinessMask
			&& lhs.opacityMask == rhs.opacityMask
			&& lhs.emissiveMask == rhs.emissiveMask
			&& lhs.normalMask == rhs.normalMask
			&& lhs.heightMask == rhs.heightMask
			&& lhs.occlusionMask == rhs.occlusionMask
			&& lhs.transmittanceMask == rhs.transmittanceMask
			&& lhs.environment == rhs.environment
			&& lhs.normalFactor == rhs.normalFactor
			&& lhs.heightFactor == rhs.heightFactor
			&& lhs.normalGMultiplier == rhs.normalGMultiplier
			&& lhs.needsGammaCorrection == rhs.needsGammaCorrection
			&& lhs.needsYInversion == rhs.needsYInversion;
	}

	bool operator!=( TextureConfiguration const & lhs, TextureConfiguration const & rhs )
	{
		return !( lhs == rhs );
	}
}
