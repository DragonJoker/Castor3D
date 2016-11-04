#include <Log/Logger.hpp>

#include <Engine.hpp>
#include <TargetCache.hpp>

#include <Plugin/PostFxPlugin.hpp>
#include <Render/RenderSystem.hpp>

#include "BloomPostEffect.hpp"

using namespace Bloom;

C3D_Bloom_API void GetRequiredVersion( Castor3D::Version & p_version )
{
	p_version = Castor3D::Version();
}

C3D_Bloom_API Castor3D::PluginType GetType()
{
	return Castor3D::PluginType::ePostEffect;
}

C3D_Bloom_API Castor::String GetName()
{
	return BloomPostEffect::Name;
}

C3D_Bloom_API void OnLoad( Castor3D::Engine * p_engine )
{
	p_engine->GetRenderTargetCache().GetPostEffectFactory().Register( BloomPostEffect::Type, &BloomPostEffect::Create );
}

C3D_Bloom_API void OnUnload( Castor3D::Engine * p_engine )
{
	p_engine->GetRenderTargetCache().GetPostEffectFactory().Unregister( BloomPostEffect::Type );
}
