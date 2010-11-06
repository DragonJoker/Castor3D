#include "PrecompiledHeader.h"

#include "overlay/OverlayManager.h"
#include "overlay/Overlay.h"
#include "render_system/RenderSystem.h"

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
	RenderSystem::GetSingletonPtr<RenderSystem>()->BeginOverlaysRendering();

	for (OverlayPtrIntMap::iterator l_it = m_mapOverlaysByZIndex.begin() ; l_it != m_mapOverlaysByZIndex.end() ; ++l_it)
	{
		l_it->second->Apply( DTTriangles);
	}

	RenderSystem::GetSingletonPtr<RenderSystem>()->EndOverlaysRendering();
}