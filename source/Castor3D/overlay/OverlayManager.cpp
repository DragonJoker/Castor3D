#include "Castor3D/PrecompiledHeader.h"

#include "Castor3D/overlay/OverlayManager.h"
#include "Castor3D/overlay/Overlay.h"
#include "Castor3D/render_system/RenderSystem.h"
#include "Castor3D/scene/SceneFileParser.h"

using namespace Castor3D;

OverlayManager :: OverlayManager()
	:	m_iCurrentZIndex( 1)
{
}

OverlayManager :: ~OverlayManager()
{
	Cleanup();
}

void OverlayManager :: Cleanup()
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	m_mapOverlaysByZIndex.clear();
	m_mapOverlaysByName.clear();
}

void OverlayManager :: RenderOverlays()
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	RenderSystem::GetSingletonPtr()->BeginOverlaysRendering();

	for (OverlayPtrIntMap::iterator l_it = m_mapOverlaysByZIndex.begin() ; l_it != m_mapOverlaysByZIndex.end() ; ++l_it)
	{
		l_it->second->Render( eTriangles);
	}

	RenderSystem::GetSingletonPtr()->EndOverlaysRendering();
}

bool OverlayManager :: Write( File & p_file)const
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	bool l_bReturn = true;
	OverlayPtrIntMap::const_iterator l_it = m_mapOverlaysByZIndex.begin();

	bool l_bFirst = true;

	while (l_bReturn && l_it != m_mapOverlaysByZIndex.end())
	{
		if (l_bFirst)
		{
			l_bFirst = false;
		}
		else
		{
			p_file.WriteLine( "\n");
		}

		l_bReturn = l_it->second->Write( p_file);
		++l_it;
	}

	return l_bReturn;
}

bool OverlayManager :: Read( File & p_file)
{
	SceneFileParser l_parser( RenderSystem::GetSingletonPtr()->GetSceneManager());
	return l_parser.ParseFile( p_file);
}

bool OverlayManager :: Save( File & p_file)const
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	bool l_bReturn = p_file.Write( m_mapOverlaysByZIndex.size()) == sizeof( size_t);
	OverlayPtrIntMap::const_iterator l_it = m_mapOverlaysByZIndex.begin();

	while (l_bReturn && l_it != m_mapOverlaysByZIndex.end())
	{
		l_bReturn = l_it->second->Save( p_file);
		++l_it;
	}

	return l_bReturn;
}

bool OverlayManager :: Load( File & p_file)
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	size_t l_uiSize;
	bool l_bReturn = p_file.Write( l_uiSize) == sizeof( size_t);
	String l_strName;
	Overlay::eOVERLAY_TYPE l_eType;
	OverlayPtr l_pOverlay;

	for (size_t i = 0 ; i < l_uiSize && l_bReturn ; i++)
	{
		l_bReturn = p_file.Read( l_strName);

		if (l_bReturn)
		{
			l_bReturn = p_file.Read( l_eType) == sizeof( Overlay::eOVERLAY_TYPE);
		}

		if (l_bReturn)
		{
			switch (l_eType)
			{
			case Overlay::eOverlayPanel:
				l_pOverlay = CreateOverlay<PanelOverlay>( l_strName, NULL);
				break;

			case Overlay::eOverlayBorderPanel:
				l_pOverlay = CreateOverlay<BorderPanelOverlay>( l_strName, NULL);
				break;

			case Overlay::eOverlayText:
				l_pOverlay = CreateOverlay<TextOverlay>( l_strName, NULL);
				break;

			default:
				l_pOverlay.reset();
				l_bReturn = false;
			}
		}

		if (l_bReturn)
		{
			l_bReturn = l_pOverlay->Load( p_file);
		}
	}

	return l_bReturn;
}