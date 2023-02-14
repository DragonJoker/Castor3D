/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Shadow_H___
#define ___C3D_Shadow_H___

#include "SceneModule.hpp"

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
		uint32_t pcfFilterSize{ 4u };
		castor::RangedValue< uint32_t > pcfSampleCount{ 16u, castor::makeRange( 0u, 64u ) };
		RsmConfig rsmConfig;
		LpvConfig lpvConfig;
	};
}

#endif
