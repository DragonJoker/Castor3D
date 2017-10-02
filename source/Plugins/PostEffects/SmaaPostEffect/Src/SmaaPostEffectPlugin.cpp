#include <Log/Logger.hpp>

#include <Engine.hpp>
#include <Cache/TargetCache.hpp>

#include <Plugin/PostFxPlugin.hpp>
#include <Render/RenderSystem.hpp>
#include <Render/RenderTarget.hpp>

#include "SmaaPostEffect.hpp"

#ifndef CASTOR_PLATFORM_WINDOWS
#	define C3D_Smaa_API
#else
#	ifdef SmaaPostEffect_EXPORTS
#		define C3D_Smaa_API __declspec( dllexport )
#	else
#		define C3D_Smaa_API __declspec( dllimport )
#	endif
#endif

extern "C"
{
	C3D_Smaa_API void getRequiredVersion( castor3d::Version * p_version )
	{
		*p_version = castor3d::Version();
	}

	C3D_Smaa_API void getType( castor3d::PluginType * p_type )
	{
		*p_type = castor3d::PluginType::ePostEffect;
	}

	C3D_Smaa_API void getName( char const ** p_name )
	{
		*p_name = smaa::PostEffect::Name.c_str();
	}

	C3D_Smaa_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * p_plugin )
	{
		engine->getRenderTargetCache().getPostEffectFactory().registerType( smaa::PostEffect::Type
			, &smaa::PostEffect::create );
	}

	C3D_Smaa_API void OnUnload( castor3d::Engine * engine )
	{
		engine->getRenderTargetCache().getPostEffectFactory().unregisterType( smaa::PostEffect::Type );
	}
}
