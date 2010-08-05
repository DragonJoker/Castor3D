#include "PrecompiledHeader.h"

#include "overlay/OverlayManager.h"
#include "render_system/RenderSystem.h"

using namespace Castor3D;

OverlayManager :: OverlayManager()
	:	m_iCurrentZIndex( 1)
{
}

OverlayManager :: ~OverlayManager()
{
	map::deleteAll( m_mapOverlaysByName);
}

void OverlayManager :: RenderOverlays()
{
	RenderSystem::GetSingletonPtr()->BeginOverlaysRendering();
	map::cycle( m_mapOverlaysByZIndex, & Overlay::Render);
	RenderSystem::GetSingletonPtr()->EndOverlaysRendering();
}