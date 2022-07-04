/*
See LICENSE file in root folder
*/
#ifndef ___C3DAS_AtmosphereConfig_H___
#define ___C3DAS_AtmosphereConfig_H___

#include "AtmosphereCameraUbo.hpp"
#include "AtmosphereScatteringUbo.hpp"
#include "AtmosphereWeatherUbo.hpp"

#include <Castor3D/Shader/Ubos/MatrixUbo.hpp>

#include <ShaderWriter/BaseTypes/Array.hpp>
#include <ShaderWriter/BaseTypes/Int.hpp>
#include <ShaderWriter/BaseTypes/CombinedImage.hpp>
#include <ShaderWriter/CompositeTypes/Function.hpp>
#include <ShaderWriter/CompositeTypes/StructHelper.hpp>
#include <ShaderWriter/MatTypes/Mat4.hpp>
#include <ShaderWriter/VecTypes/Vec4.hpp>

#include <tuple>

namespace atmosphere_scattering
{
	struct CloudsModel
	{
		CloudsModel( sdw::ShaderWriter & writer
			, castor3d::shader::Utils & utils
			, AtmosphereModel & atmosphere
			, ScatteringModel & scattering
			, CameraData const & cameraData
			, AtmosphereData const & atmosphereData
			, WeatherData const & weatherData
			, uint32_t binding );
		sdw::Void applyClouds( sdw::IVec2 const & fragCoord
			, sdw::Vec2 const & targetSize
			, sdw::CombinedImage2DRgba32 const & transmittance
			, sdw::Vec4 & bg
			, sdw::Vec4 & emission
			, sdw::Vec4 & distance );

	private:
		sdw::Vec2 getUVProjection( sdw::Vec3 const & p );
		sdw::Float getHeightFraction( sdw::Vec3 const & inPos );
		sdw::RetFloat getDensityForCloud( sdw::Float const & heightFraction
			, sdw::Float const & cloudType );
		sdw::RetFloat sampleCloudDensity( sdw::Vec3 const & p
			, sdw::Boolean const & expensive
			, sdw::Float const & lod );
		sdw::RetFloat raymarchToLight( sdw::Vec3 const & o
			, sdw::Float const & stepSize
			, sdw::Vec3 const & lightDir
			, sdw::Float const & originalDensity
			, sdw::Float const & lightDotEye );
		sdw::RetVec4 raymarchToCloud( sdw::Vec3 const & startPos
			, sdw::Vec3 const & endPos
			, sdw::Vec3 const & bg
			, sdw::IVec2 const & fragCoord
			, sdw::Vec3 const & sunColor
			, sdw::Vec4 & cloudPos );
		sdw::RetFloat computeFogAmount( sdw::Vec3 const & startPos
			, sdw::Float const & factor );
		sdw::RetBoolean raySphereintersectSkyMap( sdw::Vec3 const & rd
			, sdw::Float const & radius
			, sdw::Vec3 & startPos );
		sdw::RetBoolean raySphereIntersect( sdw::Vec3 const & r0
			, sdw::Vec3 const & rd
			, sdw::Float const & radius
			, sdw::Vec3 & startPos );

	private:
		sdw::ShaderWriter & writer;
		castor3d::shader::Utils & utils;
		AtmosphereModel & atmosphere;
		ScatteringModel & scattering;
		CameraData const & cameraData;
		AtmosphereData const & atmosphereData;
		WeatherData const & weatherData;
		sdw::CombinedImage3DRgba32 perlinWorleyNoiseMap;
		sdw::CombinedImage3DRgba32 worleyNoiseMap;
		sdw::CombinedImage2DRg32 weatherMap;
		sdw::Float sphereInnerRadius;
		sdw::Float sphereOuterRadius;
		sdw::Float sphereDelta;
		sdw::Vec3 sphereCenter;

	private:
		sdw::Function< sdw::Float
			, sdw::InFloat
			, sdw::InFloat > m_getDensityForCloud;
		sdw::Function< sdw::Float
			, sdw::InVec3
			, sdw::InBoolean
			, sdw::InFloat > m_sampleCloudDensity;
		sdw::Function< sdw::Float
			, sdw::InVec3
			, sdw::InFloat
			, sdw::InVec3
			, sdw::InFloat
			, sdw::InFloat > m_raymarchToLight;
		sdw::Function< sdw::Vec4
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InIVec2
			, sdw::InVec3
			, sdw::OutVec4 > m_raymarchToCloud;
		sdw::Function< sdw::Float
			, sdw::InVec3
			, sdw::InFloat > m_computeFogAmount;
		sdw::Function< sdw::Boolean
			, sdw::InVec3
			, sdw::InFloat
			, sdw::OutVec3 > m_raySphereintersectSkyMap;
		sdw::Function< sdw::Boolean
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InFloat
			, sdw::OutVec3 > m_raySphereIntersect;
		sdw::Function< sdw::Void
			, sdw::InIVec2
			, sdw::InVec2
			, sdw::InCombinedImage2DRgba32
			, sdw::InOutVec4
			, sdw::OutVec4
			, sdw::OutVec4 > m_applyClouds;
	};
}

#endif
