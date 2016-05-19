#include "GpuQuery.hpp"

using namespace Castor;

namespace Castor3D
{
	GpuQuery::GpuQuery( RenderSystem & p_renderSystem, eQUERY_TYPE p_query )
		: OwnedBy< RenderSystem >( p_renderSystem )
		, m_type( p_query )
	{
	}

	GpuQuery::~GpuQuery()
	{
	}

	bool GpuQuery::Create()
	{
		return DoCreate();
	}

	void GpuQuery::Destroy()
	{
		if ( m_active )
		{
			End();
		}

		DoDestroy();
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
