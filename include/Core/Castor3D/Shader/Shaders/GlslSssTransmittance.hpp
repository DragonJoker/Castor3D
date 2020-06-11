/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslSssTransmittance_H___
#define ___C3D_GlslSssTransmittance_H___

#include "Castor3D/Shader/Shaders/GlslLighting.hpp"

#include <ShaderWriter/Intrinsics/Intrinsics.hpp>

namespace castor3d
{
	namespace shader
	{
		class SssTransmittance
		{
		public:
			C3D_API SssTransmittance( sdw::ShaderWriter & writer
				, Shadow const & shadow
				, Utils const & utils
				, bool shadowMap );
			C3D_API void declare( LightType type );

			C3D_API sdw::Vec3 compute( shader::BaseMaterial const & material
				, shader::DirectionalLight const & light
				, sdw::Vec2 const & uv
				, sdw::Vec3 const & position
				, sdw::Vec3 const & normal
				, sdw::Float const & translucency )const;
			C3D_API sdw::Vec3 compute( shader::BaseMaterial const & material
				, shader::PointLight const & light
				, sdw::Vec2 const & uv
				, sdw::Vec3 const & position
				, sdw::Vec3 const & normal
				, sdw::Float const & translucency )const;
			C3D_API sdw::Vec3 compute( shader::BaseMaterial const & material
				, shader::SpotLight const & light
				, sdw::Vec2 const & uv
				, sdw::Vec3 const & position
				, sdw::Vec3 const & normal
				, sdw::Float const & translucency )const;

		private:
			sdw::ShaderWriter & m_writer;
			Shadow const & m_shadow;
			Utils const & m_utils;

			bool m_shadowMap;

			sdw::Function < sdw::Vec3
				, sdw::InFloat
				, sdw::InInt
				, sdw::InVec4Array
				, sdw::InFloat
				, sdw::InVec3
				, sdw::InVec3
				, sdw::InFloat
				, sdw::InVec3
				, sdw::InFloat
				, sdw::InVec3 > m_compute;
		};
	}
}

#endif
