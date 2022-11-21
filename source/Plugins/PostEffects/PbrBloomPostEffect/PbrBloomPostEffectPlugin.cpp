#include "PbrBloomPostEffect/PbrBloomPostEffect.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/TargetCache.hpp>
#include <Castor3D/Plugin/PostFxPlugin.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Scene/SceneFileParser.hpp>

#include <CastorUtils/FileParser/ParserParameter.hpp>
#include <CastorUtils/Log/Logger.hpp>

#ifndef CU_PlatformWindows
#	define C3D_PbrBloom_API
#else
#	ifdef PbrBloomPostEffect_EXPORTS
#		define C3D_PbrBloom_API __declspec( dllexport )
#	else
#		define C3D_PbrBloom_API __declspec( dllimport )
#	endif
#endif

namespace PbrBloom
{
	struct ParserContext
	{
		float bloomStrength{ BaseBloomStrength };
		uint32_t blurRadius{ BaseBlurRadius };
		uint32_t passes{ BaseFilterCount };
	};

	enum class PbrBloomSection
		: uint32_t
	{
		eRoot = CU_MakeSectionName( 'P', 'B', 'B', 'M' ),
	};

	static ParserContext & getParserContext( castor::FileParserContext & context )
	{
		return *static_cast< ParserContext * >( context.getUserContext( PostEffect::Type ) );
	}

	static CU_ImplementAttributeParser( parserPbrBloom )
	{
	}
	CU_EndAttributePush( PbrBloomSection::eRoot )

	static CU_ImplementAttributeParser( parserBloomStrength )
	{
		auto & bloomContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			float value;
			params[0]->get( value );
			bloomContext.bloomStrength = value;
		}
	}
	CU_EndAttribute()

	static CU_ImplementAttributeParser( parserBlurRadius )
	{
		auto & bloomContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			uint32_t value;
			params[0]->get( value );
			bloomContext.blurRadius = value;
		}
	}
	CU_EndAttribute()

	static CU_ImplementAttributeParser( parserPasses )
	{
		auto & bloomContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			uint32_t value;
			params[0]->get( value );
			bloomContext.passes = value;
		}
	}
	CU_EndAttribute()

	static CU_ImplementAttributeParser( parserBloomEnd )
	{
		auto & bloomContext = getParserContext( context );
		castor3d::Parameters parameters;
		parameters.add( cuT( "bloomStrength" )
			, castor::string::toString( bloomContext.bloomStrength ) );
		parameters.add( cuT( "blurRadius" )
			, castor::string::toString( bloomContext.blurRadius ) );
		parameters.add( cuT( "passes" )
			, castor::string::toString( bloomContext.passes ) );

		auto & parsingContext = castor3d::getParserContext( context );
		auto effect = parsingContext.renderTarget->getPostEffect( PostEffect::Type );
		effect->enable( true );
		effect->setParameters( parameters );

		delete reinterpret_cast< ParserContext * >( context.unregisterUserContext( PostEffect::Type ) );
	}
	CU_EndAttributePop()

	static castor::AttributeParsers createParsers()
	{
		castor::AttributeParsers result;
		addParser( result
			, uint32_t( castor3d::CSCNSection::eRenderTarget )
			, cuT( "pbr_bloom" )
			, &parserPbrBloom );
		addParser( result
			, uint32_t( PbrBloomSection::eRoot )
			, cuT( "bloomStrength" )
			, &parserBloomStrength
			, { castor::makeParameter< castor::ParameterType::eFloat >( castor::makeRange( 0.0f, 1.0f ) ) } );
		addParser( result
			, uint32_t( PbrBloomSection::eRoot )
			, cuT( "blurRadius" )
			, &parserBlurRadius
			, { castor::makeParameter< castor::ParameterType::eUInt32 >( castor::makeRange( 1u, 10u ) ) } );
		addParser( result
			, uint32_t( PbrBloomSection::eRoot )
			, cuT( "passes" )
			, &parserPasses
			, { castor::makeParameter< castor::ParameterType::eUInt32 >( castor::makeRange( 1u, 10u ) ) } );
		addParser( result
			, uint32_t( PbrBloomSection::eRoot )
			, cuT( "}" )
			, &parserBloomEnd );
		return result;
	}

	static castor::StrUInt32Map createSections()
	{
		return
		{
			{ uint32_t( PbrBloomSection::eRoot ), cuT( "pbr_bloom" ) },
		};
	}

	static void * createContext( castor::FileParserContext & context )
	{
		return new ParserContext;
	}
}

extern "C"
{
	C3D_PbrBloom_API void getRequiredVersion( castor3d::Version * p_version );
	C3D_PbrBloom_API void getType( castor3d::PluginType * p_type );
	C3D_PbrBloom_API void getName( char const ** p_name );
	C3D_PbrBloom_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * p_plugin );
	C3D_PbrBloom_API void OnUnload( castor3d::Engine * engine );

	C3D_PbrBloom_API void getRequiredVersion( castor3d::Version * p_version )
	{
		*p_version = castor3d::Version();
	}

	C3D_PbrBloom_API void getType( castor3d::PluginType * p_type )
	{
		*p_type = castor3d::PluginType::ePostEffect;
	}

	C3D_PbrBloom_API void getName( char const ** p_name )
	{
		*p_name = PbrBloom::PostEffect::Name.c_str();
	}

	C3D_PbrBloom_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * p_plugin )
	{
		engine->getPostEffectFactory().registerType( PbrBloom::PostEffect::Type
			, &PbrBloom::PostEffect::create );
		engine->registerParsers( PbrBloom::PostEffect::Type
			, PbrBloom::createParsers()
			, PbrBloom::createSections()
			, &PbrBloom::createContext );
	}

	C3D_PbrBloom_API void OnUnload( castor3d::Engine * engine )
	{
		engine->unregisterParsers( PbrBloom::PostEffect::Type );
		engine->getPostEffectFactory().unregisterType( PbrBloom::PostEffect::Type );
	}
}
