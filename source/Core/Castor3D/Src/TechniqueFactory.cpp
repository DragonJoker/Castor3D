#include "TechniqueFactory.hpp"

using namespace Castor;

namespace Castor3D
{
	TechniqueFactory::TechniqueFactory()
		:	Factory< RenderTechnique, Castor::String, std::shared_ptr< RenderTechnique >, std::function< RenderTechniqueSPtr( RenderTarget &, RenderSystem *, Parameters const & ) > >()
	{
		Initialise();
	}

	TechniqueFactory::~TechniqueFactory()
	{
	}

	void TechniqueFactory::Initialise()
	{
	}
}
