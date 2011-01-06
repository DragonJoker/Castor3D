#include "Castor3D/PrecompiledHeader.h"

#include "Castor3D/overlay/OverlayManager.h"
#include "Castor3D/overlay/Overlay.h"
#include "Castor3D/render_system/RenderSystem.h"

using namespace Castor3D;

OverlayManager :: OverlayManager()
	:	m_iCurrentZIndex( 1)
{
}

OverlayManager :: ~OverlayManager()
{
//	map::deleteAll( m_mapOverlaysByName);
}

void OverlayManager :: RenderOverlays()
{
	RenderSystem::GetSingletonPtr()->BeginOverlaysRendering();
/*
	for (OverlayPtrIntMap::iterator l_it = m_mapOverlaysByZIndex.begin() ; l_it != m_mapOverlaysByZIndex.end() ; ++l_it)
	{
		l_it->second->Apply( eTriangles);
	}
*/
	RenderSystem::GetSingletonPtr()->EndOverlaysRendering();
}