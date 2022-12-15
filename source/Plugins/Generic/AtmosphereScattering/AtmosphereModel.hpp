/*
See LICENSE file in root folder
*/
#ifndef ___C3DAS_AtmosphereModel_H___
#define ___C3DAS_AtmosphereModel_H___

#include "AtmosphereCameraUbo.hpp"
#include "AtmosphereScatteringUbo.hpp"

#include <Castor3D/Shader/Shaders/GlslRay.hpp>
#include <Castor3D/Shader/Ubos/MatrixUbo.hpp>

#include <CastorUtils/Math/Length.hpp>

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
	using castor3d::shader::Intersection;
	using castor3d::shader::RetIntersection;
	using castor3d::shader::InIntersection;
	using castor3d::shader::InOutIntersection;
	using castor3d::shader::OutIntersection;
	using castor3d::shader::Ray;
	using castor3d::shader::RetRay;
	using castor3d::shader::InRay;
	using castor3d::shader::InOutRay;
	using castor3d::shader::OutRay;

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

		explicit SingleScatteringResult( sdw::ShaderWriter & writer );

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

	struct AtmosphereModel
	{
		struct Settings
		{
			explicit Settings( castor::Length l )
				: length{ std::move( l ) }
			{
			}

			Settings & setCameraData( CameraData const * v )
			{
				cameraData = v;
				return *this;
			}

			Settings & setUseGround( bool v )
			{
				useGround = v;
				return *this;
			}

			Settings & setVariableSampleCount( bool v )
			{
				variableSampleCount = v;
				return *this;
			}

			Settings & setMieRayPhase( bool v )
			{
				mieRayPhase = v;
				return *this;
			}

			Settings & setIlluminanceIsOne( bool v )
			{
				illuminanceIsOne = v;
				return *this;
			}

			Settings & setMultiScatApprox( bool v )
			{
				multiScatApproxEnabled = v;
				return *this;
			}

			Settings & setShadowMap( bool v )
			{
				shadowMapEnabled = v;
				return *this;
			}

			Settings & setMultiScatteringPowerSerie( bool v )
			{
				multiScatteringPowerSerie = v;
				return *this;
			}

			castor::Length length;
			CameraData const * cameraData{};
			bool useGround{};
			bool variableSampleCount{};
			bool mieRayPhase{};
			bool illuminanceIsOne{};
			bool multiScatApproxEnabled{};
			bool shadowMapEnabled{};
			bool multiScatteringPowerSerie{ true };
		};

		AtmosphereModel( sdw::ShaderWriter & writer
			, AtmosphereData const & atmosphereData
			, Settings settings );
		AtmosphereModel( sdw::ShaderWriter & writer
			, AtmosphereData const & atmosphereData
			, Settings settings
			, VkExtent2D transmittanceExtent );

		auto const & getLengthUnit()const noexcept
		{
			return settings.length;
		}

		auto getPlanetPosition()const noexcept
		{
			return atmosphereData.planetPosition();
		}

		auto getSunDirection()const noexcept
		{
			return atmosphereData.sunDirection();
		}

		auto getPlanetRadius()const noexcept
		{
			return atmosphereData.bottomRadius();
		}

		auto getAtmosphereRadius()const noexcept
		{
			return atmosphereData.topRadius();
		}

		auto getAtmosphereH( sdw::Float const d )const noexcept
		{
			return sqrt( max( 0.0_f, d * d - getPlanetRadius() * getPlanetRadius() ) );
		}

		auto getAtmosphereH()const noexcept
		{
			return getAtmosphereH( getAtmosphereRadius()  );
		}

		auto getSunIlluminance()const noexcept
		{
			return atmosphereData.sunIlluminance();
		}

		auto getAtmosphereThickness()const noexcept
		{
			return getAtmosphereRadius() - getPlanetRadius();
		}

		auto getCameraPosition()const noexcept
		{
			CU_Require( settings.cameraData );
			return settings.cameraData->position();
		}

		auto camProjToWorld( sdw::Vec4 const & clipSpace )const noexcept
		{
			CU_Require( settings.cameraData );
			return settings.cameraData->camProjToWorld( clipSpace );
		}

		auto objProjToWorld( sdw::Vec4 const & clipSpace )const noexcept
		{
			CU_Require( settings.cameraData );
			return settings.cameraData->objProjToWorld( clipSpace );
		}

		void setTransmittanceMap( sdw::CombinedImage2DRgba32 const & value )
		{
			transmittanceTexture = &value;
		}

		void setMultiscatterMap( sdw::CombinedImage2DRgba32 const & value )
		{
			multiScatTexture = &value;
		}

		sdw::Vec3 getPositionToPlanet( sdw::Vec3 const & position )const;
		RetRay castRay( sdw::Vec2 const & uv );
		RetRay castRay( sdw::Vec2 const & screenPoint
			, sdw::Vec2 const & screenSize );
		RetSingleScatteringResult integrateScatteredLuminance( sdw::Vec2 const & pixPos
			, Ray const & ray
			, sdw::Vec3 const & sunDir
			, sdw::Float const & sampleCountIni
			, sdw::Float const & depthBufferValue
			, sdw::Float const & tMaxMax = sdw::Float{ 9000000.0_f } );
		sdw::RetBoolean moveToTopAtmosphere( Ray & ray );
		sdw::RetVec3 getSunRadiance( sdw::Vec3 const & cameraPosition
			, sdw::Vec3 const & sunDir
			, sdw::CombinedImage2DRgba32 const & transmittanceMap );
		sdw::RetFloat getPlanetShadow( sdw::Vec3 const & planetO
			, sdw::Vec3 const & position );

		// - Returns distance from rayOrigin to first intersecion with sphere,
		//   or -1.0 if no intersection.
		RetIntersection raySphereIntersectNearest( Ray const & ray
			, sdw::Vec3 const & sphereCenter
			, sdw::Float const & sphereRadius );
		RetIntersection raySphereIntersectNearest( Ray const & ray
			, sdw::Float const & sphereRadius );
		sdw::RetInt raySphereIntersect( Ray const & ray
			, sdw::Float const & sphereRadius
			, Intersection const & ground
			, sdw::Boolean const & clampToGround
			, Intersection & nearest
			, Intersection & farthest );
		RetIntersection raySphereintersectSkyMap( sdw::Vec3 const & rd
			, sdw::Float const & radius );

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
		sdw::Float doGetPlanetShadow( Ray const & rayToSun
			, sdw::Vec3 const & planetO
			, sdw::Vec3 const & upVector );

	private:
		sdw::ShaderWriter & writer;

	public:
		AtmosphereData const & atmosphereData;
		Settings settings;
		VkExtent2D transmittanceExtent{};
		sdw::Float planetRadiusOffset;
		sdw::CombinedImage2DRgba32 const * transmittanceTexture{};
		sdw::CombinedImage2DRgba32 const * multiScatTexture{};
		std::shared_ptr< castor3d::shader::Shadow > shadows;

	private:
		sdw::Function< Ray
			, sdw::InVec2 > m_castRay;
		sdw::Function< Ray
			, sdw::InVec2 > m_castRayM;
		sdw::Function< SingleScatteringResult
			, sdw::InVec2
			, InRay
			, sdw::InVec3
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InFloat > m_integrateScatteredLuminance;
		sdw::Function< sdw::Boolean
			, InOutRay > m_moveToTopAtmosphere;
		sdw::Function< Intersection
			, InRay
			, sdw::InVec3
			, sdw::InFloat > m_raySphereIntersectNearest;
		sdw::Function< sdw::Int
			, InRay
			, sdw::InFloat
			, InIntersection
			, sdw::InBoolean
			, OutIntersection
			, OutIntersection > m_raySphereIntersect;
		sdw::Function< Intersection
			, sdw::InVec3
			, sdw::InFloat > m_raySphereintersectSkyMap;
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
			, sdw::InFloat
			, sdw::InVec2
			, sdw::InVec2 > m_getWorldPos;
		sdw::Function< sdw::Vec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InCombinedImage2DRgba32 > m_getSunRadiance;
		sdw::Function< sdw::Float
			, sdw::InVec3
			, sdw::InVec3 > m_getPlanetShadow;
	};
}

#endif
