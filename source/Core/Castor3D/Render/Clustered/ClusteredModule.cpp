#include "Castor3D/Render/Clustered/ClusteredModule.hpp"

namespace c3d
{
	String getName( ClusterSplitScheme value )
	{
		switch ( value )
		{
		case ClusterSplitScheme::eExponentialBase:
			return cuT( "exponential" );
		case ClusterSplitScheme::eLinear:
			return cuT( "linear" );
		case ClusterSplitScheme::eExponentialLinearHybrid:
			return cuT( "hybrid" );
		default:
			CU_Failure( "Unknown ClusterSplitScheme." );
			return cuT( "unknown" );
		}
	}
}
