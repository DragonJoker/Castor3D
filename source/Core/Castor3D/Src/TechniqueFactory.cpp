#include "TechniqueFactory.hpp"

#include "DirectRenderTechnique.hpp"

using namespace Castor;

namespace Castor3D
{
	TechniqueFactory::TechniqueFactory()
		:	Factory< RenderTechniqueBase, Castor::String, std::function< RenderTechniqueBaseSPtr( RenderTarget &, RenderSystem *, Parameters const & ) > >()
	{
		Initialise();
	}

	TechniqueFactory::~TechniqueFactory()
	{
	}

	void TechniqueFactory::Initialise()
	{
		Register( cuT( "direct" ), &DirectRenderTechnique::CreateInstance );
	}
}
