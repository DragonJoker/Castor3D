#include "CastorUtils/Data/Text/TextPosition.hpp"

namespace castor
{
	TextWriter< Position >::TextWriter( String const & tabs )
		: TextWriterT< Position >{ tabs }
	{
	}

	bool TextWriter< Position >::operator()( Position const & object, TextFile & file )
	{
		StringStream stream{ makeStringStream() };
		stream.setf( std::ios::showpoint );
		stream << object[0] << " " << object[1];
		return file.print( 1024, cuT( "%s" ), stream.str().c_str() ) > 0;
	}
}
