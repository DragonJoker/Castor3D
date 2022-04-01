/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Shadow_H___
#define ___C3D_Shadow_H___

#include "SceneModule.hpp"

#include "Castor3D/Limits.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LpvConfig.hpp"
#include "Castor3D/Render/GlobalIllumination/ReflectiveShadowMaps/RsmConfig.hpp"

#include <CastorUtils/FileParser/FileParserModule.hpp>
#include <CastorUtils/Math/RangedValue.hpp>

namespace castor3d
{
	struct ShadowConfig
	{
		C3D_API void accept( ConfigurationVisitorBase & visitor
			, LightType lightType );

		C3D_API static void addParsers( castor::AttributeParsers & result );

		bool enabled{};
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
		LpvConfig lpvConfig;
		RsmConfig rsmConfig;
	};

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
			&& lhs.lpvConfig == rhs.lpvConfig
			&& lhs.rsmConfig == rhs.rsmConfig;
	}
}

namespace castor
{
	template<>
	struct ParserEnumTraits< castor3d::ShadowType >
	{
		static inline xchar const * const Name = cuT( "ShadowType" );
		static inline UInt32StrMap const Values = []()
			{
				UInt32StrMap result;
				result = castor3d::getEnumMapT< castor3d::ShadowType >();
				return result;
			}( );
	};

	template<>
	struct ParserEnumTraits< castor3d::GlobalIlluminationType >
	{
		static inline xchar const * const Name = cuT( "GlobalIlluminationType" );
		static inline UInt32StrMap const Values = []()
			{
				UInt32StrMap result;
				result = castor3d::getEnumMapT< castor3d::GlobalIlluminationType >();
				return result;
			}( );
	};
}

#endif
