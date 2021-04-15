#include "CastorUtils/Data/Text/TextFont.hpp"

namespace castor
{
	TextWriter< Font >::TextWriter( String const & tabs, Path const & folder )
		: TextWriterT< Font >{ tabs }
		, m_folder{ folder }
	{
	}

	bool TextWriter< Font >::operator()( Font const & object, StringStream & file )
	{
		Logger::logInfo( cuT( "Writing Font " ) + object.getName() );
		bool result = false;

		if ( auto block{ beginBlock( file, cuT( "font" ), object.getName() ) } )
		{
			result = writeFile( file, "file", object.getFilePath(), m_folder, {} )
				&& write( file, "height", object.getHeight() );
		}

		return result;
	}
}
