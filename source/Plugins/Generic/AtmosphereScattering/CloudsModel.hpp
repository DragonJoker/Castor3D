/*
See LICENSE file in root folder
*/
#ifndef ___C3DAS_AtmosphereConfig_H___
#define ___C3DAS_AtmosphereConfig_H___

#include "AtmosphereCameraUbo.hpp"
#include "AtmosphereModel.hpp"
#include "AtmosphereScatteringUbo.hpp"
#include "CloudsUbo.hpp"

#include <Castor3D/Shader/Shaders/SdwModule.hpp>
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
			, CloudsData const & clouds
			, uint32_t binding );
		sdw::Void applyClouds( sdw::IVec2 const & fragCoord
			, sdw::Vec2 const & targetSize
			, sdw::Vec4 & bg
			, sdw::Vec4 & emission );

	private:
		sdw::Vec2 getUVProjection( sdw::Vec3 const & p );
		sdw::Float getHeightFraction( sdw::Vec3 const & inPos );
		sdw::RetFloat getRelativeHeightInAtmosphere( sdw::Vec3 const & pos
			, sdw::Vec3 const & startPos
			, Ray const & ray );
		sdw::RetFloat getDensityHeightGradientForPoint( sdw::Float const & heightFraction
			, sdw::Float const & cloudType );
		sdw::RetVec3 skewSamplePointWithWind( sdw::Vec3 const & point
			, sdw::Float const & heightFraction );
		sdw::RetFloat sampleLowFrequency( sdw::Vec3 const & skewedSamplePoint
			, sdw::Vec3 const & unskewedSamplePoint
			, sdw::Float const & heightFraction
			, sdw::Float const & lod );
		sdw::RetFloat erodeWithHighFrequency( sdw::Float const & baseDensity
			, sdw::Vec3 const & skewedSamplePoint
			, sdw::Float const & heightFraction
			, sdw::Float const & lod );
		sdw::RetFloat sampleCloudDensity( sdw::Vec3 const & samplePoint
			, sdw::Boolean const & expensive
			, sdw::Float const & heightFraction
			, sdw::Float const & lod );
		sdw::RetFloat raymarchToLight( Ray const & ray
			, sdw::Vec3 const & o
			, sdw::Float const & stepSize
			, sdw::Vec3 const & lightDir
			, sdw::Float const & originalDensity
			, sdw::Float const & lightDotEye
			, sdw::Float & densityAlongLight );
		sdw::RetVec3 raymarchToCloud( Ray const & ray
			, sdw::Vec3 const & startPos
			, sdw::Vec3 const & endPos
			, sdw::Vec3 const & bg
			, sdw::IVec2 const & fragCoord
			, sdw::Vec3 const & sunColor
			, sdw::Float & accumDensity
			, sdw::Float & maxEarthShadow );
		sdw::RetFloat computeFogAmount( sdw::Vec3 const & startPos
			, sdw::Vec3 const & wolrdPos
			, sdw::Float const & factor );
		RetIntersection raySphereintersectSkyMap( sdw::Vec3 const & rd
			, sdw::Float const & radius );
		sdw::RetFloat henyeyGreenstein( sdw::Float const & g
			, sdw::Float const & cosTheta );
		sdw::Float henyeyGreenstein( sdw::Float const & g
			, sdw::Float const & cosTheta
			, sdw::Float const & silverIntensity
			, sdw::Float const & silverSpread );
		sdw::RetVec4 computeLighting( Ray const & ray
			, sdw::Vec3 const & bg
			, sdw::Vec3 const & startPos
			, sdw::Vec3 const & endPos
			, sdw::Vec3 const & sunColor
			, sdw::Float const & fogAmount
			, sdw::Float const & maxEarthShadow
			, sdw::Float const & cloudAlphaness
			, sdw::Vec3 const & rayMarchResult
			, sdw::Float const & accumDensity );
		sdw::RetVec4 computeEmission( Ray const & ray
			, sdw::Vec3 const & startPos
			, sdw::Vec3 const & sunColor
			, sdw::Float const & cloudAlphaness
			, sdw::Float const & maxEarthShadow );

	private:
		sdw::ShaderWriter & writer;
		castor3d::shader::Utils & utils;
		AtmosphereModel & atmosphere;
		ScatteringModel & scattering;
		CloudsData const & clouds;
		sdw::CombinedImage3DRgba32 perlinWorleyNoiseMap;
		sdw::CombinedImage3DRgba32 worleyNoiseMap;
		sdw::CombinedImage2DRg32 curlNoiseMap;
		sdw::CombinedImage2DRg32 weatherMap;
		sdw::Float cloudsInnerRadius;
		sdw::Float cloudsOuterRadius;
		sdw::Float cloudsThickness;

	private:
		sdw::Function< sdw::Float
			, sdw::InVec3
			, sdw::InVec3
			, InRay > m_getRelativeHeightInAtmosphere;
		sdw::Function< sdw::Float
			, sdw::InFloat
			, sdw::InFloat > m_getDensityHeightGradientForPoint;
		sdw::Function< sdw::Vec3
			, sdw::InVec3
			, sdw::InFloat > m_skewSamplePointWithWind;
		sdw::Function< sdw::Float
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InFloat
			, sdw::InFloat > m_sampleLowFrequency;
		sdw::Function< sdw::Float
			, sdw::InFloat
			, sdw::InVec3
			, sdw::InFloat
			, sdw::InFloat > m_erodeWithHighFrequency;
		sdw::Function< sdw::Float
			, sdw::InVec3
			, sdw::InBoolean
			, sdw::InFloat
			, sdw::InFloat > m_sampleCloudDensity;
		sdw::Function< sdw::Float
			, InRay
			, sdw::InVec3
			, sdw::InFloat
			, sdw::InVec3
			, sdw::InFloat
			, sdw::InFloat
			, sdw::OutFloat > m_raymarchToLight;
		sdw::Function< sdw::Vec3
			, InRay
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InIVec2
			, sdw::InVec3
			, sdw::OutFloat
			, sdw::OutFloat > m_raymarchToCloud;
		sdw::Function< sdw::Float
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InFloat > m_computeFogAmount;
		sdw::Function< Intersection
			, sdw::InVec3
			, sdw::InFloat > m_raySphereintersectSkyMap;
		sdw::Function< sdw::Float
			, sdw::InFloat
			, sdw::InFloat > m_henyeyGreenstein;
		sdw::Function< sdw::Vec4
			, InRay
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InVec3
			, sdw::InFloat > m_computeLighting;
		sdw::Function< sdw::Vec4
			, InRay
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InFloat
			, sdw::InFloat > m_computeEmission;
		sdw::Function< sdw::Void
			, sdw::InIVec2
			, sdw::InVec2
			, sdw::InOutVec4
			, sdw::OutVec4 > m_applyClouds;
	};
}

#endif
