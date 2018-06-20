#include <Log/Logger.hpp>

#include <Engine.hpp>
#include <Cache/TargetCache.hpp>
#include <Plugin/PostFxPlugin.hpp>
#include <Render/RenderSystem.hpp>
#include <Render/RenderTarget.hpp>
#include <Scene/SceneFileParser.hpp>
#include <Math/Range.hpp>

#include "SmaaPostEffect.hpp"
#include "Smaa_Parsers.hpp"

#ifndef CASTOR_PLATFORM_WINDOWS
#	define C3D_Smaa_API
#else
#	ifdef SmaaPostEffect_EXPORTS
#		define C3D_Smaa_API __declspec( dllexport )
#	else
#		define C3D_Smaa_API __declspec( dllimport )
#	endif
#endif

namespace
{
	void parseError( castor::String const & error )
	{
		castor::StringStream stream{ castor::makeStringStream() };
		stream << cuT( "Error, : " ) << error;
		castor::Logger::logError( stream.str() );
	}

	void addParser( castor::FileParser::AttributeParsersBySection & parsers
		, uint32_t section
		, castor::String const & name
		, castor::ParserFunction function
		, castor::ParserParameterArray && array = castor::ParserParameterArray{} )
	{
		auto sectionIt = parsers.find( section );

		if ( sectionIt != parsers.end()
			&& sectionIt->second.find( name ) != sectionIt->second.end() )
		{
			parseError( cuT( "Parser " ) + name + cuT( " for section " ) + castor::string::toString( section ) + cuT( " already exists." ) );
		}
		else
		{
			parsers[section][name] = { function, array };
		}
	}

	castor::FileParser::AttributeParsersBySection createParsers()
	{
		static castor::UIntStrMap modes
		{
			{ "1X", uint32_t( smaa::Mode::e1X ) },
			{ "T2X", uint32_t( smaa::Mode::eT2X ) },
			{ "S2X", uint32_t( smaa::Mode::eS2X ) },
			{ "4X", uint32_t( smaa::Mode::e4X ) }
		};
		static castor::UIntStrMap presets
		{
			{ "low", uint32_t( smaa::Preset::eLow ) },
			{ "medium", uint32_t( smaa::Preset::eMedium ) },
			{ "high", uint32_t( smaa::Preset::eHigh ) },
			{ "ultra", uint32_t( smaa::Preset::eUltra ) },
			{ "custom", uint32_t( smaa::Preset::eCustom ) }
		};
		static castor::UIntStrMap detections
		{
			{ "depth", uint32_t( smaa::EdgeDetectionType::eDepth ) },
			{ "colour", uint32_t( smaa::EdgeDetectionType::eColour ) },
			{ "luma", uint32_t( smaa::EdgeDetectionType::eLuma ) }
		};
		castor::FileParser::AttributeParsersBySection result;

		addParser( result, uint32_t( castor3d::CSCNSection::eRenderTarget ), cuT( "smaa" ), &smaa::parserSmaa );

		addParser( result, uint32_t( smaa::SmaaSection::eRoot ), cuT( "mode" ), &smaa::parserMode, { castor::makeParameter< castor::ParameterType::eCheckedText >( modes ) } );
		addParser( result, uint32_t( smaa::SmaaSection::eRoot ), cuT( "preset" ), &smaa::parserPreset, { castor::makeParameter< castor::ParameterType::eCheckedText >( presets ) } );
		addParser( result, uint32_t( smaa::SmaaSection::eRoot ), cuT( "edgeDetection" ), &smaa::parserEdgeDetection, { castor::makeParameter< castor::ParameterType::eCheckedText >( detections ) } );
		addParser( result, uint32_t( smaa::SmaaSection::eRoot ), cuT( "disableDiagonalDetection" ), &smaa::parserDisableDiagonalDetection, { castor::makeParameter< castor::ParameterType::eBool >() } );
		addParser( result, uint32_t( smaa::SmaaSection::eRoot ), cuT( "disableCornerDetection" ), &smaa::parserDisableCornerDetection, { castor::makeParameter< castor::ParameterType::eBool >() } );
		addParser( result, uint32_t( smaa::SmaaSection::eRoot ), cuT( "threshold" ), &smaa::parserThreshold, { castor::makeParameter< castor::ParameterType::eFloat >( castor::makeRange( 0.0f, 0.5f ) ) } );
		addParser( result, uint32_t( smaa::SmaaSection::eRoot ), cuT( "maxSearchSteps" ), &smaa::parserMaxSearchSteps, { castor::makeParameter< castor::ParameterType::eInt32 >( castor::makeRange( 0, 112 ) ) } );
		addParser( result, uint32_t( smaa::SmaaSection::eRoot ), cuT( "maxSearchStepsDiag" ), &smaa::parserMaxSearchStepsDiag, { castor::makeParameter< castor::ParameterType::eInt32 >( castor::makeRange( 0, 20 ) ) } );
		addParser( result, uint32_t( smaa::SmaaSection::eRoot ), cuT( "cornerRounding" ), &smaa::parserCornerRounding, { castor::makeParameter< castor::ParameterType::eInt32 >( castor::makeRange( 0, 100 ) ) } );
		addParser( result, uint32_t( smaa::SmaaSection::eRoot ), cuT( "predication" ), &smaa::parserPredication, { castor::makeParameter< castor::ParameterType::eBool >() } );
		addParser( result, uint32_t( smaa::SmaaSection::eRoot ), cuT( "reprojection" ), &smaa::parserReprojection, { castor::makeParameter< castor::ParameterType::eBool >() } );
		addParser( result, uint32_t( smaa::SmaaSection::eRoot ), cuT( "predicationScale" ), &smaa::parserPredicationScale, { castor::makeParameter< castor::ParameterType::eFloat >( castor::makeRange( 1.0f, 5.0f ) ) } );
		addParser( result, uint32_t( smaa::SmaaSection::eRoot ), cuT( "predicationStrength" ), &smaa::parserPredicationStrength, { castor::makeParameter< castor::ParameterType::eFloat >( castor::makeRange( 0.0f, 1.0f ) ) } );
		addParser( result, uint32_t( smaa::SmaaSection::eRoot ), cuT( "reprojectionWeightScale" ), &smaa::parserReprojectionWeightScale, { castor::makeParameter< castor::ParameterType::eFloat >( castor::makeRange( 0.0f, 80.0f ) ) } );
		addParser( result, uint32_t( smaa::SmaaSection::eRoot ), cuT( "localContrastAdaptationFactor" ), &smaa::parserLocalContrastAdaptationFactor, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParser( result, uint32_t( smaa::SmaaSection::eRoot ), cuT( "predicationThreshold" ), &smaa::parserPredicationThreshold, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParser( result, uint32_t( smaa::SmaaSection::eRoot ), cuT( "}" ), &smaa::parserSmaaEnd );

		return result;
	}

	castor::StrUIntMap createSections()
	{
		return
		{
			{ uint32_t( smaa::SmaaSection::eRoot ), cuT( "smaa" ) },
		};
	}
}

extern "C"
{
	C3D_Smaa_API void getRequiredVersion( castor3d::Version * version )
	{
		*version = castor3d::Version();
	}

	C3D_Smaa_API void getType( castor3d::PluginType * type )
	{
		*type = castor3d::PluginType::ePostEffect;
	}

	C3D_Smaa_API void getName( char const ** name )
	{
		*name = smaa::PostEffect::Name.c_str();
	}

	C3D_Smaa_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * plugin )
	{
		engine->getRenderTargetCache().getPostEffectFactory().registerType( smaa::PostEffect::Type
			, &smaa::PostEffect::create );
		engine->registerParsers( smaa::PostEffect::Type, createParsers() );
		engine->registerSections( smaa::PostEffect::Type, createSections() );
	}

	C3D_Smaa_API void OnUnload( castor3d::Engine * engine )
	{
		engine->unregisterParsers( smaa::PostEffect::Type );
		engine->getRenderTargetCache().getPostEffectFactory().unregisterType( smaa::PostEffect::Type );
	}
}
