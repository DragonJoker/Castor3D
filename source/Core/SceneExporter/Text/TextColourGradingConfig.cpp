#include "TextColourGradingConfig.hpp"

#include <Castor3D/Miscellaneous/Logger.hpp>

namespace castor
{
	using namespace castor3d;

	TextWriter< ColourGradingConfig >::TextWriter( String const & tabs )
		: TextWriterT< ColourGradingConfig >{ tabs }
	{
	}

	bool TextWriter< ColourGradingConfig >::operator()( ColourGradingConfig const & object
		, StringStream & file )
	{
		bool result{ false };
		log::info << tabs() << cuT( "Writing ColourGradingConfig" ) << std::endl;

		if ( auto block{ beginBlock( file, "colour_grading" ) } )
		{
			result = writeOpt( file, cuT( "enabled" ), object.enabled, true )
				&& writeOpt( file, cuT( "post_exposure" ), object.postExposure, 1.0f )
				&& writeOpt( file, cuT( "colour_filter" ), object.colourFilter, { 1.0f, 1.0f, 1.0f } )
				&& writeOpt( file, cuT( "saturation" ), object.saturation, 1.0f )
				&& writeOpt( file, cuT( "enable_contrast" ), object.enableContrast, true )
				&& writeOpt( file, cuT( "contrast" ), object.contrast, 1.0f )
				&& writeOpt( file, cuT( "mid_gray" ), object.midGray, 0.4135884f )
				&& writeOpt( file, cuT( "enable_hue_shift" ), object.enableHueShift, true )
				&& writeOpt( file, cuT( "hue_shift" ), object.hueShift, 0.0f )
				&& writeOpt( file, cuT( "enable_white_balance" ), object.enableWhiteBalance, true )
				&& writeOpt( file, cuT( "white_balance" ), object.whiteBalance, { 1.0f, 1.0f, 1.0f } )
				&& writeOpt( file, cuT( "enable_split_toning" ), object.enableSplitToning, true )
				&& writeOpt( file, cuT( "split_toning_shadows" ), object.splitToningShadows, { 0.5f, 0.5f, 0.5f } )
				&& writeOpt( file, cuT( "split_toning_highlights" ), object.splitToningHighlights, { 0.5f, 0.5f, 0.5f } )
				&& writeOpt( file, cuT( "split_toning_balance" ), object.splitToningBalance, 1.0f )
				&& writeOpt( file, cuT( "enable_channel_mix" ), object.enableChannelMix, true )
				&& writeOpt( file, cuT( "channel_mix_red" ), object.channelMixRed, { 1.0f, 0.0f, 0.0f } )
				&& writeOpt( file, cuT( "channel_mix_green" ), object.channelMixGreen, { 0.0f, 1.0f, 0.0f } )
				&& writeOpt( file, cuT( "channel_mix_blue" ), object.channelMixBlue, { 0.0f, 0.0f, 1.0f } )
				&& writeOpt( file, cuT( "enable_shadow_midtone_hilight" ), object.enableShadowMidToneHighlight, true )
				&& writeOpt( file, cuT( "shadows" ), object.shadows, { 1.0f, 1.0f, 1.0f } )
				&& writeOpt( file, cuT( "midtones" ), object.midtones, { 1.0f, 1.0f, 1.0f } )
				&& writeOpt( file, cuT( "highlights" ), object.highlights, { 1.0f, 1.0f, 1.0f } )
				&& writeOpt( file, cuT( "shadows_range" ), castor::Point2f{ object.shadowsStart, object.shadowsEnd }, { 0.0f, 1.0f } )
				&& writeOpt( file, cuT( "highlights_range" ), castor::Point2f{ object.highlightsStart, object.highlightsEnd }, { 0.0f, 1.0f } );
		}

		return result;
	}
}
