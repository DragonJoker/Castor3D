﻿#include "SubsurfaceScattering.hpp"

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
			if ( result && obj.isDistanceBasedTransmittanceEnabled() )
			{
				result = file.writeText( m_tabs + cuT( "\tdistance_based_transmittance true\n" ) ) > 0;
				castor::TextWriter< SubsurfaceScattering >::checkError( result, "SubsurfaceScattering distance based transmittance enabled" );
				
				if ( result )
				{
					result = file.writeText( m_tabs + cuT( "\ttransmittance_coefficients " ) ) > 0
						&& Point3f::TextWriter{ String{} }( obj.getTransmittanceCoefficients(), file ) 
						&& file.writeText( cuT( "\n" ) ) > 0;
					castor::TextWriter< SubsurfaceScattering >::checkError( result, "SubsurfaceScattering transmittance coefficients" );
				}
			}

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

			result = file.writeText( m_tabs + cuT( "}\n" ) ) > 0;
		}

		return result;
	}
}
