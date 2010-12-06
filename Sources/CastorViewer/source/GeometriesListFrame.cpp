#include "PrecompiledHeader.h"

#include "GeometriesListFrame.h"
#include "MainFrame.h"

#include "icon1.xpm"
#include "icon2.xpm"
#include "icon3.xpm"
#include "icon4.xpm"
#include "icon5.xpm"
#include "icon6.xpm"
#include "icon7.xpm"

using namespace Castor3D;
using namespace CastorViewer;

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

GeometriesListFrame :: GeometriesListFrame( wxWindow * parent, const wxString & title,
											ScenePtr p_scene, const wxPoint & pos,
											const wxSize & size)
	:	wxFrame( parent, wxID_ANY, CU_T( "Geometries list"), pos, size),
		m_scene( p_scene),
		m_items( NULL),
		m_nbItems( 0)
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
	GeometryTreeItemData * l_pItemData;

	int l_iCount = 0;

    wxImageList * l_images = new wxImageList( 16, 16, true);

	wxBusyCursor l_wait;
    wxIcon l_icons[7];
    l_icons[0] = wxIcon( icon1_xpm);
    l_icons[1] = wxIcon( icon2_xpm);
    l_icons[2] = wxIcon( icon3_xpm);
    l_icons[3] = wxIcon( icon4_xpm);
    l_icons[4] = wxIcon( icon5_xpm);
	l_icons[5] = wxIcon( icon6_xpm);
	l_icons[6] = wxIcon( icon7_xpm);

    int l_sizeOrig = l_icons[0].GetWidth();

    for (size_t i = 0; i < WXSIZEOF( l_icons); i++)
    {
        if (l_sizeOrig == 16)
        {
            l_images->Add( l_icons[i]);
        }
        else
        {
            l_images->Add( wxBitmap( wxBitmap( l_icons[i]).ConvertToImage().Rescale( 16, 16)));
        }
    }

	m_pTreeGeometries->AssignImageList( l_images);

	for (GeometryPtrStrMap::iterator l_it = m_scene->GetGeometriesIterator() ; l_it != m_scene->GetGeometriesEnd() ; ++l_it)
	{
		l_iCount = 0;
		l_pGeometry = l_it->second;
		l_strName = l_pGeometry->GetName().c_str();

		m_items.push_back( l_strName);
		l_idGeometry = m_pTreeGeometries->AppendItem( l_idRoot, l_strName, eFolder, eFolderSelected, new GeometryTreeItemData( l_pGeometry));
		m_pTreeGeometries->AppendItem( l_idGeometry, l_pGeometry->IsVisible() ? CU_T( "Visible") : CU_T( "Caché"), eFile, eFileSelected, new GeometryTreeItemData( l_pGeometry));

		for (size_t i = 0 ; i < l_pGeometry->GetMesh()->GetNbSubmeshes() ; i++)
		{
			wxString l_strSubmesh;
			l_strSubmesh << l_iCount++;
			l_idSubmesh = m_pTreeGeometries->AppendItem( l_idGeometry, l_strSubmesh, eFile, eFileSelected, new GeometryTreeItemData( l_pGeometry));
		}

		m_setGeometriesInTree.insert( std::set <wxTreeItemId>::value_type( l_idGeometry));
	}
}

GeometriesListFrame :: ~GeometriesListFrame()
{
}

BEGIN_EVENT_TABLE( GeometriesListFrame, wxFrame)
EVT_CHECKLISTBOX(			eGeometriesList,	GeometriesListFrame::OnCheck)
EVT_BUTTON(					eDeleteSelected,	GeometriesListFrame::OnDelete)
EVT_TREE_ITEM_EXPANDED(		wxID_ANY,			GeometriesListFrame::OnExpandItem)
EVT_TREE_ITEM_COLLAPSED(	wxID_ANY,			GeometriesListFrame::OnCollapseItem)
EVT_TREE_ITEM_ACTIVATED(	wxID_ANY,			GeometriesListFrame::OnActivateItem)
END_EVENT_TABLE()

void GeometriesListFrame :: OnCheck( wxCommandEvent & event)
{
	unsigned int l_selected = static_cast <unsigned int>( event.GetInt());
	GeometryPtr l_geometry = m_scene->GetGeometry( m_items[l_selected].c_str());

	if ( ! l_geometry.null())
	{
		l_geometry->GetParent()->SetVisible( true);//m_list->IsChecked( l_selected));
	}
}

void GeometriesListFrame :: OnDelete( wxCommandEvent & event)
{
	wxArrayTreeItemIds l_arraySelected;
	m_pTreeGeometries->GetSelections( l_arraySelected);

	for (size_t i = 0 ; i < l_arraySelected.size() ; i++)
	{
		GeometryPtr l_geometry = m_scene->GetGeometry( m_items[l_arraySelected[i]].c_str());
		m_scene->RemoveGeometry( l_geometry);
	}
}

void GeometriesListFrame :: OnExpandItem( wxTreeEvent & event)
{
	m_pTreeGeometries->SetItemImage( event.GetItem(), eFolderOpened);
	m_pTreeGeometries->SetItemImage( event.GetItem(), eFolderOpened, wxTreeItemIcon_Selected);
}

void GeometriesListFrame :: OnCollapseItem( wxTreeEvent & event)
{
	m_pTreeGeometries->SetItemImage( event.GetItem(), eFolder);
	m_pTreeGeometries->SetItemImage( event.GetItem(), eFolderSelected, wxTreeItemIcon_Selected);
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
	}
	else if (l_strText == "Caché")
	{
		m_pComboMaterials->Hide();
		l_pItemData->GetGeometry()->SetVisible( true);
		m_pTreeGeometries->SetItemText( l_item, CU_T( "Visible"));
	}
	else if (m_setGeometriesInTree.find( l_item) == m_setGeometriesInTree.end())
	{
		m_pComboMaterials->Clear();

		StringArray l_arrayNames;
		MaterialManager::GetMaterialNames( l_arrayNames);
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
}