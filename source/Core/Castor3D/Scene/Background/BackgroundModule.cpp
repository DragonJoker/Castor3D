#include "Castor3D/Scene/Background/BackgroundModule.hpp"

namespace castor3d
{
	castor::StringView getName( SkyboxFace face )
	{
		switch ( face )
		{
		case SkyboxFace::eLeft:
			return cuT( "left" );
		case SkyboxFace::eRight:
			return cuT( "right" );
		case SkyboxFace::eBottom:
			return cuT( "bottom" );
		case SkyboxFace::eTop:
			return cuT( "top" );
		case SkyboxFace::eBack:
			return cuT( "back" );
		case SkyboxFace::eFront:
			return cuT( "front" );
		default:
			CU_Failure( "Unsupported SkyboxFace" );
			return cuT( "unsupported" );
		}
	}
}
