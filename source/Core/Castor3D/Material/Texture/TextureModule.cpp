#include "Castor3D/Material/Texture/TextureModule.hpp"

namespace castor3d
{
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
		case TextureFlag::eReflection:
			return cuT( "reflection" );
		case TextureFlag::eRefraction:
			return cuT( "refraction" );
		case TextureFlag::eOcclusion:
			return cuT( "occlusion" );
		case TextureFlag::eTransmittance:
			return cuT( "transmittance" );
		default:
			CU_Failure( "Unsupported TextureFlag" );
			return castor::cuEmptyString;
		}
	}

	castor::String getName( CubeMapFace value )
	{
		switch ( value )
		{
		case castor3d::CubeMapFace::ePositiveX:
			return cuT( "PositiveX" );
		case castor3d::CubeMapFace::eNegativeX:
			return cuT( "NegativeX" );
		case castor3d::CubeMapFace::ePositiveY:
			return cuT( "PositiveY" );
		case castor3d::CubeMapFace::eNegativeY:
			return cuT( "NegativeY" );
		case castor3d::CubeMapFace::ePositiveZ:
			return cuT( "PositiveZ" );
		case castor3d::CubeMapFace::eNegativeZ:
			return cuT( "NegativeZ" );
		default:
			CU_Failure( "Unsupported CubeMapFace" );
			return castor::cuEmptyString;
		}
	}
}
