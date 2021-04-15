#include "Castor3D/Text/TextLight.hpp"

#include "Castor3D/Text/TextDirectionalLight.hpp"
#include "Castor3D/Text/TextPointLight.hpp"
#include "Castor3D/Text/TextSpotLight.hpp"

using namespace castor3d;

namespace castor
{
	TextWriter< Light >::TextWriter( String const & tabs )
		: TextWriterT< Light >{ tabs }
	{
	}

	bool TextWriter< Light >::operator()( Light const & light
		, StringStream & file )
	{
		switch ( light.getLightType() )
		{
		case LightType::eDirectional:
			return writeSub( file, *light.getDirectionalLight() );
		case LightType::ePoint:
			return writeSub( file, *light.getPointLight() );
		case LightType::eSpot:
			return writeSub( file, *light.getSpotLight() );
		default:
			return false;
		}
	}
}
