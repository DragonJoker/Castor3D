/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Shadow_H___
#define ___C3D_Shadow_H___

#include "SceneModule.hpp"

#include "Castor3D/Limits.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LpvConfig.hpp"

#include <CastorUtils/Math/RangedValue.hpp>

namespace castor3d
{
	struct ShadowConfig
	{
		ShadowType filterType{ ShadowType::eNone };
		GlobalIlluminationType globalIllumination{ GlobalIlluminationType::eNone };
		uint32_t volumetricSteps{ 0u };
		float volumetricScattering{ 0.2f };
		castor::Point2f rawOffsets;
		castor::Point2f pcfOffsets;
		float vsmMinVariance{};
		float vsmLightBleedingReduction{};
		castor::RangedValue< uint32_t > pcfFilterSize{ 4u, castor::makeRange( 0u, MaxPcfFilterSize ) };
		castor::RangedValue< uint32_t > pcfSampleCount{ 8u, castor::makeRange( 0u, MaxPcfSampleCount ) };
		RsmConfig rsmConfig;
		LpvConfig lpvConfig;
	};

	inline bool operator==( RsmConfig const & lhs, RsmConfig const & rhs )noexcept
	{
		return lhs.intensity == rhs.intensity
			&& lhs.maxRadius == rhs.maxRadius
			&& lhs.sampleCount == rhs.sampleCount;
	}

	inline bool operator==( ShadowConfig const & lhs, ShadowConfig const & rhs )noexcept
	{
		return lhs.filterType == rhs.filterType
			&& lhs.globalIllumination == rhs.globalIllumination
			&& lhs.volumetricSteps == rhs.volumetricSteps
			&& lhs.volumetricScattering == rhs.volumetricScattering
			&& lhs.rawOffsets == rhs.rawOffsets
			&& lhs.pcfOffsets == rhs.pcfOffsets
			&& lhs.vsmMinVariance == rhs.vsmMinVariance
			&& lhs.vsmLightBleedingReduction == rhs.vsmLightBleedingReduction
			&& lhs.pcfFilterSize == rhs.pcfFilterSize
			&& lhs.pcfSampleCount == rhs.pcfSampleCount
			&& lhs.rsmConfig == rhs.rsmConfig
			&& lhs.lpvConfig == rhs.lpvConfig;
	}
}

#endif
