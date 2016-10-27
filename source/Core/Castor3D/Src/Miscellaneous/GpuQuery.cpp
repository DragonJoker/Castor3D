#include "GpuQuery.hpp"

using namespace Castor;

namespace Castor3D
{
	GpuQuery::GpuQuery( RenderSystem & p_renderSystem, QueryType p_query )
		: OwnedBy< RenderSystem >( p_renderSystem )
		, m_type( p_query )
	{
	}

	GpuQuery::~GpuQuery()
	{
	}

	bool GpuQuery::Initialise()
	{
		return DoInitialise();
	}

	void GpuQuery::Cleanup()
	{
		if ( m_active )
		{
			End();
		}

		DoCleanup();
	}

	bool GpuQuery::Begin()
	{
		m_active = true;
		return DoBegin();
	}

	void GpuQuery::End()
	{
		DoEnd();
		m_active = false;
	}
}
