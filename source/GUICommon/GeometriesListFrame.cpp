#include "GuiCommon/PrecompiledHeader.hpp"

#include "GuiCommon/GeometriesListFrame.hpp"
#include "GuiCommon/ImagesLoader.hpp"

#include "xpms/geo_visible.xpm"
#include "xpms/geo_visible_sel.xpm"
#include "xpms/geo_cachee.xpm"
#include "xpms/geo_cachee_sel.xpm"
#include "xpms/dossier.xpm"
#include "xpms/dossier_sel.xpm"
#include "xpms/dossier_ouv.xpm"
#include "xpms/dossier_ouv_sel.xpm"
#include "xpms/submesh.xpm"
#include "xpms/submesh_sel.xpm"

using namespace Castor3D;
using namespace GuiCommon;

//*************************************************************************************************************

wxGeometryTreeItemData :: wxGeometryTreeItemData( GeometryPtr p_pGeometry)
	:	wxTreeItemData(),
		m_pGeometry( p_pGeometry)
{
}

wxGeometryTreeItemData :: ~wxGeometryTreeItemData()
{
}

//*************************************************************************************************************

wxGeometriesListFrame :: wxGeometriesListFrame( wxWindow * parent, const wxString & title, ScenePtr p_scene, const wxPoint & pos, const wxSize & size)
	:	wxFrame( parent, wxID_ANY, wxT( "Geometries list"), pos, size, wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX | wxCLIP_CHILDREN | wxFRAME_FLOAT_ON_PARENT)
	,	m_scene( p_scene)
	,	m_nbItems( 0)
{
	m_pTreeGeometries = new wxTreeCtrl( this, eID_TREE_GEOMETRIES, wxDefaultPosition, GetClientSize() - wxSize( 0, 50), wxTR_HIDE_ROOT | wxTR_HAS_BUTTONS | wxTR_MULTIPLE | wxTR_EXTENDED);
	m_pTreeGeometries->Show();
	m_pComboMaterials = new wxComboBox( this, eID_COMBO_MATERIALS, wxEmptyString, wxPoint( 0, GetClientSize().y - 45), wxSize( GetClientSize().x, 20));
	m_pComboMaterials->Hide();
	m_pButtonDeleteSelected = new wxButton( this, eID_BUTTON_DELETE, wxT( "Delete"), wxPoint( 0, GetClientSize().y - 20), wxSize( GetClientSize().x, 20));

	wxTreeItemId l_idRoot = m_pTreeGeometries->AddRoot( wxT( "Geometries"));
	GeometryPtr l_pGeometry;
	wxString l_strName;
	wxTreeItemId l_idGeometry;
	wxTreeItemId l_idSubmesh;

	int l_iCount = 0;

    m_pImages = new wxImageList( 32, 32, true);

	wxBusyCursor l_wait;
	wxBitmap * l_icons[eTREE_ID_COUNT];
	l_icons[eTREE_ID_VISIBLE]			= ImagesLoader::AddBitmap(	eBMP_VISIBLE,			geo_visible_xpm);
	l_icons[eTREE_ID_VISIBLE_SEL]		= ImagesLoader::AddBitmap(	eBMP_VISIBLE_SEL,		geo_visible_sel_xpm);
	l_icons[eTREE_ID_HIDDEN]			= ImagesLoader::AddBitmap(	eBMP_HIDDEN,			geo_cachee_xpm);
	l_icons[eTREE_ID_HIDDEN_SEL]		= ImagesLoader::AddBitmap(	eBMP_HIDDEN_SEL,		geo_cachee_sel_xpm);
	l_icons[eTREE_ID_GEOMETRY]			= ImagesLoader::AddBitmap(	eBMP_GEOMETRY,			dossier_xpm);
	l_icons[eTREE_ID_GEOMETRY_SEL]		= ImagesLoader::AddBitmap(	eBMP_GEOMETRY_SEL,		dossier_sel_xpm);
	l_icons[eTREE_ID_GEOMETRY_OPEN]		= ImagesLoader::AddBitmap(	eBMP_GEOMETRY_OPEN,		dossier_ouv_xpm);
	l_icons[eTREE_ID_GEOMETRY_OPEN_SEL]	= ImagesLoader::AddBitmap(	eBMP_GEOMETRY_OPEN_SEL,	dossier_ouv_sel_xpm);
	l_icons[eTREE_ID_SUBMESH]			= ImagesLoader::AddBitmap(	eBMP_SUBMESH,			submesh_xpm);
	l_icons[eTREE_ID_SUBMESH_SEL]		= ImagesLoader::AddBitmap(	eBMP_SUBMESH_SEL,		submesh_sel_xpm);

    for (size_t i = 0; i < eTREE_ID_COUNT ; i++)
	{
		int l_sizeOrig = l_icons[i]->GetWidth();

        if (l_sizeOrig == 32)
        {
            m_pImages->Add( * l_icons[i]);
        }
        else
        {
            m_pImages->Add( l_icons[i]->ConvertToImage().Rescale( 32, 32, wxIMAGE_QUALITY_HIGHEST));
        }
    }

	m_pTreeGeometries->AssignImageList( m_pImages);

    if (m_scene)
    {
        for (GeometryPtrStrMap::iterator l_it = m_scene->GeometriesBegin() ; l_it != m_scene->GeometriesEnd() ; ++l_it)
        {
            l_iCount = 0;
            l_pGeometry = l_it->second;
            l_strName = l_pGeometry->GetName();

            m_items.push_back( l_strName);
            l_idGeometry = m_pTreeGeometries->AppendItem( l_idRoot, l_strName, eTREE_ID_GEOMETRY, eTREE_ID_GEOMETRY_SEL, new wxGeometryTreeItemData( l_pGeometry));
            m_pTreeGeometries->AppendItem( l_idGeometry, l_pGeometry->IsVisible() ? wxT( "Visible") : wxT( "Hidden"), eTREE_ID_VISIBLE, eTREE_ID_VISIBLE_SEL, new wxGeometryTreeItemData( l_pGeometry));

            for (size_t i = 0 ; i < l_pGeometry->GetMesh()->GetNbSubmeshes() ; i++)
            {
                wxString l_strSubmesh = wxT( "Submesh ");
                l_strSubmesh << l_iCount++;
                l_idSubmesh = m_pTreeGeometries->AppendItem( l_idGeometry, l_strSubmesh, eTREE_ID_SUBMESH, eTREE_ID_SUBMESH_SEL, new wxGeometryTreeItemData( l_pGeometry));
            }

            m_setGeometriesInTree.insert( std::set <wxTreeItemId>::value_type( l_idGeometry));
        }
    }
}

wxGeometriesListFrame :: ~wxGeometriesListFrame()
{
}

BEGIN_EVENT_TABLE( wxGeometriesListFrame, wxFrame)
	EVT_BUTTON(					eID_BUTTON_DELETE,	wxGeometriesListFrame::OnDelete)
	EVT_TREE_ITEM_EXPANDED(		wxID_ANY,			wxGeometriesListFrame::OnExpandItem)
	EVT_TREE_ITEM_COLLAPSED(	wxID_ANY,			wxGeometriesListFrame::OnCollapseItem)
	EVT_TREE_ITEM_ACTIVATED(	wxID_ANY,			wxGeometriesListFrame::OnActivateItem)
END_EVENT_TABLE()

void wxGeometriesListFrame :: OnDelete( wxCommandEvent & event)
{
	wxArrayTreeItemIds l_arraySelected;
	m_pTreeGeometries->GetSelections( l_arraySelected);

	for (size_t i = 0 ; i < l_arraySelected.size() ; i++)
	{
		wxTreeItemId l_selected = l_arraySelected[i];
		GeometryPtr l_geometry = m_scene->GetGeometry( (char const *)( m_pTreeGeometries->GetItemText( l_selected).c_str()));

		if (l_geometry)
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

void wxGeometriesListFrame :: OnExpandItem( wxTreeEvent & event)
{
	wxTreeItemId l_item = event.GetItem();
	m_pTreeGeometries->SetItemImage( l_item, eTREE_ID_GEOMETRY_OPEN);
	m_pTreeGeometries->SetItemImage( l_item, eTREE_ID_GEOMETRY_OPEN_SEL, wxTreeItemIcon_Selected);

	event.Skip();
}

void wxGeometriesListFrame :: OnCollapseItem( wxTreeEvent & event)
{
	wxTreeItemId l_item = event.GetItem();
	m_pTreeGeometries->SetItemImage( l_item, eTREE_ID_GEOMETRY);
	m_pTreeGeometries->SetItemImage( l_item, eTREE_ID_GEOMETRY_SEL, wxTreeItemIcon_Selected);

	event.Skip();
}

void wxGeometriesListFrame :: OnActivateItem( wxTreeEvent & event)
{
	wxTreeItemId l_item = event.GetItem();
	wxGeometryTreeItemData * l_pItemData = (wxGeometryTreeItemData *)m_pTreeGeometries->GetItemData( l_item);
	wxString l_strText = m_pTreeGeometries->GetItemText( l_item);

	if (l_strText == wxT( "Visible"))
	{
		m_pComboMaterials->Hide();
		l_pItemData->GetGeometry()->SetVisible( false);
		m_pTreeGeometries->SetItemText( l_item, wxT( "Hidden"));
		m_pTreeGeometries->SetItemImage( l_item, eTREE_ID_HIDDEN);
		m_pTreeGeometries->SetItemImage( l_item, eTREE_ID_HIDDEN_SEL, wxTreeItemIcon_Selected);
	}
	else if (l_strText == wxT( "Hidden"))
	{
		m_pComboMaterials->Hide();
		l_pItemData->GetGeometry()->SetVisible( true);
		m_pTreeGeometries->SetItemText( l_item, wxT( "Visible"));
		m_pTreeGeometries->SetItemImage( l_item, eTREE_ID_VISIBLE);
		m_pTreeGeometries->SetItemImage( l_item, eTREE_ID_VISIBLE_SEL, wxTreeItemIcon_Selected);
	}
	else if (m_setGeometriesInTree.find( l_item) == m_setGeometriesInTree.end())
	{
		m_pComboMaterials->Clear();

		StringArray l_arrayNames;
		Root::GetSingleton()->GetMaterialManager()->GetNames( l_arrayNames);
		wxArrayString l_wxArrayNames;

		for (size_t i = 0 ; i < l_arrayNames.size() ; i++)
		{
			l_wxArrayNames.push_back( l_arrayNames[i]);
		}

		m_pComboMaterials->Append( l_wxArrayNames);
		m_pComboMaterials->Show();
	}
	else
	{
		m_pComboMaterials->Hide();
	}

	event.Skip();
}
