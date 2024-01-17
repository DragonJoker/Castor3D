#include "DrawEdgesPostEffect/DrawEdgesPostEffect.hpp"

#include "DrawEdgesPostEffect/DrawEdges_Parsers.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/TargetCache.hpp>
#include <Castor3D/Plugin/PostFxPlugin.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTarget.hpp>

#include <CastorUtils/FileParser/FileParser.hpp>
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

extern "C"
{
	C3D_DrawEdges_API void getRequiredVersion( castor3d::Version * version );
	C3D_DrawEdges_API void getType( castor3d::PluginType * type );
	C3D_DrawEdges_API void getName( char const ** name );
	C3D_DrawEdges_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * plugin );
	C3D_DrawEdges_API void OnUnload( castor3d::Engine * engine );

	C3D_DrawEdges_API void getRequiredVersion( castor3d::Version * version )
	{
		*version = castor3d::Version();
	}

	C3D_DrawEdges_API void getType( castor3d::PluginType * type )
	{
		*type = castor3d::PluginType::ePostEffect;
	}

	C3D_DrawEdges_API void getName( char const ** name )
	{
		*name = draw_edges::PostEffect::Name.c_str();
	}

	C3D_DrawEdges_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * plugin )
	{
		engine->getPostEffectFactory().registerType( draw_edges::PostEffect::Type
			, &draw_edges::PostEffect::create );
		engine->registerParsers( draw_edges::PostEffect::Type
			, draw_edges::createParsers()
			, draw_edges::createSections()
			, nullptr );
	}

	C3D_DrawEdges_API void OnUnload( castor3d::Engine * engine )
	{
		engine->unregisterParsers( draw_edges::PostEffect::Type );
		engine->getPostEffectFactory().unregisterType( draw_edges::PostEffect::Type );
	}
}
