#include "Castor3D/Material/Pass/SubsurfaceScattering.hpp"

#include "Castor3D/Material/Pass/PassVisitor.hpp"

namespace castor3d
{
	void SubsurfaceScattering::accept( PassVisitorBase & vis )
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
