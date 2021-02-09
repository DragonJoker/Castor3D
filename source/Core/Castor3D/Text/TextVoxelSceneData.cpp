#include "Castor3D/Text/TextVoxelSceneData.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"

using namespace castor3d;

namespace castor
{
	TextWriter< VoxelSceneData >::TextWriter( String const & tabs )
		: TextWriterT< VoxelSceneData >{ tabs }
	{
	}

	bool TextWriter< VoxelSceneData >::operator()( VoxelSceneData const & obj, TextFile & file )
	{
		log::info << cuT( "Writing VoxelSceneData" ) << std::endl;
		bool result{ false };

		if ( auto block = beginBlock( cuT( "voxel_cone_tracing" ), file ) )
		{
			result = write( cuT( "enabled" ), obj.enabled, file )
				&& write( cuT( "conservative_rasterization" ), obj.enableConservativeRasterization, file )
				&& write( cuT( "temporal_smoothing" ), obj.enableTemporalSmoothing, file )
				&& write( cuT( "occlusion" ), obj.enableOcclusion, file )
				&& write( cuT( "secondary_bounce" ), obj.enableSecondaryBounce, file )
				&& write( cuT( "num_cones" ), obj.numCones, file )
				&& write( cuT( "max_distance" ), obj.maxDistance, file )
				&& write( cuT( "ray_step_size" ), obj.rayStepSize, file )
				&& write( cuT( "voxel_size" ), obj.voxelSizeFactor, file );
		}

		return result;
	}
}
