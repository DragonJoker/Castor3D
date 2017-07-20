#include <Log/Logger.hpp>

#include <Engine.hpp>
#include <Cache/TargetCache.hpp>

#include <Plugin/PostFxPlugin.hpp>
#include <Render/RenderSystem.hpp>
#include <Render/RenderTarget.hpp>

#include "FxaaPostEffect.hpp"

#ifndef CASTOR_PLATFORM_WINDOWS
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
	C3D_Fxaa_API void GetRequiredVersion( Castor3D::Version * p_version )
	{
		*p_version = Castor3D::Version();
	}

	C3D_Fxaa_API void GetType( Castor3D::PluginType * p_type )
	{
		*p_type = Castor3D::PluginType::ePostEffect;
	}

	C3D_Fxaa_API void GetName( char const ** p_name )
	{
		*p_name = fxaa::PostEffect::Name.c_str();
	}

	C3D_Fxaa_API void OnLoad( Castor3D::Engine * p_engine, Castor3D::Plugin * p_plugin )
	{
		p_engine->GetRenderTargetCache().GetPostEffectFactory().Register( fxaa::PostEffect::Type
			, &fxaa::PostEffect::Create );
	}

	C3D_Fxaa_API void OnUnload( Castor3D::Engine * p_engine )
	{
		p_engine->GetRenderTargetCache().GetPostEffectFactory().Unregister( fxaa::PostEffect::Type );
	}
}
