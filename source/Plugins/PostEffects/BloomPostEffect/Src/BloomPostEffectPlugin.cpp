#include <Logger.hpp>

#include <Engine.hpp>

#include <Plugin/PostFxPlugin.hpp>
#include <Render/RenderSystem.hpp>

#include "BloomPostEffect.hpp"

using namespace Bloom;

C3D_Bloom_API void GetRequiredVersion( Castor3D::Version & p_version )
{
	p_version = Castor3D::Version();
}

C3D_Bloom_API Castor3D::ePLUGIN_TYPE GetType()
{
	return Castor3D::ePLUGIN_TYPE_POSTFX;
}

C3D_Bloom_API Castor::String GetName()
{
	return BloomPostEffect::Name;
}

C3D_Bloom_API Castor::String GetPostEffectType()
{
	return BloomPostEffect::Type;
}

C3D_Bloom_API Castor3D::PostEffectSPtr CreateEffect( Castor3D::RenderSystem * p_renderSystem, Castor3D::RenderTarget & p_renderTarget, Castor3D::Parameters const & p_params )
{
	return std::make_shared< BloomPostEffect >( *p_renderSystem, p_renderTarget, p_params );
}
