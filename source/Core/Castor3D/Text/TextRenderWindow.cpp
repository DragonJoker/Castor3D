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

		if ( auto block = beginBlock( cuT( "window" ), window.getName(), file ) )
		{
			result = write( cuT( "vsync" ), window.isVSyncEnabled(), file )
				&& write( cuT( "fullscreen" ), window.isFullscreen(), file );

			if ( result && window.getRenderTarget() )
			{
				result = write( *window.getRenderTarget(), file );
			}
		}

		return result;
	}
}
