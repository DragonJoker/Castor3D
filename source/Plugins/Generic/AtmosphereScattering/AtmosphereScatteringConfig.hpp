/*
See LICENSE file in root folder
*/
#ifndef ___C3DAS_AtmosphereScatteringConfig_H___
#define ___C3DAS_AtmosphereScatteringConfig_H___

#include "AtmosphereScatteringPrerequisites.hpp"

#include <CastorUtils/Math/Angle.hpp>
#include <CastorUtils/Math/SquareMatrix.hpp>

namespace atmosphere_scattering
{
	// An atmosphere layer of width 'width', and whose density is defined as
	//   'exp_term' * exp('exp_scale' * h) + 'linear_term' * h + 'constant_term',
	// clamped to [0,1], and where h is the altitude.
	struct DensityProfileLayer
	{
		float layerWidth;
		float expTerm;
		float expScale;
		float linearTerm;
		float constantTerm;
	};

	inline bool operator==( DensityProfileLayer const & lhs
		, DensityProfileLayer const & rhs )
	{
		return lhs.layerWidth == rhs.layerWidth
			&& lhs.expTerm == rhs.expTerm
			&& lhs.expScale == rhs.expScale
			&& lhs.linearTerm == rhs.linearTerm
			&& lhs.constantTerm == rhs.constantTerm;
	}

	using DensityProfileLayers = std::array< DensityProfileLayer, 2u >;

	// All units in kilometers
	static constexpr float EarthBottomRadius = 6360.0f;
	static constexpr float EarthTopRadius = 6460.0f;   // 100km atmosphere radius, less edge visible and it contain 99.99% of the atmosphere medium https://en.wikipedia.org/wiki/K%C3%A1rm%C3%A1n_line
	static constexpr float EarthRayleighScaleHeight = 8.0f;
	static constexpr float EarthMieScaleHeight = 1.2f;
	static constexpr double MaxSunZenithAngle = castor::Pi< double > * 120.0 / 180.0;

	template< template< typename DataT > typename WrapperT >
	struct AtmosphereScatteringConfigT
	{
		template< typename ... ParamsT >
		AtmosphereScatteringConfigT( ParamsT & ... params )
			: sunDirection{ 1.0f, 0.0f, 0.0f, -20.0_degrees }
			, solarIrradiance{ params..., castor::Point3f{ 1.0f, 1.0f, 1.0f } }
			, sunAngularRadius{ params..., 0.004675f }
			, sunIlluminance{ params..., castor::Point3f{ 1.0f, 1.0f, 1.0f } }
			, sunIlluminanceScale{ params..., 1.0f }
			, rayMarchMinMaxSPP{ params..., castor::Point2f{ 4.0f, 14.0f } }
			, absorptionExtinction{ params..., castor::Point3f{ 0.000650f, 0.001881f, 0.000085f } }
			, muSMin{ params..., float( cos( MaxSunZenithAngle ) ) }
			, rayleighScattering{ params..., castor::Point3f{ 0.005802f, 0.013558f, 0.033100f } }
			, miePhaseFunctionG{ params..., 0.8f }
			, mieScattering{ params..., castor::Point3f{ 0.003996f, 0.003996f, 0.003996f } }
			, bottomRadius{ params..., EarthBottomRadius }
			, mieExtinction{ params..., castor::Point3f{ 0.004440f, 0.004440f, 0.004440f } }
			, topRadius{ params..., EarthTopRadius }
			, mieAbsorption{}
			, multipleScatteringFactor{ params..., 1.0f }
			, groundAlbedo{ params..., castor::Point3f{ 0.0f, 0.0f, 0.0f } }
			, multiScatteringLUTRes{ params..., 32.0f }
			, rayleighDensity{ params..., DensityProfileLayers{ DensityProfileLayer{ 0.0f, 0.0f, 0.0f, 0.0f, 0.0f }
				, DensityProfileLayer{ 0.0f, 1.0f, -1.0f / EarthRayleighScaleHeight, 0.0f, 0.0f } } }
			, mieDensity{ params..., DensityProfileLayers{ DensityProfileLayer{ 0.0f, 0.0f, 0.0f, 0.0f, 0.0f }
				, DensityProfileLayer{ 0.0f, 1.0f, -1.0f / EarthMieScaleHeight, 0.0f, 0.0f } } }
			, absorptionDensity{ params..., DensityProfileLayers{ DensityProfileLayer{ 25.0f, 0.0f, 0.0f, 1.0f / 15.0f, -2.0f / 3.0f }
				, DensityProfileLayer{ 0.0f, 0.0f, 0.0f, -1.0f / 15.0f, 8.0f / 3.0f } } }
		{
		}

		template< template< typename DataU > typename WrapperU >
		AtmosphereScatteringConfigT & operator=( AtmosphereScatteringConfigT< WrapperU > const & rhs )
		{
			sunDirection = rhs.sunDirection;
			solarIrradiance = rhs.solarIrradiance;
			sunAngularRadius = rhs.sunAngularRadius;
			sunIlluminance = rhs.sunIlluminance;
			sunIlluminanceScale = rhs.sunIlluminanceScale;
			rayMarchMinMaxSPP = rhs.rayMarchMinMaxSPP;
			absorptionExtinction = rhs.absorptionExtinction;
			muSMin = rhs.muSMin;
			rayleighScattering = rhs.rayleighScattering;
			miePhaseFunctionG = rhs.miePhaseFunctionG;
			mieScattering = rhs.mieScattering;
			bottomRadius = rhs.bottomRadius;
			mieExtinction = rhs.mieExtinction;
			topRadius = rhs.topRadius;
			mieAbsorption = rhs.mieAbsorption;
			multipleScatteringFactor = rhs.multipleScatteringFactor;
			groundAlbedo = rhs.groundAlbedo;
			multiScatteringLUTRes = rhs.multiScatteringLUTRes;
			rayleighDensity = rhs.rayleighDensity;
			mieDensity = rhs.mieDensity;
			absorptionDensity = rhs.absorptionDensity;

			return *this;
		}

		castor::Point4f sunDirection;

		// The solar irradiance at the top of the atmosphere.
		WrapperT< castor::Point3f > solarIrradiance;
		// The sun's angular radius. Warning: the implementation uses approximations
		// that are valid only if this angle is smaller than 0.1 radians.
		WrapperT< float > sunAngularRadius;

		WrapperT< castor::Point3f > sunIlluminance;
		WrapperT< float > sunIlluminanceScale;

		WrapperT< castor::Point2f > rayMarchMinMaxSPP;
		castor::Point2f pad0;

		// The extinction coefficient of molecules that absorb light (e.g. ozone) at
		// the altitude where their density is maximum, as a function of wavelength.
		// The extinction coefficient at altitude h is equal to
		// 'absorption_extinction' times 'absorption_density' at this altitude.
		WrapperT< castor::Point3f > absorptionExtinction;
		// The cosine of the maximum Sun zenith angle for which atmospheric scattering
		// must be precomputed (for maximum precision, use the smallest Sun zenith
		// angle yielding negligible sky light radiance values. For instance, for the
		// Earth case, 102 degrees is a good choice - yielding mu_s_min = -0.2).
		WrapperT< float > muSMin;

		// The scattering coefficient of air molecules at the altitude where their
		// density is maximum (usually the bottom of the atmosphere), as a function of
		// wavelength. The scattering coefficient at altitude h is equal to
		// 'rayleigh_scattering' times 'rayleigh_density' at this altitude.
		WrapperT< castor::Point3f > rayleighScattering;
		// The asymetry parameter for the Cornette-Shanks phase function for the
		// aerosols.
		WrapperT< float > miePhaseFunctionG;

		// The scattering coefficient of aerosols at the altitude where their density
		// is maximum (usually the bottom of the atmosphere), as a function of
		// wavelength. The scattering coefficient at altitude h is equal to
		// 'mie_scattering' times 'mie_density' at this altitude.
		WrapperT< castor::Point3f > mieScattering;
		// The distance between the planet center and the bottom of the atmosphere.
		WrapperT< float > bottomRadius;

		// The extinction coefficient of aerosols at the altitude where their density
		// is maximum (usually the bottom of the atmosphere), as a function of
		// wavelength. The extinction coefficient at altitude h is equal to
		// 'mie_extinction' times 'mie_density' at this altitude.
		WrapperT< castor::Point3f > mieExtinction;
		// The distance between the planet center and the top of the atmosphere.
		WrapperT< float > topRadius{ EarthTopRadius };

		castor::Point3f mieAbsorption;
		WrapperT< float > multipleScatteringFactor;

		// The average albedo of the ground.
		WrapperT< castor::Point3f > groundAlbedo;
		WrapperT< float > multiScatteringLUTRes;

		// The density profile of air molecules, i.e. a function from altitude to
		// dimensionless values between 0 (null density) and 1 (maximum density).
		WrapperT< DensityProfileLayers > rayleighDensity;
		// The density profile of aerosols, i.e. a function from altitude to
		// dimensionless values between 0 (null density) and 1 (maximum density).
		WrapperT< DensityProfileLayers > mieDensity;
		// The density profile of air molecules that absorb light (e.g. ozone), i.e.
		// a function from altitude to dimensionless values between 0 (null density)
		// and 1 (maximum density).
		WrapperT< DensityProfileLayers > absorptionDensity;
	};
}

#endif
