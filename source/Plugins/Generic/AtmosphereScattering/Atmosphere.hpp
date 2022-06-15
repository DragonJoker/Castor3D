/*
See LICENSE file in root folder
*/
#ifndef ___C3DAS_AtmosphereConfig_H___
#define ___C3DAS_AtmosphereConfig_H___

#include "AtmosphereScatteringUbo.hpp"

#include <Castor3D/Shader/Ubos/MatrixUbo.hpp>

#include <ShaderWriter/BaseTypes/Array.hpp>
#include <ShaderWriter/BaseTypes/Int.hpp>
#include <ShaderWriter/BaseTypes/CombinedImage.hpp>
#include <ShaderWriter/CompositeTypes/Function.hpp>
#include <ShaderWriter/CompositeTypes/StructInstance.hpp>
#include <ShaderWriter/MatTypes/Mat4.hpp>
#include <ShaderWriter/VecTypes/Vec4.hpp>

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

	struct AtmosphereConfig
	{
		struct LuminanceSettings
		{
			bool useGround{};
			bool useDepthBuffer{};
			bool variableSampleCount{};
			bool mieRayPhase{};

			bool illuminanceIsOne{};
			bool multiScatApproxEnabled{};
			bool shadowMapEnabled{};
			uint32_t multiScatteringPowerSerie{ 1u };
		};

		AtmosphereConfig( sdw::ShaderWriter & writer
			, AtmosphereData const & atmosphereData
			, castor3d::shader::MatrixData const & matrixData
			, LuminanceSettings luminanceSettings );
		AtmosphereConfig( sdw::ShaderWriter & writer
			, AtmosphereData const & atmosphereData
			, castor3d::shader::MatrixData const & matrixData
			, LuminanceSettings luminanceSettings
			, VkExtent2D transmittanceExtent
			, sdw::CombinedImage2DRgba32 const * transmittanceLut );

		SingleScatteringResult integrateScatteredLuminance( sdw::Vec2 const & pixPos
			, sdw::Vec3 const & worldPos
			, sdw::Vec3 const & worldDir
			, sdw::Vec3 const & sunDir
			, sdw::Float const & sampleCountIni
			, sdw::Float const & depthBufferValue
			, sdw::Float const & tMaxMax = sdw::Float{ 9000000.0_f } );
		sdw::Boolean moveToTopAtmosphere( sdw::Vec3 & worldPos
				, sdw::Vec3 const & worldDir );

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

	private:
		sdw::ShaderWriter & writer;
		AtmosphereData const & atmosphereData;
		castor3d::shader::MatrixData const & matrixData;
		LuminanceSettings luminanceSettings{};
		VkExtent2D transmittanceExtent{};
		sdw::CombinedImage2DRgba32 const * transmittanceTexture{};
		sdw::CombinedImage2DRgba32 const * multiScatTexture{};
		sdw::CombinedImage2DShadowR32 const * shadowMapTexture{};
		sdw::Mat4 const * shadowmapViewProjMat;

	private:
		sdw::Function< SingleScatteringResult
			, sdw::InVec2
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InFloat > m_integrateScatteredLuminance;
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
		sdw::Function< sdw::Vec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InFloat > m_getMultipleScattering;
		sdw::Function< sdw::Float
			, sdw::InVec3 > m_getShadow;
	};
}

#endif
