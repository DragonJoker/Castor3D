#include "Castor3D/Text/TextImageBackground.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"

using namespace castor3d;

namespace castor
{
	TextWriter< ImageBackground >::TextWriter( String const & tabs, Path const & folder )
		: TextWriterT< ImageBackground >{ tabs }
		, m_folder{ folder }
	{
	}

	bool TextWriter< ImageBackground >::operator()( ImageBackground const & obj
		, StringStream & file )
	{
		log::info << tabs() << cuT( "Writing ImageBackground" ) << std::endl;
		return writeFile( file
			, "background_image"
			, castor::Path{ obj.getTexture().getDefaultView().toString() }
			, m_folder
			, cuT( "Textures" ) );
	}
}
