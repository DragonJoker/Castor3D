#include "PostEffect.hpp"

using namespace Castor;
using namespace Castor3D;

//*************************************************************************************************

PostEffect::PostEffect( RenderSystem & p_renderSystem, RenderTarget & p_renderTarget, Parameters const & CU_PARAM_UNUSED( p_param ) )
	: OwnedBy< RenderSystem >( p_renderSystem )
	, m_renderTarget( p_renderTarget )
{
}

PostEffect::~PostEffect()
{
}

//*************************************************************************************************
