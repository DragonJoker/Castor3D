#include "TextClustersConfig.hpp"

#include <Castor3D/Miscellaneous/Logger.hpp>

namespace c3d
{
	TextWriter< ClustersConfig >::TextWriter( String const & tabs )
		: TextWriterT< ClustersConfig >{ tabs }
	{
	}

	bool TextWriter< ClustersConfig >::operator()( ClustersConfig const & object
		, StringStream & file )
	{
		static const String splitSchemes[uint32_t( ClusterSplitScheme::eCount )] =
		{
			getName( ClusterSplitScheme::eExponentialBase ),
			getName( ClusterSplitScheme::eLinear ),
			getName( ClusterSplitScheme::eExponentialLinearHybrid ),
		};

		bool result{ false };
		log::info << tabs() << cuT( "Writing ClustersConfig" ) << std::endl;

		if ( auto block{ beginBlock( file, cuT( "clusters" ) ) } )
		{
			result = writeOpt( file, cuT( "enabled" ), object.enabled, true )
				&& writeOpt( file, cuT( "use_lights_bvh" ), object.useLightsBVH.value(), true )
				&& writeOpt( file, cuT( "sort_lights" ), object.sortLights.value(), true )
				&& writeOpt( file, cuT( "limit_clusters_to_lights_aabb" ), object.limitClustersToLightsAABB.value(), true )
				&& writeOpt( file, cuT( "parse_depth_buffer" ), object.parseDepthBuffer.value(), false )
				&& writeOpt( file, cuT( "use_spot_bounding_cone" ), object.useSpotBoundingCone.value(), true )
				&& writeOpt( file, cuT( "use_spot_tight_aabb" ), object.useSpotTightBoundingBox.value(), true )
				&& writeOpt( file, cuT( "enable_reduce_warp_optimisation" ), object.enableReduceWarpOptimisation.value(), false )
				&& writeOpt( file, cuT( "enable_bvh_warp_optimisation" ), object.enableBVHWarpOptimisation.value(), true )
				&& writeOpt( file, cuT( "split_scheme" ), splitSchemes[uint32_t( object.splitScheme.value() )], splitSchemes[uint32_t( ClusterSplitScheme::eExponentialLinearHybrid )] )
				&& writeOpt( file, cuT( "min_distance" ), object.minDistance.value(), 1.0f );
		}

		return result;
	}
}
