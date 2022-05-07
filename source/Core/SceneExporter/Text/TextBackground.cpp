#include "TextBackground.hpp"

#include "TextColourBackground.hpp"
#include "TextImageBackground.hpp"
#include "TextSkyboxBackground.hpp"

namespace castor
{
	using namespace castor3d;

	TextWriter< SceneBackground >::TextWriter( String const & tabs
		, Path const & folder )
		: TextWriterT< SceneBackground >{ tabs }
		, m_folder{ folder }
	{
	}

	bool TextWriter< SceneBackground >::operator()( SceneBackground const & background
		, StringStream & file )
	{
		switch ( background.getType() )
		{
		case BackgroundType::eColour:
			return true;
		case BackgroundType::eImage:
			return writeSub( file, static_cast< ImageBackground const & >( background ), m_folder );
		case BackgroundType::eSkybox:
			return writeSub( file, static_cast< SkyboxBackground const & >( background ), m_folder );
		default:
			return false;
		}
	}
}
