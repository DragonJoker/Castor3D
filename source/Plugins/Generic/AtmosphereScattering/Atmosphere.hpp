/*
See LICENSE file in root folder
*/
#ifndef ___C3DAS_AtmosphereConfig_H___
#define ___C3DAS_AtmosphereConfig_H___

#include "AtmosphereCameraUbo.hpp"
#include "AtmosphereScatteringUbo.hpp"

#include <Castor3D/Shader/Ubos/MatrixUbo.hpp>

#include <ShaderWriter/BaseTypes/Array.hpp>
#include <ShaderWriter/BaseTypes/Int.hpp>
#include <ShaderWriter/BaseTypes/CombinedImage.hpp>
#include <ShaderWriter/CompositeTypes/Function.hpp>
#include <ShaderWriter/CompositeTypes/StructInstance.hpp>
#include <ShaderWriter/MatTypes/Mat4.hpp>
#include <ShaderWriter/VecTypes/Vec4.hpp>

#include <tuple>

namespace atmosphere_scattering
{
	struct SingleScatteringResult
		: public sdw::StructInstance
	{
		SingleScatteringResult( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled );

		SDW_DeclStructInstance( , SingleScatteringResult );

		static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache );

		// Scattered light (luminance)
		sdw::Vec3 luminance;
		// Optical depth (1/m)
		sdw::Vec3 opticalDepth;
		// Transmittance in [0,1] (unitless)
		sdw::Vec3 transmittance;
		sdw::Vec3 multiScatAs1;

		sdw::Vec3 newMultiScatStep0Out;
		sdw::Vec3 newMultiScatStep1Out;

	private:
		using sdw::StructInstance::getMember;
		using sdw::StructInstance::getMemberArray;
	};

	struct MediumSampleRGB
		: public sdw::StructInstance
	{
		MediumSampleRGB( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled );

		SDW_DeclStructInstance( , MediumSampleRGB );

		static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache );

		sdw::Vec3 scattering;
		sdw::Vec3 absorption;
		sdw::Vec3 extinction;

		sdw::Vec3 scatteringMie;
		sdw::Vec3 absorptionMie;
		sdw::Vec3 extinctionMie;

		sdw::Vec3 scatteringRay;
		sdw::Vec3 absorptionRay;
		sdw::Vec3 extinctionRay;

		sdw::Vec3 scatteringOzo;
		sdw::Vec3 absorptionOzo;
		sdw::Vec3 extinctionOzo;

		sdw::Vec3 albedo;

	private:
		using sdw::StructInstance::getMember;
		using sdw::StructInstance::getMemberArray;
	};

	struct LuminanceSettings
	{
		bool useGround{};
		CameraData const * cameraData;
		bool variableSampleCount{};
		bool mieRayPhase{};

		bool illuminanceIsOne{};
		bool multiScatApproxEnabled{};
		bool shadowMapEnabled{};
		uint32_t multiScatteringPowerSerie{ 1u };
	};

	struct AtmosphereConfig
	{
		AtmosphereConfig( sdw::ShaderWriter & writer
			, AtmosphereData const & atmosphereData
			, LuminanceSettings luminanceSettings );
		AtmosphereConfig( sdw::ShaderWriter & writer
			, AtmosphereData const & atmosphereData
			, LuminanceSettings luminanceSettings
			, VkExtent2D transmittanceExtent
			, sdw::CombinedImage2DRgba32 const * transmittanceLut );

		sdw::Vec3 getClipSpace( sdw::Vec2 const & fragPos
			, sdw::Vec2 const & fragSize
			, sdw::Float const & fragDepth );
		sdw::Vec3 getClipSpace( sdw::Vec2 const & uv
			, sdw::Float const & fragDepth );
		sdw::Vec3 getMultipleScattering( sdw::Vec3 const & scattering
			, sdw::Vec3 const & extinction
			, sdw::Vec3 const & worldPos
			, sdw::Float const & viewZenithCosAngle );
		SingleScatteringResult integrateScatteredLuminance( sdw::Vec2 const & pixPos
			, sdw::Vec3 const & worldPos
			, sdw::Vec3 const & worldDir
			, sdw::Vec3 const & sunDir
			, sdw::Float const & sampleCountIni
			, sdw::Float const & depthBufferValue
			, castor3d::shader::Light const & light
			, sdw::Vec3 const & surfaceWorldNormal
			, sdw::Mat4 const & lightMatrix
			, sdw::UInt const & cascadeIndex
			, sdw::UInt const & maxCascade
			, sdw::Float const & tMaxMax = sdw::Float{ 9000000.0_f } );
		SingleScatteringResult integrateScatteredLuminanceShadow( sdw::Vec2 const & pixPos
			, sdw::Vec3 const & worldPos
			, sdw::Vec3 const & worldDir
			, sdw::Vec3 const & sunDir
			, sdw::Float const & sampleCountIni
			, sdw::Float const & depthBufferValue
			, castor3d::shader::Light const & light
			, sdw::Vec3 const & surfaceWorldNormal
			, sdw::Mat4 const & lightMatrix
			, sdw::UInt const & cascadeIndex
			, sdw::UInt const & maxCascade
			, sdw::Float const & tMaxMax = sdw::Float{ 9000000.0_f } );
		SingleScatteringResult integrateScatteredLuminanceNoShadow( sdw::Vec2 const & pixPos
			, sdw::Vec3 const & worldPos
			, sdw::Vec3 const & worldDir
			, sdw::Vec3 const & sunDir
			, sdw::Float const & sampleCountIni
			, sdw::Float const & depthBufferValue
			, sdw::Float const & tMaxMax = sdw::Float{ 9000000.0_f } );
		sdw::Boolean moveToTopAtmosphere( sdw::Vec3 & worldPos
				, sdw::Vec3 const & worldDir );
		sdw::Vec3 getSunRadiance( sdw::Vec3 const & cameraPosition
			, sdw::Vec3 const & sunDir
			, sdw::CombinedImage2DRgba32 const & transmittanceMap );

		// - r0: ray origin
		// - rd: normalized ray direction
		// - s0: sphere center
		// - sR: sphere radius
		// - Returns distance from r0 to first intersecion with sphere,
		//   or -1.0 if no intersection.
		sdw::Float raySphereIntersectNearest( sdw::Vec3 const & r0
			, sdw::Vec3 const & rd
			, sdw::Vec3 const & s0
			, sdw::Float const & sR );

		// Reference implementation (i.e. not schlick approximation). 
		// See http://www.pbr-book.org/3ed-2018/Volume_Scattering/Phase_Functions.html
		sdw::Float hgPhase( sdw::Float const & g
			, sdw::Float const & cosTheta );

		MediumSampleRGB sampleMediumRGB( sdw::Vec3 const & worldPos );
		sdw::Float rayleighPhase( sdw::Float const & cosTheta );
		// We should precompute those terms from resolutions (Or set resolution as #defined constants)
		sdw::Float fromUnitToSubUvs( sdw::Float u, sdw::Float resolution );
		sdw::Float fromSubUvsToUnit( sdw::Float u, sdw::Float resolution );

		sdw::Vec3 getWorldPos( sdw::Float const & depth
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
			, sdw::Vec3 const & worldPos
			, sdw::Vec3 const & worldDir
			, sdw::Float const & tMaxMax
			, sdw::Vec3 const & earthO
			, sdw::Float const & tBottom
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
		std::tuple< sdw::Vec3, sdw::Float, sdw::Vec2, sdw::Vec3, sdw::Vec3 > doGetSunTransmittance( sdw::Vec3 const & sunDir
			, MediumSampleRGB const & medium
			, sdw::Vec3 const & P
			, sdw::Float const & dt
			, sdw::Vec3 & opticalDepth );
		std::tuple< sdw::Vec3, sdw::Float, sdw::Vec3 > doGetScatteredLuminance( sdw::Vec3 const & sunDir
			, MediumSampleRGB const & medium
			, sdw::Vec3 const & P
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
		void doProcessGround( sdw::Vec3 const & worldPos
			, sdw::Vec3 const & worldDir
			, sdw::Vec3 const & sunDir
			, sdw::Float const & tMax
			, sdw::Float const & tBottom
			, sdw::Vec3 const & globalL
			, sdw::Vec3 const & throughput
			, sdw::Vec3 & L );

	private:
		sdw::ShaderWriter & writer;

	public:
		AtmosphereData const & atmosphereData;
		LuminanceSettings luminanceSettings{};
		VkExtent2D transmittanceExtent{};
		sdw::CombinedImage2DRgba32 const * transmittanceTexture{};
		sdw::CombinedImage2DRgba32 const * multiScatTexture{};
		std::shared_ptr< castor3d::shader::Shadow > shadows;

	private:
		sdw::Function< SingleScatteringResult
			, sdw::InVec2
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InFloat > m_integrateScatteredLuminance;
		sdw::Function< SingleScatteringResult
			, sdw::InVec2
			, sdw::InVec3
			, sdw::InVec3
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
			, sdw::InOutVec3
			, sdw::InVec3 > m_moveToTopAtmosphere;
		sdw::Function< sdw::Float
			, sdw::InVec3
			, sdw::InVec3
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
