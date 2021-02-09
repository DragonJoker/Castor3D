#include "Castor3D/Text/TextTextureConfiguration.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"

#include <CastorUtils/Data/Text/TextPoint.hpp>

using namespace castor3d;

namespace castor
{
	TextWriter< TextureConfiguration >::TextWriter( String const & tabs
		, MaterialType type )
		: TextWriterT< TextureConfiguration >{ tabs, cuT( "TextureConfiguration" ) }
		, m_type{ type }
	{
	}

	bool TextWriter< TextureConfiguration >::operator()( TextureConfiguration const & configuration
		, castor::TextFile & file )
	{
		bool result = true;

		if ( configuration.colourMask[0] )
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
			result = writeOpt( cuT( "invert_y" ), configuration.needsYInversion, file );
		}

		if ( result )
		{
			auto rotate = castor::Point2f{ configuration.rotate };
			auto translate = castor::Point3f{ configuration.translate };

			if ( translate != castor::Point3f{}
				|| rotate != castor::Point2f{ 1.0f, 0.0f } )
			{
				if ( auto animBlock = beginBlock( "animation", file ) )
				{
					result = writeOpt( cuT( "translate" ), translate, castor::Point3f{}, file )
						&& writeOpt( cuT( "rotate" ), rotate, castor::Point2f{ 1.0f, 0.0f }, file );
				}
			}
		}

		return result;
	}
}
