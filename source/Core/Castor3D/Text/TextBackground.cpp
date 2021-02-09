#include "Castor3D/Text/TextBackground.hpp"

#include "Castor3D/Text/TextColourBackground.hpp"
#include "Castor3D/Text/TextImageBackground.hpp"
#include "Castor3D/Text/TextSkyboxBackground.hpp"

using namespace castor3d;

namespace castor
{
	TextWriter< SceneBackground >::TextWriter( String const & tabs )
		: TextWriterT< SceneBackground >{ tabs }
	{
	}

	bool TextWriter< SceneBackground >::operator()( SceneBackground const & background
		, TextFile & file )
	{
		switch ( background.getType() )
		{
		case BackgroundType::eColour:
			return write( static_cast< ColourBackground const & >( background ), file );
		case BackgroundType::eImage:
			return write< >( static_cast< ImageBackground const & >( background ), file );
		case BackgroundType::eSkybox:
			return write< >( static_cast< SkyboxBackground const & >( background ), file );
		default:
			return false;
		}
	}
}
