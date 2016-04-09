#include <Engine.hpp>
#include <RenderSystem.hpp>

#include <Logger.hpp>

#include "BloomPostEffect.hpp"

#include <PostFxPlugin.hpp>

using namespace Castor3D;
using namespace Castor;
using namespace Bloom;

C3D_Bloom_API void GetRequiredVersion( Version & p_version )
{
	p_version = Version();
}

C3D_Bloom_API ePLUGIN_TYPE GetType()
{
	return ePLUGIN_TYPE_POSTFX;
}

C3D_Bloom_API String GetName()
{
	return cuT( "Bloom PostEffect" );
}

C3D_Bloom_API String GetPostEffectType()
{
	return cuT( "bloom" );
}

C3D_Bloom_API PostEffectSPtr CreateEffect( RenderSystem * p_renderSystem, RenderTarget & p_renderTarget, Parameters const & p_params )
{
	return std::make_shared< BloomPostEffect >( p_renderSystem, p_renderTarget, p_params );
}
