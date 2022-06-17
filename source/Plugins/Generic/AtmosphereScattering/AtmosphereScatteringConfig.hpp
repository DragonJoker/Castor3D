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
	struct CameraConfig
	{
		castor::Matrix4x4f invViewProj;
		castor::Point3f position;
	};

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

	// All units in kilometers
	static constexpr float EarthBottomRadius = 6360.0f;
	static constexpr float EarthTopRadius = 6460.0f;   // 100km atmosphere radius, less edge visible and it contain 99.99% of the atmosphere medium https://en.wikipedia.org/wiki/K%C3%A1rm%C3%A1n_line
	static constexpr float EarthRayleighScaleHeight = 8.0f;
	static constexpr float EarthMieScaleHeight = 1.2f;
	static constexpr double MaxSunZenithAngle = castor::Pi< double > * 120.0 / 180.0;

	struct AtmosphereScatteringConfig
	{
		castor::Point4f sunDirection{ 1.0f, 0.0f, 0.0f, -20.0_degrees };

		// The solar irradiance at the top of the atmosphere.
		castor::Point3f solarIrradiance{ 1.0f, 1.0f, 1.0f };
		// The sun's angular radius. Warning: the implementation uses approximations
		// that are valid only if this angle is smaller than 0.1 radians.
		float sunAngularRadius{ 0.004675f };

		castor::Point3f sunIlluminance{ 1.0f, 1.0f, 1.0f };
		float sunIlluminanceScale{ 1.0f };

		castor::Point2f rayMarchMinMaxSPP{ 4.0f, 14.0f };
		castor::Point2f pad0;

		// The extinction coefficient of molecules that absorb light (e.g. ozone) at
		// the altitude where their density is maximum, as a function of wavelength.
		// The extinction coefficient at altitude h is equal to
		// 'absorption_extinction' times 'absorption_density' at this altitude.
		castor::Point3f absorptionExtinction{ 0.000650f, 0.001881f, 0.000085f };
		// The cosine of the maximum Sun zenith angle for which atmospheric scattering
		// must be precomputed (for maximum precision, use the smallest Sun zenith
		// angle yielding negligible sky light radiance values. For instance, for the
		// Earth case, 102 degrees is a good choice - yielding mu_s_min = -0.2).
		float muSMin{ float( cos( MaxSunZenithAngle ) ) };

		// The scattering coefficient of air molecules at the altitude where their
		// density is maximum (usually the bottom of the atmosphere), as a function of
		// wavelength. The scattering coefficient at altitude h is equal to
		// 'rayleigh_scattering' times 'rayleigh_density' at this altitude.
		castor::Point3f rayleighScattering{ 0.005802f, 0.013558f, 0.033100f };
		// The asymetry parameter for the Cornette-Shanks phase function for the
		// aerosols.
		float miePhaseFunctionG{ 0.8f };

		// The scattering coefficient of aerosols at the altitude where their density
		// is maximum (usually the bottom of the atmosphere), as a function of
		// wavelength. The scattering coefficient at altitude h is equal to
		// 'mie_scattering' times 'mie_density' at this altitude.
		castor::Point3f mieScattering{ 0.003996f, 0.003996f, 0.003996f };
		// The distance between the planet center and the bottom of the atmosphere.
		float bottomRadius{ EarthBottomRadius };

		// The extinction coefficient of aerosols at the altitude where their density
		// is maximum (usually the bottom of the atmosphere), as a function of
		// wavelength. The extinction coefficient at altitude h is equal to
		// 'mie_extinction' times 'mie_density' at this altitude.
		castor::Point3f mieExtinction{ 0.004440f, 0.004440f, 0.004440f };
		// The distance between the planet center and the top of the atmosphere.
		float topRadius{ EarthTopRadius };

		castor::Point3f mieAbsorption;
		float multipleScatteringFactor{ 1.0f };

		// The average albedo of the ground.
		castor::Point3f groundAlbedo{ 0.0f, 0.0f, 0.0f };
		float multiScatteringLUTRes{ 32.0f };

		// The density profile of air molecules, i.e. a function from altitude to
		// dimensionless values between 0 (null density) and 1 (maximum density).
		DensityProfileLayer rayleighDensity[2]{ { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f }
			, { 0.0f, 1.0f, -1.0f / EarthRayleighScaleHeight, 0.0f, 0.0f } };
		// The density profile of aerosols, i.e. a function from altitude to
		// dimensionless values between 0 (null density) and 1 (maximum density).
		DensityProfileLayer mieDensity[2]{ { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f }
			, { 0.0f, 1.0f, -1.0f / EarthMieScaleHeight, 0.0f, 0.0f } };
		// The density profile of air molecules that absorb light (e.g. ozone), i.e.
		// a function from altitude to dimensionless values between 0 (null density)
		// and 1 (maximum density).
		DensityProfileLayer absorptionDensity[2]{ { 25.0f, 0.0f, 0.0f, 1.0f / 15.0f, -2.0f / 3.0f }
			, { 0.0f, 0.0f, 0.0f, -1.0f / 15.0f, 8.0f / 3.0f } };
	};
}

#endif
