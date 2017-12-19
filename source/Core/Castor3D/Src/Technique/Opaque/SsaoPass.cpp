#include "SsaoPass.hpp"

#include "Engine.hpp"

using namespace castor;
using namespace castor3d;

namespace castor3d
{
	SsaoPass::SsaoPass( Engine & engine
		, Size const & size
		, SsaoConfig const & config )
		: m_engine{ engine }
		, m_linearisePass{ engine, size }
		, m_rawSsaoPass{ engine, size, config }
		, m_horizontalBlur{ engine, size }
		, m_verticalBlur{ engine, size }
	{
	}

	SsaoPass::~SsaoPass()
	{
	}

	void SsaoPass::render( TextureUnit const & depthBuffer
		, Viewport const & viewport
		, RenderInfo & info )
	{
		m_linearisePass.linearise( depthBuffer, viewport );
		m_rawSsaoPass.compute( m_linearisePass.getResult(), viewport );
		m_horizontalBlur.blur( m_rawSsaoPass.getResult(), Point2i{ 1, 0 } );
		m_verticalBlur.blur( m_horizontalBlur.getResult(), Point2i{ 0, 1 } );
	}

	TextureUnit const & SsaoPass::getResult()const
	{
		return m_rawSsaoPass.getResult();
	}
}
