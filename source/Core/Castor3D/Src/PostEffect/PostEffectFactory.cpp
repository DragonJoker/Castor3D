#include "PostEffectFactory.hpp"

using namespace Castor;

namespace Castor3D
{
	PostEffectFactory::PostEffectFactory()
		:	Factory< PostEffect, Castor::String, std::shared_ptr< PostEffect >, std::function< PostEffectSPtr( RenderTarget &, RenderSystem &, Parameters const & ) > >()
	{
		Initialise();
	}

	PostEffectFactory::~PostEffectFactory()
	{
	}

	void PostEffectFactory::Initialise()
	{
	}
}
