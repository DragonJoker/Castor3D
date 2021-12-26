#include "OceanRendering/OceanRenderPass.hpp"
#include "OceanRendering/Ocean_Parsers.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>
#include <Castor3D/Engine.hpp>
#include <Castor3D/Material/Pass/PassFactory.hpp>
#include <Castor3D/Scene/SceneFileParser.hpp>

namespace
{
	void parseError( castor::String const & error )
	{
		castor::StringStream stream{ castor::makeStringStream() };
		stream << cuT( "Error, : " ) << error;
		castor::Logger::logError( stream.str() );
	}

	void addParser( castor::AttributeParsers & parsers
		, uint32_t section
		, castor::String const & name
		, castor::ParserFunction function
		, castor::ParserParameterArray && array = castor::ParserParameterArray{} )
	{
		auto nameIt = parsers.find( name );

		if ( nameIt != parsers.end()
			&& nameIt->second.find( section ) != nameIt->second.end() )
		{
			parseError( cuT( "Parser " ) + name + cuT( " for section " ) + castor::string::toString( section ) + cuT( " already exists." ) );
		}
		else
		{
			parsers[name][section] = { function, array };
		}
	}

	castor::AttributeParsers createParsers()
	{
		castor::AttributeParsers result;

		addParser( result, uint32_t( castor3d::CSCNSection::eRenderTarget ), cuT( "ocean_rendering" ), &ocean::parserOceanRendering );

		addParser( result, uint32_t( ocean::OceanSection::eRoot ), cuT( "tessellationFactor" ), &ocean::parserTessellationFactor, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		addParser( result, uint32_t( ocean::OceanSection::eRoot ), cuT( "dampeningFactor" ), &ocean::parserDampeningFactor, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParser( result, uint32_t( ocean::OceanSection::eRoot ), cuT( "refractionDistortionFactor" ), &ocean::parserRefrDistortionFactor, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParser( result, uint32_t( ocean::OceanSection::eRoot ), cuT( "refractionHeightFactor" ), &ocean::parserRefrHeightFactor, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParser( result, uint32_t( ocean::OceanSection::eRoot ), cuT( "refractionDistanceFactor" ), &ocean::parserRefrDistanceFactor, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParser( result, uint32_t( ocean::OceanSection::eRoot ), cuT( "depthSofteningDistance" ), &ocean::parserDepthSofteningDistance, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParser( result, uint32_t( ocean::OceanSection::eRoot ), cuT( "foamHeightStart" ), &ocean::parserFoamHeightStart, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParser( result, uint32_t( ocean::OceanSection::eRoot ), cuT( "foamFadeDistance" ), &ocean::parserFoamFadeDistance, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParser( result, uint32_t( ocean::OceanSection::eRoot ), cuT( "foamTiling" ), &ocean::parserFoamTiling, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParser( result, uint32_t( ocean::OceanSection::eRoot ), cuT( "foamAngleExponent" ), &ocean::parserFoamAngleExponent, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParser( result, uint32_t( ocean::OceanSection::eRoot ), cuT( "foamBrightness" ), &ocean::parserFoamBrightness, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParser( result, uint32_t( ocean::OceanSection::eRoot ), cuT( "normalMapScrollSpeed" ), &ocean::parserNormalMapScrollSpeed, { castor::makeParameter< castor::ParameterType::ePoint2F >() } );
		addParser( result, uint32_t( ocean::OceanSection::eRoot ), cuT( "normalMapScroll" ), &ocean::parserNormalMapScroll, { castor::makeParameter< castor::ParameterType::ePoint4F >() } );
		addParser( result, uint32_t( ocean::OceanSection::eRoot ), cuT( "ssrStepSize" ), &ocean::parserSsrStepSize, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParser( result, uint32_t( ocean::OceanSection::eRoot ), cuT( "ssrForwardStepsCount" ), &ocean::parserSsrFwdStepCount, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParser( result, uint32_t( ocean::OceanSection::eRoot ), cuT( "ssrBackwardStepsCount" ), &ocean::parserSsrBckStepCount, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParser( result, uint32_t( ocean::OceanSection::eRoot ), cuT( "ssrDepthMult" ), &ocean::parserSsrDepthMult, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParser( result, uint32_t( ocean::OceanSection::eRoot ), cuT( "foam" ), &ocean::parserFoam, { castor::makeParameter< castor::ParameterType::ePath >() } );
		addParser( result, uint32_t( ocean::OceanSection::eRoot ), cuT( "normals1" ), &ocean::parserNormals1, { castor::makeParameter< castor::ParameterType::ePath >() } );
		addParser( result, uint32_t( ocean::OceanSection::eRoot ), cuT( "normals2" ), &ocean::parserNormals2, { castor::makeParameter< castor::ParameterType::ePath >() } );
		addParser( result, uint32_t( ocean::OceanSection::eRoot ), cuT( "noise" ), &ocean::parserNoise, { castor::makeParameter< castor::ParameterType::ePath >() } );
		addParser( result, uint32_t( ocean::OceanSection::eRoot ), cuT( "}" ), &ocean::parserWaveRenderingEnd );
		addParser( result, uint32_t( ocean::OceanSection::eRoot ), cuT( "waves" ), &ocean::parserWaves );

		addParser( result, uint32_t( ocean::OceanSection::eWaves ), cuT( "}" ), &ocean::parserWavesEnd );
		addParser( result, uint32_t( ocean::OceanSection::eWaves ), cuT( "wave" ), &ocean::parserWave );

		addParser( result, uint32_t( ocean::OceanSection::eWave ), cuT( "direction" ), &ocean::parserWaveDirection, { castor::makeParameter< castor::ParameterType::ePoint3F >() } );
		addParser( result, uint32_t( ocean::OceanSection::eWave ), cuT( "steepness" ), &ocean::parserWaveSteepness, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParser( result, uint32_t( ocean::OceanSection::eWave ), cuT( "length" ), &ocean::parserWaveLength, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParser( result, uint32_t( ocean::OceanSection::eWave ), cuT( "amplitude" ), &ocean::parserWaveAmplitude, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParser( result, uint32_t( ocean::OceanSection::eWave ), cuT( "speed" ), &ocean::parserWaveSpeed, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParser( result, uint32_t( ocean::OceanSection::eWave ), cuT( "}" ), &ocean::parserWaveEnd );

		return result;
	}

	castor::StrUInt32Map createSections()
	{
		return
		{
			{ uint32_t( ocean::OceanSection::eRoot ), cuT( "ocean_rendering" ) },
			{ uint32_t( ocean::OceanSection::eWaves ), cuT( "waves" ) },
			{ uint32_t( ocean::OceanSection::eWave ), cuT( "wave" ) },
		};
	}

	void * createContext( castor::FileParserContext & context )
	{
		ocean::ParserContext * userContext = new ocean::ParserContext;
		userContext->engine = static_cast< castor3d::SceneFileParser * >( context.parser )->getEngine();
		return userContext;
	}
}

#ifndef CU_PlatformWindows
#	define C3D_OceanRendering_API
#else
#	ifdef OceanRendering_EXPORTS
#		define C3D_OceanRendering_API __declspec( dllexport )
#	else
#		define C3D_OceanRendering_API __declspec( dllimport )
#	endif
#endif

extern "C"
{
	C3D_OceanRendering_API void getRequiredVersion( castor3d::Version * p_version );
	C3D_OceanRendering_API void getType( castor3d::PluginType * p_type );
	C3D_OceanRendering_API void getName( char const ** p_name );
	C3D_OceanRendering_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * p_plugin );
	C3D_OceanRendering_API void OnUnload( castor3d::Engine * engine );

	C3D_OceanRendering_API void getRequiredVersion( castor3d::Version * version )
	{
		*version = castor3d::Version();
	}

	C3D_OceanRendering_API void getType( castor3d::PluginType * type )
	{
		*type = castor3d::PluginType::eGeneric;
	}

	C3D_OceanRendering_API void getName( char const ** name )
	{
		*name = ocean::OceanRenderPass::FullName.c_str();
	}

	C3D_OceanRendering_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * plugin )
	{
		auto event = castor::makeUnique< castor3d::RenderPassRegisterInfo >( ocean::OceanRenderPass::Type
			, &ocean::OceanRenderPass::create
			, ocean::OceanRenderPass::Event );
		engine->registerRenderPassType( ocean::OceanRenderPass::Type, std::move( event ) );
		engine->registerParsers( ocean::OceanRenderPass::Type
			, createParsers()
			, createSections()
			, &createContext );
	}

	C3D_OceanRendering_API void OnUnload( castor3d::Engine * engine )
	{
		engine->unregisterParsers( ocean::OceanRenderPass::Type );
		engine->unregisterRenderPassType( ocean::OceanRenderPass::Type );
	}
}
