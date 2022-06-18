/*
See LICENSE file in root folder
*/
#ifndef ___C3DAS_ScatteringConfig_H___
#define ___C3DAS_ScatteringConfig_H___

#include "Atmosphere.hpp"
namespace atmosphere_scattering
{
	struct ScatteringConfig
	{
		ScatteringConfig( sdw::ShaderWriter & writer
			, AtmosphereConfig & atmosphereConfig
			, CameraData const & cameraData
			, AtmosphereData const & atmosphereData
			, bool colorTransmittance
			, bool fastSky
			, bool fastAerialPerspective
			, bool renderSunDisk );
		sdw::Vec3 getSunLuminance( sdw::Vec3 const & worldPos
			, sdw::Vec3 const & worldDir
			, sdw::CombinedImage2DRgba32 const & transmittanceMap );
		sdw::Float aerialPerspectiveDepthToSlice( sdw::Float const & depth );
		sdw::Void getPixelTransLum( sdw::Vec2 const & fragPos
			, sdw::Vec2 const & fragSize
			, sdw::Float const & fragDepth
			, sdw::CombinedImage2DRgba32 const & transmittanceMap
			, sdw::CombinedImage2DRgba32 const & skyViewMap
			, sdw::CombinedImage3DRgba32 const & volumMap
			, sdw::Vec4 & transmittance
			, sdw::Vec4 & luminance );

	private:
		sdw::ShaderWriter & m_writer;
		AtmosphereConfig & m_atmosphereConfig;
		CameraData const & m_cameraData;
		AtmosphereData const & m_atmosphereData;
		bool m_colorTransmittance;
		bool m_fastSky;
		bool m_fastAerialPerspective;
		bool m_renderSunDisk;

		sdw::Function< sdw::Vec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InCombinedImage2DRgba32 > m_getValFromTLUT;
		sdw::Function< sdw::Vec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InCombinedImage2DRgba32 > m_getSunLuminance;
		sdw::Function< sdw::Float
			, sdw::InFloat > m_aerialPerspectiveDepthToSlice;
		sdw::Function< sdw::Void
			, sdw::InVec2
			, sdw::InVec2
			, sdw::InFloat
			, sdw::InCombinedImage2DRgba32
			, sdw::InCombinedImage2DRgba32
			, sdw::InCombinedImage3DRgba32
			, sdw::OutVec4
			, sdw::OutVec4 > m_getPixelTransLum;
	};
}

#endif
