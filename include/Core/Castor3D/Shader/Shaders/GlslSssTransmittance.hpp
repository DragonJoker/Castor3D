/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslSssTransmittance_H___
#define ___C3D_GlslSssTransmittance_H___

#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"

#include <ShaderWriter/Intrinsics/Intrinsics.hpp>

namespace castor3d
{
	namespace shader
	{
		class SssTransmittance
		{
		public:
			C3D_API SssTransmittance( sdw::ShaderWriter & writer
				, Shadow & shadow
				, Utils & utils
				, ShadowOptions shadowOptions
				, SssProfiles const & sssProfiles );
			C3D_API void declare();

			C3D_API sdw::Vec3 compute( shader::LightMaterial const & material
				, shader::DirectionalLight const & light
				, Surface const & surface );
			C3D_API sdw::Vec3 compute( shader::LightMaterial const & material
				, shader::PointLight const & light
				, Surface const & surface );
			C3D_API sdw::Vec3 compute( shader::LightMaterial const & material
				, shader::SpotLight const & light
				, Surface const & surface );

		private:
			sdw::ShaderWriter & m_writer;
			Shadow & m_shadow;
			Utils & m_utils;
			SssProfiles const & m_sssProfiles;

			bool m_shadowMap;

			sdw::Function < sdw::Vec3
				, sdw::InFloat
				, sdw::InFloat
				, sdw::InUInt
				, sdw::InVec3
				, sdw::InFloat
				, sdw::InVec3
				, sdw::InFloat > m_compute;
		};
	}
}

#endif
