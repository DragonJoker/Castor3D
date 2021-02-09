#include "Castor3D/Text/TextSubsurfaceScattering.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"

#include <CastorUtils/Data/Text/TextPoint.hpp>

using namespace castor3d;

namespace castor
{
	TextWriter< SubsurfaceScattering >::TextWriter( String const & tabs )
		: TextWriterT< SubsurfaceScattering >{ tabs }
	{
	}

	bool TextWriter< SubsurfaceScattering >::operator()( SubsurfaceScattering const & obj
		, TextFile & file )
	{
		auto result = false;

		if ( auto block = beginBlock( "subsurface_scattering", file ) )
		{
			result = write( cuT( "strength" ), obj.getStrength(), file )
				&& write( cuT( "gaussian_width" ), obj.getGaussianWidth(), file );

			if ( auto profBlock = beginBlock( cuT( "transmittance_profile" ), file ) )
			{
				for ( auto & factor : obj )
				{
					result = result && write( cuT( "factor" ), factor, file );
				}
			}
		}

		return result;
	}
}
