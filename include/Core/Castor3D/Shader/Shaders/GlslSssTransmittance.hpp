/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslSssTransmittance_H___
#define ___C3D_GlslSssTransmittance_H___

#include "Castor3D/Shader/Shaders/SdwModule.hpp"

#include <ShaderWriter/BaseTypes/Float.hpp>
#include <ShaderWriter/BaseTypes/UInt.hpp>
#include <ShaderWriter/VecTypes/Vec3.hpp>

namespace c3d::shader
{
	class SssTransmittance
	{
	public:
		C3D_API SssTransmittance( sdw::ShaderWriter & writer
			, Shadow & shadows
			, ShadowOptions shadowOptions
			, SssProfiles const & sssProfiles
			, sdw::CombinedImage1DArrayRgba16 const & sssDiffusionProfiles );

		C3D_API sdw::Vec3 compute( DebugOutputCategory const & debugOutput
			, BlendComponents const & components
			, shader::DirectionalLight const & light
			, DirectionalShadowData const & shadow
			, LightSurface const & lightSurface
			, sdw::Vec3 const & lightRadiance );
		C3D_API sdw::Vec3 compute( DebugOutputCategory const & debugOutput
			, BlendComponents const & components
			, shader::PointLight const & light
			, PointShadowData const & shadow
			, LightSurface const & lightSurface
			, sdw::Vec3 const & lightRadiance );
		C3D_API sdw::Vec3 compute( DebugOutputCategory const & debugOutput
			, BlendComponents const & components
			, shader::SpotLight const & light
			, SpotShadowData const & shadow
			, LightSurface const & lightSurface
			, sdw::Vec3 const & lightRadiance );

	private:
		sdw::Vec3 doCompute( DebugOutputCategory const & debugOutput
			, sdw::Float const & lightSpaceDepth
			, sdw::Float const & shadowDepth
			, sdw::UInt const & sssProfileIndex
			, sdw::Vec3 const & worldNormal
			, sdw::Float const & transmittanceFactor
			, sdw::Vec3 const & lightToVertex );

		sdw::ShaderWriter & m_writer;
		Shadow & m_shadows;
		SssProfiles const & m_sssProfiles;
		sdw::CombinedImage1DArrayRgba16 const & m_sssDiffusionProfiles;

		SceneFlags m_shadowsType;

		sdw::Function< sdw::Vec3
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InUInt
			, sdw::InVec3
			, sdw::InFloat
			, sdw::InVec3
			, sdw::InFloat > m_compute;
		sdw::Function< sdw::Vec3
			, InShadowData
			, sdw::InUInt
			, sdw::InFloat
			, InDirectionalLight
			, sdw::InMat4
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InVec3 > m_computeDirectional;
		sdw::Function< sdw::Vec3
			, InShadowData
			, sdw::InUInt
			, sdw::InFloat
			, InPointLight
			, sdw::InInt
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InVec3 > m_computePoint;
		sdw::Function< sdw::Vec3
			, InShadowData
			, sdw::InUInt
			, sdw::InFloat
			, InSpotLight
			, sdw::InMat4
			, sdw::InInt
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InVec3 > m_computeSpot;
	};
}

#endif
