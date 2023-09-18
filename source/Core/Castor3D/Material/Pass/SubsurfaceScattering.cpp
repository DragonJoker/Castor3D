#include "Castor3D/Material/Pass/SubsurfaceScattering.hpp"

#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"

CU_ImplementSmartPtr( castor3d, SubsurfaceScattering )

namespace castor3d
{
	void SubsurfaceScattering::accept( ConfigurationVisitorBase & vis )
	{
		vis.visit( cuT( "Strength" )
			, m_strength );
		vis.visit( cuT( "Gaussian Width" )
			, m_gaussianWidth );
		vis.visit( cuT( "Surface Radius" )
			, m_subsurfaceRadius );
		uint32_t i = 0u;

		for ( auto & profileFactors : m_profileFactors )
		{
			vis.visit( cuT( "Factors " ) + castor::string::toString( i )
				, profileFactors );
			++i;
		}
	}
}
