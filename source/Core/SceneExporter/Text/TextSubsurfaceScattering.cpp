#include "TextSubsurfaceScattering.hpp"

#include <Castor3D/Miscellaneous/Logger.hpp>

#include <CastorUtils/Data/Text/TextPoint.hpp>

namespace castor
{
	using namespace castor3d;

	TextWriter< SubsurfaceScattering >::TextWriter( String const & tabs )
		: TextWriterT< SubsurfaceScattering >{ tabs }
	{
	}

	bool TextWriter< SubsurfaceScattering >::operator()( SubsurfaceScattering const & obj
		, StringStream & file )
	{
		auto result = false;
		log::info << tabs() << cuT( "Writing SubsurfaceScattering" ) << std::endl;

		if ( auto block{ beginBlock( file, "subsurface_scattering" ) } )
		{
			result = write( file, cuT( "strength" ), obj.getStrength() )
				&& write( file, cuT( "gaussian_width" ), obj.getGaussianWidth() );

			if ( auto profBlock{ beginBlock( file, cuT( "transmittance_profile" ) ) } )
			{
				for ( auto & factor : obj )
				{
					result = result && writeNamedSub( file, cuT( "factor" ), factor );
				}
			}
		}

		return result;
	}
}
