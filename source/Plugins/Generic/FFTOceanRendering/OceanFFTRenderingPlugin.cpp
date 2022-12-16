#include "FFTOceanRendering/OceanFFTRenderPass.hpp"
#include "FFTOceanRendering/OceanFFT_Parsers.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>
#include <Castor3D/Engine.hpp>
#include <Castor3D/Material/Pass/PassFactory.hpp>
#include <Castor3D/Scene/SceneFileParser.hpp>

namespace
{
	castor::AttributeParsers createParsers()
	{
		castor::AttributeParsers result;

		addParser( result, uint32_t( castor3d::CSCNSection::eRenderTarget ), cuT( "fft_ocean_rendering" ), &ocean_fft::parserOceanRendering );

		addParser( result, uint32_t( ocean_fft::OceanSection::eRoot ), cuT( "depthSofteningDistance" ), &ocean_fft::parserDepthSofteningDistance, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParser( result, uint32_t( ocean_fft::OceanSection::eRoot ), cuT( "refractionRatio" ), &ocean_fft::parserRefrRatio, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParser( result, uint32_t( ocean_fft::OceanSection::eRoot ), cuT( "refractionDistortionFactor" ), &ocean_fft::parserRefrDistortionFactor, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParser( result, uint32_t( ocean_fft::OceanSection::eRoot ), cuT( "refractionHeightFactor" ), &ocean_fft::parserRefrHeightFactor, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParser( result, uint32_t( ocean_fft::OceanSection::eRoot ), cuT( "refractionDistanceFactor" ), &ocean_fft::parserRefrDistanceFactor, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParser( result, uint32_t( ocean_fft::OceanSection::eRoot ), cuT( "ssrStepSize" ), &ocean_fft::parserSsrStepSize, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParser( result, uint32_t( ocean_fft::OceanSection::eRoot ), cuT( "ssrForwardStepsCount" ), &ocean_fft::parserSsrFwdStepCount, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParser( result, uint32_t( ocean_fft::OceanSection::eRoot ), cuT( "ssrBackwardStepsCount" ), &ocean_fft::parserSsrBckStepCount, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParser( result, uint32_t( ocean_fft::OceanSection::eRoot ), cuT( "ssrDepthMult" ), &ocean_fft::parserSsrDepthMult, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParser( result, uint32_t( ocean_fft::OceanSection::eRoot ), cuT( "density" ), &ocean_fft::parserDensity, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParser( result, uint32_t( ocean_fft::OceanSection::eRoot ), cuT( "fft_config" ), &ocean_fft::parserFftConfig );
		addParser( result, uint32_t( ocean_fft::OceanSection::eRoot ), cuT( "}" ), &ocean_fft::parserOceanRenderingEnd );

		addParser( result, uint32_t( ocean_fft::OceanSection::eFFT ), cuT( "disableRandomSeed" ), &ocean_fft::parserFftDisableRandomSeed, { castor::makeParameter< castor::ParameterType::eBool >() } );
		addParser( result, uint32_t( ocean_fft::OceanSection::eFFT ), cuT( "size" ), &ocean_fft::parserFftSize, { castor::makeParameter< castor::ParameterType::ePoint2F >() } );
		addParser( result, uint32_t( ocean_fft::OceanSection::eFFT ), cuT( "heightMapSamples" ), &ocean_fft::parserFftHeightMapSamples, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		addParser( result, uint32_t( ocean_fft::OceanSection::eFFT ), cuT( "displacementDownsample" ), &ocean_fft::parserFftDisplacementDownsample, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		addParser( result, uint32_t( ocean_fft::OceanSection::eFFT ), cuT( "normalMapFreqMod" ), &ocean_fft::parserFftNormalMapFreqMod, { castor::makeParameter< castor::ParameterType::ePoint2F >() } );
		addParser( result, uint32_t( ocean_fft::OceanSection::eFFT ), cuT( "amplitude" ), &ocean_fft::parserFftAmplitude, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParser( result, uint32_t( ocean_fft::OceanSection::eFFT ), cuT( "windDirection" ), &ocean_fft::parserFftWindDirection, { castor::makeParameter< castor::ParameterType::ePoint2F >() } );
		addParser( result, uint32_t( ocean_fft::OceanSection::eFFT ), cuT( "windVelocity" ), &ocean_fft::parserFftWindVelocity, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParser( result, uint32_t( ocean_fft::OceanSection::eFFT ), cuT( "patchSize" ), &ocean_fft::parserFftPatchSize, { castor::makeParameter< castor::ParameterType::ePoint2F >() } );
		addParser( result, uint32_t( ocean_fft::OceanSection::eFFT ), cuT( "blocksCount" ), &ocean_fft::parserFftBlocksCount, { castor::makeParameter< castor::ParameterType::ePoint2U >() } );
		addParser( result, uint32_t( ocean_fft::OceanSection::eFFT ), cuT( "lod0Distance" ), &ocean_fft::parserFftLOD0Distance, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParser( result, uint32_t( ocean_fft::OceanSection::eFFT ), cuT( "}" ), &ocean_fft::parserFftConfigEnd );

		return result;
	}

	castor::StrUInt32Map createSections()
	{
		return
		{
			{ uint32_t( ocean_fft::OceanSection::eRoot ), cuT( "fft_ocean_rendering" ) },
			{ uint32_t( ocean_fft::OceanSection::eFFT ), cuT( "fft_config" ) },
		};
	}

	void * createContext( castor::FileParserContext & context )
	{
		auto userContext = new ocean_fft::ParserContext;
		userContext->engine = static_cast< castor3d::SceneFileParser * >( context.parser )->getEngine();
		return userContext;
	}
}

#ifndef CU_PlatformWindows
#	define C3D_OceanRendering_API
#else
#	ifdef FFTOceanRendering_EXPORTS
#		define C3D_OceanRendering_API __declspec( dllexport )
#	else
#		define C3D_OceanRendering_API __declspec( dllimport )
#	endif
#endif

extern "C"
{
	C3D_OceanRendering_API void getRequiredVersion( castor3d::Version * version );
	C3D_OceanRendering_API void getType( castor3d::PluginType * type );
	C3D_OceanRendering_API void getName( char const ** name );
	C3D_OceanRendering_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * plugin );
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
		*name = ocean_fft::OceanRenderPass::FullName.c_str();
	}

	C3D_OceanRendering_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * plugin )
	{
		auto event = castor::makeUnique< castor3d::RenderPassRegisterInfo >( ocean_fft::OceanRenderPass::Type
			, &ocean_fft::OceanRenderPass::create
			, ocean_fft::OceanRenderPass::Event );
		engine->registerRenderPassType( ocean_fft::OceanRenderPass::Type, std::move( event ) );
		engine->registerParsers( ocean_fft::OceanRenderPass::Type
			, createParsers()
			, createSections()
			, &createContext );
	}

	C3D_OceanRendering_API void OnUnload( castor3d::Engine * engine )
	{
		engine->unregisterParsers( ocean_fft::OceanRenderPass::Type );
		engine->unregisterRenderPassType( ocean_fft::OceanRenderPass::Type );
	}
}
