#include <Logger.hpp>

#include <Engine.hpp>

#include <Plugin/PostFxPlugin.hpp>
#include <Render/RenderSystem.hpp>

#include "GrayScalePostEffect.hpp"

using namespace GrayScale;

C3D_GrayScale_API void GetRequiredVersion( Castor3D::Version & p_version )
{
	p_version = Castor3D::Version();
}

C3D_GrayScale_API Castor3D::ePLUGIN_TYPE GetType()
{
	return Castor3D::ePLUGIN_TYPE_POSTFX;
}

C3D_GrayScale_API Castor::String GetName()
{
	return GrayScalePostEffect::Name;
}

C3D_GrayScale_API Castor::String GetPostEffectType()
{
	return GrayScalePostEffect::Type;
}

C3D_GrayScale_API Castor3D::PostEffectSPtr CreateEffect( Castor3D::RenderSystem * p_renderSystem, Castor3D::RenderTarget & p_renderTarget, Castor3D::Parameters const & p_params )
{
	return std::make_shared< GrayScalePostEffect >( *p_renderSystem, p_renderTarget, p_params );
}
