#include "DrawEdgesPostEffect/DrawEdgesPostEffect.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/TargetCache.hpp>
#include <Castor3D/Plugin/PostFxPlugin.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTarget.hpp>

#include <CastorUtils/Log/Logger.hpp>

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
	}

	C3D_DrawEdges_API void OnUnload( castor3d::Engine * engine )
	{
		engine->getRenderTargetCache().getPostEffectFactory().unregisterType( draw_edges::PostEffect::Type );
	}
}
