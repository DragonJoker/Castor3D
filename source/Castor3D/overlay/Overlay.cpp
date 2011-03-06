#include "Castor3D/PrecompiledHeader.h"

#include "Castor3D/overlay/Overlay.h"
#include "Castor3D/overlay/OverlayManager.h"
#include "Castor3D/material/Material.h"
#include "Castor3D/material/MaterialManager.h"
#include "Castor3D/material/Pass.h"
#include "Castor3D/material/TextureUnit.h"
#include "Castor3D/text/Font.h"
#include "Castor3D/scene/SceneManager.h"
#include "Castor3D/scene/SceneFileParser.h"

using namespace Castor3D;

//*************************************************************************************************

Overlay :: Overlay( OverlayManager * p_pManager, const String & p_strName, Overlay * p_pParent, eOVERLAY_TYPE p_eType)
	:	m_pParent( p_pParent)
	,	m_ptPosition( 0, 0)
	,	m_ptSize( 0, 0)
	,	m_strName( p_strName)
	,	m_bVisible( true)
	,	m_iCurrentZIndex( 100)
	,	m_eType( p_eType)
	,	m_pManager( p_pManager)
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
	if (m_pParent != NULL)
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
	if (m_pParent != NULL)
	{
		m_ptSize = p_ptSize * m_pParent->m_ptSize;
	}
	else
	{
		m_ptSize = p_ptSize;
	}
}

bool Overlay :: Write( File & p_file)const
{
	String l_strTabs;
	Overlay * l_pParent = m_pParent;

	while (l_pParent != NULL)
	{
		l_strTabs += CU_T( '\t');
		l_pParent = m_pParent->GetParent();
	}

	bool l_bReturn = p_file.Print( 1024, "%s\tposition %f %f", l_strTabs.char_str(), m_ptPosition[0], m_ptPosition[1]) > 0;

	if (l_bReturn)
	{
		l_bReturn = p_file.Print( 1024, "%s\tsize %f %f", l_strTabs.char_str(), m_ptSize[0], m_ptSize[1]) > 0;
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.WriteLine( l_strTabs + CU_T( "\tvisible ") + (m_bVisible ? String( "true") : String( "false"))) > 0;
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.Print( 1024, "%s\tzindex %i", l_strTabs.char_str(), m_iCurrentZIndex) > 0;
	}

	if (l_bReturn && m_pMaterial != NULL)
	{
		l_bReturn = p_file.WriteLine( l_strTabs + "\tmaterial " + m_pMaterial->GetName()) > 0;
	}

	for (OverlayPtrIntMap::const_iterator l_it = m_mapOverlays.begin() ; l_it != m_mapOverlays.end() ; ++l_it)
	{
		l_bReturn = l_it->second->Write( p_file);
	}

	return l_bReturn;
}

bool Overlay :: Save( File & p_file)const
{
	bool l_bReturn = p_file.Write( m_strName);

	if (l_bReturn)
	{
		l_bReturn = p_file.Write( m_eType) == sizeof( eOVERLAY_TYPE);
	}

	if (l_bReturn)
	{
		l_bReturn = m_ptPosition.Save( p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = m_ptSize.Save( p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.Write( m_bVisible) == sizeof( bool);
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.Write( m_iCurrentZIndex) == sizeof( int);
	}

	if (l_bReturn)
	{
		String l_strName;

		if (m_pMaterial != NULL)
		{
			l_strName = m_pMaterial->GetName();
		}

		l_bReturn = p_file.Write( l_strName);
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.Write( m_mapOverlays.size()) == sizeof( size_t);
	}

	for (OverlayPtrIntMap::const_iterator l_it = m_mapOverlays.begin() ; l_it != m_mapOverlays.end() ; ++l_it)
	{
		l_bReturn = l_it->second->Save( p_file);
	}

	return l_bReturn;
}

bool Overlay :: Load( File & p_file)
{
	bool l_bReturn = m_ptPosition.Load( p_file);
	size_t l_uiSize;

	if (l_bReturn)
	{
		l_bReturn = m_ptSize.Load( p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.Read( m_bVisible) == sizeof( bool);
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.Read( m_iCurrentZIndex) == sizeof( int);
	}

	if (l_bReturn)
	{
		String l_strName;
		l_bReturn = p_file.Read( l_strName);

		if ( ! l_strName.empty())
		{
			m_pMaterial = RenderSystem::GetSingletonPtr()->GetSceneManager()->GetMaterialManager()->GetElement( l_strName);
			l_bReturn = m_pMaterial != NULL;
		}
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.Read( l_uiSize) == sizeof( size_t);
	}

	String l_strName;
	eOVERLAY_TYPE l_eType;
	OverlayPtr l_pOverlay;

	for (size_t i = 0 ; i < l_uiSize && l_bReturn ; i++)
	{
		l_bReturn = p_file.Read( l_strName);

		if (l_bReturn)
		{
			l_bReturn = p_file.Read( l_eType) == sizeof( eOVERLAY_TYPE);
		}

		if (l_bReturn)
		{
			switch (l_eType)
			{
			case eOverlayPanel:
				l_pOverlay = m_pManager->CreateOverlay<PanelOverlay>( l_strName, this);
				break;

			case eOverlayBorderPanel:
				l_pOverlay = m_pManager->CreateOverlay<BorderPanelOverlay>( l_strName, this);
				break;

			case eOverlayText:
				l_pOverlay = m_pManager->CreateOverlay<TextOverlay>( l_strName, this);
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

//*************************************************************************************************

void PanelOverlay :: Render( ePRIMITIVE_TYPE p_displayMode)
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

bool PanelOverlay :: Write( File & p_file)const
{
	String l_strTabs;
	Overlay * l_pParent = m_pParent;

	while (l_pParent != NULL)
	{
		l_strTabs += CU_T( '\t');
		l_pParent = m_pParent->GetParent();
	}

	bool l_bReturn = p_file.WriteLine( l_strTabs + CU_T( "panel_overlay ") + m_strName + CU_T( "\n") + l_strTabs + CU_T( "{\n")) > 0;

	if (l_bReturn)
	{
		l_bReturn = Overlay::Write( p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.WriteLine( l_strTabs + CU_T( "}\n")) > 0;
	}

	return l_bReturn;
}

bool PanelOverlay :: Save( File & p_file)const
{
	return Overlay::Save( p_file);
}

bool PanelOverlay :: Load( File & p_file)
{
	return Overlay::Load( p_file);
}

//*************************************************************************************************

void BorderPanelOverlay :: Render( ePRIMITIVE_TYPE p_displayMode)
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

bool BorderPanelOverlay :: Write( File & p_file)const
{
	String l_strTabs;
	Overlay * l_pParent = m_pParent;

	while (l_pParent != NULL)
	{
		l_strTabs += CU_T( '\t');
		l_pParent = m_pParent->GetParent();
	}

	bool l_bReturn = p_file.WriteLine( l_strTabs + CU_T( "border_panel_overlay ") + m_strName + CU_T( "\n") + l_strTabs + CU_T( "{\n")) > 0;

	if (l_bReturn)
	{
		l_bReturn = p_file.Print( 1024, "%s\tborder_size %f %f %f %f", l_strTabs.char_str(), m_ptBorderSize[0], m_ptBorderSize[1], m_ptBorderSize[2], m_ptBorderSize[3]) > 0;
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.WriteLine( l_strTabs + CU_T( "\tborder_material ") + m_pBorderMaterial->GetName()) > 0;
	}

	if (l_bReturn)
	{
		l_bReturn = Overlay::Write( p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.WriteLine( l_strTabs + CU_T( "}\n")) > 0;
	}

	return l_bReturn;
}

bool BorderPanelOverlay :: Save( File & p_file)const
{
	bool l_bReturn = m_ptBorderSize.Save( p_file);

	if (l_bReturn)
	{
		String l_strName;

		if (m_pBorderMaterial != NULL)
		{
			l_strName = m_pBorderMaterial->GetName();
		}

		l_bReturn = p_file.Write( l_strName);
	}

	if (l_bReturn)
	{
		l_bReturn = Overlay::Save( p_file);
	}

	return l_bReturn;
}

bool BorderPanelOverlay :: Load( File & p_file)
{
	bool l_bReturn = m_ptBorderSize.Load( p_file);

	if (l_bReturn)
	{
		String l_strName;
		l_bReturn = p_file.Read( l_strName);

		if ( ! l_strName.empty())
		{
			m_pBorderMaterial = RenderSystem::GetSingletonPtr()->GetSceneManager()->GetMaterialManager()->GetElement( l_strName);
			l_bReturn = m_pBorderMaterial != NULL;
		}
	}

	if (l_bReturn)
	{
		l_bReturn = Overlay::Load( p_file);
	}

	return l_bReturn;
}

//*************************************************************************************************

TextOverlay :: ~TextOverlay()
{
}

void TextOverlay :: Render( ePRIMITIVE_TYPE p_displayMode)
{
	if (m_bVisible)
	{
		m_pRenderer->DrawText();

		for (OverlayPtrIntMap::iterator l_it = m_mapOverlays.begin() ; l_it != m_mapOverlays.end() ; ++l_it)
		{
			l_it->second->Render( p_displayMode);
		}
	}
}

void TextOverlay :: SetFont( const String & p_strFont, FontManager * p_pManager)
{
	m_pFont = TextFontPtr( new TextFont( p_pManager, p_strFont));
	_updateMaterial();
}

void TextOverlay :: SetMaterial( MaterialPtr p_pMaterial)
{
	m_pMaterial = p_pMaterial;
	_updateMaterial();
}

VertexBufferPtr TextOverlay :: GetVertexBuffer()const
{
	return TextFont::GetVertexBuffer();
}

IndexBufferPtr TextOverlay :: GetIndexBuffer()const
{
	return TextFont::GetIndexBuffer();
}

bool TextOverlay :: Write( File & p_file)const
{
	String l_strTabs;
	Overlay * l_pParent = m_pParent;

	while (l_pParent != NULL)
	{
		l_strTabs += CU_T( '\t');
		l_pParent = m_pParent->GetParent();
	}

	bool l_bReturn = p_file.WriteLine( l_strTabs + CU_T( "text_overlay ") + m_strName + CU_T( "\n") + l_strTabs + CU_T( "{\n")) > 0;

	if (l_bReturn)
	{
		l_bReturn = p_file.WriteLine( l_strTabs + CU_T( "\tfont ") + m_pFont->GetFontName()) > 0;
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.WriteLine( l_strTabs + CU_T( "\tcaption ") + m_strCaption) > 0;
	}

	if (l_bReturn)
	{
		l_bReturn = Overlay::Write( p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.WriteLine( l_strTabs + CU_T( "}\n")) > 0;
	}

	return l_bReturn;
}

bool TextOverlay :: Save( File & p_file)const
{
	bool l_bReturn = p_file.Write( m_pFont->GetFontName());

	if (l_bReturn)
	{
		l_bReturn = p_file.Write( m_strCaption);
	}

	if (l_bReturn)
	{
		l_bReturn = Overlay::Save( p_file);
	}

	return l_bReturn;
}

bool TextOverlay :: Load( File & p_file)
{
	String l_strName;
	bool l_bReturn = p_file.Write( l_strName);

	if (l_bReturn)
	{
		m_pFont = TextFontPtr( new TextFont( RenderSystem::GetSingletonPtr()->GetSceneManager()->GetFontManager(), l_strName));
		l_bReturn = p_file.Read( m_strCaption);
	}

	if (l_bReturn)
	{
		Overlay::Load( p_file);
	}

	return l_bReturn;
}

void TextOverlay :: _updateMaterial()
{
	if (m_pFont != NULL && m_pMaterial != NULL)
	{
		m_pMaterial->GetPass( 0)->AddTextureUnit()->SetTexture2D( m_pFont->GetImage());
	}
}

//*************************************************************************************************
