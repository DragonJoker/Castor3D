/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslLpvGI_H___
#define ___C3D_GlslLpvGI_H___

#include "SdwModule.hpp"

#include <ShaderWriter/Intrinsics/Intrinsics.hpp>

namespace castor3d
{
	namespace shader
	{
		class LpvGI
		{
		public:
			C3D_API LpvGI( sdw::ShaderWriter & writer );
			C3D_API sdw::Vec3 computeLPVRadiance( sdw::Vec3 wsPosition
				, sdw::Vec3 wsNormal
				, sdw::Vec3 minVolumeCorners
				, sdw::Float cellSize
				, sdw::Vec3 gridSize
				, sdw::SampledImage3DRgba16 lpvAccumulatorR
				, sdw::SampledImage3DRgba16 lpvAccumulatorG
				, sdw::SampledImage3DRgba16 lpvAccumulatorB );

		private:
			sdw::ShaderWriter & m_writer;
			sdw::Function< sdw::Vec4
				, sdw::InVec3 > m_evalSH;
			sdw::Function< sdw::Vec3
				, sdw::InVec3
				, sdw::InVec3
				, sdw::InVec3
				, sdw::InFloat
				, sdw::InVec3
				, sdw::InSampledImage3DRgba16
				, sdw::InSampledImage3DRgba16
				, sdw::InSampledImage3DRgba16 > m_computeLPVRadiance;
		};

	}
}

#endif
