#include "Castor3D/Material/MaterialModule.hpp"

namespace castor3d
{
	castor::String getName( MaterialType value )
	{
		switch ( value )
		{
		case MaterialType::ePhong:
			return cuT( "phong" );
		case MaterialType::eMetallicRoughness:
			return cuT( "metallic_roughness" );
		case MaterialType::eSpecularGlossiness:
			return cuT( "specular_glossiness" );
		case MaterialType::eCount:
			return cuT( "unsupported" );
		default:
			CU_Failure( "Unsupported MaterialType" );
			return castor::cuEmptyString;
		}
	}

	castor::String getName( TextureFlag value
		, MaterialType material )
	{
		switch ( value )
		{
		case TextureFlag::eNone:
			return cuT( "none" );
		case TextureFlag::eDiffuse:
			return ( material == MaterialType::eSpecularGlossiness
				? castor::String{ cuT( "albedo" ) }
				: ( material == MaterialType::eMetallicRoughness
					? castor::String{ cuT( "albedo" ) }
			: castor::String{ cuT( "diffuse" ) } ) );
		case TextureFlag::eNormal:
			return cuT( "normal" );
		case TextureFlag::eOpacity:
			return cuT( "opacity" );
		case TextureFlag::eSpecular:
			return ( material == MaterialType::eMetallicRoughness
				? castor::String{ cuT( "metallic" ) }
			: castor::String{ cuT( "specular" ) } );
		case TextureFlag::eHeight:
			return cuT( "height" );
		case TextureFlag::eGlossiness:
			return ( material == MaterialType::eSpecularGlossiness
				? castor::String{ cuT( "glossiness" ) }
				: ( material == MaterialType::eMetallicRoughness
					? castor::String{ cuT( "roughness" ) }
			: castor::String{ cuT( "shininess" ) } ) );
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
