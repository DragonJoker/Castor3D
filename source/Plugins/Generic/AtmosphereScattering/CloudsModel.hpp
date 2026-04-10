/*
See LICENSE file in root folder
*/
#ifndef ___C3DAS_CloudsModel_H___
#define ___C3DAS_CloudsModel_H___

#include "AtmosphereCameraUbo.hpp"
#include "AtmosphereModel.hpp"
#include "CloudsUbo.hpp"

#include <Castor3D/Shader/Shaders/GlslRay.hpp>

#include <ShaderWriter/CompositeTypes/Function.hpp>

namespace atmosphere_scattering
{
	struct CloudsTraverseData
		: public sdw::StructInstanceHelperT< "CloudsTraverseData"
			, sdw::type::MemoryLayout::eC
			, sdw::Vec3Field< "sunLuminance" >
			, sdw::FloatField< "volumesTransmittance" >
			, sdw::I32Vec2Field< "pixelCoord" >
			, sdw::FloatField< "fogAmount" >
			, sdw::FloatField< "accumDensity" >
			, sdw::Vec3Field< "currentPos" >
			, sdw::FloatField< "planetShadow" >
			, sdw::Vec3Field< "stepVector" > >
	{
		SDW_DeclStructInstance( , CloudsTraverseData );

		CloudsTraverseData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstanceHelperT{ writer, c3d::move( expr ), enabled }
			, sunLuminance{ getMember< "sunLuminance" >() }
			, volumesTransmittance{ getMember< "volumesTransmittance" >() }
			, pixelCoord{ getMember< "pixelCoord" >() }
			, fogAmount{ getMember< "fogAmount" >() }
			, accumDensity{ getMember< "accumDensity" >() }
			, currentPos{ getMember< "currentPos" >() }
			, planetShadow{ getMember< "planetShadow" >() }
			, stepVector{ getMember< "stepVector" >() }
		{
		}

		explicit CloudsTraverseData( sdw::ShaderWriter & writer );

		void initialise( AtmosphereModel const & atmosphere, ScatteringModel & scattering
			, sdw::I32Vec2 const & pixelCoord, c3ds::Ray const & ray );

		static sdw::expr::ExprList getZeroInit();

		sdw::Vec3 sunLuminance;
		sdw::Float volumesTransmittance;
		sdw::I32Vec2 pixelCoord;
		sdw::Float fogAmount;
		sdw::Float accumDensity;
		sdw::Vec3 currentPos;
		sdw::Float planetShadow;
		sdw::Vec3 stepVector;
	};

	struct CloudsModel
	{
		CloudsModel( sdw::ShaderWriter & writer
			, c3d::shader::Utils & utils
			, AtmosphereModel & atmosphere
			, ScatteringModel & scattering
			, CloudsData const & clouds
			, uint32_t & binding
			, uint32_t set );

		sdw::RetFloat computeFogAmount( sdw::Vec3 const & startPos
			, sdw::Vec3 const & wolrdPos
			, sdw::Float const & factor
			, sdw::Float const & viewHeight );
		sdw::RetFloat getHeightFraction( sdw::Vec3 const & inPos );
		sdw::RetFloat sampleCloudDensity( sdw::Vec3 const & samplePoint
			, sdw::Boolean const & expensive
			, sdw::Float const & heightFraction
			, sdw::Float const & lod );
		sdw::RetFloat raymarchToLight( sdw::Vec3 const & viewDir
			, sdw::Vec3 const & pos
			, sdw::Float const & stepSize
			, sdw::Vec3 const & lightDir );
		sdw::RetVec3 integrateInscatter( sdw::Vec3 const & luminance
			, sdw::Float const & transmittance
			, sdw::Float const & stepTransmittance
			, sdw::Float const & sigmaS );

		sdw::RetVec4 applyClouds( c3d::shader::Ray const & ray
			, sdw::Float const & objectId
			, sdw::Float const & linearDepth
			, sdw::IVec2 const & fragCoord
			, sdw::Vec3 const & sunLuminance
			, sdw::Vec3 const & skyLuminance
			, sdw::Float const & skyBlendFactor );

		auto getCloudsDensity()const
		{
			return clouds.density();
		}

		auto getCloudsCoverage()const
		{
			return clouds.coverage();
		}

		auto getCloudsBottomColour()const
		{
			return clouds.bottomColor();
		}

		auto getCloudsTopColour()const
		{
			return clouds.bottomColor();
		}

		auto & getAtmosphere()const
		{
			return atmosphere;
		}

		auto & getScattering()const
		{
			return scattering;
		}

		auto & getUtils()const
		{
			return utils;
		}

	private:
		static sdw::Vec2 getSphericalProjection( sdw::Vec3 const & p );
		sdw::RetVec3 skewSamplePointWithWind( sdw::Vec3 const & point
			, sdw::Float const & heightFraction );
		sdw::RetFloat getRelativeHeightInAtmosphere( sdw::Vec3 const & pos
			, sdw::Vec3 const & startPos
			, Ray const & ray );
		sdw::RetFloat getDensityHeightGradientForPoint( sdw::Float const & heightFraction
			, sdw::Float const & cloudType );
		sdw::RetFloat sampleLowFrequency( sdw::Vec2 const & unskewedUV
			, sdw::Vec2 const & skewedUV
			, sdw::Float const & heightFraction
			, sdw::Float const & lod );
		sdw::RetFloat erodeWithHighFrequency( sdw::Float const & baseDensity
			, sdw::Vec3 const & skewedSamplePoint
			, sdw::Vec2 const & skewedUV
			, sdw::Float const & heightFraction
			, sdw::Float const & lod );
		sdw::RetVec4 raymarchToCloud( sdw::Vec3 const & startPos
			, sdw::Vec3 const & endPos
			, sdw::IVec2 const & fragCoord
			, sdw::Vec3 const & sunColor
			, sdw::Float const & planetShadow );
		sdw::RetFloat henyeyGreenstein( sdw::Float const & g
			, sdw::Float const & cosTheta );
		sdw::Float henyeyGreenstein( sdw::Float const & g
			, sdw::Float const & cosTheta
			, sdw::Float const & silverIntensity
			, sdw::Float const & silverSpread );
		sdw::RetFloat henyeyGreensteinPhase( sdw::Float const & g
			, sdw::Float const & cosTheta );
		sdw::RetVec4 computeLighting( Ray const & ray
			, sdw::Vec3 const & sunRadiance
			, sdw::Vec3 const & sunLuminance
			, sdw::Vec3 const & skyLuminance
			, sdw::Float const & fadeOut
			, sdw::Float const & planetShadow
			, sdw::Vec4 const & rayMarchResult );
		sdw::Float getLightEnergy( sdw::Float const & cosTheta
			, sdw::Float const & coneDensity );

	private:
		sdw::ShaderWriter & writer;
		c3d::shader::Utils & utils;
		AtmosphereModel & atmosphere;
		ScatteringModel & scattering;
		CloudsData const & clouds;
		sdw::CombinedImage3DRgba8Unorm perlinWorleyNoiseMap;
		sdw::CombinedImage3DRgba8Unorm worleyNoiseMap;
		sdw::CombinedImage2DRg8Unorm curlNoiseMap;
		sdw::CombinedImage2DRg32 weatherMap;

	public:
		sdw::Float cloudsInnerRadius;
		sdw::Float cloudsOuterRadius;
		sdw::Float cloudsThickness;

	private:
		sdw::Function< sdw::Float
			, sdw::InVec3 > m_getHeightFraction;
		sdw::Function< sdw::Float
			, sdw::InVec3, sdw::InVec3, InRay > m_getRelativeHeightInAtmosphere;
		sdw::Function< sdw::Float
			, sdw::InFloat, sdw::InFloat > m_getDensityHeightGradientForPoint;
		sdw::Function< sdw::Vec3
			, sdw::InVec3, sdw::InFloat > m_skewSamplePointWithWind;
		sdw::Function< sdw::Float
			, sdw::InVec2, sdw::InVec2, sdw::InFloat, sdw::InFloat > m_sampleLowFrequency;
		sdw::Function< sdw::Float
			, sdw::InFloat, sdw::InVec3, sdw::InVec2, sdw::InFloat, sdw::InFloat > m_erodeWithHighFrequency;
		sdw::Function< sdw::Float
			, sdw::InVec3, sdw::InBoolean, sdw::InFloat, sdw::InFloat > m_sampleCloudDensity;
		sdw::Function< sdw::Float
			, sdw::InVec3, sdw::InVec3, sdw::InFloat, sdw::InVec3 > m_raymarchToLight;
		sdw::Function< sdw::Vec4
			, sdw::InVec3, sdw::InVec3, sdw::InIVec2, sdw::InVec3, sdw::OutFloat > m_raymarchToCloud;
		sdw::Function< sdw::Float
			, sdw::InVec3, sdw::InVec3, sdw::InFloat, sdw::InFloat > m_computeFogAmount;
		sdw::Function< sdw::Float
			, sdw::InFloat, sdw::InFloat > m_henyeyGreenstein;
		sdw::Function< sdw::Float
			, sdw::InFloat, sdw::InFloat > m_henyeyGreensteinPhase;
		sdw::Function< sdw::Vec4
			, InRay, sdw::InVec3, sdw::InVec3, sdw::InOutVec3, sdw::InFloat, sdw::InFloat, sdw::InVec4 > m_computeLighting;
		sdw::Function< sdw::Vec4
			, c3d::shader::InRay, sdw::InFloat, sdw::InFloat, sdw::InIVec2, sdw::InOutVec3, sdw::InOutVec3, sdw::OutFloat > m_applyClouds;
		sdw::Function< sdw::Vec3
			, sdw::InVec3, sdw::InFloat, sdw::InFloat, sdw::InFloat > m_integrateInscatter;
	};
}

#endif
