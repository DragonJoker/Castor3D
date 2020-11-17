/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslLpvGI_H___
#define ___C3D_GlslLpvGI_H___

#include "SdwModule.hpp"

#include "Castor3D/Scene/SceneModule.hpp"

#include <ShaderWriter/Intrinsics/Intrinsics.hpp>

namespace castor3d
{
	namespace shader
	{
		class LpvGI
		{
		public:
			C3D_API LpvGI( sdw::ShaderWriter & writer );
			C3D_API void declare( uint32_t setIndex
				, uint32_t & bindingIndex
				, SceneFlags sceneFlags );
			C3D_API void declare( uint32_t bindingIndex );
			C3D_API sdw::Vec3 computeLPVRadiance( sdw::Vec3 wsPosition
				, sdw::Vec3 wsNormal
				, sdw::Vec3 minVolumeCorners
				, sdw::Float cellSize
				, sdw::Vec3 gridSize );
			C3D_API sdw::Vec3 computeResult( SceneFlags sceneFlags
				, sdw::Vec3 wsPosition
				, sdw::Vec3 wsNormal
				, sdw::Vec3 minVolumeCorners
				, sdw::Float cellSize
				, sdw::Vec3 gridSize
				, sdw::Array< sdw::Vec4 > allMinVolumeCorners
				, sdw::Vec4 allCellSizes
				, sdw::UVec4 gridSizes
				, sdw::Vec3 allButAmbient
				, sdw::Vec3 ambient );

		private:
			sdw::ShaderWriter & m_writer;
			sdw::Function< sdw::Vec4
				, sdw::InVec3 > m_evalSH;
			sdw::Function< sdw::Vec3
				, sdw::InVec3
				, sdw::InVec3
				, sdw::InVec3
				, sdw::InFloat
				, sdw::InVec3 > m_computeLPVRadiance;
		};

	}
}

#endif
