/*
See LICENSE file in root folder
*/
#ifndef ___C3DAS_AtmosphereModel_H___
#define ___C3DAS_AtmosphereModel_H___

#include "AtmosphereCameraUbo.hpp"
#include "AtmosphereScatteringUbo.hpp"

#include <Castor3D/Render/Volumetric/VolumeComponentPlugin.hpp>
#include <Castor3D/Shader/Shaders/GlslRay.hpp>
#include <Castor3D/Shader/Shaders/GlslVolumeShaders.hpp>

#include <ShaderWriter/CompositeTypes/Function.hpp>
#include <ShaderWriter/CompositeTypes/StructHelper.hpp>

namespace atmosphere_scattering
{
	static constexpr bool useUnified = true;

	using c3d::shader::Intersection;
	using c3d::shader::RetIntersection;
	using c3d::shader::InIntersection;
	using c3d::shader::InOutIntersection;
	using c3d::shader::OutIntersection;
	using c3d::shader::Ray;
	using c3d::shader::RetRay;
	using c3d::shader::InRay;
	using c3d::shader::InOutRay;
	using c3d::shader::OutRay;

	struct MediumSampleRGB;

	struct AtmosphereTraverseData
		: public sdw::StructInstanceHelperT< "AtmosphereTraverseData"
			, sdw::type::MemoryLayout::eC
			, sdw::Vec3Field< "rayMieMultiScatter" >
			, sdw::FloatField< "viewHeight" >
			, sdw::Vec3Field< "mieSingleScatter" >
			, sdw::FloatField< "sampleCountFloor" >
			, sdw::Vec3Field< "sunDirection" >
			, sdw::FloatField< "sampleCount" >
			, sdw::Vec3Field< "rayleighSingleScatter" >
			, sdw::FloatField< "tMaxFloor" > >
	{
		SDW_DeclStructInstance( , AtmosphereTraverseData );

		AtmosphereTraverseData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstanceHelperT{ writer, c3d::move( expr ), enabled }
			, rayMieMultiScatter{ getMember< "rayMieMultiScatter" >() }
			, viewHeight{ getMember< "viewHeight" >() }
			, mieSingleScatter{ getMember< "mieSingleScatter" >() }
			, sampleCountFloor{ getMember< "sampleCountFloor" >() }
			, sunDirection{ getMember< "sunDirection" >() }
			, sampleCount{ getMember< "sampleCount" >() }
			, rayleighSingleScatter{ getMember< "rayleighSingleScatter" >() }
			, tMaxFloor{ getMember< "tMaxFloor" >() }
		{
		}

		explicit AtmosphereTraverseData( sdw::ShaderWriter & writer );

		void initialise( bool variableSampleCount, AtmosphereData const & atmosphereData
			, c3ds::Ray const & ray, sdw::Float const & range );
		sdw::RetVec3 computeScattering( sdw::Float const & rayleighPhaseValue, sdw::Float const & miePhaseValue, sdw::Vec3 const & globalLuminance )const;

		static sdw::expr::ExprList getZeroInit();

		sdw::Vec3 rayMieMultiScatter;
		sdw::Float viewHeight;
		sdw::Vec3 mieSingleScatter;
		sdw::Float sampleCountFloor;
		sdw::Vec3 sunDirection;
		sdw::Float sampleCount;
		sdw::Vec3 rayleighSingleScatter;
		sdw::Float tMaxFloor;
	};

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
		SDW_DeclStructInstance( , SingleScatteringResult );

		SingleScatteringResult( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstanceHelperT{ writer, c3d::move( expr ), enabled }
			, luminance{ getMember< "luminance" >() }
			, opticalDepth{ getMember< "opticalDepth" >() }
			, transmittance{ getMember< "transmittance" >() }
			, multiScatAs1{ getMember< "multiScatAs1" >() }
			, newMultiScatStep0Out{ getMember< "newMultiScatStep0Out" >() }
			, newMultiScatStep1Out{ getMember< "newMultiScatStep1Out" >() }
		{
		}

		explicit SingleScatteringResult( sdw::ShaderWriter & writer );

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
		static sdw::expr::ExprList getZeroInit();
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
		SDW_DeclStructInstance( , MediumSampleRGB );

		MediumSampleRGB( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstanceHelperT{ writer, c3d::move( expr ), enabled }
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
			explicit Settings( c3d::Length l )
				: length{ c3d::move( l ) }
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

			c3d::Length length;
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
			, c3d::Extent2D transmittanceExtent );

		bool hasVariableSampleCount()const noexcept
		{
			return settings.variableSampleCount;
		}

		bool hasMultiscattering()const noexcept
		{
			return settings.multiScatApproxEnabled && multiScatTexture;
		}

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

		auto getAtmosphereH( sdw::Float const & d )const noexcept
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

		auto getRayMarchMinSPP()const noexcept
		{
			return atmosphereData.rayMarchMinMaxSPP().x();
		}

		auto getRayMarchMaxSPP()const noexcept
		{
			return atmosphereData.rayMarchMinMaxSPP().y();
		}

		auto getMiePhaseFunctionG()const noexcept
		{
			return atmosphereData.miePhaseFunctionG();
		}

		auto getAtmosphereThickness()const noexcept
		{
			return getAtmosphereRadius() - getPlanetRadius();
		}

		auto getCameraPosition()const noexcept
		{
			CU_Require( settings.cameraData );
			return settings.cameraData
				? settings.cameraData->position()
				: vec3( 0.0_f );
		}

		auto camProjToWorld( sdw::Vec4 const & clipSpace )const noexcept
		{
			CU_Require( settings.cameraData );
			return  settings.cameraData
				? settings.cameraData->camProjToWorld( clipSpace )
				: clipSpace;
		}

		auto objProjToWorld( sdw::Vec4 const & clipSpace )const noexcept
		{
			CU_Require( settings.cameraData );
			return settings.cameraData
				? settings.cameraData->objProjToWorld( clipSpace )
				: clipSpace;
		}

		void setTransmittanceMap( sdw::CombinedImage2DRgba16 const & value )
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
		sdw::RetBoolean moveToTopAtmosphere( Ray const & ray );
		void moveToBottomAtmosphere( Ray const & ray
			, sdw::Float const & objectId
			, sdw::Float const & linearDepth );
		sdw::RetVec3 getSunRadiance( sdw::Vec3 const & position
			, sdw::Vec3 const & sunDir
			, sdw::CombinedImage2DRgba16 const & transmittanceMap );
		sdw::RetVec3 getSunRadiance( sdw::Vec3 const & sunDir
			, sdw::CombinedImage2DRgba16 const & transmittanceMap );
		sdw::RetFloat getPlanetShadow( sdw::Vec3 const & planetO
			, sdw::Vec3 const & position );
		sdw::Vec3 getSunDir( sdw::Vec3 const & upVector )const;
		sdw::RetVec3 integrateInscatter( sdw::Vec3 const & luminance
			, sdw::Vec3 const & transmittance
			, sdw::Vec3 const & stepTransmittance
			, sdw::Vec3 const & sigmaS );

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
		static sdw::Float rayleighPhase( sdw::Float const & cosTheta );
		// We should precompute those terms from resolutions (Or set resolution as #defined constants)
		static sdw::Float fromUnitToSubUvs( sdw::Float const & u, sdw::Float const & resolution );
		static sdw::Float fromSubUvsToUnit( sdw::Float const & u, sdw::Float const & resolution );

		sdw::RetVec3 getWorldPos( sdw::Float const & depth
			, sdw::Vec2 const & pixPos
			, sdw::Vec2 const & texSize );
		sdw::RetVec3 getMultipleScattering( sdw::Float const & worldPosLength
			, sdw::Float const & viewZenithCosAngle );

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
		sdw::RetVec2 lutTransmittanceParamsToUv( sdw::Float const & viewHeight
			, sdw::Float const & viewZenithCosAngle );
		sdw::Void uvToSkyViewLutParams( sdw::Float & viewZenithCosAngle
			, sdw::Float & lightViewCosAngle
			, sdw::Float const & viewHeight
			, sdw::Vec2 const & uv
			, sdw::Vec2 const & size );
		sdw::RetVec2 skyViewLutParamsToUv( sdw::Boolean const & intersectGround
			, sdw::Float const & viewZenithCosAngle
			, sdw::Float const & lightViewCosAngle
			, sdw::Float const & viewHeight
			, sdw::Vec2 const & size );

		sdw::Float getPlanetShadow( Ray const & rayToSun
			, sdw::Vec3 const & planetO
			, sdw::Vec3 const & upVector );

	private:
		sdw::Function< sdw::Vec3, sdw::InVec3, sdw::InVec3, sdw::InVec3, sdw::InVec3 > getIntegrateInscatter();

	private:
		sdw::ShaderWriter & writer;

	public:
		AtmosphereData const & atmosphereData;
		Settings settings;
		c3d::Extent2D transmittanceExtent{};
		sdw::Float planetRadiusOffset;
		sdw::CombinedImage2DRgba16 const * transmittanceTexture{};
		sdw::CombinedImage2DRgba32 const * multiScatTexture{};
		c3d::shader::Shadow * shadows{};

	private:
		sdw::Function< Ray
			, sdw::InVec2 > m_castRay;
		sdw::Function< SingleScatteringResult
			, sdw::InVec2, InRay, sdw::InVec3, sdw::InFloat, sdw::InFloat, sdw::InFloat > m_integrateScatteredLuminance;
		sdw::Function< sdw::Boolean
			, InOutRay > m_moveToTopAtmosphere;
		sdw::Function< sdw::Void
			, InOutRay, sdw::InFloat, sdw::InFloat > m_moveToBottomAtmosphere;
		sdw::Function< Intersection
			, InRay, sdw::InVec3, sdw::InFloat > m_raySphereIntersectNearest;
		sdw::Function< sdw::Int
			, InRay, sdw::InFloat, InIntersection, sdw::InBoolean, OutIntersection, OutIntersection > m_raySphereIntersect;
		sdw::Function< Intersection
			, sdw::InVec3, sdw::InFloat > m_raySphereintersectSkyMap;
		sdw::Function< sdw::Float
			, sdw::InFloat, sdw::InFloat > m_hgPhase;
		sdw::Function< MediumSampleRGB
			, sdw::InVec3 > m_sampleMediumRGB;
		sdw::Function< sdw::Void
			, sdw::OutFloat, sdw::OutFloat, sdw::InVec2 > m_uvToLutTransmittanceParams;
		sdw::Function< sdw::Vec2
			, sdw::InFloat, sdw::InFloat > m_lutTransmittanceParamsToUv;
		sdw::Function< sdw::Void
			, sdw::OutFloat, sdw::OutFloat, sdw::InFloat, sdw::InVec2, sdw::InVec2 > m_uvToSkyViewLutParams;
		sdw::Function< sdw::Vec2
			, sdw::InBoolean, sdw::InFloat, sdw::InFloat, sdw::InFloat, sdw::InVec2 > m_skyViewLutParamsToUv;
		sdw::Function< sdw::Vec3
			, sdw::InFloat, sdw::InVec2, sdw::InVec2 > m_getWorldPos;
		sdw::Function < sdw::Vec3
			, sdw::InFloat, sdw::InFloat > m_getMultipleScattering;
		sdw::Function< sdw::Vec3
			, sdw::InVec3, sdw::InVec3, sdw::InCombinedImage2DRgba16 > m_getSunRadiance;
		sdw::Function< sdw::Float
			, sdw::InVec3, sdw::InVec3 > m_getPlanetShadow;
		sdw::Function< sdw::Vec3
			, sdw::InVec3, sdw::InVec3, sdw::InVec3, sdw::InVec3 > m_integrateInscatter;
	};
}

#endif
