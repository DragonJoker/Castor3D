#include "Castor3D/Text/TextShadow.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"

using namespace castor3d;

namespace castor
{
	TextWriter< Shadow >::TextWriter( String const & tabs )
		: TextWriterT< Shadow >{ tabs }
	{
	}

	bool TextWriter< Shadow >::operator()( Shadow const & light, TextFile & file )
	{
		log::info << tabs() << cuT( "Writing Shadow" ) << std::endl;
		bool result = false;

		if ( auto block = beginBlock( "raw_config", file ) )
		{
			result = write( cuT( "min_offset" ), light.getShadowRawOffsets()[0], file )
				&& write( cuT( "max_slope_offset" ), light.getShadowRawOffsets()[1], file );
		}

		if ( auto block = beginBlock( "pcf_config", file ) )
		{
			result = write( cuT( "min_offset" ), light.getShadowPcfOffsets()[0], file )
				&& write( cuT( "max_slope_offset" ), light.getShadowPcfOffsets()[1], file );
		}

		if ( auto block = beginBlock( "vsm_config", file ) )
		{
			result = write( cuT( "variance_max" ), light.getShadowVariance()[0], file )
				&& write( cuT( "variance_bias" ), light.getShadowVariance()[1], file );
		}

		return result;
	}
}
