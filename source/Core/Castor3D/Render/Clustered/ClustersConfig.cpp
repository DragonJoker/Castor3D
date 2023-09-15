#include "Castor3D/Render/Clustered/ClustersConfig.hpp"

#include "Castor3D/Config.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"

namespace castor3d
{
	ClustersConfig::ClustersConfig()
		: enabled{ C3D_UseClusteredRendering }
		, useLightsBVH{ true }
		, sortLights{ true }
		, parseDepthBuffer{ false }
		, limitClustersToLightsAABB{ false }
	{
	}

	void ClustersConfig::accept( ConfigurationVisitorBase & visitor )
	{
		if ( enabled )
		{
			visitor.visit( cuT( "Clusters" ) );
			visitor.visit( cuT( "Use BVH" ), useLightsBVH );
			visitor.visit( cuT( "Sort Lights" ), sortLights );
			visitor.visit( cuT( "Use Depth Buffer" ), parseDepthBuffer );
			visitor.visit( cuT( "Limit Clusters To Lights AABB" ), limitClustersToLightsAABB );
		}
	}
}
