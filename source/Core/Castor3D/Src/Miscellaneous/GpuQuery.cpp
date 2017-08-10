#include "GpuQuery.hpp"

using namespace castor;

namespace castor3d
{
	GpuQuery::GpuQuery( RenderSystem & renderSystem, QueryType p_query )
		: OwnedBy< RenderSystem >( renderSystem )
		, m_type( p_query )
	{
	}

	GpuQuery::~GpuQuery()
	{
	}

	bool GpuQuery::initialise()
	{
		return doInitialise();
	}

	void GpuQuery::cleanup()
	{
		if ( m_active )
		{
			end();
		}

		doCleanup();
	}

	void GpuQuery::begin()
	{
		m_active = true;
		doBegin();
	}

	void GpuQuery::end()
	{
		doEnd();
		m_active = false;
	}
}
