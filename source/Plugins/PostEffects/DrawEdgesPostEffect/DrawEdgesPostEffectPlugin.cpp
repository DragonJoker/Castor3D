#include "DrawEdgesPostEffect/DrawEdgesPostEffect.hpp"

#include "DrawEdgesPostEffect/DrawEdges_Parsers.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/TargetCache.hpp>
#include <Castor3D/Plugin/PostFxPlugin.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Scene/SceneFileParser.hpp>

#include <CastorUtils/FileParser/ParserParameter.hpp>
#include <CastorUtils/Log/Logger.hpp>
#include <CastorUtils/Math/Range.hpp>

#ifndef CU_PlatformWindows
#	define C3D_DrawEdges_API
#else
#	ifdef DrawEdgesPostEffect_EXPORTS
#		define C3D_DrawEdges_API __declspec( dllexport )
#	else
#		define C3D_DrawEdges_API __declspec( dllimport )
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
		using namespace draw_edges;
		castor::AttributeParsers result;

		addParser( result
			, uint32_t( castor3d::CSCNSection::eRenderTarget )
			, PostEffect::Type
			, &parserDrawEdges );
		addParser( result
			, uint32_t( Section::eRoot )
			, PostEffect::NormalDepthWidth
			, &parserNormalDepthWidth
			, { castor::makeParameter< castor::ParameterType::eFloat >( castor::makeRange( 1.0f, 1000.0f ) ) } );
		addParser( result
			, uint32_t( Section::eRoot )
			, PostEffect::ObjectWidth
			, &parserObjectWidth
			, { castor::makeParameter< castor::ParameterType::eFloat >( castor::makeRange( 1.0f, 1000.0f ) ) } );
		addParser( result
			, uint32_t( Section::eRoot )
			, cuT( "}" )
			, &parserDrawEdgesEnd );
		return result;
	}

	castor::StrUInt32Map createSections()
	{
		return
		{
			{ uint32_t( draw_edges::Section::eRoot ), draw_edges::PostEffect::Type },
		};
	}
}

extern "C"
{
	C3D_DrawEdges_API void getRequiredVersion( castor3d::Version * p_version )
	{
		*p_version = castor3d::Version();
	}

	C3D_DrawEdges_API void getType( castor3d::PluginType * p_type )
	{
		*p_type = castor3d::PluginType::ePostEffect;
	}

	C3D_DrawEdges_API void getName( char const ** p_name )
	{
		*p_name = draw_edges::PostEffect::Name.c_str();
	}

	C3D_DrawEdges_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * p_plugin )
	{
		engine->getRenderTargetCache().getPostEffectFactory().registerType( draw_edges::PostEffect::Type
			, &draw_edges::PostEffect::create );
		engine->registerParsers( draw_edges::PostEffect::Type, createParsers() );
		engine->registerSections( draw_edges::PostEffect::Type, createSections() );
	}

	C3D_DrawEdges_API void OnUnload( castor3d::Engine * engine )
	{
		engine->unregisterSections( draw_edges::PostEffect::Type );
		engine->unregisterParsers( draw_edges::PostEffect::Type );
		engine->getRenderTargetCache().getPostEffectFactory().unregisterType( draw_edges::PostEffect::Type );
	}
}
