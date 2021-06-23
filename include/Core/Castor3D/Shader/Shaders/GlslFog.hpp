/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslFog_H___
#define ___C3D_GlslFog_H___

#include "SdwModule.hpp"
#include "Castor3D/Scene/SceneModule.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include <ShaderWriter/Intrinsics/Intrinsics.hpp>

namespace castor3d
{
	namespace shader
	{
		class Fog
		{
		public:
			C3D_API Fog( FogType flags
				, sdw::ShaderWriter & writer );
			C3D_API sdw::Vec4 apply( sdw::Vec4 const & bgColour
				, sdw::Vec4 const & colour
				, sdw::Float const & dist
				, sdw::Float const & y
				, SceneData const & sceneData );

		protected:
			sdw::Function< sdw::Vec4
				, sdw::InVec4
				, sdw::InVec4
				, sdw::InFloat
				, sdw::InFloat
				, InSceneData > m_fog;
		};

		class CommonFog
		{
		public:
			C3D_API explicit CommonFog( sdw::ShaderWriter & writer );
			C3D_API sdw::Vec4 apply( sdw::Vec4 const & bgColour
				, sdw::Vec4 const & colour
				, sdw::Float const & dist
				, sdw::Float const & y
				, SceneData const & sceneData );

		protected:
			sdw::Function< sdw::Vec4
				, sdw::InVec4
				, sdw::InVec4
				, sdw::InFloat
				, sdw::InFloat
				, InSceneData > m_fog;
		};
	}
}

#endif
