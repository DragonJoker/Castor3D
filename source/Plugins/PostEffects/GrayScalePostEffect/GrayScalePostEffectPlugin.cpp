#include "GrayScalePostEffect/GrayScalePostEffect.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/TargetCache.hpp>
#include <Castor3D/Plugin/PostFxPlugin.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTarget.hpp>

#include <CastorUtils/Log/Logger.hpp>

#ifndef CU_PlatformWindows
#	define C3D_GrayScale_API
#else
#	ifdef GrayScalePostEffect_EXPORTS
#		define C3D_GrayScale_API __declspec( dllexport )
#	else
#		define C3D_GrayScale_API __declspec( dllimport )
#	endif
#endif

extern "C"
{
	C3D_GrayScale_API void getRequiredVersion( castor3d::Version * p_version );
	C3D_GrayScale_API void getType( castor3d::PluginType * p_type );
	C3D_GrayScale_API void getName( char const ** p_name );
	C3D_GrayScale_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * p_plugin );
	C3D_GrayScale_API void OnUnload( castor3d::Engine * engine );

	C3D_GrayScale_API void getRequiredVersion( castor3d::Version * p_version )
	{
		*p_version = castor3d::Version();
	}

	C3D_GrayScale_API void getType( castor3d::PluginType * p_type )
	{
		*p_type = castor3d::PluginType::ePostEffect;
	}

	C3D_GrayScale_API void getName( char const ** p_name )
	{
		*p_name = grayscale::PostEffect::Name.c_str();
	}

	C3D_GrayScale_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * p_plugin )
	{
		engine->getPostEffectFactory().registerType( grayscale::PostEffect::Type
			, &grayscale::PostEffect::create );
	}

	C3D_GrayScale_API void OnUnload( castor3d::Engine * engine )
	{
		engine->getPostEffectFactory().unregisterType( grayscale::PostEffect::Type );
	}
}
