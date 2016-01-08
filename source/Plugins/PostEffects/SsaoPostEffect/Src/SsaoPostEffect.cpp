#include "SsaoPostEffect.hpp"

using namespace Castor;

namespace Castor3D
{
	SsaoPostEffect::SsaoPostEffect( RenderSystem * p_renderSystem, RenderTarget & p_renderTarget, Parameters const & p_param )
		: PostEffect( p_renderSystem, p_renderTarget, p_param )
	{
	}

	SsaoPostEffect::~SsaoPostEffect()
	{
	}

	bool SsaoPostEffect::Initialise()
	{
		return false;
	}

	void SsaoPostEffect::Cleanup()
	{
	}

	bool SsaoPostEffect::Apply()
	{
		return false;
	}
}
