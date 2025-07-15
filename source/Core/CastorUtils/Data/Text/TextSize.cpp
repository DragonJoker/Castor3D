#include "CastorUtils/Data/Text/TextSize.hpp"

namespace c3d
{
	TextWriter< Size >::TextWriter( String const & tabs )
		: TextWriterT< Size >{ tabs }
	{
	}

	bool TextWriter< Size >::operator()( Size const & object, StringStream & file )
	{
		StringStream stream{ makeStringStream() };
		stream.setf( std::ios::showpoint );
		stream << object[0] << " " << object[1];
		return writeText( file, stream.str() );
	}
}
