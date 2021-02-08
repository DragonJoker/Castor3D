#include "Castor3D/Render/Technique/Voxelize/VoxelSceneData.hpp"

using namespace castor;

namespace castor3d
{
	VoxelSceneData::TextWriter::TextWriter( String const & tabs )
		: castor::TextWriter< VoxelSceneData >{ tabs }
	{
	}

	bool VoxelSceneData::TextWriter::operator()( VoxelSceneData const & obj, TextFile & file )
	{
		log::info << m_tabs << cuT( "Writing VoxelSceneData" ) << std::endl;
		return beginBlock( cuT( "voxel_cone_tracing" ), file )
			&& write( cuT( "enabled" ), obj.enabled, file )
			&& write( cuT( "conservative_rasterization" ), obj.enableConservativeRasterization, file )
			&& write( cuT( "temporal_smoothing" ), obj.enableTemporalSmoothing, file )
			&& write( cuT( "occlusion" ), obj.enableOcclusion, file )
			&& write( cuT( "secondary_bounce" ), obj.enableSecondaryBounce, file )
			&& write( cuT( "num_cones" ), obj.numCones, file )
			&& write( cuT( "max_distance" ), obj.maxDistance, file )
			&& write( cuT( "ray_step_size" ), obj.rayStepSize, file )
			&& write( cuT( "voxel_size" ), obj.voxelSizeFactor, file )
			&& endBlock( file );
	}
}
