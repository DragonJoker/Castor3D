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
		log::info << tabs() << cuT( "Writing VoxelSceneData" ) << std::endl;
		auto result = file.writeText( cuT( "\n" ) + tabs() + cuT( "//Voxel Cone Tracing\n" ) ) > 0;

		if ( result )
		{
			result = false;

			if ( auto block{ beginBlock( file, cuT( "voxel_cone_tracing" ) ) } )
			{
				result = write( file, cuT( "enabled" ), obj.enabled )
					&& write( file, cuT( "conservative_rasterization" ), obj.enableConservativeRasterization )
					&& write( file, cuT( "temporal_smoothing" ), obj.enableTemporalSmoothing )
					&& write( file, cuT( "occlusion" ), obj.enableOcclusion )
					&& write( file, cuT( "secondary_bounce" ), obj.enableSecondaryBounce )
					&& write( file, cuT( "num_cones" ), obj.numCones )
					&& write( file, cuT( "max_distance" ), obj.maxDistance )
					&& write( file, cuT( "ray_step_size" ), obj.rayStepSize )
					&& write( file, cuT( "voxel_size" ), obj.voxelSizeFactor );
			}
		}

		return result;
	}
}
