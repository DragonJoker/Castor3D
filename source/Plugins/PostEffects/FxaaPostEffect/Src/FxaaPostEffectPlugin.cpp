#include <Log/Logger.hpp>

#include <Engine.hpp>
#include <TargetCache.hpp>

#include <Plugin/PostFxPlugin.hpp>
#include <Render/RenderSystem.hpp>
#include <Render/RenderTarget.hpp>

#include "FxaaPostEffect.hpp"

#ifndef _WIN32
#	define C3D_Fxaa_API
#else
#	ifdef FxaaPostEffect_EXPORTS
#		define C3D_Fxaa_API __declspec( dllexport )
#	else
#		define C3D_Fxaa_API __declspec( dllimport )
#	endif
#endif

using namespace Fxaa;

C3D_Fxaa_API void GetRequiredVersion( Castor3D::Version & p_version )
{
	p_version = Castor3D::Version();
}

C3D_Fxaa_API Castor3D::PluginType GetType()
{
	return Castor3D::PluginType::ePostEffect;
}

C3D_Fxaa_API Castor::String GetName()
{
	return FxaaPostEffect::Name;
}

C3D_Fxaa_API void OnLoad( Castor3D::Engine * p_engine )
{
	p_engine->GetRenderTargetCache().GetPostEffectFactory().Register( FxaaPostEffect::Type, &FxaaPostEffect::Create );
}

C3D_Fxaa_API void OnUnload( Castor3D::Engine * p_engine )
{
	p_engine->GetRenderTargetCache().GetPostEffectFactory().Unregister( FxaaPostEffect::Type );
}
