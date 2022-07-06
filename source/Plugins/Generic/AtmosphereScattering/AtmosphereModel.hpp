/*
See LICENSE file in root folder
*/
#ifndef ___C3DAS_AtmosphereModel_H___
#define ___C3DAS_AtmosphereModel_H___

#include "AtmosphereCameraUbo.hpp"
#include "AtmosphereScatteringUbo.hpp"

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
	struct SingleScatteringResult
		: public sdw::StructInstanceHelperT< "SingleScatteringResult"
			, sdw::type::MemoryLayout::eC
			, sdw::Vec3Field< "luminance" >
			, sdw::Vec3Field< "opticalDepth" >
			, sdw::Vec3Field< "transmittance" >
			, sdw::Vec3Field< "multiScatAs1" >
			, sdw::Vec3Field< "newMultiScatStep0Out" >
			, sdw::Vec3Field< "newMultiScatStep1Out" > >
	{
		SingleScatteringResult( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstanceHelperT{ writer, std::move( expr ), enabled }
		{
		}

		// Scattered light (luminance)
		auto luminance()const { return getMember< "luminance" >(); }
		// Optical depth (1/m)
		auto opticalDepth()const { return getMember< "opticalDepth" >(); }
		// Transmittance in [0,1] (unitless)
		auto transmittance()const { return getMember< "transmittance" >(); }
		auto multiScatAs1()const { return getMember< "multiScatAs1" >(); }
		auto newMultiScatStep0Out()const { return getMember< "newMultiScatStep0Out" >(); }
		auto newMultiScatStep1Out()const { return getMember< "newMultiScatStep1Out" >(); }
	};
	Writer_Parameter( SingleScatteringResult );

	struct MediumSampleRGB
		: public sdw::StructInstanceHelperT< "MediumSampleRGB"
			, sdw::type::MemoryLayout::eC
			, sdw::Vec3Field< "scattering" >
			, sdw::Vec3Field< "absorption" >
			, sdw::Vec3Field< "extinction" >
			, sdw::Vec3Field< "scatteringMie" >
			, sdw::Vec3Field< "absorptionMie" >
			, sdw::Vec3Field< "extinctionMie" >
			, sdw::Vec3Field< "scatteringRay" >
			, sdw::Vec3Field< "absorptionRay" >
			, sdw::Vec3Field< "extinctionRay" >
			, sdw::Vec3Field< "scatteringOzo" >
			, sdw::Vec3Field< "absorptionOzo" >
			, sdw::Vec3Field< "extinctionOzo" >
			, sdw::Vec3Field< "albedo" > >
	{
		MediumSampleRGB( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstanceHelperT{ writer, std::move( expr ), enabled }
		{
		}

		auto scattering()const { return getMember< "scattering" >(); }
		auto absorption()const { return getMember< "absorption" >(); }
		auto extinction()const { return getMember< "extinction" >(); }
		auto scatteringMie()const { return getMember< "scatteringMie" >(); }
		auto absorptionMie()const { return getMember< "absorptionMie" >(); }
		auto extinctionMie()const { return getMember< "extinctionMie" >(); }
		auto scatteringRay()const { return getMember< "scatteringRay" >(); }
		auto absorptionRay()const { return getMember< "absorptionRay" >(); }
		auto extinctionRay()const { return getMember< "extinctionRay" >(); }
		auto scatteringOzo()const { return getMember< "scatteringOzo" >(); }
		auto absorptionOzo()const { return getMember< "absorptionOzo" >(); }
		auto extinctionOzo()const { return getMember< "extinctionOzo" >(); }
		auto albedo()const { return getMember< "albedo" >(); }
	};
	Writer_Parameter( MediumSampleRGB );

	struct Intersection
		: public sdw::StructInstanceHelperT < "Intersection"
		, sdw::type::MemoryLayout::eC
		, sdw::Vec3Field< "point" >
		, sdw::BooleanField< "valid" >
		, sdw::FloatField< "t" > >
	{
		Intersection( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstanceHelperT{ writer, std::move( expr ), enabled }
		{
		}

		auto point()const { return getMember< "point" >(); }
		auto valid()const { return getMember< "valid" >(); }
		auto t()const { return getMember< "t" >(); }
	};
	Writer_Parameter( Intersection );

	struct Ray
		: public sdw::StructInstanceHelperT < "Ray"
		, sdw::type::MemoryLayout::eC
		, sdw::Vec3Field< "origin" >
		, sdw::Vec3Field< "direction" > >
	{
		Ray( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstanceHelperT{ writer, std::move( expr ), enabled }
			, origin{ getMember< "origin" >() }
			, direction{ getMember< "direction" >() }
		{
		}

		sdw::Vec3 step( sdw::Float const & t )const
		{
			return origin + t * direction;
		}

		sdw::Vec3 origin;
		sdw::Vec3 direction;
	};
	Writer_Parameter( Ray );

	struct LuminanceSettings
	{
		CameraData const * cameraData{};
		bool useGround{};
		bool useDepthBuffer{};
		bool variableSampleCount{};
		bool mieRayPhase{};

		bool illuminanceIsOne{};
		bool multiScatApproxEnabled{};
		bool shadowMapEnabled{};
		uint32_t multiScatteringPowerSerie{ 1u };
	};

	struct AtmosphereModel
	{
		AtmosphereModel( sdw::ShaderWriter & writer
			, castor3d::shader::Utils & utils
			, AtmosphereData const & atmosphereData
			, LuminanceSettings luminanceSettings );
		AtmosphereModel( sdw::ShaderWriter & writer
			, castor3d::shader::Utils & utils
			, AtmosphereData const & atmosphereData
			, LuminanceSettings luminanceSettings
			, VkExtent2D transmittanceExtent );

		auto getSunDirection()const
		{
			return atmosphereData.sunDirection;
		}

		auto getEarthRadius()const
		{
			return atmosphereData.bottomRadius;
		}

		auto getAtmosphereRadius()const
		{
			return atmosphereData.topRadius;
		}

		auto getCameraPosition()const
		{
			return luminanceSettings.cameraData->position();
		}

		auto camProjToWorld( sdw::Vec4 const & clipSpace )const
		{
			return luminanceSettings.cameraData->camProjToWorld( clipSpace );
		}

		auto camProjToView( sdw::Vec4 const & clipSpace )const
		{
			return luminanceSettings.cameraData->camInvProj() * clipSpace;
		}

		auto camViewToWorld( sdw::Vec4 const & viewSpace )const
		{
			return luminanceSettings.cameraData->camInvView()  * viewSpace;
		}

		void setTransmittanceMap( sdw::CombinedImage2DRgba32 const & value )
		{
			transmittanceTexture = &value;
		}

		void setMultiscatterMap( sdw::CombinedImage2DRgba32 const & value )
		{
			multiScatTexture = &value;
		}

		sdw::Vec3 getCameraPositionFromEarth()const;
		RetRay castRay( sdw::Vec2 const & uv );
		RetRay castRay( sdw::Vec2 const & screenPoint
			, sdw::Vec2 const & screenSize );
		sdw::RetVec3 getMultipleScattering( sdw::Vec3 const & scattering
			, sdw::Vec3 const & extinction
			, sdw::Vec3 const & worldPos
			, sdw::Float const & viewZenithCosAngle );
		RetSingleScatteringResult integrateScatteredLuminance( sdw::Vec2 const & pixPos
			, Ray const & ray
			, sdw::Vec3 const & sunDir
			, sdw::Float const & sampleCountIni
			, sdw::Float const & depthBufferValue
			, castor3d::shader::Light const & light
			, sdw::Vec3 const & surfaceWorldNormal
			, sdw::Mat4 const & lightMatrix
			, sdw::UInt const & cascadeIndex
			, sdw::UInt const & maxCascade
			, sdw::Float const & tMaxMax = sdw::Float{ 9000000.0_f } );
		RetSingleScatteringResult integrateScatteredLuminanceShadow( sdw::Vec2 const & pixPos
			, Ray const & ray
			, sdw::Vec3 const & sunDir
			, sdw::Float const & sampleCountIni
			, sdw::Float const & depthBufferValue
			, castor3d::shader::Light const & light
			, sdw::Vec3 const & surfaceWorldNormal
			, sdw::Mat4 const & lightMatrix
			, sdw::UInt const & cascadeIndex
			, sdw::UInt const & maxCascade
			, sdw::Float const & tMaxMax = sdw::Float{ 9000000.0_f } );
		RetSingleScatteringResult integrateScatteredLuminanceNoShadow( sdw::Vec2 const & pixPos
			, Ray const & ray
			, sdw::Vec3 const & sunDir
			, sdw::Float const & sampleCountIni
			, sdw::Float const & depthBufferValue
			, sdw::Float const & tMaxMax = sdw::Float{ 9000000.0_f } );
		sdw::RetBoolean moveToTopAtmosphere( Ray & ray );
		sdw::RetVec3 getSunRadiance( sdw::Vec3 const & cameraPosition
			, sdw::Vec3 const & sunDir
			, sdw::CombinedImage2DRgba32 const & transmittanceMap );

		// - Returns distance from rayOrigin to first intersecion with sphere,
		//   or -1.0 if no intersection.
		RetIntersection raySphereIntersectNearest( Ray const & ray
			, sdw::Vec3 const & sphereCenter
			, sdw::Float const & sphereRadius );
		RetIntersection raySphereIntersectNearest( Ray const & ray
			, sdw::Float const & sphereRadius );

		// Reference implementation (i.e. not schlick approximation). 
		// See http://www.pbr-book.org/3ed-2018/Volume_Scattering/Phase_Functions.html
		sdw::RetFloat hgPhase( sdw::Float const & g
			, sdw::Float const & cosTheta );

		RetMediumSampleRGB sampleMediumRGB( sdw::Vec3 const & worldPos );
		sdw::Float rayleighPhase( sdw::Float const & cosTheta );
		// We should precompute those terms from resolutions (Or set resolution as #defined constants)
		sdw::Float fromUnitToSubUvs( sdw::Float u, sdw::Float resolution );
		sdw::Float fromSubUvsToUnit( sdw::Float u, sdw::Float resolution );

		sdw::RetVec3 getWorldPos( sdw::Float const & depth
			, sdw::Vec2 const & pixPos
			, sdw::Vec2 const & texSize );

		////////////////////////////////////////////////////////////
		// LUT functions
		////////////////////////////////////////////////////////////

		// Transmittance LUT function parameterisation from Bruneton 2017 https://github.com/ebruneton/precomputed_atmospheric_scattering
		// uv in [0,1]
		// viewZenithCosAngle in [-1,1]
		// viewHeight in [bottomRAdius, topRadius]
		sdw::Void uvToLutTransmittanceParams( sdw::Float & viewHeight
			, sdw::Float & viewZenithCosAngle
			, sdw::Vec2 const & uv );
		sdw::Void lutTransmittanceParamsToUv( sdw::Float const & viewHeight
			, sdw::Float const & viewZenithCosAngle
			, sdw::Vec2 & uv );
		sdw::Void uvToSkyViewLutParams( sdw::Float & viewZenithCosAngle
			, sdw::Float & lightViewCosAngle
			, sdw::Float const & viewHeight
			, sdw::Vec2 const & uv
			, sdw::Vec2 const & size );
		sdw::Void skyViewLutParamsToUv( sdw::Boolean const & intersectGround
			, sdw::Float const & viewZenithCosAngle
			, sdw::Float const & lightViewCosAngle
			, sdw::Float const & viewHeight
			, sdw::Vec2 & uv
			, sdw::Vec2 const & size );

	private:
		void doInitRay( sdw::Float const & depthBufferValue
			, sdw::Vec2 const & pixPos
			, Ray const & ray
			, sdw::Float const & tMaxMax
			, sdw::Vec3 const & earthO
			, Intersection const & tBottom
			, SingleScatteringResult const & result
			, sdw::Float & tMax );
		sdw::Float doInitSampleCount( sdw::Float const & tMax
			, sdw::Float & sampleCount
			, sdw::Float & sampleCountFloor
			, sdw::Float & tMaxFloor );
		std::pair< sdw::Float, sdw::Float > doInitPhaseFunctions( sdw::Vec3 const & sunDir
			, sdw::Vec3 const & worldDir );
		void doStepRay( sdw::Float const & s
			, sdw::Float const & sampleCount
			, sdw::Float const & sampleCountFloor
			, sdw::Float const & tMaxFloor
			, sdw::Float const & tMax
			, sdw::Float const & sampleSegmentT
			, sdw::Float & t
			, sdw::Float & dt );
		std::tuple< sdw::Vec3, sdw::Float, sdw::Vec2, sdw::Vec3, sdw::Vec3 > doGetSunTransmittance( Ray const & rayToSun
			, MediumSampleRGB const & medium
			, sdw::Float const & dt
			, sdw::Vec3 & opticalDepth );
		std::tuple< sdw::Vec3, sdw::Float, sdw::Vec3 > doGetScatteredLuminance( Ray const & rayToSun
			, MediumSampleRGB const & medium
			, sdw::Vec3 const & earthO
			, sdw::Vec3 const & upVector
			, sdw::Float const & sunZenithCosAngle
			, sdw::Float const & miePhaseValue
			, sdw::Float const & rayleighPhaseValue );
		void doComputeStep( MediumSampleRGB const & medium
			, sdw::Float const & dt
			, sdw::Vec3 const & sampleTransmittance
			, sdw::Float const & earthShadow
			, sdw::Vec3 const & transmittanceToSun
			, sdw::Vec3 const & multiScatteredLuminance
			, sdw::Vec3 const & S
			, sdw::Float const & t
			, sdw::Float & tPrev
			, sdw::Vec3 & throughput
			, sdw::Vec3 & L
			, SingleScatteringResult & result );
		void doProcessGround( sdw::Vec3 const & sunDir
			, sdw::Float const & tMax
			, Intersection const & tBottom
			, sdw::Vec3 const & globalL
			, sdw::Vec3 const & throughput
			, sdw::Vec3 & L );

	private:
		sdw::ShaderWriter & writer;
		castor3d::shader::Utils & utils;

	public:
		AtmosphereData const & atmosphereData;
		LuminanceSettings luminanceSettings{};
		VkExtent2D transmittanceExtent{};
		sdw::CombinedImage2DRgba32 const * transmittanceTexture{};
		sdw::CombinedImage2DRgba32 const * multiScatTexture{};
		std::shared_ptr< castor3d::shader::Shadow > shadows;

	private:
		sdw::Function< Ray
			, sdw::InVec2 > m_castRay;
		sdw::Function< SingleScatteringResult
			, sdw::InVec2
			, InRay
			, sdw::InVec3
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InFloat > m_integrateScatteredLuminance;
		sdw::Function< SingleScatteringResult
			, sdw::InVec2
			, InRay
			, sdw::InVec3
			, sdw::InFloat
			, sdw::InFloat
			, castor3d::shader::InLight
			, sdw::InVec3
			, sdw::InMat4
			, sdw::InUInt
			, sdw::InUInt
			, sdw::InFloat > m_integrateScatteredLuminanceShadow;
		sdw::Function< sdw::Boolean
			, InOutRay > m_moveToTopAtmosphere;
		sdw::Function< Intersection
			, InRay
			, sdw::InVec3
			, sdw::InFloat > m_raySphereIntersectNearest;
		sdw::Function< sdw::Float
			, sdw::InFloat
			, sdw::InFloat > m_hgPhase;
		sdw::Function< MediumSampleRGB
			, sdw::InVec3 > m_sampleMediumRGB;
		sdw::Function< sdw::Void
			, sdw::OutFloat
			, sdw::OutFloat
			, sdw::InVec2 > m_uvToLutTransmittanceParams;
		sdw::Function< sdw::Void
			, sdw::InFloat
			, sdw::InFloat
			, sdw::OutVec2 > m_lutTransmittanceParamsToUv;
		sdw::Function< sdw::Void
			, sdw::OutFloat
			, sdw::OutFloat
			, sdw::InFloat
			, sdw::InVec2
			, sdw::InVec2 > m_uvToSkyViewLutParams;
		sdw::Function< sdw::Void
			, sdw::InBoolean
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InFloat
			, sdw::OutVec2
			, sdw::InVec2 > m_skyViewLutParamsToUv;
		sdw::Function< sdw::Vec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InFloat > m_getMultipleScattering;
		sdw::Function< sdw::Vec3
			, sdw::InFloat
			, sdw::InVec2
			, sdw::InVec2 > m_getWorldPos;
		sdw::Function< sdw::Vec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InCombinedImage2DRgba32 > m_getSunRadiance;
	};
}

#endif
