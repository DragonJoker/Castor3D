/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ReflectiveShadowMapping_HPP___
#define ___C3D_ReflectiveShadowMapping_HPP___

#include "ReflectiveShadowMapsModule.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include "Castor3D/Shader/Shaders/GlslLight.hpp"

#include <ShaderWriter/CompositeTypes/ArrayStorageBuffer.hpp>

namespace castor3d
{
	class ReflectiveShadowMapping
	{
	public:
		C3D_API ReflectiveShadowMapping( sdw::ShaderWriter & writer
			, sdw::ArrayStorageBufferT< sdw::Vec4 > & rsmSamples );
		C3D_API sdw::Vec3 directional( shader::DirectionalShadowData const & shadowData
			, sdw::Vec3 const & viewPosition
			, sdw::Vec3 const & worldPosition
			, sdw::Vec3 const & worldNormal
			, shader::RsmConfigData const & rsmData );
		C3D_API sdw::Vec3 point( shader::PointShadowData const & shadowData
			, sdw::Vec3 const & lightPosition
			, sdw::Vec3 const & worldPosition
			, sdw::Vec3 const & worldNormal
			, shader::RsmConfigData const & rsmData );
		C3D_API sdw::Vec3 spot( shader::SpotShadowData const & shadowData
			, sdw::Vec3 const & worldPosition
			, sdw::Vec3 const & worldNormal
			, shader::RsmConfigData const & rsmData );

	private:
		sdw::ShaderWriter & m_writer;
		sdw::ArrayStorageBufferT< sdw::Vec4 > & m_rsmSamples;
		sdw::Function< sdw::Vec3
			, shader::InDirectionalShadowData
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InVec3
			, shader::InRsmConfigData > m_directional;
		sdw::Function< sdw::Vec3
			, shader::InPointShadowData
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InVec3
			, shader::InRsmConfigData > m_point;
		sdw::Function< sdw::Vec3
			, shader::InSpotShadowData
			, sdw::InVec3
			, sdw::InVec3
			, shader::InRsmConfigData > m_spot;
	};
}

#endif
