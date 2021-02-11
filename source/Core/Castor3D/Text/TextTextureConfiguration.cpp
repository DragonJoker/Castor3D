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
		log::info << tabs() << cuT( "Writing TextureConfiguration" ) << std::endl;
		bool result = true;

		if ( configuration.colourMask[0] )
		{
			switch ( m_type )
			{
			case MaterialType::ePhong:
				result = writeMask( file, cuT( "diffuse_mask" ), configuration.colourMask[0] );
				break;
			case MaterialType::eMetallicRoughness:
			case MaterialType::eSpecularGlossiness:
				result = writeMask( file, cuT( "albedo_mask" ), configuration.colourMask[0] );
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
				result = writeMask( file, cuT( "specular_mask" ), configuration.specularMask[0] );
				break;
			case MaterialType::eMetallicRoughness:
				result = writeMask( file, cuT( "metalness_mask" ), configuration.specularMask[0] );
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
				result = writeMask( file, cuT( "shininess_mask" ), configuration.glossinessMask[0] );
				break;
			case MaterialType::eMetallicRoughness:
				result = writeMask( file, cuT( "roughness_mask" ), configuration.glossinessMask[0] );
				break;
			case MaterialType::eSpecularGlossiness:
				result = writeMask( file, cuT( "glossiness_mask" ), configuration.glossinessMask[0] );
				break;
			default:
				break;
			}

			checkError( result, "gloss mask" );
		}

		if ( result && configuration.opacityMask[0] )
		{
			result = writeMask( file, cuT( "opacity_mask" ), configuration.opacityMask[0] );
		}

		if ( result && configuration.emissiveMask[0] )
		{
			result = writeMask( file, cuT( "emissive_mask" ), configuration.emissiveMask[0] );
		}

		if ( result && configuration.normalMask[0] )
		{
			result = writeMask( file, cuT( "normal_mask" ), configuration.normalMask[0] );

			if ( result && configuration.normalFactor != 1.0f )
			{
				result = write( file, cuT( "normal_factor" ), configuration.normalFactor );
			}

			if ( result )
			{
				result = writeOpt( file, cuT( "normal_directx" ), configuration.normalGMultiplier != 1.0f );
			}
		}

		if ( result && configuration.heightMask[0] )
		{
			result = writeMask( file, cuT( "height_mask" ), configuration.heightMask[0] );

			if ( result && configuration.heightFactor != 1.0f )
			{
				result = write( file, cuT( "height_factor" ), configuration.heightFactor );
			}
		}

		if ( result && configuration.occlusionMask[0] )
		{
			result = writeMask( file, cuT( "occlusion_mask" ), configuration.occlusionMask[0] );
		}

		if ( result && configuration.transmittanceMask[0] )
		{
			result = writeMask( file, cuT( "transmittance_mask" ), configuration.transmittanceMask[0] );
		}

		if ( result )
		{
			result = writeOpt( file, cuT( "invert_y" ), configuration.needsYInversion );
		}

		if ( result )
		{
			auto rotate = castor::Point2f{ configuration.rotate };
			auto translate = castor::Point3f{ configuration.translate };

			if ( translate != castor::Point3f{}
				|| rotate != castor::Point2f{ 1.0f, 0.0f } )
			{
				if ( auto animBlock{ beginBlock( file, "animation" ) } )
				{
					result = writeNamedSubOpt( file, cuT( "translate" ), translate, castor::Point3f{} )
						&& writeNamedSubOpt( file, cuT( "rotate" ), rotate, castor::Point2f{ 1.0f, 0.0f } );
				}
			}
		}

		return result;
	}
}
