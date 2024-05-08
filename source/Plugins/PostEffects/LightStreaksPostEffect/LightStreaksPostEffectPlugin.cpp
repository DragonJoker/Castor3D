#include "LightStreaksPostEffect/LightStreaksPostEffect.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/TargetCache.hpp>
#include <Castor3D/Plugin/PostFxPlugin.hpp>
#include <Castor3D/Render/RenderSystem.hpp>

#include <CastorUtils/Log/Logger.hpp>

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
	C3D_LightStreaks_API void getRequiredVersion( castor3d::Version * version );
	C3D_LightStreaks_API void isDebug( int * value );
	C3D_LightStreaks_API void getType( castor3d::PluginType * type );
	C3D_LightStreaks_API void getName( char const ** name );
	C3D_LightStreaks_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * plugin );
	C3D_LightStreaks_API void OnUnload( castor3d::Engine * engine );

	C3D_LightStreaks_API void getRequiredVersion( castor3d::Version * version )
	{
		*version = castor3d::Version();
	}

	C3D_LightStreaks_API void isDebug( int * value )
	{
		*value = castor::system::isDebug() ? 1 : 0;
	}

	C3D_LightStreaks_API void getType( castor3d::PluginType * type )
	{
		*type = castor3d::PluginType::ePostEffect;
	}

	C3D_LightStreaks_API void getName( char const ** name )
	{
		*name = light_streaks::PostEffect::Name.c_str();
	}

	C3D_LightStreaks_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * plugin )
	{
		engine->getPostEffectFactory().registerType( light_streaks::PostEffect::Type
			, &light_streaks::PostEffect::create );
	}

	C3D_LightStreaks_API void OnUnload( castor3d::Engine * engine )
	{
		engine->getPostEffectFactory().unregisterType( light_streaks::PostEffect::Type );
	}
}
