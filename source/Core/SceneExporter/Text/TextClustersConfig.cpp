#include "TextClustersConfig.hpp"

#include <Castor3D/Miscellaneous/Logger.hpp>

namespace castor
{
	using namespace castor3d;

	TextWriter< ClustersConfig >::TextWriter( String const & tabs )
		: TextWriterT< ClustersConfig >{ tabs }
	{
	}

	bool TextWriter< ClustersConfig >::operator()( ClustersConfig const & object
		, StringStream & file )
	{
		static const String splitSchemes[uint32_t( castor3d::ClusterSplitScheme::eCount )] =
		{
			getName( castor3d::ClusterSplitScheme::eExponentialBase ),
			getName( castor3d::ClusterSplitScheme::eExponentialBiased ),
			getName( castor3d::ClusterSplitScheme::eLinear ),
			getName( castor3d::ClusterSplitScheme::eExponentialLinearHybrid ),
		};

		bool result{ false };
		log::info << tabs() << cuT( "Writing ClustersConfig" ) << std::endl;

		if ( auto block{ beginBlock( file, "clusters" ) } )
		{
			result = writeOpt( file, cuT( "enabled" ), object.enabled, true )
				&& writeOpt( file, cuT( "use_lights_bvh" ), object.useLightsBVH, true )
				&& writeOpt( file, cuT( "sort_lights" ), object.sortLights, true )
				&& writeOpt( file, cuT( "limit_clusters_to_lights_aabb" ), object.limitClustersToLightsAABB, true )
				&& writeOpt( file, cuT( "parse_depth_buffer" ), object.parseDepthBuffer, false )
				&& writeOpt( file, cuT( "use_spot_bounding_cone" ), object.useSpotBoundingCone, true )
				&& writeOpt( file, cuT( "use_spot_tight_aabb" ), object.useSpotTightBoundingBox, true )
				&& writeOpt( file, cuT( "enable_reduce_warp_optimisation" ), object.enableReduceWarpOptimisation, false )
				&& writeOpt( file, cuT( "enable_bvh_warp_optimisation" ), object.enableBVHWarpOptimisation, true )
				&& writeOpt( file, cuT( "split_scheme" ), splitSchemes[uint32_t( object.splitScheme )], splitSchemes[uint32_t( castor3d::ClusterSplitScheme::eExponentialBase )] )
				&& writeOpt( file, cuT( "bias" ), object.bias, 1.0f );
		}

		return result;
	}
}
