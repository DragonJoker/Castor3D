#include "TextRenderWindow.hpp"

#include "TextRenderTarget.hpp"

#include <Castor3D/Miscellaneous/Logger.hpp>

namespace castor
{
	using namespace castor3d;

	TextWriter< RenderWindow >::TextWriter( castor::String const & tabs )
		: TextWriterT< RenderWindow >{ tabs }
	{
	}

	bool TextWriter< RenderWindow >::operator()( RenderWindow const & window
		, castor::StringStream & file )
	{
		log::info << tabs() << cuT( "Writing Window " ) << window.getName() << std::endl;
		bool result = false;

		if ( auto block{ beginBlock( file, cuT( "window" ), window.getName() ) } )
		{
			result = write( file, cuT( "vsync" ), window.isVSyncEnabled() )
				&& write( file, cuT( "fullscreen" ), window.isFullscreen() );

			if ( result && window.getRenderTarget() )
			{
				result = writeSub( file, *window.getRenderTarget() );
			}
		}

		return result;
	}
}
