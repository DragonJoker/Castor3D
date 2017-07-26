#include <Log/Logger.hpp>

#include <Engine.hpp>
#include <Cache/TargetCache.hpp>

#include <Plugin/PostFxPlugin.hpp>
#include <Render/RenderSystem.hpp>
#include <Render/RenderTarget.hpp>

#include "GrayScalePostEffect.hpp"

#ifndef CASTOR_PLATFORM_WINDOWS
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
	C3D_GrayScale_API void GetRequiredVersion( Castor3D::Version * p_version )
	{
		*p_version = Castor3D::Version();
	}

	C3D_GrayScale_API void GetType( Castor3D::PluginType * p_type )
	{
		*p_type = Castor3D::PluginType::ePostEffect;
	}

	C3D_GrayScale_API void GetName( char const ** p_name )
	{
		*p_name = GrayScale::GrayScalePostEffect::Name.c_str();
	}

	C3D_GrayScale_API void OnLoad( Castor3D::Engine * engine, Castor3D::Plugin * p_plugin )
	{
		engine->GetRenderTargetCache().GetPostEffectFactory().Register( GrayScale::GrayScalePostEffect::Type
			, &GrayScale::GrayScalePostEffect::Create );
	}

	C3D_GrayScale_API void OnUnload( Castor3D::Engine * engine )
	{
		engine->GetRenderTargetCache().GetPostEffectFactory().Unregister( GrayScale::GrayScalePostEffect::Type );
	}
}
