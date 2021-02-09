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

		if ( auto block = beginBlock( cuT( "hdr_config" ), file ) )
		{
			result = write( cuT( "exposure" ), obj.exposure, file )
				&& write( cuT( "gamma " ), obj.gamma, file );
		}

		return result;
	}
}
