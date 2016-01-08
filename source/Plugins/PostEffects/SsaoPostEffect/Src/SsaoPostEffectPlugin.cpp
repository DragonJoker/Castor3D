#include <Engine.hpp>
#include <RenderSystem.hpp>

#include <Logger.hpp>

#include "SsaoPostEffect.hpp"

#include <PostFxPlugin.hpp>

using namespace Castor3D;
using namespace Castor;

C3D_Ssao_API void GetRequiredVersion( Version & p_version )
{
	p_version = Version();
}

C3D_Ssao_API ePLUGIN_TYPE GetType()
{
	return ePLUGIN_TYPE_POSTFX;
}

C3D_Ssao_API String GetName()
{
	return cuT( "Ssao PostEffect" );
}

C3D_Ssao_API String GetPostEffectType()
{
	return cuT( "ssao" );
}

C3D_Ssao_API PostEffectSPtr CreateEffect( RenderSystem * p_renderSystem, RenderTarget & p_renderTarget, Parameters const & p_param )
{
	 return std::make_shared< SsaoPostEffect >( p_renderSystem, p_renderTarget, p_param );
}
