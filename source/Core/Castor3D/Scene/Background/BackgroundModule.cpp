#include "Castor3D/Scene/Background/BackgroundModule.hpp"

namespace castor3d
{
	castor::String getName( BackgroundType value )
	{
		switch ( value )
		{
		case BackgroundType::eColour:
			return cuT( "colour" );
		case BackgroundType::eImage:
			return cuT( "image" );
		case BackgroundType::eSkybox:
			return cuT( "skybox" );
		default:
			CU_Failure( "Unsupported BackgroundType" );
			return castor::cuEmptyString;
		}
	}
}
