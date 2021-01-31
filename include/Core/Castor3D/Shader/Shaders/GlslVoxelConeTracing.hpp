/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslVoxelConeTracing_H___
#define ___C3D_GlslVoxelConeTracing_H___

#include "SdwModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"

#include <ShaderWriter/CompositeTypes/Function.hpp>

namespace castor3d::shader
{
	class VoxelConeTracing
	{
	public:
		C3D_API VoxelConeTracing( sdw::ShaderWriter & writer
			, Utils const & utils );
		C3D_API void declare();

		C3D_API sdw::Vec4 traceConeRadiance( sdw::SampledImage3DRgba32 const & voxels
			, sdw::Vec3 const & wsPosition
			, sdw::Vec3 const & wsNormal
			, VoxelData const & voxelData )const;
		C3D_API sdw::Vec4 traceConeReflection( sdw::SampledImage3DRgba32 const & voxels
			, sdw::Vec3 const & wsPosition
			, sdw::Vec3 const & wsNormal
			, sdw::Vec3 const & wsViewVector
			, sdw::Float const & roughness
			, VoxelData const & voxelData )const;

	private:
		sdw::ShaderWriter & m_writer;
		Utils const & m_utils;
		sdw::Function< sdw::Vec4
			, sdw::InSampledImage3DRgba32
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InFloat
			, InVoxelData > m_traceCone;
		sdw::Function< sdw::Vec4
			, sdw::InSampledImage3DRgba32
			, sdw::InVec3
			, sdw::InVec3
			, InVoxelData > m_traceConeRadiance;
		sdw::Function< sdw::Vec4
			, sdw::InSampledImage3DRgba32
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InFloat
			, InVoxelData > m_traceConeReflection;
	};
}

#endif
