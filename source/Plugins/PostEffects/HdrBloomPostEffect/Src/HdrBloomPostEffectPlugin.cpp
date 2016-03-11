#include <Engine.hpp>
#include <RenderSystem.hpp>

#include <Logger.hpp>

#include "HdrBloomPostEffect.hpp"

#include <PostFxPlugin.hpp>

using namespace Castor3D;
using namespace Castor;
using namespace HdrBloom;

C3D_HdrBloom_API void GetRequiredVersion( Version & p_version )
{
	p_version = Version();
}

C3D_HdrBloom_API ePLUGIN_TYPE GetType()
{
	return ePLUGIN_TYPE_POSTFX;
}

C3D_HdrBloom_API String GetName()
{
	return cuT( "HDR Bloom PostEffect" );
}

C3D_HdrBloom_API String GetPostEffectType()
{
	return cuT( "hdr_bloom" );
}

C3D_HdrBloom_API PostEffectSPtr CreateEffect( RenderSystem * p_renderSystem, RenderTarget & p_renderTarget, Parameters const & p_params )
{
	return std::make_shared< HdrBloomPostEffect >( p_renderSystem, p_renderTarget, p_params );
}
