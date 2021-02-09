#include "Castor3D/Text/TextImageBackground.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"

using namespace castor3d;

namespace castor
{
	TextWriter< ImageBackground >::TextWriter( String const & tabs )
		: TextWriterT< ImageBackground >{ tabs }
	{
	}

	bool TextWriter< ImageBackground >::operator()( ImageBackground const & obj
		, TextFile & file )
	{
		log::info << tabs() << cuT( "Writing ImageBackground" ) << std::endl;
		return writeFile( "background_image"
			, castor::Path{ obj.getTexture().getDefaultView().toString() }
			, cuT( "Textures" )
			, file );
	}
}
