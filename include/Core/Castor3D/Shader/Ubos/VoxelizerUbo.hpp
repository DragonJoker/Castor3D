/*
See LICENSE file in root folder
*/
#ifndef ___C3D_VoxelizerUbo_H___
#define ___C3D_VoxelizerUbo_H___

#include "UbosModule.hpp"

#include "Castor3D/Buffer/UniformBufferOffset.hpp"

#include <ShaderWriter/Optional/OptionalUbo.hpp>

namespace castor3d
{
	class VoxelizerUbo
	{
	public:
		using Configuration = VoxelizerUboConfiguration;

	public:
		C3D_API static uint32_t const BindingPoint;
		C3D_API static castor::String const BufferVoxelizer;
		C3D_API static castor::String const Center;
		C3D_API static castor::String const Size;
		C3D_API static castor::String const SizeInverse;
		C3D_API static castor::String const Resolution;
		C3D_API static castor::String const ResolutionInverse;
		C3D_API static castor::String const Transform;
	};
}

#define UBO_VOXELIZER( writer, binding, set )\
	sdw::Ubo voxelizer{ writer\
		, castor3d::VoxelizerUbo::BufferVoxelizer\
		, binding\
		, set };\
	auto c3d_voxelTransform = voxelizer.declMember< sdw::Mat4 >( castor3d::VoxelizerUbo::Transform );\
	auto c3d_voxelCenter = voxelizer.declMember< sdw::Vec4 >( castor3d::VoxelizerUbo::Center );\
	auto c3d_voxelSize = voxelizer.declMember< sdw::Float >( castor3d::VoxelizerUbo::Size );\
	auto c3d_voxelSizeInverse = voxelizer.declMember< sdw::Float >( castor3d::VoxelizerUbo::SizeInverse );\
	auto c3d_voxelResolution = voxelizer.declMember< sdw::Float >( castor3d::VoxelizerUbo::Resolution );\
	auto c3d_voxelResolutionInverse = voxelizer.declMember< sdw::Float >( castor3d::VoxelizerUbo::ResolutionInverse );\
	voxelizer.end()

#endif
