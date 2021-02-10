#include "Castor3D/Text/TextShadow.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Text/TextLpvConfig.hpp"
#include "Castor3D/Text/TextRsmConfig.hpp"

using namespace castor3d;

namespace castor
{
	TextWriter< ShadowConfig >::TextWriter( String const & tabs )
		: TextWriterT< ShadowConfig >{ tabs }
	{
	}

	bool TextWriter< ShadowConfig >::operator()( ShadowConfig const & object, TextFile & file )
	{
		log::info << tabs() << cuT( "Writing ShadowConfig" ) << std::endl;
		bool result = false;

		if ( auto block = beginBlock( file, "shadows" ) )
		{
			result = write( file, "filter", getName( object.filterType ) )
				&& write( file, "global_illumination", getName( object.globalIllumination ) );

			if ( auto rawBlock = beginBlock( file, "raw_config" ) )
			{
				result = result
					&& write( file, cuT( "min_offset" ), object.rawOffsets[0] )
					&& write( file, cuT( "max_slope_offset" ), object.rawOffsets[1] );
			}

			if ( auto pcfBlock = beginBlock( file, "pcf_config" ) )
			{
				result = result
					&& write( file, cuT( "min_offset" ), object.pcfOffsets[0] )
					&& write( file, cuT( "max_slope_offset" ), object.pcfOffsets[1] );
			}

			if ( auto vsmBlock = beginBlock( file, "vsm_config" ) )
			{
				result = result
					&& write( file, cuT( "variance_max" ), object.variance[0] )
					&& write( file, cuT( "variance_bias" ), object.variance[1] );
			}

			result = result
				&& write( file, object.rsmConfig )
				&& write( file, object.lpvConfig );
		}

		return result;
	}
}
