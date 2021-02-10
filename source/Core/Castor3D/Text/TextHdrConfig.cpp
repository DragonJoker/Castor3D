#include "Castor3D/Text/TextHdrConfig.hpp"

using namespace castor3d;

namespace castor
{
	TextWriter< HdrConfig >::TextWriter( String const & tabs )
		: TextWriterT< HdrConfig >{ tabs }
	{
	}

	bool TextWriter< HdrConfig >::operator()( HdrConfig const & obj
		, TextFile & file )
	{
		bool result = false;

		if ( auto block = beginBlock( file, cuT( "hdr_config" ) ) )
		{
			result = write( file, cuT( "exposure" ), obj.exposure )
				&& write( file, cuT( "gamma" ), obj.gamma );
		}

		return result;
	}
}
