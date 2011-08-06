#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/Overlay.hpp"
#include "Castor3D/Material.hpp"
#include "Castor3D/Pass.hpp"
#include "Castor3D/TextureUnit.hpp"
#include "Castor3D/Font.hpp"
#include "Castor3D/SceneFileParser.hpp"
#include "Castor3D/Scene.hpp"
#include "Castor3D/Root.hpp"

using namespace Castor3D;

#undef DrawText

//*************************************************************************************************

Factory<OverlayCategory, eOVERLAY_TYPE>::obj_map Factory<OverlayCategory, eOVERLAY_TYPE>::m_mapRegistered;

//*************************************************************************************************

bool Loader<OverlayCategory> :: Write( const OverlayCategory & p_overlay, File & p_file)
{
	String l_strTabs;
	Overlay * l_pParent = p_overlay.GetOverlay()->GetParent();

	while (l_pParent)
	{
		l_strTabs += cuT( '\t');
		l_pParent = l_pParent->GetParent();
	}

	bool l_bReturn = p_file.Print( 1024, cuT( "%s\tposition "), l_strTabs.c_str()) > 0;

	if (l_bReturn)
	{
		l_bReturn = Loader<Point2r>::Write( p_overlay.GetPosition(), p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.Print( 1024, cuT( "\n%s\tsize "), l_strTabs.c_str()) > 0;
	}

	if (l_bReturn)
	{
		l_bReturn = Loader<Point2r>::Write( p_overlay.GetSize(), p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.WriteLine( l_strTabs + cuT( "\tvisible ") + (p_overlay.IsVisible() ? String( "true") : String( "false"))) > 0;
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.Print( 1024, cuT( "%s\tzindex %i"), l_strTabs.c_str(), p_overlay.GetZIndex()) > 0;
	}

	if (l_bReturn && p_overlay.GetMaterial())
	{
		l_bReturn = p_file.WriteLine( l_strTabs + cuT( "\tmaterial ") + p_overlay.GetMaterial()->GetName()) > 0;
	}

	for (OverlayPtrIntMap::const_iterator l_it = p_overlay.GetOverlay()->Begin() ; l_it != p_overlay.GetOverlay()->End() ; ++l_it)
	{
		switch (l_it->second->GetType())
		{
		case eOVERLAY_TYPE_PANEL:
			l_bReturn = Loader<PanelOverlay>::Write( * static_pointer_cast<PanelOverlay>( l_it->second->GetOverlayCategory()), p_file);
			break;

		case eOVERLAY_TYPE_BORDER_PANEL:
			l_bReturn = Loader<BorderPanelOverlay>::Write( * static_pointer_cast<BorderPanelOverlay>( l_it->second->GetOverlayCategory()), p_file);
			break;

		case eOVERLAY_TYPE_TEXT:
			l_bReturn = Loader<TextOverlay>::Write( * static_pointer_cast<TextOverlay>( l_it->second->GetOverlayCategory()), p_file);
			break;

		default:
			l_bReturn = false;
		}
	}

	return l_bReturn;
}

//*************************************************************************************************

bool Loader<PanelOverlay> :: Write( const PanelOverlay & p_overlay, File & p_file)
{
	String l_strTabs;
	Overlay * l_pParent = p_overlay.GetOverlay()->GetParent();

	while (l_pParent)
	{
		l_strTabs += cuT( '\t');
		l_pParent = l_pParent->GetParent();
	}

	bool l_bReturn = p_file.WriteLine( l_strTabs + cuT( "panel_overlay ") + p_overlay.GetOverlay()->GetName() + cuT( "\n") + l_strTabs + cuT( "{\n")) > 0;

	if (l_bReturn)
	{
		l_bReturn = Loader<Overlay>::Write( * p_overlay.GetOverlay(), p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.WriteLine( l_strTabs + cuT( "}\n")) > 0;
	}

	return l_bReturn;
}

//*************************************************************************************************

bool Loader<BorderPanelOverlay> :: Write( const BorderPanelOverlay & p_overlay, File & p_file)
{
	String l_strTabs;
	Overlay * l_pParent = p_overlay.GetOverlay()->GetParent();

	while (l_pParent)
	{
		l_strTabs += cuT( '\t');
		l_pParent = l_pParent->GetParent();
	}

	bool l_bReturn = p_file.WriteLine( l_strTabs + cuT( "border_panel_overlay ") + p_overlay.GetOverlay()->GetName() + cuT( "\n") + l_strTabs + cuT( "{\n")) > 0;

	if (l_bReturn)
	{
		l_bReturn = p_file.Print( 1024, cuT( "%s\tborder_size "), l_strTabs.c_str()) > 0;
	}

	if (l_bReturn)
	{
		l_bReturn = Loader<Point4r>::Write( p_overlay.GetBorderSize(), p_file);
	}

	if (l_bReturn && p_overlay.GetBorderMaterial())
	{
		l_bReturn = p_file.WriteLine( l_strTabs + cuT( "\tborder_material ") + p_overlay.GetBorderMaterial()->GetName()) > 0;
	}

	if (l_bReturn)
	{
		l_bReturn = Loader<Overlay>::Write( * p_overlay.GetOverlay(), p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.WriteLine( l_strTabs + cuT( "}\n")) > 0;
	}

	return l_bReturn;
}

//*************************************************************************************************

bool Loader<TextOverlay> :: Write( const TextOverlay & p_overlay, File & p_file)
{
	String l_strTabs;
	Overlay * l_pParent = p_overlay.GetOverlay()->GetParent();

	while (l_pParent)
	{
		l_strTabs += cuT( '\t');
		l_pParent = l_pParent->GetParent();
	}

	bool l_bReturn = p_file.WriteLine( l_strTabs + cuT( "text_overlay ") + p_overlay.GetOverlay()->GetName() + cuT( "\n") + l_strTabs + cuT( "{\n")) > 0;

	if (l_bReturn)
	{
		l_bReturn = p_file.WriteLine( l_strTabs + cuT( "\tfont ") + p_overlay.GetFont()->GetFontName()) > 0;
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.WriteLine( l_strTabs + cuT( "\tcaption ") + p_overlay.GetCaption()) > 0;
	}

	if (l_bReturn)
	{
		l_bReturn = Loader<Overlay>::Write( * p_overlay.GetOverlay(), p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.WriteLine( l_strTabs + cuT( "}\n")) > 0;
	}

	return l_bReturn;
}

//*************************************************************************************************

OverlayCategory :: OverlayCategory( eOVERLAY_TYPE p_eType)
	:	m_eType( p_eType)
	,	m_bVisible( true)
	,	m_iCurrentZIndex( 100)
{
}

OverlayCategory :: ~OverlayCategory()
{
}

void OverlayCategory :: SetPosition( Overlay * p_pParent, const Point2r & p_ptPosition)
{
	if (p_pParent)
	{
		m_ptPosition.copy( p_pParent->GetPosition() + p_ptPosition * p_pParent->GetSize());
	}
	else
	{
		m_ptPosition.copy( p_ptPosition);
	}
}

void OverlayCategory :: SetSize( Overlay * p_pParent, const Point2r & p_ptSize)
{
	if (p_pParent)
	{
		m_ptSize.copy( p_ptSize * p_pParent->GetSize());
	}
	else
	{
		m_ptSize.copy( p_ptSize);
	}
}

void OverlayCategory :: SetMaterial( MaterialPtr p_pMaterial)
{
	m_pMaterial = p_pMaterial;

	if (m_pMaterial)
	{
		m_strMatName = m_pMaterial->GetName();
	}
	else
	{
		m_strMatName = cuEmptyString;
	}
}

BEGIN_SERIALISE_MAP( OverlayCategory, Serialisable)
	ADD_ELEMENT( m_eType)
	ADD_ELEMENT( m_ptPosition)
	ADD_ELEMENT( m_ptSize)
	ADD_ELEMENT( m_bVisible)
	ADD_ELEMENT( m_iCurrentZIndex)
	ADD_ELEMENT( m_strMatName)
END_SERIALISE_MAP()

//*************************************************************************************************

Overlay :: Overlay( eOVERLAY_TYPE p_eType)
	:	m_pParent( NULL)
	,	m_pScene( NULL)
{
	Factory<OverlayCategory, eOVERLAY_TYPE> l_factory;
	m_pOverlayCategory = l_factory.Create( p_eType);
	m_pOverlayCategory->SetOverlay( this);
}

Overlay :: Overlay( Scene * p_pScene, String const & p_strName, Overlay * p_pParent, eOVERLAY_TYPE p_eType)
	:	m_pParent( p_pParent)
	,	m_strName( p_strName)
	,	m_pScene( p_pScene)
{
	Factory<OverlayCategory, eOVERLAY_TYPE> l_factory;
	m_pOverlayCategory = l_factory.Create( p_eType);
	m_pOverlayCategory->SetOverlay( this);
}

Overlay :: ~Overlay()
{
}

void Overlay :: Register()
{
	Factory<OverlayCategory, eOVERLAY_TYPE> l_factory;
	l_factory.Register<PanelOverlay>(		eOVERLAY_TYPE_PANEL,		PanelOverlayPtr( new PanelOverlay));
	l_factory.Register<BorderPanelOverlay>(	eOVERLAY_TYPE_BORDER_PANEL,	BorderPanelOverlayPtr( new BorderPanelOverlay));
	l_factory.Register<TextOverlay>(		eOVERLAY_TYPE_TEXT,			TextOverlayPtr( new TextOverlay));
}

void Overlay :: Render( ePRIMITIVE_TYPE p_displayMode)
{
	m_pOverlayCategory->Render( m_pRenderer, p_displayMode);

	if (IsVisible())
	{
		for (OverlayPtrIntMap::iterator l_it = m_mapOverlays.begin() ; l_it != m_mapOverlays.end() ; ++l_it)
		{
			l_it->second->Render( p_displayMode);
		}
	}
}

bool Overlay :: AddChild( OverlayPtr p_pOverlay, int p_iZIndex)
{
	bool l_bReturn = false;

	if (p_iZIndex == 0)
	{
		m_mapOverlays.insert( OverlayPtrIntMap::value_type( GetZIndex(), p_pOverlay));
		SetZIndex( GetZIndex() + 100);
		l_bReturn = true;
	}
	else if (m_mapOverlays.find( p_iZIndex) == m_mapOverlays.end())
	{
		m_mapOverlays.insert( OverlayPtrIntMap::value_type( p_iZIndex, p_pOverlay));
		l_bReturn = true;
	}

	while (m_mapOverlays.find( GetZIndex()) != m_mapOverlays.end())
	{
		SetZIndex( GetZIndex() + 100);
		l_bReturn = true;
	}

	return l_bReturn;
}

int Overlay :: GetAvailableZIndex( Overlay * p_pParent, Scene * p_pScene, int p_iZIndex)
{
	int l_iReturn = p_iZIndex;

	if (p_pParent == NULL)
	{
		if (p_iZIndex == 0)
		{
			l_iReturn = p_pScene->IncrementZIndex();
		}
		else if ( ! p_pScene->HasOverlay( p_iZIndex))
		{
			l_iReturn = p_iZIndex;
		}
	}
	else
	{
		l_iReturn = p_iZIndex * 100;
	}

	return l_iReturn;
}

BEGIN_SERIALISE_MAP( Overlay, Serialisable)
	ADD_ELEMENT( m_strName)
	ADD_ELEMENT( * m_pOverlayCategory)
END_SERIALISE_MAP()

BEGIN_POST_SERIALISE( Overlay, Serialisable)
	bool l_bReturn = m_pFile->Write( GetChildsCount()) == sizeof( size_t);

	for (OverlayPtrIntMap::const_iterator l_it = m_mapOverlays.begin() ; l_it != m_mapOverlays.end() ; ++l_it)
	{
		l_bReturn = l_it->second->Serialise( * m_pFile);
	}
END_POST_SERIALISE()

BEGIN_PRE_UNSERIALISE( Overlay, Serialisable)
	m_pScene = static_cast<Overlay *>( m_pParentSerialisable)->GetScene();
END_PRE_UNSERIALISE()

BEGIN_POST_UNSERIALISE( Overlay, Serialisable)
	size_t l_uiSize;
	String l_strName;
	eOVERLAY_TYPE l_eType;
	OverlayPtr l_pOverlay;
	bool l_bReturn = m_pFile->Read( l_uiSize) == sizeof( size_t);

	for (size_t i = 0 ; i < l_uiSize && l_bReturn ; i++)
	{
		l_bReturn = m_pFile->Read( l_strName);

		if (l_bReturn)
		{
			l_bReturn = m_pFile->Read( l_eType) == sizeof( eOVERLAY_TYPE);
		}

		if (l_bReturn)
		{
			l_pOverlay = OverlayPtr( new Overlay( m_pScene, l_strName, this, l_eType));
			l_bReturn = l_pOverlay.use_count() > 0;
		}

		if (l_bReturn)
		{
			l_bReturn = l_pOverlay->Unserialise( * m_pFile, this);
		}
	}
END_POST_UNSERIALISE()

//*************************************************************************************************

PanelOverlay :: PanelOverlay()
	:	OverlayCategory( eOVERLAY_TYPE_PANEL)
{
}

PanelOverlay :: ~PanelOverlay()
{
}

OverlayCategoryPtr PanelOverlay :: Clone()
{
	return OverlayCategoryPtr( new PanelOverlay);
}

void PanelOverlay :: Render( OverlayRendererPtr p_pRenderer, ePRIMITIVE_TYPE p_displayMode)
{
	CASTOR_TRACK;
	if (IsVisible())
	{
		p_pRenderer->DrawPanel();
	}
}

//*************************************************************************************************

BorderPanelOverlay :: BorderPanelOverlay()
	:	OverlayCategory( eOVERLAY_TYPE_BORDER_PANEL)
{
}

BorderPanelOverlay :: ~BorderPanelOverlay()
{
}

OverlayCategoryPtr BorderPanelOverlay :: Clone()
{
	return OverlayCategoryPtr( new BorderPanelOverlay);
}

void BorderPanelOverlay :: Render( OverlayRendererPtr p_pRenderer, ePRIMITIVE_TYPE p_displayMode)
{
	CASTOR_TRACK;
	if (IsVisible())
	{
		p_pRenderer->DrawBorderPanel();
	}
}

void BorderPanelOverlay :: SetBorderMaterial( MaterialPtr p_pMaterial)
{
	m_pBorderMaterial = p_pMaterial;

	if (m_pBorderMaterial)
	{
		m_strBorderMatName = m_pBorderMaterial->GetName();
	}
	else
	{
		m_strBorderMatName = cuEmptyString;
	}
}

BEGIN_SERIALISE_MAP( BorderPanelOverlay, OverlayCategory)
	ADD_ELEMENT( m_ptBorderSize)
	ADD_ELEMENT( m_strBorderMatName)
END_SERIALISE_MAP()

BEGIN_POST_UNSERIALISE( BorderPanelOverlay, OverlayCategory)
	Root::MaterialManager * l_pManager = Root::GetSingleton()->GetMaterialManager();
	SetBorderMaterial( l_pManager->GetElement( m_strBorderMatName));
END_POST_UNSERIALISE()

//*************************************************************************************************

TextOverlay :: TextOverlay()
	:	OverlayCategory( eOVERLAY_TYPE_TEXT)
{
}

TextOverlay :: ~TextOverlay()
{
}

OverlayCategoryPtr TextOverlay :: Clone()
{
	return OverlayCategoryPtr( new TextOverlay);
}

void TextOverlay :: Render( OverlayRendererPtr p_pRenderer, ePRIMITIVE_TYPE p_displayMode)
{
	CASTOR_TRACK;
	if (IsVisible())
	{
		p_pRenderer->DrawText();
	}
}

void TextOverlay :: SetFont( String const & p_strFont)
{
	m_pFont = TextFontPtr( new TextFont( p_strFont));
	m_strFontName = p_strFont;
	_updateMaterial();
}

void TextOverlay :: SetMaterial( MaterialPtr p_pMaterial)
{
	OverlayCategory::SetMaterial( p_pMaterial);
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

void TextOverlay :: _updateMaterial()
{
	if (m_pFont && GetMaterial())
	{
		GetMaterial()->GetPass( 0)->AddTextureUnit()->SetTexture( eTEXTURE_TYPE_2D, m_pFont->GetImage());
	}
}

BEGIN_SERIALISE_MAP( TextOverlay, OverlayCategory)
	ADD_ELEMENT( m_strFontName)
	ADD_ELEMENT( m_strCaption)
END_SERIALISE_MAP()

//*************************************************************************************************