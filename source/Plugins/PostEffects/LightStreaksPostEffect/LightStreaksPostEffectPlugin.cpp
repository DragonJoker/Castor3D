#include <Log/Logger.hpp>

#include <Engine.hpp>
#include <Cache/TargetCache.hpp>

#include <Plugin/PostFxPlugin.hpp>
#include <Render/RenderSystem.hpp>

#include "LightStreaksPostEffect.hpp"

#ifndef CU_PlatformWindows
#	define C3D_LightStreaks_API
#else
#	ifdef LightStreaksPostEffect_EXPORTS
#		define C3D_LightStreaks_API __declspec( dllexport )
#	else
#		define C3D_LightStreaks_API __declspec( dllimport )
#	endif
#endif

extern "C"
{
	C3D_LightStreaks_API void getRequiredVersion( castor3d::Version * p_version )
	{
		*p_version = castor3d::Version();
	}

	C3D_LightStreaks_API void getType( castor3d::PluginType * p_type )
	{
		*p_type = castor3d::PluginType::ePostEffect;
	}

	C3D_LightStreaks_API void getName( char const ** p_name )
	{
		*p_name = light_streaks::PostEffect::Name.c_str();
	}

	C3D_LightStreaks_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * p_plugin )
	{
		engine->getRenderTargetCache().getPostEffectFactory().registerType( light_streaks::PostEffect::Type
			, &light_streaks::PostEffect::create );
	}

	C3D_LightStreaks_API void OnUnload( castor3d::Engine * engine )
	{
		engine->getRenderTargetCache().getPostEffectFactory().unregisterType( light_streaks::PostEffect::Type );
	}
}
