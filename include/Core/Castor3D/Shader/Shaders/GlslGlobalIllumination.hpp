/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslGlobalIllumination_H___
#define ___C3D_GlslGlobalIllumination_H___

#include "SdwModule.hpp"

#include "Castor3D/Scene/SceneModule.hpp"
#include "Castor3D/Shader/Ubos/LayeredLpvGridConfigUbo.hpp"
#include "Castor3D/Shader/Ubos/LpvGridConfigUbo.hpp"

#include <ShaderWriter/Intrinsics/Intrinsics.hpp>

namespace castor3d
{
	namespace shader
	{
		class GlobalIllumination
		{
		public:
			C3D_API explicit GlobalIllumination( sdw::ShaderWriter & writer
				, Utils & utils );
			C3D_API void declare( uint32_t setIndex
				, uint32_t & bindingIndex
				, SceneFlags sceneFlags );
			C3D_API void declareVct( uint32_t & bindingIndex
				, uint32_t setIndex = 0u );
			C3D_API void declareLpv( uint32_t & bindingIndex
				, uint32_t setIndex = 0u );
			C3D_API void declareLayeredLpv( uint32_t & bindingIndex
				, uint32_t setIndex = 0u );
			C3D_API sdw::Vec3 computeLPVRadiance( sdw::Vec3 wsPosition
				, sdw::Vec3 wsNormal
				, LpvGridData lpvGridData );
			C3D_API sdw::Vec3 computeLLPVRadiance( sdw::Vec3 wsPosition
				, sdw::Vec3 wsNormal
				, LayeredLpvGridData llpvGridData );
			C3D_API sdw::Vec3 computeDiffuse( SceneFlags sceneFlags
				, sdw::Vec3 wsPosition
				, sdw::Vec3 wsNormal
				, sdw::Vec3 diffuse
				, sdw::Vec3 allButAmbient
				, sdw::Vec3 ambient
				, sdw::Float occlusion );
			C3D_API sdw::Vec3 computeSpecular( SceneFlags sceneFlags
				, sdw::Vec3 wsCamera
				, sdw::Vec3 wsPosition
				, sdw::Vec3 wsNormal
				, sdw::Float roughness
				, sdw::Vec3 f0
				, sdw::Vec3 specular
				, sdw::Float occlusion );

		private:
			sdw::ShaderWriter & m_writer;
			Utils & m_utils;
			sdw::Function< sdw::Vec4
				, sdw::InVec3 > m_evalSH;
			sdw::Function< sdw::Vec3
				, sdw::InVec3
				, sdw::InVec3
				, InLpvGridData > m_computeLPVRadiance;
			sdw::Function< sdw::Vec3
				, sdw::InVec3
				, sdw::InVec3
				, InLayeredLpvGridData > m_computeLLPVRadiance;
		};

	}
}

#endif
