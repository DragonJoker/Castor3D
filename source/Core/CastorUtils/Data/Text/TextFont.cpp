#include "CastorUtils/Data/Text/TextFont.hpp"

namespace castor
{
	TextWriter< Font >::TextWriter( String const & tabs )
		: TextWriterT< Font >{ tabs }
	{
	}

	bool TextWriter< Font >::operator()( Font const & object, TextFile & file )
	{
		Logger::logInfo( cuT( "Writing Font " ) + object.getName() );
		bool result = false;

		if ( beginBlock( cuT( "font" ), object.getName(), file ) )
		{
			result = writeFile( "file", object.getFilePath(), {}, file )
				&& write( "height", object.getHeight(), file );
		}

		return result;
	}
}
