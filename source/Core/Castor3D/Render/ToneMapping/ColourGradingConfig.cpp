#include "Castor3D/Render/ToneMapping/ColourGradingConfig.hpp"

#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>

namespace castor3d
{
	namespace clrgrdcfg
	{
		static CU_ImplementAttributeParserBlock( parserColourGrading, CameraContext )
		{
		}
		CU_EndAttributePushBlock( CSCNSection::eColourGrading, blockContext )

		static CU_ImplementAttributeParserBlock( parserPostExposure, CameraContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( blockContext->colourGradingConfig.postExposure );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserEnabled, CameraContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( blockContext->colourGradingConfig.enabled );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserEnableSplitToning, CameraContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( blockContext->colourGradingConfig.enableSplitToning );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserEnableShadowMidToneHighlight, CameraContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( blockContext->colourGradingConfig.enableShadowMidToneHighlight );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserEnableChannelMix, CameraContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( blockContext->colourGradingConfig.enableChannelMix );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserEnableWhiteBalance, CameraContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( blockContext->colourGradingConfig.enableWhiteBalance );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserEnableHueShift, CameraContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( blockContext->colourGradingConfig.enableHueShift );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserEnableContrast, CameraContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( blockContext->colourGradingConfig.enableContrast );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserWhiteBalance, CameraContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( blockContext->colourGradingConfig.whiteBalance );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserFilter, CameraContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( blockContext->colourGradingConfig.colourFilter );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserMidGray, CameraContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( blockContext->colourGradingConfig.midGray );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserContrast, CameraContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( blockContext->colourGradingConfig.contrast );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSaturation, CameraContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( blockContext->colourGradingConfig.saturation );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserHueShift, CameraContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( blockContext->colourGradingConfig.hueShift );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSplitToningShadows, CameraContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( blockContext->colourGradingConfig.splitToningShadows );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSplitToningHighlights, CameraContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( blockContext->colourGradingConfig.splitToningHighlights );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSplitToningBalance, CameraContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( blockContext->colourGradingConfig.splitToningBalance );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserChannelMixRed, CameraContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( blockContext->colourGradingConfig.channelMixRed );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserChannelMixGreen, CameraContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( blockContext->colourGradingConfig.channelMixGreen );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserChannelMixBlue, CameraContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( blockContext->colourGradingConfig.channelMixBlue );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserShadowsRange, CameraContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				castor::Point2f range;
				params[0]->get( range );
				blockContext->colourGradingConfig.shadowsStart = range->x;
				blockContext->colourGradingConfig.shadowsEnd = range->y;
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserHighlightsRange, CameraContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				castor::Point2f range;
				params[0]->get( range );
				blockContext->colourGradingConfig.highlightsStart = range->x;
				blockContext->colourGradingConfig.highlightsEnd = range->y;
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserShadows, CameraContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( blockContext->colourGradingConfig.shadows );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserMidtones, CameraContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( blockContext->colourGradingConfig.midtones );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserHighlights, CameraContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( blockContext->colourGradingConfig.highlights );
			}
		}
		CU_EndAttribute()
	}

	ColourGradingConfig::ColourGradingConfig()
		: whiteBalance{ 1.0f, 1.0f, 1.0f }
		, postExposure{ 1.0f }
		, colourFilter{ 1.0f, 1.0f, 1.0f }
		, midGray{ 0.4135884f }
		, splitToningShadows{ 0.5f, 0.5f, 0.5f }
		, contrast{ 1.0f }
		, splitToningHighlights{ 0.5f, 0.5f, 0.5f }
		, splitToningBalance{ 1.0f }
		, channelMixRed{ 1.0f, 0.0f, 0.0f }
		, saturation{ 1.0f }
		, channelMixGreen{ 0.0f, 1.0f, 0.0f }
		, shadowsStart{ 0.0f }
		, channelMixBlue{ 0.0f, 0.0f, 1.0f }
		, shadowsEnd{ 1.0f }
		, shadows{ 1.0f, 1.0f, 1.0f }
		, highlightsStart{ 0.0f }
		, midtones{ 1.0f, 1.0f, 1.0f }
		, highlightsEnd{ 1.0f }
		, highlights{ 1.0f, 1.0f, 1.0f }
		, hueShift{ 0.0f }
		, enabled{ true }
		, enableSplitToning{ true }
		, enableShadowMidToneHighlight{ true }
		, enableChannelMix{ true }
		, enableWhiteBalance{ true }
		, enableHueShift{ true }
		, enableContrast{ true }
	{
	}

	void ColourGradingConfig::accept( ConfigurationVisitorBase & visitor )
	{
		visitor.visit( cuT( "Colour Grading" ) );
		visitor.visit( cuT( "Enabled" ), enabled );
		visitor.visit( cuT( "Post Exposure" ), postExposure );
		visitor.visit( cuT( "Colour Filter" ), ColourWrapper{ &colourFilter } );
		visitor.visit( cuT( "Saturation" ), saturation );
		{
			auto block = visitor.visit( cuT( "Contrast" ) );
			block.visit( cuT( "Enable Contrast" ), enableContrast );
			block.visit( cuT( "Contrast Value" ), contrast );
			block.visit( cuT( "Mid. Gray" ), midGray );
		}
		{
			auto block = visitor.visit( cuT( "Hue Shift" ) );
			block.visit( cuT( "Enable Hue Shift" ), enableHueShift );
			block.visit( cuT( "Hue Shift Value" ), hueShift );
		}
		{
			auto block = visitor.visit( cuT( "White Balance" ) );
			block.visit( cuT( "Enable White Balance" ), enableWhiteBalance );
			block.visit( cuT( "White Balance Value" ), ColourWrapper{ &whiteBalance } );
		}
		{
			auto block = visitor.visit( cuT( "Split Toning" ) );
			block.visit( cuT( "Enable Split Toning" ), enableSplitToning );
			block.visit( cuT( "Split Toning Shadows" ), ColourWrapper{ &splitToningShadows } );
			block.visit( cuT( "Split Toning Highlights" ), ColourWrapper{ &splitToningHighlights } );
			block.visit( cuT( "Split Toning Balance" ), splitToningBalance );
		}
		{
			auto block = visitor.visit( cuT( "Channel Mix" ) );
			block.visit( cuT( "Enable Channel Mix" ), enableChannelMix );
			block.visit( cuT( "Channel Mix Red" ), ColourWrapper{ &channelMixRed } );
			block.visit( cuT( "Channel Mix Green" ), ColourWrapper{ &channelMixGreen } );
			block.visit( cuT( "Channel Mix Blue" ), ColourWrapper{ &channelMixBlue } );
		}
		{
			auto block = visitor.visit( cuT( "Shadow Midtone Highlights" ) );
			block.visit( cuT( "Enable Shadow Midtone Highlights" ), enableShadowMidToneHighlight );
			block.visit( cuT( "Shadows" ), ColourWrapper{ &shadows } );
			block.visit( cuT( "Midtones" ), ColourWrapper{ &midtones } );
			block.visit( cuT( "Highlights" ), ColourWrapper{ &highlights } );
			block.visit( cuT( "Shadows Start" ), shadowsStart );
			block.visit( cuT( "Shadows End" ), shadowsEnd );
			block.visit( cuT( "Highlights Start" ), highlightsStart );
			block.visit( cuT( "Highlights End" ), highlightsEnd );
		}
	}

	void ColourGradingConfig::addParsers( castor::AttributeParsers & result )
	{
		using namespace castor;
		BlockParserContextT< CameraContext > cameraContext{ result, CSCNSection::eCamera, CSCNSection::eScene };
		BlockParserContextT< CameraContext > gradingContext{ result, CSCNSection::eColourGrading, CSCNSection::eCamera };

		cameraContext.addPushParser( cuT( "colour_grading" ), CSCNSection::eColourGrading, clrgrdcfg::parserColourGrading );
		gradingContext.addParser( cuT( "enabled" ), clrgrdcfg::parserEnabled, { makeParameter< ParameterType::eBool >() } );
		gradingContext.addParser( cuT( "enable_split_toning" ), clrgrdcfg::parserEnableSplitToning, { makeParameter< ParameterType::eBool >() } );
		gradingContext.addParser( cuT( "enable_contrast" ), clrgrdcfg::parserEnableContrast, { makeParameter< ParameterType::eBool >() } );
		gradingContext.addParser( cuT( "enable_white_balance" ), clrgrdcfg::parserEnableWhiteBalance, { makeParameter< ParameterType::eBool >() } );
		gradingContext.addParser( cuT( "enable_channel_mix" ), clrgrdcfg::parserEnableChannelMix, { makeParameter< ParameterType::eBool >() } );
		gradingContext.addParser( cuT( "enable_shadow_midtone_hilight" ), clrgrdcfg::parserEnableShadowMidToneHighlight, { makeParameter< ParameterType::eBool >() } );
		gradingContext.addParser( cuT( "enable_hue_shift" ), clrgrdcfg::parserEnableHueShift, { makeParameter< ParameterType::eBool >() } );
		gradingContext.addParser( cuT( "post_exposure" ), clrgrdcfg::parserPostExposure, { makeParameter< ParameterType::eFloat >() } );
		gradingContext.addParser( cuT( "mid_gray" ), clrgrdcfg::parserMidGray, { makeParameter< ParameterType::eFloat >() } );
		gradingContext.addParser( cuT( "contrast" ), clrgrdcfg::parserContrast, { makeParameter< ParameterType::eFloat >() } );
		gradingContext.addParser( cuT( "split_toning_balance" ), clrgrdcfg::parserSplitToningBalance, { makeParameter< ParameterType::eFloat >() } );
		gradingContext.addParser( cuT( "saturation" ), clrgrdcfg::parserSaturation, { makeParameter< ParameterType::eFloat >() } );
		gradingContext.addParser( cuT( "hue_shift" ), clrgrdcfg::parserHueShift, { makeParameter< ParameterType::eFloat >() } );
		gradingContext.addParser( cuT( "shadows_range" ), clrgrdcfg::parserShadowsRange, { makeParameter< ParameterType::ePoint2F >() } );
		gradingContext.addParser( cuT( "highlights_range" ), clrgrdcfg::parserHighlightsRange, { makeParameter< ParameterType::ePoint2F >() } );
		gradingContext.addParser( cuT( "white_balance" ), clrgrdcfg::parserWhiteBalance, { makeParameter< ParameterType::ePoint3F >() } );
		gradingContext.addParser( cuT( "colour_filter" ), clrgrdcfg::parserFilter, { makeParameter< ParameterType::ePoint3F >() } );
		gradingContext.addParser( cuT( "split_toning_shadows" ), clrgrdcfg::parserSplitToningShadows, { makeParameter< ParameterType::ePoint3F >() } );
		gradingContext.addParser( cuT( "split_toning_highlights" ), clrgrdcfg::parserSplitToningHighlights, { makeParameter< ParameterType::ePoint3F >() } );
		gradingContext.addParser( cuT( "channel_mix_red" ), clrgrdcfg::parserChannelMixRed, { makeParameter< ParameterType::ePoint3F >() } );
		gradingContext.addParser( cuT( "channel_mix_green" ), clrgrdcfg::parserChannelMixGreen, { makeParameter< ParameterType::ePoint3F >() } );
		gradingContext.addParser( cuT( "channel_mix_blue" ), clrgrdcfg::parserChannelMixBlue, { makeParameter< ParameterType::ePoint3F >() } );
		gradingContext.addParser( cuT( "shadows" ), clrgrdcfg::parserShadows, { makeParameter< ParameterType::ePoint3F >() } );
		gradingContext.addParser( cuT( "midtones" ), clrgrdcfg::parserMidtones, { makeParameter< ParameterType::ePoint3F >() } );
		gradingContext.addParser( cuT( "highlights" ), clrgrdcfg::parserHighlights, { makeParameter< ParameterType::ePoint3F >() } );
		gradingContext.addDefaultPopParser();
	}
}
