#include "PrecompiledHeader.h"

#include "main/Module_Main.h"

#include "main/RendererServer.h"

using namespace Castor3D;

RendererServer :: ~RendererServer()
{
	vector::deleteAll( m_rendererDrivers);
}

void RendererServer :: AddRendererDriver( RendererDriver * p_driver)
{
	m_rendererDrivers.push_back( p_driver);
}

size_t RendererServer :: GetDriverCount()const
{
	return m_rendererDrivers.size();
}

RendererDriver * RendererServer :: GetDriver( size_t p_index)
{
	return vector::getOrNull( m_rendererDrivers, p_index);
}