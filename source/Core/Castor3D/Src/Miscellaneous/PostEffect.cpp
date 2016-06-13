#include "PostEffect.hpp"

using namespace Castor;
namespace Castor3D
{
	PostEffect::PostEffect( RenderSystem & p_renderSystem, RenderTarget & p_renderTarget, String const & p_name, Parameters const & CU_PARAM_UNUSED( p_param ) )
		: OwnedBy< RenderSystem >{ p_renderSystem }
		, Named{ p_name }
		, m_renderTarget{ p_renderTarget }
	{
	}

	PostEffect::~PostEffect()
	{
	}

	bool PostEffect::WriteInto( Castor::TextFile & p_file )
	{
		return DoWriteInto( p_file );
	}
}
