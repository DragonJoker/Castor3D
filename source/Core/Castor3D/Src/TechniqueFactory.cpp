#include "TechniqueFactory.hpp"

#include "DeferredShadingRenderTechnique.hpp"
#include "DirectRenderTechnique.hpp"
#include "MsaaRenderTechnique.hpp"
#include "SsaaRenderTechnique.hpp"

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
		Register( cuT( "deferred" ), &DeferredShadingRenderTechnique::CreateInstance );
		Register( cuT( "direct" ), &DirectRenderTechnique::CreateInstance );
		Register( cuT( "msaa" ), &MsaaRenderTechnique::CreateInstance );
		Register( cuT( "ssaa" ), &SsaaRenderTechnique::CreateInstance );
	}
}
