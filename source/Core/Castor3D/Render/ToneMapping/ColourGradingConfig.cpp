#include "Castor3D/Render/ToneMapping/ColourGradingConfig.hpp"

#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>

namespace castor3d
{
	namespace clrgrdcfg
	{
		static CU_ImplementAttributeParser( parserColourGrading )
		{
		}
		CU_EndAttributePush( CSCNSection::eColourGrading )

		static CU_ImplementAttributeParser( parserPostExposure )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( parsingContext.colourGradingConfig.postExposure );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserEnabled )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( parsingContext.colourGradingConfig.enabled );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserEnableSplitToning )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( parsingContext.colourGradingConfig.enableSplitToning );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserEnableShadowMidToneHighlight )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( parsingContext.colourGradingConfig.enableShadowMidToneHighlight );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserEnableChannelMix )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( parsingContext.colourGradingConfig.enableChannelMix );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserEnableWhiteBalance )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( parsingContext.colourGradingConfig.enableWhiteBalance );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserEnableHueShift )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( parsingContext.colourGradingConfig.enableHueShift );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserEnableContrast )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( parsingContext.colourGradingConfig.enableContrast );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserWhiteBalance )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( parsingContext.colourGradingConfig.whiteBalance );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserFilter )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( parsingContext.colourGradingConfig.colourFilter );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserMidGray )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( parsingContext.colourGradingConfig.midGray );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserContrast )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( parsingContext.colourGradingConfig.contrast );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserSaturation )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( parsingContext.colourGradingConfig.saturation );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserHueShift )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( parsingContext.colourGradingConfig.hueShift );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserSplitToningShadows )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( parsingContext.colourGradingConfig.splitToningShadows );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserSplitToningHighlights )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( parsingContext.colourGradingConfig.splitToningHighlights );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserSplitToningBalance )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( parsingContext.colourGradingConfig.splitToningBalance );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserChannelMixRed )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( parsingContext.colourGradingConfig.channelMixRed );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserChannelMixGreen )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( parsingContext.colourGradingConfig.channelMixGreen );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserChannelMixBlue )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( parsingContext.colourGradingConfig.channelMixBlue );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserShadowsRange )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				castor::Point2f range;
				params[0]->get( range );
				parsingContext.colourGradingConfig.shadowsStart = range->x;
				parsingContext.colourGradingConfig.shadowsEnd = range->y;
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserHighlightsRange )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				castor::Point2f range;
				params[0]->get( range );
				parsingContext.colourGradingConfig.highlightsStart = range->x;
				parsingContext.colourGradingConfig.highlightsEnd = range->y;
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserShadows )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( parsingContext.colourGradingConfig.shadows );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserMidtones )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( parsingContext.colourGradingConfig.midtones );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserHighlights )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( parsingContext.colourGradingConfig.highlights );
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
		visitor.visit( cuT( "Colour Filter" ), colourFilter );
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
			block.visit( cuT( "White Balance Value" ), whiteBalance );
		}
		{
			auto block = visitor.visit( cuT( "Split Toning" ) );
			block.visit( cuT( "Enable Split Toning" ), enableSplitToning );
			block.visit( cuT( "Split Toning Shadows" ), splitToningShadows );
			block.visit( cuT( "Split Toning Highlights" ), splitToningHighlights );
			block.visit( cuT( "Split Toning Balance" ), splitToningBalance );
		}
		{
			auto block = visitor.visit( cuT( "Channel Mix" ) );
			block.visit( cuT( "Enable Channel Mix" ), enableChannelMix );
			block.visit( cuT( "Channel Mix Red" ), channelMixRed );
			block.visit( cuT( "Channel Mix Green" ), channelMixGreen );
			block.visit( cuT( "Channel Mix Blue" ), channelMixBlue );
		}
		{
			auto block = visitor.visit( cuT( "Shadow Midtone Highlights" ) );
			block.visit( cuT( "Enable Shadow Midtone Highlights" ), enableShadowMidToneHighlight );
			block.visit( cuT( "Shadows" ), shadows );
			block.visit( cuT( "Midtones" ), midtones );
			block.visit( cuT( "Highlights" ), highlights );
			block.visit( cuT( "Shadows Start" ), shadowsStart );
			block.visit( cuT( "Shadows End" ), shadowsEnd );
			block.visit( cuT( "Highlights Start" ), highlightsStart );
			block.visit( cuT( "Highlights End" ), highlightsEnd );
		}
	}

	void ColourGradingConfig::addParsers( castor::AttributeParsers & result )
	{
		using namespace castor;
		addParser( result, uint32_t( CSCNSection::eCamera ), cuT( "colour_grading" ), clrgrdcfg::parserColourGrading );
		addParser( result, uint32_t( CSCNSection::eColourGrading ), cuT( "enabled" ), clrgrdcfg::parserEnabled, { makeParameter< ParameterType::eBool >() } );
		addParser( result, uint32_t( CSCNSection::eColourGrading ), cuT( "enable_split_toning" ), clrgrdcfg::parserEnableSplitToning, { makeParameter< ParameterType::eBool >() } );
		addParser( result, uint32_t( CSCNSection::eColourGrading ), cuT( "enable_contrast" ), clrgrdcfg::parserEnableContrast, { makeParameter< ParameterType::eBool >() } );
		addParser( result, uint32_t( CSCNSection::eColourGrading ), cuT( "enable_white_balance" ), clrgrdcfg::parserEnableWhiteBalance, { makeParameter< ParameterType::eBool >() } );
		addParser( result, uint32_t( CSCNSection::eColourGrading ), cuT( "enable_channel_mix" ), clrgrdcfg::parserEnableChannelMix, { makeParameter< ParameterType::eBool >() } );
		addParser( result, uint32_t( CSCNSection::eColourGrading ), cuT( "enable_shadow_midtone_hilight" ), clrgrdcfg::parserEnableShadowMidToneHighlight, { makeParameter< ParameterType::eBool >() } );
		addParser( result, uint32_t( CSCNSection::eColourGrading ), cuT( "enable_hue_shift" ), clrgrdcfg::parserEnableHueShift, { makeParameter< ParameterType::eBool >() } );
		addParser( result, uint32_t( CSCNSection::eColourGrading ), cuT( "post_exposure" ), clrgrdcfg::parserPostExposure, { makeParameter< ParameterType::eFloat >() } );
		addParser( result, uint32_t( CSCNSection::eColourGrading ), cuT( "mid_gray" ), clrgrdcfg::parserMidGray, { makeParameter< ParameterType::eFloat >() } );
		addParser( result, uint32_t( CSCNSection::eColourGrading ), cuT( "contrast" ), clrgrdcfg::parserContrast, { makeParameter< ParameterType::eFloat >() } );
		addParser( result, uint32_t( CSCNSection::eColourGrading ), cuT( "split_toning_balance" ), clrgrdcfg::parserSplitToningBalance, { makeParameter< ParameterType::eFloat >() } );
		addParser( result, uint32_t( CSCNSection::eColourGrading ), cuT( "saturation" ), clrgrdcfg::parserSaturation, { makeParameter< ParameterType::eFloat >() } );
		addParser( result, uint32_t( CSCNSection::eColourGrading ), cuT( "hue_shift" ), clrgrdcfg::parserHueShift, { makeParameter< ParameterType::eFloat >() } );
		addParser( result, uint32_t( CSCNSection::eColourGrading ), cuT( "shadows_range" ), clrgrdcfg::parserShadowsRange, { makeParameter< ParameterType::ePoint2F >() } );
		addParser( result, uint32_t( CSCNSection::eColourGrading ), cuT( "highlights_range" ), clrgrdcfg::parserHighlightsRange, { makeParameter< ParameterType::ePoint2F >() } );
		addParser( result, uint32_t( CSCNSection::eColourGrading ), cuT( "white_balance" ), clrgrdcfg::parserWhiteBalance, { makeParameter< ParameterType::ePoint3F >() } );
		addParser( result, uint32_t( CSCNSection::eColourGrading ), cuT( "colour_filter" ), clrgrdcfg::parserFilter, { makeParameter< ParameterType::ePoint3F >() } );
		addParser( result, uint32_t( CSCNSection::eColourGrading ), cuT( "split_toning_shadows" ), clrgrdcfg::parserSplitToningShadows, { makeParameter< ParameterType::ePoint3F >() } );
		addParser( result, uint32_t( CSCNSection::eColourGrading ), cuT( "split_toning_highlights" ), clrgrdcfg::parserSplitToningHighlights, { makeParameter< ParameterType::ePoint3F >() } );
		addParser( result, uint32_t( CSCNSection::eColourGrading ), cuT( "channel_mix_red" ), clrgrdcfg::parserChannelMixRed, { makeParameter< ParameterType::ePoint3F >() } );
		addParser( result, uint32_t( CSCNSection::eColourGrading ), cuT( "channel_mix_green" ), clrgrdcfg::parserChannelMixGreen, { makeParameter< ParameterType::ePoint3F >() } );
		addParser( result, uint32_t( CSCNSection::eColourGrading ), cuT( "channel_mix_blue" ), clrgrdcfg::parserChannelMixBlue, { makeParameter< ParameterType::ePoint3F >() } );
		addParser( result, uint32_t( CSCNSection::eColourGrading ), cuT( "shadows" ), clrgrdcfg::parserShadows, { makeParameter< ParameterType::ePoint3F >() } );
		addParser( result, uint32_t( CSCNSection::eColourGrading ), cuT( "midtones" ), clrgrdcfg::parserMidtones, { makeParameter< ParameterType::ePoint3F >() } );
		addParser( result, uint32_t( CSCNSection::eColourGrading ), cuT( "highlights" ), clrgrdcfg::parserHighlights, { makeParameter< ParameterType::ePoint3F >() } );
	}
}
