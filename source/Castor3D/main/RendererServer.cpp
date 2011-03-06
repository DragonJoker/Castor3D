#include "Castor3D/PrecompiledHeader.h"

#include "Castor3D/main/RendererServer.h"

using namespace Castor3D;

RendererServer :: RendererServer()
{
	m_mapNames.insert( StrIntMap::value_type( RendererDriver::eOpenGl2,		"Gl2RenderSystem"));
	m_mapNames.insert( StrIntMap::value_type( RendererDriver::eOpenGl3,		"Gl3RenderSystem"));
	m_mapNames.insert( StrIntMap::value_type( RendererDriver::eDirect3D9,	"Dx9RenderSystem"));
}

RendererServer :: ~RendererServer()
{
	Cleanup();
}

void RendererServer :: Cleanup()
{
	m_mapRenderers.clear();
	m_mapNames.clear();
}

void RendererServer :: AddRenderer( RendererDriver::eDRIVER_TYPE p_eType, RendererDriverPtr p_driver)
{
	if (m_mapRenderers.find( p_eType) == m_mapRenderers.end())
	{
		m_mapRenderers.insert( RendererDriverPtrIntMap::value_type( p_eType, p_driver));
	}
}

size_t RendererServer :: GetRendererCount()const
{
	return m_mapRenderers.size();
}

RendererDriverPtr RendererServer :: GetRenderer( RendererDriver::eDRIVER_TYPE p_eType)
{
	RendererDriverPtr l_pReturn;

	if (m_mapRenderers.find( p_eType) != m_mapRenderers.end())
	{
		l_pReturn = m_mapRenderers.find( p_eType)->second;
	}

	return l_pReturn;
}

String RendererServer :: GetRendererName( RendererDriver::eDRIVER_TYPE p_eType)
{
	String l_strReturn;

	if (m_mapNames.find( p_eType) != m_mapNames.end())
	{
		l_strReturn = m_mapNames.find( p_eType)->second;
	}

	return l_strReturn;
}