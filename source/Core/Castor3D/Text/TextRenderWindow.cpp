#include "Castor3D/Text/TextRenderWindow.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Text/TextRenderTarget.hpp"

using namespace castor3d;

namespace castor
{
	TextWriter< RenderWindow >::TextWriter( castor::String const & tabs )
		: TextWriterT< RenderWindow >{ tabs }
	{
	}

	bool TextWriter< RenderWindow >::operator()( RenderWindow const & window
		, castor::TextFile & file )
	{
		log::info << tabs() << cuT( "Writing Window " ) << window.getName() << std::endl;
		bool result = false;

		if ( auto block{ beginBlock( file, cuT( "window" ), window.getName() ) } )
		{
			result = write( file, cuT( "vsync" ), window.isVSyncEnabled() )
				&& write( file, cuT( "fullscreen" ), window.isFullscreen() );

			if ( result && window.getRenderTarget() )
			{
				result = write( file, *window.getRenderTarget() );
			}
		}

		return result;
	}
}
