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

		if ( auto block = beginBlock( file, cuT( "font" ), object.getName() ) )
		{
			result = writeFile( file, "file", object.getFilePath(), {} )
				&& write( file, "height", object.getHeight() );
		}

		return result;
	}
}
