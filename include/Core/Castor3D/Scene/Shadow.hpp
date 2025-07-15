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

namespace c3d
{
	struct ShadowConfig
	{
		C3D_API void accept( ConfigurationVisitorBase & visitor
			, LightType lightType );

		C3D_API static void addParsers( AttributeParsers & result
			, CSCNSection light, CSCNSection shadows
			, CSCNSection shadowsRaw, CSCNSection shadowsPcf, CSCNSection shadowsVsm
			, RawParserFunctionT< void > parserShadows, RawParserFunctionT< void > parserShadowProducer
			, RawParserFunctionT< ShadowContext > parserRawConfig, RawParserFunctionT< ShadowContext > parserPcfConfig, RawParserFunctionT< ShadowContext > parserVsmConfig );

		bool enabled{};
		ShadowType filterType{ ShadowType::eNone };
		GlobalIlluminationType globalIllumination{ GlobalIlluminationType::eNone };
		uint32_t volumetricSteps{ 0u };
		float volumetricScattering{ 0.2f };
		Point2f rawOffsets;
		Point2f pcfOffsets;
		float vsmMinVariance{};
		float vsmLightBleedingReduction{};
		RangedValue< uint32_t > pcfFilterSize{ 4u, makeRange( 0u, MaxPcfFilterSize ) };
		RangedValue< uint32_t > pcfSampleCount{ 8u, makeRange( 0u, MaxPcfSampleCount ) };
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

namespace c3d
{
	template<>
	struct ParserEnumTraits< ShadowType >
	{
		static inline xchar const * const Name = cuT( "ShadowType" );
		static inline UInt32StrMap const Values = []()
			{
				UInt32StrMap result;
				result = getEnumMapT< ShadowType >();
				return result;
			}( );
	};

	template<>
	struct ParserEnumTraits< GlobalIlluminationType >
	{
		static inline xchar const * const Name = cuT( "GlobalIlluminationType" );
		static inline UInt32StrMap const Values = []()
			{
				UInt32StrMap result;
				result = getEnumMapT< GlobalIlluminationType >();
				return result;
			}( );
	};
}

#endif
