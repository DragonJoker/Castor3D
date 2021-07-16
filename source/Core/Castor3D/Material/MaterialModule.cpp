#include "Castor3D/Material/MaterialModule.hpp"

namespace castor3d
{
	castor::String getName( TextureFlag value
		, bool isPbr )
	{
		switch ( value )
		{
		case TextureFlag::eNone:
			return cuT( "none" );
		case TextureFlag::eDiffuse:
			return ( isPbr
				? castor::String{ cuT( "albedo" ) }
				: castor::String{ cuT( "diffuse" ) } );
		case TextureFlag::eNormal:
			return cuT( "normal" );
		case TextureFlag::eOpacity:
			return cuT( "opacity" );
		case TextureFlag::eSpecular:
			return cuT( "specular" );
		case TextureFlag::eMetalness:
			return cuT( "metallic" );
		case TextureFlag::eHeight:
			return cuT( "height" );
		case TextureFlag::eGlossiness:
			return ( isPbr
				? castor::String{ cuT( "glossiness" ) }
				: castor::String{ cuT( "shininess" ) } );
		case TextureFlag::eRoughness:
			return cuT( "roughness" );
		case TextureFlag::eEmissive:
			return cuT( "emissive" );
		case TextureFlag::eOcclusion:
			return cuT( "occlusion" );
		case TextureFlag::eTransmittance:
			return cuT( "transmittance" );
		default:
			CU_Failure( "Unsupported TextureFlag" );
			return castor::cuEmptyString;
		}
	}

	bool operator==( TextureFlagsId const & lhs, TextureFlagsId const & rhs )
	{
		return lhs.id == rhs.id
			&& lhs.flags == rhs.flags;
	}

	VkFormat convert( castor::PixelFormat format )
	{
		return VkFormat( format );
	}

	castor::PixelFormat convert( VkFormat format )
	{
		return castor::PixelFormat( format );
	}
}
