#include "Castor3D/Material/Pass/SubsurfaceScattering.hpp"

#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"

CU_ImplementSmartPtr( castor3d, SubsurfaceScattering )

namespace castor3d
{
	SubsurfaceScattering::SubsurfaceScattering()
		: m_gaussianWidth{ m_dirty, 1.0f, [this](){ onChanged( *this ); } }
		, m_strength{ m_dirty, 1.0f, [this](){ onChanged( *this ); } }
		, m_subsurfaceRadius{ m_dirty, 1.0f, [this](){ onChanged( *this ); } }
		, m_thicknessScale{ m_dirty, 8.25f, [this](){ onChanged( *this ); } }
		, m_originalBlendFactors{ m_dirty, { 1.0f, 1.0f, 1.0f }, [this](){ onChanged( *this ); } }
		, m_subsurfaceBlends{ m_dirty, { 1.0f, 1.0f, 1.0f }, [this](){ onChanged( *this ); } }
		, m_subsurfaceGaussianExponents{ m_dirty, { 1.0f, 1.0f, 1.0f }, [this](){ onChanged( *this ); } }
		, m_transmittanceCoefficients{ m_dirty, { 1.0f, 1.0f, 1.0f }, [this](){ onChanged( *this ); } }
	{
	}

	void SubsurfaceScattering::accept( ConfigurationVisitorBase & vis )
	{
		vis.visit( cuT( "Strength" )
			, m_strength );
		vis.visit( cuT( "Gaussian Width" )
			, m_gaussianWidth );
		vis.visit( cuT( "Surface Radius" )
			, m_subsurfaceRadius );
		vis.visit( cuT( "Thickness Scale" )
			, m_thicknessScale );
		uint32_t i = 0u;

		for ( auto & profileFactors : m_profileFactors )
		{
			vis.visit( cuT( "Factors " ) + castor::string::toString( i )
				, profileFactors );
			++i;
		}
	}
}
