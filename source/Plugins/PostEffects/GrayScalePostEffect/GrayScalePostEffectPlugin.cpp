#include "GrayScalePostEffect/GrayScalePostEffect.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/TargetCache.hpp>
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
	C3D_GrayScale_API void getRequiredVersion( c3d::Version * version );
	C3D_GrayScale_API void isDebug( int * value );
	C3D_GrayScale_API void getType( c3d::PluginType * type );
	C3D_GrayScale_API void getName( char const ** name );
	C3D_GrayScale_API void onLoad( c3d::Engine * engine );
	C3D_GrayScale_API void onUnload( c3d::Engine * engine );

	C3D_GrayScale_API void getRequiredVersion( c3d::Version * version )
	{
		*version = c3d::Version();
	}

	C3D_GrayScale_API void isDebug( int * value )
	{
		*value = c3d::system::isDebug() ? 1 : 0;
	}

	C3D_GrayScale_API void getType( c3d::PluginType * type )
	{
		*type = c3d::PluginType::ePostEffect;
	}

	C3D_GrayScale_API void getName( char const ** name )
	{
		*name = grayscale::PostEffect::Name.c_str();
	}

	C3D_GrayScale_API void onLoad( c3d::Engine * engine )
	{
		engine->getPostEffectFactory().registerType( grayscale::PostEffect::Type
			, &grayscale::PostEffect::create );
	}

	C3D_GrayScale_API void onUnload( c3d::Engine * engine )
	{
		engine->getPostEffectFactory().unregisterType( grayscale::PostEffect::Type );
	}
}
