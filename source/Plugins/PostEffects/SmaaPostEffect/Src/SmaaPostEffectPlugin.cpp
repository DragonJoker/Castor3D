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
	void parseError( castor::String const & p_error )
	{
		castor::StringStream strError;
		strError << cuT( "Error, : " ) << p_error;
		castor::Logger::logError( strError.str() );
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
			{ "1X", uint32_t( smaa::PostEffect::Mode::e1X ) },
			{ "T2X", uint32_t( smaa::PostEffect::Mode::eT2X ) },
			{ "S2X", uint32_t( smaa::PostEffect::Mode::eS2X ) },
			{ "4X", uint32_t( smaa::PostEffect::Mode::e4X ) }
		};
		static castor::UIntStrMap presets
		{
			{ "low", uint32_t( smaa::PostEffect::Preset::eLow ) },
			{ "medium", uint32_t( smaa::PostEffect::Preset::eMedium ) },
			{ "high", uint32_t( smaa::PostEffect::Preset::eHigh ) },
			{ "ultra", uint32_t( smaa::PostEffect::Preset::eUltra ) },
			{ "custom", uint32_t( smaa::PostEffect::Preset::eCustom ) }
		};
		castor::FileParser::AttributeParsersBySection result;

		addParser( result, uint32_t( castor3d::CSCNSection::eRenderTarget ), cuT( "smaa" ), &smaa::parserSmaa );

		addParser( result, uint32_t( smaa::SmaaSection::eRoot ), cuT( "mode" ), &smaa::parserMode, { castor::makeParameter< castor::ParameterType::eCheckedText >( modes ) } );
		addParser( result, uint32_t( smaa::SmaaSection::eRoot ), cuT( "preset" ), &smaa::parserPreset, { castor::makeParameter< castor::ParameterType::eCheckedText >( presets ) } );
		addParser( result, uint32_t( smaa::SmaaSection::eRoot ), cuT( "threshold" ), &smaa::parserThreshold, { castor::makeParameter< castor::ParameterType::eFloat >( castor::makeRange( 0.0f, 0.5f ) ) } );
		addParser( result, uint32_t( smaa::SmaaSection::eRoot ), cuT( "maxSearchSteps" ), &smaa::parserMaxSearchSteps, { castor::makeParameter< castor::ParameterType::eInt32 >( castor::makeRange( 0, 98 ) ) } );
		addParser( result, uint32_t( smaa::SmaaSection::eRoot ), cuT( "maxSearchStepsDiag" ), &smaa::parserMaxSearchStepsDiag, { castor::makeParameter< castor::ParameterType::eInt32 >( castor::makeRange( 0, 20 ) ) } );
		addParser( result, uint32_t( smaa::SmaaSection::eRoot ), cuT( "cornerRounding" ), &smaa::parserCornerRounding, { castor::makeParameter< castor::ParameterType::eInt32 >( castor::makeRange( 0, 100 ) ) } );
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
