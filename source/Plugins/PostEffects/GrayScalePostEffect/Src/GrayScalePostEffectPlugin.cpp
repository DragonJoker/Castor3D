#include <Log/Logger.hpp>

#include <Engine.hpp>
#include <Cache/TargetCache.hpp>

#include <Plugin/PostFxPlugin.hpp>
#include <Render/RenderSystem.hpp>
#include <Render/RenderTarget.hpp>

#include "GrayScalePostEffect.hpp"

#ifndef _WIN32
#	define C3D_GrayScale_API
#else
#	ifdef GrayScalePostEffect_EXPORTS
#		define C3D_GrayScale_API __declspec( dllexport )
#	else
#		define C3D_GrayScale_API __declspec( dllimport )
#	endif
#endif

using namespace GrayScale;

C3D_GrayScale_API void GetRequiredVersion( Castor3D::Version & p_version )
{
	p_version = Castor3D::Version();
}

C3D_GrayScale_API Castor3D::PluginType GetType()
{
	return Castor3D::PluginType::ePostEffect;
}

C3D_GrayScale_API Castor::String GetName()
{
	return GrayScalePostEffect::Name;
}

C3D_GrayScale_API void OnLoad( Castor3D::Engine * p_engine )
{
	p_engine->GetRenderTargetCache().GetPostEffectFactory().Register( GrayScalePostEffect::Type, &GrayScalePostEffect::Create );
}

C3D_GrayScale_API void OnUnload( Castor3D::Engine * p_engine )
{
	p_engine->GetRenderTargetCache().GetPostEffectFactory().Unregister( GrayScalePostEffect::Type );
}
