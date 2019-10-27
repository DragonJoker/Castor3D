#include "Castor3D/Castor3DPrerequisites.hpp"

namespace castor3d
{
	castor::String getName( LightType value )
	{
		switch ( value )
		{
		case LightType::eDirectional:
			return cuT( "directional" );
		case LightType::ePoint:
			return cuT( "point" );
		case LightType::eSpot:
			return cuT( "spot" );
		default:
			CU_Failure( "Unsupported LightType" );
			return castor::cuEmptyString;
		}
	}
}
