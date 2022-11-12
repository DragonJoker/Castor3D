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
		struct Settings
		{
			Settings()
			{
			}

			Settings & setColorTransmittance( bool v )
			{
				colorTransmittance = v;
				return *this;
			}

			Settings & setFastSky( bool v )
			{
				fastSky = v;
				return *this;
			}

			Settings & setFastAerialPerspective( bool v )
			{
				fastAerialPerspective = v;
				return *this;
			}

			Settings & setRenderSunDisk( bool v )
			{
				renderSunDisk = v;
				return *this;
			}

			Settings & setBloomSunDisk( bool v )
			{
				bloomSunDisk = v;
				return *this;
			}

			Settings & setNeedsMultiscatter( bool v )
			{
				needsMultiscatter = v;
				return *this;
			}

			bool colorTransmittance{};
			bool fastSky{};
			bool fastAerialPerspective{};
			bool renderSunDisk{};
			bool bloomSunDisk{};
			bool needsMultiscatter{};
		};

		ScatteringModel( sdw::ShaderWriter & writer
			, AtmosphereModel & atmosphere
			, Settings settings
			, uint32_t & binding
			, uint32_t set );
		sdw::RetVec3 getSunLuminance( Ray const & ray );
		sdw::RetVec3 getSunRadiance( sdw::Vec3 const & sunDir );
		sdw::Float aerialPerspectiveDepthToSlice( sdw::Float const & depth );
		castor3d::shader::RetRay getPixelTransLum( sdw::Vec2 const & fragPos
			, sdw::Vec2 const & fragSize
			, sdw::Float const & fragDepth
			, sdw::Vec4 & transmittance
			, sdw::Vec4 & luminance );
		sdw::Vec4 rescaleLuminance( sdw::Vec4 const & luminance );

	private:
		void doRenderSky( sdw::Vec2 const & fragSize
			, sdw::Float const & fragDepth
			, Ray const & ray
			, sdw::Float const & viewHeight
			, sdw::Vec3 & L
			, sdw::Vec4 & luminance );
		sdw::Boolean doRenderFastAerial( sdw::Vec2 const & fragPos
			, sdw::Vec2 const & fragSize
			, sdw::Float const & fragDepth
			, sdw::Vec3 const & worldPos
			, sdw::Float const & viewHeight
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
		Settings m_settings;
		sdw::CombinedImage2DRgba32 transmittanceMap;
		sdw::CombinedImage2DRgba32 multiScatterMap;
		sdw::CombinedImage2DRgba32 skyViewMap;
		sdw::CombinedImage3DRgba32 volumeMap;

		sdw::Function< sdw::Vec3
			, InRay > m_getSunLuminance;
		sdw::Function< castor3d::shader::Ray
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
