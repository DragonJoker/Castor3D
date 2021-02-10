#include "CastorUtils/Data/Text/TextSize.hpp"

namespace castor
{
	TextWriter< Size >::TextWriter( String const & tabs )
		: TextWriterT< Size >{ tabs }
	{
	}

	bool TextWriter< Size >::operator()( Size const & object, TextFile & file )
	{
		StringStream stream{ makeStringStream() };
		stream.setf( std::ios::showpoint );
		stream << object[0] << " " << object[1];
		return file.print( 1024, cuT( "%s" ), stream.str().c_str() ) > 0;
	}
}
