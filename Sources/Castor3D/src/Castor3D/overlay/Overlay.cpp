#include "PrecompiledHeader.h"

#include "overlay/Overlay.h"
#include "material/Material.h"

using namespace Castor3D;

Overlay :: Overlay( const String & p_strName, OverlayPtr p_pParent)
	:	m_pParent( p_pParent),
		m_ptPosition( 0, 0),
		m_ptSize( 0, 0),
		m_strName( p_strName),
		m_bVisible( true),
		m_iCurrentZIndex( 100)
{
}

bool Overlay :: AddChild( OverlayPtr p_pOverlay, int p_iZIndex)
{
	bool l_bReturn = false;

	if (p_iZIndex == 0)
	{
		m_mapOverlays.insert( OverlayPtrIntMap::value_type( m_iCurrentZIndex, p_pOverlay));
		m_iCurrentZIndex += 100;
		l_bReturn = true;
	}
	else if (m_mapOverlays.find( p_iZIndex) == m_mapOverlays.end())
	{
		m_mapOverlays.insert( OverlayPtrIntMap::value_type( p_iZIndex, p_pOverlay));
		l_bReturn = true;
	}

	while (m_mapOverlays.find( m_iCurrentZIndex) != m_mapOverlays.end())
	{
		m_iCurrentZIndex += 100;
		l_bReturn = true;
	}

	return l_bReturn;
}

void Overlay :: SetPosition( const Point2r & p_ptPosition)
{
	if ( ! m_pParent.null())
	{
		m_ptPosition = m_pParent->m_ptPosition + p_ptPosition * m_pParent->m_ptSize;
	}
	else
	{
		m_ptPosition = p_ptPosition;
	}
}

void Overlay :: SetSize( const Point2r & p_ptSize)
{
	if ( ! m_pParent.null())
	{
		m_ptSize = p_ptSize * m_pParent->m_ptSize;
	}
	else
	{
		m_ptSize = p_ptSize;
	}
}

void PanelOverlay :: Render( eDRAW_TYPE p_displayMode)
{
	if (m_bVisible)
	{
		m_pRenderer->DrawPanel();

		for (OverlayPtrIntMap::iterator l_it = m_mapOverlays.begin() ; l_it != m_mapOverlays.end() ; ++l_it)
		{
			l_it->second->Render( p_displayMode);
		}
	}
}

void BorderPanelOverlay :: Render( eDRAW_TYPE p_displayMode)
{
	if (m_bVisible)
	{
		m_pRenderer->DrawBorderPanel();

		for (OverlayPtrIntMap::iterator l_it = m_mapOverlays.begin() ; l_it != m_mapOverlays.end() ; ++l_it)
		{
			l_it->second->Render( p_displayMode);
		}
	}
}