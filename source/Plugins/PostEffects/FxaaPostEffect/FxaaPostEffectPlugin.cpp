#include "FxaaPostEffect/FxaaPostEffect.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/TargetCache.hpp>
#include <Castor3D/Plugin/PostFxPlugin.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTarget.hpp>

#include <CastorUtils/Log/Logger.hpp>

#ifndef CU_PlatformWindows
#	define C3D_Fxaa_API
#else
#	ifdef FxaaPostEffect_EXPORTS
#		define C3D_Fxaa_API __declspec( dllexport )
#	else
#		define C3D_Fxaa_API __declspec( dllimport )
#	endif
#endif

extern "C"
{
	C3D_Fxaa_API void getRequiredVersion( castor3d::Version * p_version )
	{
		*p_version = castor3d::Version();
	}

	C3D_Fxaa_API void getType( castor3d::PluginType * p_type )
	{
		*p_type = castor3d::PluginType::ePostEffect;
	}

	C3D_Fxaa_API void getName( char const ** p_name )
	{
		*p_name = fxaa::PostEffect::Name.c_str();
	}

	C3D_Fxaa_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * p_plugin )
	{
		engine->getPostEffectFactory().registerType( fxaa::PostEffect::Type
			, &fxaa::PostEffect::create );
	}

	C3D_Fxaa_API void OnUnload( castor3d::Engine * engine )
	{
		engine->getPostEffectFactory().unregisterType( fxaa::PostEffect::Type );
	}
}
