#include "Castor3D/Shader/Ubos/VoxelizerUbo.hpp"

namespace castor3d
{
	uint32_t const VoxelizerUbo::BindingPoint = 13u;
	castor::String const VoxelizerUbo::BufferVoxelizer = cuT( "Voxelizer" );
	castor::String const VoxelizerUbo::Center = cuT( "c3d_voxelCenter" );
	castor::String const VoxelizerUbo::Size = cuT( "c3d_voxelSize" );
	castor::String const VoxelizerUbo::SizeInverse = cuT( "c3d_voxelSizeInverse" );
	castor::String const VoxelizerUbo::Resolution = cuT( "c3d_voxelResolution" );
	castor::String const VoxelizerUbo::ResolutionInverse = cuT( "c3d_voxelResolutionInverse" );
	castor::String const VoxelizerUbo::Transform = cuT( "c3d_voxelTransform" );
}
