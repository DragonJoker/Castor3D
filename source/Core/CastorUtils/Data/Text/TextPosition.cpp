#include "CastorUtils/Data/Text/TextPosition.hpp"

namespace castor
{
	TextWriter< Position >::TextWriter( String const & tabs )
		: TextWriterT< Position >{ tabs }
	{
	}

	bool TextWriter< Position >::operator()( Position const & object, StringStream & file )
	{
		StringStream stream{ makeStringStream() };
		stream.setf( std::ios::showpoint );
		stream << object[0] << " " << object[1];
		return writeText( file, stream.str() );
	}
}
