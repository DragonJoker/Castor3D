/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ColourGradingConfig_H___
#define ___C3D_ColourGradingConfig_H___

#include "ToneMappingModule.hpp"

#include <CastorUtils/Data/TextWriter.hpp>

#pragma warning( push )
#pragma warning( disable: 4324 )

namespace castor3d
{
	struct ColourGradingConfig
	{
		C3D_API ColourGradingConfig();

		C3D_API void accept( ConfigurationVisitorBase & visitor );

		C3D_API static void addParsers( castor::AttributeParsers & result );

		castor::Point3f whiteBalance{ 1.0f, 1.0f, 1.0f };
		float postExposure{ 1.0f };
		castor::Point3f colourFilter{ 1.0f, 1.0f, 1.0f };
		float midGray{ 0.4135884f };
		castor::Point3f splitToningShadows{ 0.5f, 0.5f, 0.5f };
		float contrast{ 1.0f };
		castor::Point3f splitToningHighlights{ 0.5f, 0.5f, 0.5f };
		float splitToningBalance{ 1.0f };
		castor::Point3f channelMixRed{ 1.0f, 0.0f, 0.0f };
		float saturation{ 1.0f };
		castor::Point3f channelMixGreen{ 0.0f, 1.0f, 0.0f };
		float shadowsStart{ 0.0f };
		castor::Point3f channelMixBlue{ 0.0f, 0.0f, 1.0f };
		float shadowsEnd{ 1.0f };
		castor::Point3f shadows{ 1.0f, 1.0f, 1.0f };
		float highlightsStart{ 0.0f };
		castor::Point3f midtones{ 1.0f, 1.0f, 1.0f };
		float highlightsEnd{ 1.0f };
		castor::Point3f highlights{ 1.0f, 1.0f, 1.0f };
		float hueShift{ 0.0f };
		alignas( alignof( uint32_t ) ) bool enabled{ true };
		alignas( alignof( uint32_t ) ) bool enableSplitToning{ true };
		alignas( alignof( uint32_t ) ) bool enableShadowMidToneHighlight{ true };
		alignas( alignof( uint32_t ) ) bool enableChannelMix{ true };
		alignas( alignof( uint32_t ) ) bool enableWhiteBalance{ true };
		alignas( alignof( uint32_t ) ) bool enableHueShift{ true };
		alignas( alignof( uint32_t ) ) bool enableContrast{ true };
		alignas( alignof( uint32_t ) ) bool pad0{};
	};

	inline bool operator==( ColourGradingConfig const & lhs, ColourGradingConfig const & rhs )
	{
		return lhs.whiteBalance == rhs.whiteBalance
			&& lhs.postExposure == rhs.postExposure
			&& lhs.colourFilter == rhs.colourFilter
			&& lhs.midGray == rhs.midGray
			&& lhs.splitToningShadows == rhs.splitToningShadows
			&& lhs.contrast == rhs.contrast
			&& lhs.splitToningHighlights == rhs.splitToningHighlights
			&& lhs.splitToningBalance == rhs.splitToningBalance
			&& lhs.channelMixRed == rhs.channelMixRed
			&& lhs.saturation == rhs.saturation
			&& lhs.channelMixGreen == rhs.channelMixGreen
			&& lhs.shadowsStart == rhs.shadowsStart
			&& lhs.channelMixBlue == rhs.channelMixBlue
			&& lhs.shadowsEnd == rhs.shadowsEnd
			&& lhs.shadows == rhs.shadows
			&& lhs.highlightsStart == rhs.highlightsStart
			&& lhs.midtones == rhs.midtones
			&& lhs.highlightsEnd == rhs.highlightsEnd
			&& lhs.highlights == rhs.highlights
			&& lhs.hueShift == rhs.hueShift
			&& lhs.enabled == rhs.enabled
			&& lhs.enableSplitToning == rhs.enableSplitToning
			&& lhs.enableShadowMidToneHighlight == rhs.enableShadowMidToneHighlight
			&& lhs.enableChannelMix == rhs.enableChannelMix
			&& lhs.enableWhiteBalance == rhs.enableWhiteBalance
			&& lhs.enableHueShift == rhs.enableHueShift
			&& lhs.enableContrast == rhs.enableContrast;
	}
}

#pragma warning( pop )

#endif
