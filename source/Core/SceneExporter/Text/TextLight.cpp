#include "TextLight.hpp"

#include "TextDirectionalLight.hpp"
#include "TextPointLight.hpp"
#include "TextSpotLight.hpp"

namespace castor
{
	using namespace castor3d;

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
