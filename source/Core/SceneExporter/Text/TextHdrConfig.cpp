#include "TextHdrConfig.hpp"

namespace c3d
{
	TextWriter< HdrConfig >::TextWriter( String const & tabs )
		: TextWriterT< HdrConfig >{ tabs }
	{
	}

	bool TextWriter< HdrConfig >::operator()( HdrConfig const & obj
		, StringStream & file )
	{
		bool result = false;

		if ( auto block{ beginBlock( file, cuT( "hdr_config" ) ) } )
		{
			result = write( file, cuT( "exposure" ), obj.exposure )
				&& write( file, cuT( "gamma" ), obj.gamma );
		}

		return result;
	}
}
