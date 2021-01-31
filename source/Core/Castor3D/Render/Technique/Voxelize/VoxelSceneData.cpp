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
		bool result = file.writeText( cuT( "\n" ) + m_tabs + cuT( "voxel_cone_tracing\n" ) ) > 0
			&& file.writeText( m_tabs + cuT( "{\n" ) ) > 0;

		if ( result )
		{
			result = file.writeText( m_tabs + cuT( "\tnum_cones " ) + string::toString( obj.numCones.value(), std::locale{ "C" } ) + cuT( "\n" ) ) > 0;
			castor::TextWriter< VoxelSceneData >::checkError( result, "VoxelSceneData num cones." );
		}

		if ( result )
		{
			result = file.writeText( m_tabs + cuT( "\tmax_distance " ) + string::toString( obj.maxDistance, std::locale{ "C" } ) + cuT( "\n" ) ) > 0;
			castor::TextWriter< VoxelSceneData >::checkError( result, "VoxelSceneData max distance." );
		}

		if ( result )
		{
			result = file.writeText( m_tabs + cuT( "\tray_step_size " ) + string::toString( obj.rayStepSize, std::locale{ "C" } ) + cuT( "\n" ) ) > 0;
			castor::TextWriter< VoxelSceneData >::checkError( result, "VoxelSceneData ray step size." );
		}

		if ( result )
		{
			result = file.writeText( m_tabs + cuT( "}\n" ) ) > 0;
		}

		return result;
	}
}
