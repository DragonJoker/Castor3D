#include "GUICommon/PrecompiledHeader.h"

#include "GUICommon/GeometriesListFrame.h"

using namespace Castor3D;
using namespace GUICommon;

//*************************************************************************************************************

GeometryTreeItemData :: GeometryTreeItemData( GeometryPtr p_pGeometry)
	:	wxTreeItemData(),
		m_pGeometry( p_pGeometry)
{
}

GeometryTreeItemData :: ~GeometryTreeItemData()
{
}

//*************************************************************************************************************

GeometriesListFrame :: GeometriesListFrame( MaterialManager * p_pManager, wxWindow * parent, const wxString & title,
											ScenePtr p_scene, const wxPoint & pos,
											const wxSize & size)
	:	wxFrame( parent, wxID_ANY, CU_T( "Geometries list"), pos, size, wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX | wxCLIP_CHILDREN | wxFRAME_FLOAT_ON_PARENT)
	,	m_scene( p_scene)
	,	m_items( NULL)
	,	m_nbItems( 0)
	,	m_pManager( p_pManager)
{
	m_pTreeGeometries = new wxTreeCtrl( this, eGeometriesList, wxDefaultPosition, GetClientSize() - wxSize( 0, 50), wxTR_HIDE_ROOT | wxTR_HAS_BUTTONS | wxTR_MULTIPLE | wxTR_EXTENDED);
	m_pTreeGeometries->Show();
	m_pComboMaterials = new wxComboBox( this, eMaterialsList, wxEmptyString, wxPoint( 0, GetClientSize().y - 45), wxSize( GetClientSize().x, 20));
	m_pComboMaterials->Hide();
	m_pButtonDeleteSelected = new wxButton( this, eDeleteSelected, CU_T( "Supprimer"), wxPoint( 0, GetClientSize().y - 20), wxSize( GetClientSize().x, 20));

	wxTreeItemId l_idRoot = m_pTreeGeometries->AddRoot( CU_T( "Géométries"));
	GeometryPtr l_pGeometry;
	wxString l_strName;
	wxTreeItemId l_idGeometry;
	wxTreeItemId l_idSubmesh;

	int l_iCount = 0;

    m_pImages = new wxImageList( 32, 32, true);

	wxBusyCursor l_wait;
	wxBitmap l_icons[eNbImages];
	l_icons[eGeoVisible]		= wxBITMAP( GeoVisible);
	l_icons[eGeoVisibleSel]		= wxBITMAP( GeoVisibleSel);
	l_icons[eGeoCachee]			= wxBITMAP( GeoCachee);
	l_icons[eGeoCacheeSel]		= wxBITMAP( GeoCacheeSel);
	l_icons[eGeometrie]			= wxBITMAP( Dossier);
	l_icons[eGeometrieSel]		= wxBITMAP( DossierSel);
	l_icons[eGeometrieOuv]		= wxBITMAP( DossierOuv);
	l_icons[eGeometrieOuvSel]	= wxBITMAP( DossierOuvSel);
	l_icons[eSubmesh]			= wxBITMAP( Submesh);
	l_icons[eSubmeshSel]		= wxBITMAP( SubmeshSel);

    for (size_t i = 0; i < eNbImages ; i++)
	{
		int l_sizeOrig = l_icons[i].GetWidth();

        if (l_sizeOrig == 32)
        {
            m_pImages->Add( l_icons[i]);
        }
        else
        {
            m_pImages->Add( l_icons[i].ConvertToImage().Rescale( 32, 32, wxIMAGE_QUALITY_HIGH));
        }
    }

	m_pTreeGeometries->AssignImageList( m_pImages);

	for (GeometryPtrStrMap::iterator l_it = m_scene->GetGeometriesIterator() ; l_it != m_scene->GetGeometriesEnd() ; ++l_it)
	{
		l_iCount = 0;
		l_pGeometry = l_it->second;
		l_strName = l_pGeometry->GetName().c_str();

		m_items.push_back( l_strName);
		l_idGeometry = m_pTreeGeometries->AppendItem( l_idRoot, l_strName, eGeometrie, eGeometrieSel, new GeometryTreeItemData( l_pGeometry));
		m_pTreeGeometries->AppendItem( l_idGeometry, l_pGeometry->IsVisible() ? wxT( "Visible") : wxT( "Caché"), eGeoVisible, eGeoVisibleSel, new GeometryTreeItemData( l_pGeometry));

		for (size_t i = 0 ; i < l_pGeometry->GetMesh()->GetNbSubmeshes() ; i++)
		{
			wxString l_strSubmesh = wxT( "Submesh ");
			l_strSubmesh << l_iCount++;
			l_idSubmesh = m_pTreeGeometries->AppendItem( l_idGeometry, l_strSubmesh, eSubmesh, eSubmeshSel, new GeometryTreeItemData( l_pGeometry));
		}

		m_setGeometriesInTree.insert( std::set <wxTreeItemId>::value_type( l_idGeometry));
	}
}

GeometriesListFrame :: ~GeometriesListFrame()
{
}

BEGIN_EVENT_TABLE( GeometriesListFrame, wxFrame)
	EVT_BUTTON(					eDeleteSelected,	GeometriesListFrame::OnDelete)
	EVT_TREE_ITEM_EXPANDED(		wxID_ANY,			GeometriesListFrame::OnExpandItem)
	EVT_TREE_ITEM_COLLAPSED(	wxID_ANY,			GeometriesListFrame::OnCollapseItem)
	EVT_TREE_ITEM_ACTIVATED(	wxID_ANY,			GeometriesListFrame::OnActivateItem)
END_EVENT_TABLE()

void GeometriesListFrame :: OnDelete( wxCommandEvent & event)
{
	wxArrayTreeItemIds l_arraySelected;
	m_pTreeGeometries->GetSelections( l_arraySelected);

	for (size_t i = 0 ; i < l_arraySelected.size() ; i++)
	{
		wxTreeItemId l_selected = l_arraySelected[i];
		GeometryPtr l_geometry = m_scene->GetGeometry( m_pTreeGeometries->GetItemText( l_selected).c_str());

		if (l_geometry != NULL)
		{
			m_scene->RemoveGeometry( l_geometry);
		}
	}

	for (size_t i = 0 ; i < l_arraySelected.size() ; i++)
	{
		wxTreeItemId l_selected = l_arraySelected[i];
		m_pTreeGeometries->Delete( l_selected);
	}

	m_pTreeGeometries->Refresh();
	Refresh();

	event.Skip();
}

void GeometriesListFrame :: OnExpandItem( wxTreeEvent & event)
{
	wxTreeItemId l_item = event.GetItem();
	m_pTreeGeometries->SetItemImage( l_item, eGeometrieOuv);
	m_pTreeGeometries->SetItemImage( l_item, eGeometrieOuvSel, wxTreeItemIcon_Selected);

	event.Skip();
}

void GeometriesListFrame :: OnCollapseItem( wxTreeEvent & event)
{
	wxTreeItemId l_item = event.GetItem();
	m_pTreeGeometries->SetItemImage( l_item, eGeometrie);
	m_pTreeGeometries->SetItemImage( l_item, eGeometrieSel, wxTreeItemIcon_Selected);

	event.Skip();
}

void GeometriesListFrame :: OnActivateItem( wxTreeEvent & event)
{
	wxTreeItemId l_item = event.GetItem();
	GeometryTreeItemData * l_pItemData = (GeometryTreeItemData *)m_pTreeGeometries->GetItemData( l_item);
	String l_strText = m_pTreeGeometries->GetItemText( l_item);

	if (l_strText == "Visible")
	{
		m_pComboMaterials->Hide();
		l_pItemData->GetGeometry()->SetVisible( false);
		m_pTreeGeometries->SetItemText( l_item, CU_T( "Caché"));
		m_pTreeGeometries->SetItemImage( l_item, eGeoCachee);
		m_pTreeGeometries->SetItemImage( l_item, eGeoCacheeSel, wxTreeItemIcon_Selected);
	}
	else if (l_strText == "Caché")
	{
		m_pComboMaterials->Hide();
		l_pItemData->GetGeometry()->SetVisible( true);
		m_pTreeGeometries->SetItemText( l_item, CU_T( "Visible"));
		m_pTreeGeometries->SetItemImage( l_item, eGeoVisible);
		m_pTreeGeometries->SetItemImage( l_item, eGeoVisibleSel, wxTreeItemIcon_Selected);
	}
	else if (m_setGeometriesInTree.find( l_item) == m_setGeometriesInTree.end())
	{
		m_pComboMaterials->Clear();

		StringArray l_arrayNames;
		m_pManager->GetMaterialNames( l_arrayNames);
		wxArrayString l_wxarrayNames;

		for (size_t i = 0 ; i < l_arrayNames.size() ; i++)
		{
			l_wxarrayNames.push_back( l_arrayNames[i]);
		}

		m_pComboMaterials->Append( l_wxarrayNames);

		m_pComboMaterials->Show();
	}
	else
	{
		m_pComboMaterials->Hide();
	}

	event.Skip();
}