#include "SsaoPostEffect.hpp"

using namespace Castor;

namespace Castor3D
{
	SsaoPostEffect::SsaoPostEffect( RenderSystem * p_renderSystem )
		: PostEffect( p_renderSystem )
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

	bool SsaoPostEffect::Apply( RenderTarget * p_pRenderTarget )
	{
		return false;
	}
}
