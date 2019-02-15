#include <Log/Logger.hpp>

#include <Engine.hpp>
#include <Cache/TargetCache.hpp>

#include <Plugin/PostFxPlugin.hpp>
#include <Render/RenderSystem.hpp>
#include <Render/RenderTarget.hpp>

#include "FilmGrainPostEffect.hpp"

#ifndef CU_PlatformWindows
#	define C3D_FilmGrain_API
#else
#	ifdef FilmGrainPostEffect_EXPORTS
#		define C3D_FilmGrain_API __declspec( dllexport )
#	else
#		define C3D_FilmGrain_API __declspec( dllimport )
#	endif
#endif

extern "C"
{
	C3D_FilmGrain_API void getRequiredVersion( castor3d::Version * p_version )
	{
		*p_version = castor3d::Version();
	}

	C3D_FilmGrain_API void getType( castor3d::PluginType * p_type )
	{
		*p_type = castor3d::PluginType::ePostEffect;
	}

	C3D_FilmGrain_API void getName( char const ** p_name )
	{
		*p_name = film_grain::PostEffect::Name.c_str();
	}

	C3D_FilmGrain_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * p_plugin )
	{
		engine->getRenderTargetCache().getPostEffectFactory().registerType( film_grain::PostEffect::Type
			, &film_grain::PostEffect::create );
	}

	C3D_FilmGrain_API void OnUnload( castor3d::Engine * engine )
	{
		engine->getRenderTargetCache().getPostEffectFactory().unregisterType( film_grain::PostEffect::Type );
	}
}
