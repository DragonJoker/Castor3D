#include "Castor3D/Castor3DPrerequisites.hpp"

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
		default:
			CU_Failure( "Unsupported MaterialType" );
			return castor::cuEmptyString;
		}
	}

	castor::String getName( BlendMode value )
	{
		switch ( value )
		{
		case BlendMode::eNoBlend:
			return cuT( "none" );
		case BlendMode::eAdditive:
			return cuT( "additive" );
		case BlendMode::eMultiplicative:
			return cuT( "multiplicative" );
		case BlendMode::eInterpolative:
			return cuT( "interpolative" );
		case BlendMode::eABuffer:
			return cuT( "a_buffer" );
		case BlendMode::eDepthPeeling:
			return cuT( "depth_peeling" );
		default:
			CU_Failure( "Unsupported BlendMode" );
			return castor::cuEmptyString;
		}
	}
}
