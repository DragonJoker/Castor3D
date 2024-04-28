#include "Castor3D/Render/Clustered/ClusteredModule.hpp"

namespace castor3d
{
	castor::String getName( ClusterSplitScheme value )
	{
		switch ( value )
		{
		case castor3d::ClusterSplitScheme::eExponentialBase:
			return cuT( "exponential" );
		case castor3d::ClusterSplitScheme::eLinear:
			return cuT( "linear" );
		case castor3d::ClusterSplitScheme::eExponentialLinearHybrid:
			return cuT( "hybrid" );
		default:
			CU_Failure( "Unknown ClusterSplitScheme." );
			return cuT( "unknown" );
		}
	}
}
