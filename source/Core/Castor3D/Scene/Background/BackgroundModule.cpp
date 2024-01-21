#include "Castor3D/Scene/Background/BackgroundModule.hpp"

namespace castor3d
{
	castor::StringView getName( SkyboxFace face )
	{
		switch ( face )
		{
		case SkyboxFace::eLeft:
			return "left";
		case SkyboxFace::eRight:
			return "right";
		case SkyboxFace::eBottom:
			return "bottom";
		case SkyboxFace::eTop:
			return "top";
		case SkyboxFace::eBack:
			return "back";
		case SkyboxFace::eFront:
			return "front";
		default:
			CU_Failure( "Unsupported SkyboxFace" );
			return "unsupported";
		}
	}
}
