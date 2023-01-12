/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslSssTransmittance_H___
#define ___C3D_GlslSssTransmittance_H___

#include "Castor3D/Shader/Shaders/SdwModule.hpp"

#include <ShaderWriter/BaseTypes/Float.hpp>
#include <ShaderWriter/BaseTypes/UInt.hpp>
#include <ShaderWriter/VecTypes/Vec3.hpp>

namespace castor3d
{
	namespace shader
	{
		class SssTransmittance
		{
		public:
			C3D_API SssTransmittance( sdw::ShaderWriter & writer
				, ShadowOptions shadowOptions
				, SssProfiles const & sssProfiles );

			C3D_API sdw::Vec3 compute( BlendComponents const & components
				, shader::DirectionalLight const & light
				, LightSurface const & lightSurface );
			C3D_API sdw::Vec3 compute( BlendComponents const & components
				, shader::PointLight const & light
				, LightSurface const & lightSurface );
			C3D_API sdw::Vec3 compute( BlendComponents const & components
				, shader::SpotLight const & light
				, LightSurface const & lightSurface );

		private:
			void doDeclare();

			sdw::ShaderWriter & m_writer;
			SssProfiles const & m_sssProfiles;

			bool m_shadowMap;

			sdw::Function< sdw::Vec3
				, sdw::InFloat
				, sdw::InFloat
				, sdw::InUInt
				, sdw::InVec3
				, sdw::InFloat
				, sdw::InVec3
				, sdw::InFloat > m_compute;
			sdw::Function < sdw::Vec3
				, sdw::InUInt
				, sdw::InFloat
				, InDirectionalLight
				, sdw::InVec3
				, sdw::InVec3 > m_computeDirectional;
			sdw::Function < sdw::Vec3
				, sdw::InUInt
				, sdw::InFloat
				, InPointLight
				, sdw::InVec3
				, sdw::InVec3 > m_computePoint;
			sdw::Function < sdw::Vec3
				, sdw::InUInt
				, sdw::InFloat
				, InSpotLight
				, sdw::InVec3
				, sdw::InVec3 > m_computeSpot;
		};
	}
}

#endif
