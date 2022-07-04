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
			, bool renderSunDisk
			, bool bloomSunDisk );
		ScatteringConfig( sdw::ShaderWriter & writer
			, AtmosphereConfig & atmosphereConfig
			, CameraData const & cameraData
			, AtmosphereData const & atmosphereData
			, WeatherData const & weatherData
			, bool colorTransmittance
			, bool fastSky
			, bool fastAerialPerspective
			, bool renderSunDisk
			, bool bloomSunDisk );
		sdw::Vec3 getSunLuminance( Ray const & ray
			, sdw::CombinedImage2DRgba32 const & transmittanceMap );
		sdw::Float aerialPerspectiveDepthToSlice( sdw::Float const & depth );
		sdw::Void getPixelTransLum( sdw::Vec2 const & fragPos
			, sdw::Vec2 const & fragSize
			, sdw::Float const & fragDepth
			, sdw::CombinedImage2DRgba32 const & transmittanceMap
			, sdw::CombinedImage2DRgba32 const & skyViewMap
			, sdw::CombinedImage3DRgba32 const & volumeMap
			, sdw::Vec4 & transmittance
			, sdw::Vec4 & luminance );
		sdw::Void getPixelTransLum( sdw::Vec2 const & fragPos
			, sdw::Vec2 const & fragSize
			, sdw::Float const & fragDepth
			, sdw::CombinedImage2DRgba32 const & transmittanceMap
			, sdw::CombinedImage2DRgba32 const & skyViewMap
			, sdw::CombinedImage3DRgba32 const & volumeMap
			, castor3d::shader::Light const & light
			, sdw::Vec3 const & surfaceWorldNormal
			, sdw::Mat4 const & lightMatrix
			, sdw::UInt const & cascadeIndex
			, sdw::UInt const & maxCascade
			, sdw::Vec4 & transmittance
			, sdw::Vec4 & luminance );
		sdw::Vec4 rescaleLuminance( sdw::Vec4 const & luminance );

	private:
		void doRenderSky( sdw::Vec2 const & fragSize
			, sdw::Float const & fragDepth
			, Ray const & ray
			, sdw::CombinedImage2DRgba32 const & transmittanceMap
			, sdw::CombinedImage2DRgba32 const & skyViewMap
			, sdw::Vec3 & L
			, sdw::Vec4 & luminance );
		void doRenderFastAerial( sdw::Vec2 const & fragPos
			, sdw::Vec2 const & fragSize
			, sdw::Float const & fragDepth
			, sdw::Vec3 const & worldPos
			, sdw::CombinedImage3DRgba32 const & volumeMap
			, sdw::Vec3 & L
			, sdw::Vec4 & luminance );
		void doRegisterOutputs( SingleScatteringResult const & ss
			, sdw::Vec3 & L
			, sdw::Vec4 & luminance
			, sdw::Vec4 & transmittance );

	private:
		sdw::ShaderWriter & m_writer;
		AtmosphereConfig & m_atmosphereConfig;
		CameraData const & m_cameraData;
		AtmosphereData const & m_atmosphereData;
		WeatherData const * m_weatherData{};
		bool m_colorTransmittance;
		bool m_fastSky;
		bool m_fastAerialPerspective;
		bool m_renderSunDisk;
		bool m_bloomSunDisk;

		sdw::Function< sdw::Vec3
			, InRay
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
		sdw::Function< sdw::Void
			, sdw::InVec2
			, sdw::InVec2
			, sdw::InFloat
			, sdw::InCombinedImage2DRgba32
			, sdw::InCombinedImage2DRgba32
			, sdw::InCombinedImage3DRgba32
			, castor3d::shader::InLight
			, sdw::InVec3
			, sdw::InMat4
			, sdw::InUInt
			, sdw::InUInt
			, sdw::OutVec4
			, sdw::OutVec4 > m_getPixelTransLumShadow;
	};
}

#endif
