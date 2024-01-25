#include "PbrBloomPostEffect/PbrBloomPostEffect.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/TargetCache.hpp>
#include <Castor3D/Plugin/PostFxPlugin.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Scene/SceneFileParser.hpp>

#include <CastorUtils/FileParser/FileParser.hpp>
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
	struct BloomContext
	{
		castor3d::RenderTargetRPtr renderTarget{};
		float bloomStrength{ BaseBloomStrength };
		uint32_t blurRadius{ BaseBlurRadius };
		uint32_t passes{ BaseFilterCount };
	};

	enum class PbrBloomSection
		: uint32_t
	{
		eRoot = CU_MakeSectionName( 'P', 'B', 'B', 'M' ),
	};

	static CU_ImplementAttributeParserNewBlock( parserPbrBloom, castor3d::TargetContext, BloomContext )
	{
		newBlockContext->renderTarget = blockContext->renderTarget;
	}
	CU_EndAttributePushNewBlock( PbrBloomSection::eRoot )

	static CU_ImplementAttributeParserBlock( parserBloomStrength, BloomContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter" ) );
		}
		else
		{
			params[0]->get( blockContext->bloomStrength );
		}
	}
	CU_EndAttribute()

	static CU_ImplementAttributeParserBlock( parserBlurRadius, BloomContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter" ) );
		}
		else
		{
			params[0]->get( blockContext->blurRadius );
		}
	}
	CU_EndAttribute()

	static CU_ImplementAttributeParserBlock( parserPasses, BloomContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter" ) );
		}
		else
		{
			params[0]->get( blockContext->passes );
		}
	}
	CU_EndAttribute()

	static CU_ImplementAttributeParserBlock( parserBloomEnd, BloomContext )
	{
		castor3d::Parameters parameters;
		parameters.add( cuT( "bloomStrength" )
			, castor::string::toString( blockContext->bloomStrength ) );
		parameters.add( cuT( "blurRadius" )
			, castor::string::toString( blockContext->blurRadius ) );
		parameters.add( cuT( "passes" )
			, castor::string::toString( blockContext->passes ) );

		auto effect = blockContext->renderTarget->getPostEffect( PostEffect::Type );
		effect->enable( true );
		effect->setParameters( parameters );
	}
	CU_EndAttributePop()

	static castor::AttributeParsers createParsers()
	{
		castor::AttributeParsers result;
		addParserT( result
			, castor3d::CSCNSection::eRenderTarget
			, PbrBloomSection::eRoot
			, cuT( "pbr_bloom" )
			, &parserPbrBloom );
		addParserT( result
			, PbrBloomSection::eRoot
			, cuT( "bloomStrength" )
			, &parserBloomStrength
			, { castor::makeParameter< castor::ParameterType::eFloat >( castor::makeRange( 0.0f, 1.0f ) ) } );
		addParserT( result
			, PbrBloomSection::eRoot
			, cuT( "blurRadius" )
			, &parserBlurRadius
			, { castor::makeParameter< castor::ParameterType::eUInt32 >( castor::makeRange( 1u, 10u ) ) } );
		addParserT( result
			, PbrBloomSection::eRoot
			, cuT( "passes" )
			, &parserPasses
			, { castor::makeParameter< castor::ParameterType::eUInt32 >( castor::makeRange( 1u, 10u ) ) } );
		addParserT( result
			, PbrBloomSection::eRoot
			, castor3d::CSCNSection::eRenderTarget
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
}

extern "C"
{
	C3D_PbrBloom_API void getRequiredVersion( castor3d::Version * version );
	C3D_PbrBloom_API void getType( castor3d::PluginType * type );
	C3D_PbrBloom_API void getName( char const ** name );
	C3D_PbrBloom_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * plugin );
	C3D_PbrBloom_API void OnUnload( castor3d::Engine * engine );

	C3D_PbrBloom_API void getRequiredVersion( castor3d::Version * version )
	{
		*version = castor3d::Version();
	}

	C3D_PbrBloom_API void getType( castor3d::PluginType * type )
	{
		*type = castor3d::PluginType::ePostEffect;
	}

	C3D_PbrBloom_API void getName( char const ** name )
	{
		*name = PbrBloom::PostEffect::Name.c_str();
	}

	C3D_PbrBloom_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * plugin )
	{
		engine->getPostEffectFactory().registerType( PbrBloom::PostEffect::Type
			, &PbrBloom::PostEffect::create );
		engine->registerParsers( PbrBloom::PostEffect::Type
			, PbrBloom::createParsers()
			, PbrBloom::createSections()
			, nullptr );
	}

	C3D_PbrBloom_API void OnUnload( castor3d::Engine * engine )
	{
		engine->unregisterParsers( PbrBloom::PostEffect::Type );
		engine->getPostEffectFactory().unregisterType( PbrBloom::PostEffect::Type );
	}
}
