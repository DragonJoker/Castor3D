/*
See LICENSE file in root folder
*/
#ifndef ___C3DAS_ScatteringModel_H___
#define ___C3DAS_ScatteringModel_H___

#include "AtmosphereModel.hpp"

namespace atmosphere_scattering
{
	struct ScatteringModel
	{
		ScatteringModel( sdw::ShaderWriter & writer
			, AtmosphereModel & atmosphere
			, bool colorTransmittance
			, bool fastSky
			, bool fastAerialPerspective
			, bool renderSunDisk
			, bool bloomSunDisk
			, uint32_t & binding
			, uint32_t set
			, bool needsMultiscatter = true );
		sdw::RetVec3 getSunLuminance( Ray const & ray );
		sdw::RetVec3 getSunRadiance( sdw::Vec3 const & sunDir );
		sdw::Float aerialPerspectiveDepthToSlice( sdw::Float const & depth );
		sdw::Void getPixelTransLum( sdw::Vec2 const & fragPos
			, sdw::Vec2 const & fragSize
			, sdw::Float const & fragDepth
			, sdw::Vec4 & transmittance
			, sdw::Vec4 & luminance );
		sdw::Void getPixelTransLum( sdw::Vec2 const & fragPos
			, sdw::Vec2 const & fragSize
			, sdw::Float const & fragDepth
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
			, sdw::Vec3 & L
			, sdw::Vec4 & luminance );
		void doRenderFastAerial( sdw::Vec2 const & fragPos
			, sdw::Vec2 const & fragSize
			, sdw::Float const & fragDepth
			, sdw::Vec3 const & worldPos
			, sdw::Vec3 & L
			, sdw::Vec4 & luminance );
		void doRegisterOutputs( SingleScatteringResult const & ss
			, sdw::Vec3 & L
			, sdw::Vec4 & luminance
			, sdw::Vec4 & transmittance );
		sdw::Float doGetSunAngle( sdw::Vec3 const & sunDir
			, sdw::Float const & minAngle
			, sdw::Float const & maxAngle )const;

	private:
		sdw::ShaderWriter & m_writer;
		AtmosphereModel & m_atmosphere;
		bool m_colorTransmittance;
		bool m_fastSky;
		bool m_fastAerialPerspective;
		bool m_renderSunDisk;
		bool m_bloomSunDisk;
		sdw::CombinedImage2DRgba32 transmittanceMap;
		sdw::CombinedImage2DRgba32 multiScatterMap;
		sdw::CombinedImage2DRgba32 skyViewMap;
		sdw::CombinedImage3DRgba32 volumeMap;

		sdw::Function< sdw::Vec3
			, InRay > m_getSunLuminance;
		sdw::Function< sdw::Void
			, sdw::InVec2
			, sdw::InVec2
			, sdw::InFloat
			, sdw::OutVec4
			, sdw::OutVec4 > m_getPixelTransLum;
		sdw::Function< sdw::Void
			, sdw::InVec2
			, sdw::InVec2
			, sdw::InFloat
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
