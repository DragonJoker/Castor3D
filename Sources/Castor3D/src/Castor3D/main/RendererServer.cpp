#include "PrecompiledHeader.h"

#include "main/Module_Main.h"

#include "main/RendererServer.h"

using namespace Castor3D;

RendererServer :: ~RendererServer()
{
	Cleanup();
}

void RendererServer :: Cleanup()
{
//	vector::deleteAll( m_rendererDrivers);
	m_rendererDrivers.clear();
}

void RendererServer :: AddRendererDriver( RendererDriverPtr p_driver)
{
	m_rendererDrivers.push_back( p_driver);
}

size_t RendererServer :: GetDriverCount()const
{
	return m_rendererDrivers.size();
}

RendererDriverPtr RendererServer :: GetDriver( size_t p_index)
{
	RendererDriverPtr l_pReturn;

	if (p_index < m_rendererDrivers.size())
	{
		l_pReturn = m_rendererDrivers[p_index];
	}

	return l_pReturn;
}