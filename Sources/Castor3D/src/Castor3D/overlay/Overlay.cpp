#include "PrecompiledHeader.h"

#include "overlay/Overlay.h"
#include "material/Material.h"
#include "render_system/OverlayRenderer.h"

using namespace Castor3D;

Overlay :: Overlay( const String & p_strName, Overlay * p_pParent, OverlayRenderer * p_pRenderer)
	:	m_pParent( p_pParent),
		m_pRenderer( p_pRenderer),
		m_ptPosition( 0, 0),
		m_ptSize( 0, 0),
		m_strName( p_strName),
		m_bVisible( true),
		m_iCurrentZIndex( 100)
{
	m_pRenderer->SetTarget( this);
}

bool Overlay :: AddChild( Overlay * p_pOverlay, int p_iZIndex)
{
	bool l_bReturn = false;

	if (p_iZIndex == 0)
	{
		m_mapOverlays.insert( std::map <int, Overlay *>::value_type( m_iCurrentZIndex, p_pOverlay));
		m_iCurrentZIndex += 100;
		l_bReturn = true;
	}
	else if (m_mapOverlays.find( p_iZIndex) == m_mapOverlays.end())
	{
		m_mapOverlays.insert( std::map <int, Overlay *>::value_type( p_iZIndex, p_pOverlay));
		l_bReturn = true;
	}

	while (m_mapOverlays.find( m_iCurrentZIndex) != m_mapOverlays.end())
	{
		m_iCurrentZIndex += 100;
		l_bReturn = true;
	}

	return l_bReturn;
}

void Overlay :: SetPosition( const Point2D<float> & p_ptPosition)
{
	if (m_pParent != NULL)
	{
		m_ptPosition.x = m_pParent->m_ptPosition.x + p_ptPosition.x * m_pParent->m_ptSize.x;
		m_ptPosition.y = m_pParent->m_ptPosition.y + p_ptPosition.y * m_pParent->m_ptSize.y;
	}
	else
	{
		m_ptPosition = p_ptPosition;
	}
}

void Overlay :: SetSize( const Point2D<float> & p_ptSize)
{
	if (m_pParent != NULL)
	{
		m_ptSize.x = p_ptSize.x * m_pParent->m_ptSize.x;
		m_ptSize.y = p_ptSize.y * m_pParent->m_ptSize.y;
	}
	else
	{
		m_ptSize = p_ptSize;
	}
}

void PanelOverlay :: Render()
{
	if (m_bVisible)
	{
		m_pRenderer->DrawPanel();

		for (std::map <int, Overlay *>::iterator l_it = m_mapOverlays.begin() ; l_it != m_mapOverlays.end() ; ++l_it)
		{
			l_it->second->Render();
		}
	}
}

void BorderPanelOverlay :: Render()
{
	if (m_bVisible)
	{
		m_pRenderer->DrawBorderPanel();

		for (std::map <int, Overlay *>::iterator l_it = m_mapOverlays.begin() ; l_it != m_mapOverlays.end() ; ++l_it)
		{
			l_it->second->Render();
		}
	}
}