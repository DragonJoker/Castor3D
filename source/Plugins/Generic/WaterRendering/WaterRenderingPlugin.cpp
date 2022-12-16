#include "WaterRendering/WaterRenderPass.hpp"
#include "WaterRendering/Water_Parsers.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>
#include <Castor3D/Engine.hpp>
#include <Castor3D/Material/Pass/PassFactory.hpp>
#include <Castor3D/Scene/SceneFileParser.hpp>

namespace
{
	castor::AttributeParsers createParsers()
	{
		castor::AttributeParsers result;

		addParser( result, uint32_t( castor3d::CSCNSection::eRenderTarget ), cuT( "water_rendering" ), &water::parserWaterRendering );

		addParser( result, uint32_t( water::WaterSection::eRoot ), cuT( "dampeningFactor" ), &water::parserDampeningFactor, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParser( result, uint32_t( water::WaterSection::eRoot ), cuT( "refractionDistortionFactor" ), &water::parserRefrDistortionFactor, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParser( result, uint32_t( water::WaterSection::eRoot ), cuT( "refractionHeightFactor" ), &water::parserRefrHeightFactor, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParser( result, uint32_t( water::WaterSection::eRoot ), cuT( "refractionDistanceFactor" ), &water::parserRefrDistanceFactor, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParser( result, uint32_t( water::WaterSection::eRoot ), cuT( "refractionRatio" ), &water::parserRefrRatio, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParser( result, uint32_t( water::WaterSection::eRoot ), cuT( "depthSofteningDistance" ), &water::parserDepthSofteningDistance, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParser( result, uint32_t( water::WaterSection::eRoot ), cuT( "normalMapScrollSpeed" ), &water::parserNormalMapScrollSpeed, { castor::makeParameter< castor::ParameterType::ePoint2F >() } );
		addParser( result, uint32_t( water::WaterSection::eRoot ), cuT( "normalMapScroll" ), &water::parserNormalMapScroll, { castor::makeParameter< castor::ParameterType::ePoint4F >() } );
		addParser( result, uint32_t( water::WaterSection::eRoot ), cuT( "ssrStepSize" ), &water::parserSsrStepSize, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParser( result, uint32_t( water::WaterSection::eRoot ), cuT( "ssrForwardStepsCount" ), &water::parserSsrFwdStepCount, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParser( result, uint32_t( water::WaterSection::eRoot ), cuT( "ssrBackwardStepsCount" ), &water::parserSsrBckStepCount, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParser( result, uint32_t( water::WaterSection::eRoot ), cuT( "ssrDepthMult" ), &water::parserSsrDepthMult, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParser( result, uint32_t( water::WaterSection::eRoot ), cuT( "normals1" ), &water::parserNormals1, { castor::makeParameter< castor::ParameterType::ePath >() } );
		addParser( result, uint32_t( water::WaterSection::eRoot ), cuT( "normals2" ), &water::parserNormals2, { castor::makeParameter< castor::ParameterType::ePath >() } );
		addParser( result, uint32_t( water::WaterSection::eRoot ), cuT( "noise" ), &water::parserNoise, { castor::makeParameter< castor::ParameterType::ePath >() } );
		addParser( result, uint32_t( water::WaterSection::eRoot ), cuT( "}" ), &water::parserWaterRenderingEnd );

		return result;
	}

	castor::StrUInt32Map createSections()
	{
		return
		{
			{ uint32_t( water::WaterSection::eRoot ), cuT( "water_rendering" ) },
		};
	}

	void * createContext( castor::FileParserContext & context )
	{
		water::ParserContext * userContext = new water::ParserContext;
		userContext->engine = static_cast< castor3d::SceneFileParser * >( context.parser )->getEngine();
		return userContext;
	}
}

#ifndef CU_PlatformWindows
#	define C3D_WaterRendering_API
#else
#	ifdef WaterRendering_EXPORTS
#		define C3D_WaterRendering_API __declspec( dllexport )
#	else
#		define C3D_WaterRendering_API __declspec( dllimport )
#	endif
#endif

extern "C"
{
	C3D_WaterRendering_API void getRequiredVersion( castor3d::Version * version );
	C3D_WaterRendering_API void getType( castor3d::PluginType * type );
	C3D_WaterRendering_API void getName( char const ** name );
	C3D_WaterRendering_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * plugin );
	C3D_WaterRendering_API void OnUnload( castor3d::Engine * engine );

	C3D_WaterRendering_API void getRequiredVersion( castor3d::Version * version )
	{
		*version = castor3d::Version();
	}

	C3D_WaterRendering_API void getType( castor3d::PluginType * type )
	{
		*type = castor3d::PluginType::eGeneric;
	}

	C3D_WaterRendering_API void getName( char const ** name )
	{
		*name = water::WaterRenderPass::FullName.c_str();
	}

	C3D_WaterRendering_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * plugin )
	{
		auto event = castor::makeUnique< castor3d::RenderPassRegisterInfo >( water::WaterRenderPass::Type
			, &water::WaterRenderPass::create
			, water::WaterRenderPass::Event );
		engine->registerRenderPassType( water::WaterRenderPass::Type, std::move( event ) );
		engine->registerParsers( water::WaterRenderPass::Type
			, createParsers()
			, createSections()
			, &createContext );
	}

	C3D_WaterRendering_API void OnUnload( castor3d::Engine * engine )
	{
		engine->unregisterParsers( water::WaterRenderPass::Type );
		engine->unregisterRenderPassType( water::WaterRenderPass::Type );
	}
}
