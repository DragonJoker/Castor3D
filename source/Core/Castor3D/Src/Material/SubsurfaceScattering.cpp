#include "SubsurfaceScattering.hpp"

using namespace castor;

namespace castor3d
{
	SubsurfaceScattering::TextWriter::TextWriter( String const & tabs )
		: castor::TextWriter< SubsurfaceScattering >{ tabs }
	{
	}

	bool SubsurfaceScattering::TextWriter::operator()( SubsurfaceScattering const & obj
		, TextFile & file )
	{
		bool result = file.writeText( cuT( "\n" )
			+ m_tabs + cuT( "subsurface_scattering\n" )
			+ m_tabs + cuT( "{\n" ) ) > 0;

		if ( result )
		{
			if ( result )
			{
				result = file.writeText( m_tabs + cuT( "\tstrength " )
					+ string::toString( obj.getStrength() )
					+ cuT( "\n" ) ) > 0;
				castor::TextWriter< SubsurfaceScattering >::checkError( result, "SubsurfaceScattering strength" );
			}

			if ( result )
			{
				result = file.writeText( m_tabs + cuT( "\tgaussian_width " )
					+ string::toString( obj.getGaussianWidth() )
					+ cuT( "\n" ) ) > 0;
				castor::TextWriter< SubsurfaceScattering >::checkError( result, "SubsurfaceScattering Gaussian width" );
			}

			if ( result )
			{
				result = file.writeText( m_tabs + cuT( "\ttransmittance_profile\n" )
					+ m_tabs + cuT( "\t{\n" ) ) > 0;
				castor::TextWriter< SubsurfaceScattering >::checkError( result, "SubsurfaceScattering transmittance profile" );

				for ( auto & factor : obj )
				{
					result = file.writeText( m_tabs + cuT( "\t\tfactor " ) ) > 0
						&& Point4f::TextWriter( String() )( factor, file )
						&& file.writeText( cuT( "\n" ) ) > 0;
					castor::TextWriter< SubsurfaceScattering >::checkError( result, "SubsurfaceScattering transmittance profile factor" );
				}

				result = file.writeText( m_tabs + cuT( "\t}\n" ) ) > 0;
			}

			result = file.writeText( m_tabs + cuT( "}\n" ) ) > 0;
		}

		return result;
	}
}
