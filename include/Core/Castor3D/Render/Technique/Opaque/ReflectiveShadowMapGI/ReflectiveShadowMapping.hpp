/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ReflectiveShadowMapping_HPP___
#define ___C3D_ReflectiveShadowMapping_HPP___

#include "ReflectiveShadowMapGIModule.hpp"

#include "Castor3D/Shader/Shaders/GlslLight.hpp"

#include <ShaderAST/Shader.hpp>

namespace castor3d
{
	class ReflectiveShadowMapping
	{
	public:
		C3D_API ReflectiveShadowMapping( sdw::ShaderWriter & writer
			, sdw::ArraySsboT< sdw::Vec2 > & rsmSamples
			, LightType lightType );
		C3D_API sdw::Vec3 directional( shader::DirectionalLight const & light
			, sdw::Vec3 const & viewPosition
			, sdw::Vec3 const & worldPosition
			, sdw::Vec3 const & worldNormal
			, sdw::Float const & rsmRMax
			, sdw::Float const & rsmIntensity
			, sdw::UInt const & rsmSampleCount );
		C3D_API sdw::Vec3 point( shader::PointLight const & light
			, sdw::Vec3 const & worldPosition
			, sdw::Vec3 const & worldNormal
			, sdw::Float const & rsmRMax
			, sdw::Float const & rsmIntensity
			, sdw::UInt const & rsmSampleCount
			, sdw::UInt const & rsmIndex );
		C3D_API sdw::Vec3 spot( shader::SpotLight const & light
			, sdw::Vec3 const & worldPosition
			, sdw::Vec3 const & worldNormal
			, sdw::Float const & rsmRMax
			, sdw::Float const & rsmIntensity
			, sdw::UInt const & rsmSampleCount
			, sdw::UInt const & rsmIndex );

	private:
		void doDeclareDirectional();
		void doDeclarePoint();
		void doDeclareSpot();

	private:
		sdw::ShaderWriter & m_writer;
		sdw::ArraySsboT< sdw::Vec2 > & m_rsmSamples;
		sdw::Function< sdw::Vec3
			, shader::InDirectionalLight
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InUInt > m_directional;
		sdw::Function< sdw::Vec3
			, shader::InPointLight
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InUInt
			, sdw::InUInt > m_point;
		sdw::Function< sdw::Vec3
			, shader::InSpotLight
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InUInt
			, sdw::InUInt > m_spot;
	};
}

#endif
