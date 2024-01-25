#include "TextShadow.hpp"

#include "TextLpvConfig.hpp"

#include <Castor3D/Miscellaneous/Logger.hpp>

namespace castor
{
	using namespace castor3d;

	TextWriter< ShadowConfig >::TextWriter( String const & tabs )
		: TextWriterT< ShadowConfig >{ tabs }
	{
	}

	bool TextWriter< ShadowConfig >::operator()( ShadowConfig const & object
		, StringStream & file )
	{
		log::info << tabs() << cuT( "Writing ShadowConfig" ) << std::endl;
		bool result = false;

		if ( auto block{ beginBlock( file, cuT( "shadows" ) ) } )
		{
			result = write( file, cuT( "filter" ), getName( object.filterType ) )
				&& write( file, cuT( "producer" ), object.filterType != ShadowType::eNone )
				&& write( file, cuT( "global_illumination" ), getName( object.globalIllumination ) );

			if ( object.volumetricSteps )
			{
				result = result
					&& write( file, cuT( "volumetric_steps" ), object.volumetricSteps )
					&& write( file, cuT( "volumetric_scattering" ), object.volumetricScattering );
			}

			if ( auto rawBlock{ beginBlock( file, cuT( "raw_config" ) ) } )
			{
				result = result
					&& write( file, cuT( "min_offset" ), object.rawOffsets[0] )
					&& write( file, cuT( "max_slope_offset" ), object.rawOffsets[1] );
			}

			if ( auto pcfBlock{ beginBlock( file, cuT( "pcf_config" ) ) } )
			{
				result = result
					&& write( file, cuT( "min_offset" ), object.pcfOffsets[0] )
					&& write( file, cuT( "max_slope_offset" ), object.pcfOffsets[1] )
					&& write( file, cuT( "filter_size" ), object.pcfFilterSize )
					&& write( file, cuT( "sample_count" ), object.pcfSampleCount );
			}

			if ( auto vsmBlock{ beginBlock( file, cuT( "vsm_config" ) ) } )
			{
				result = result
					&& write( file, cuT( "min_variance" ), object.vsmMinVariance )
					&& write( file, cuT( "light_bleeding_reduction" ), object.vsmLightBleedingReduction );
			}

			result = result
				&& writeSub( file, object.lpvConfig );
		}

		return result;
	}
}
