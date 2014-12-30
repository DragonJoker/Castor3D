#include "GeometriesListFrame.hpp"
#include "GeometryTreeItemData.hpp"
#include "SubmeshTreeItemData.hpp"
#include "ImagesLoader.hpp"

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

#include <wx/imaglist.h>
#include <wx/aui/framemanager.h>

#include <Geometry.hpp>
#include <Engine.hpp>
#include <Material.hpp>
#include <Mesh.hpp>
#include <Scene.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GuiCommon
{
	static const int GC_IMG_SIZE	= 20;

	wxGeometriesListFrame::wxGeometriesListFrame( Engine * p_pEngine, wxWindow * p_pParent, wxString const & p_strTitle, SceneSPtr p_pScene, wxPoint const & p_ptPos, wxSize const & p_size )
		:	wxPanel( p_pParent, wxID_ANY, /*p_strTitle, */p_ptPos, p_size/*, wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX | wxCLIP_CHILDREN | wxFRAME_FLOAT_ON_PARENT*/ )
		,	m_pScene( p_pScene	)
		,	m_uiNbItems( 0	)
		,	m_pEngine( p_pEngine	)
	{
		wxSize l_size = GetClientSize();
		m_pTreeGeometries = new wxTreeCtrl( this, eID_TREE_GEOMETRIES, wxDefaultPosition, l_size - wxSize( 0, 50 ), wxTR_HIDE_ROOT | wxTR_HAS_BUTTONS | wxTR_MULTIPLE | wxTR_EXTENDED );
		m_pTreeGeometries->Show();
		m_pComboMaterials = new wxComboBox( this, eID_COMBO_MATERIALS, wxEmptyString, wxPoint( 0, l_size.y - 50 ), wxSize( l_size.x, 25 ) );
		m_pComboMaterials->Hide();
		m_pButtonDeleteSelected = new wxButton( this, eID_BUTTON_DELETE, _( "Delete" ), wxPoint( 0, l_size.y - 25 ), wxSize( l_size.x, 25 ) );
		wxTreeItemId l_idRoot = m_pTreeGeometries->AddRoot( _( "Geometries" ) );
		GeometrySPtr l_pGeometry;
		wxString l_strName;
		wxTreeItemId l_idGeometry;
		wxTreeItemId l_idSubmesh;
		m_pListImages = new wxImageList( GC_IMG_SIZE, GC_IMG_SIZE, true );
		wxBusyCursor l_wait;
		wxImage * l_icons[eTREE_ID_COUNT];
		wxImagesLoader::AddBitmap(	eBMP_VISIBLE,			geo_visible_xpm	);
		wxImagesLoader::AddBitmap(	eBMP_VISIBLE_SEL,		geo_visible_sel_xpm	);
		wxImagesLoader::AddBitmap(	eBMP_HIDDEN,			geo_cachee_xpm	);
		wxImagesLoader::AddBitmap(	eBMP_HIDDEN_SEL,		geo_cachee_sel_xpm	);
		wxImagesLoader::AddBitmap(	eBMP_GEOMETRY,			dossier_xpm	);
		wxImagesLoader::AddBitmap(	eBMP_GEOMETRY_SEL,		dossier_sel_xpm	);
		wxImagesLoader::AddBitmap(	eBMP_GEOMETRY_OPEN,		dossier_ouv_xpm	);
		wxImagesLoader::AddBitmap(	eBMP_GEOMETRY_OPEN_SEL,	dossier_ouv_sel_xpm	);
		wxImagesLoader::AddBitmap(	eBMP_SUBMESH,			submesh_xpm	);
		wxImagesLoader::AddBitmap(	eBMP_SUBMESH_SEL,		submesh_sel_xpm	);
		wxImagesLoader::WaitAsyncLoads();
		l_icons[eTREE_ID_VISIBLE]			= wxImagesLoader::GetBitmap(	eBMP_VISIBLE	);
		l_icons[eTREE_ID_VISIBLE_SEL]		= wxImagesLoader::GetBitmap(	eBMP_VISIBLE_SEL	);
		l_icons[eTREE_ID_HIDDEN]			= wxImagesLoader::GetBitmap(	eBMP_HIDDEN	);
		l_icons[eTREE_ID_HIDDEN_SEL]		= wxImagesLoader::GetBitmap(	eBMP_HIDDEN_SEL	);
		l_icons[eTREE_ID_GEOMETRY]			= wxImagesLoader::GetBitmap(	eBMP_GEOMETRY	);
		l_icons[eTREE_ID_GEOMETRY_SEL]		= wxImagesLoader::GetBitmap(	eBMP_GEOMETRY_SEL	);
		l_icons[eTREE_ID_GEOMETRY_OPEN]		= wxImagesLoader::GetBitmap(	eBMP_GEOMETRY_OPEN	);
		l_icons[eTREE_ID_GEOMETRY_OPEN_SEL]	= wxImagesLoader::GetBitmap(	eBMP_GEOMETRY_OPEN_SEL	);
		l_icons[eTREE_ID_SUBMESH]			= wxImagesLoader::GetBitmap(	eBMP_SUBMESH	);
		l_icons[eTREE_ID_SUBMESH_SEL]		= wxImagesLoader::GetBitmap(	eBMP_SUBMESH_SEL	);

		for ( uint32_t i = 0; i < eTREE_ID_COUNT; i++ )
		{
			int l_sizeOrig = l_icons[i]->GetWidth();

			if ( l_sizeOrig != GC_IMG_SIZE )
			{
				l_icons[i]->Rescale( GC_IMG_SIZE, GC_IMG_SIZE, wxIMAGE_QUALITY_HIGHEST );
			}

			m_pListImages->Add( wxImage( *l_icons[i] ) );
		}

		m_pTreeGeometries->AssignImageList( m_pListImages );

		if ( m_pScene )
		{
			std::for_each( m_pScene->GeometriesBegin(), m_pScene->GeometriesEnd(), [&]( std::pair< String, GeometrySPtr > p_pair )
			{
				int l_iCount = 0;
				l_pGeometry = p_pair.second;
				l_strName = l_pGeometry->GetName();
				m_arrayItems.push_back( l_strName );
				l_idGeometry = m_pTreeGeometries->AppendItem( l_idRoot, l_strName, eTREE_ID_GEOMETRY, eTREE_ID_GEOMETRY_SEL, new wxGeometryTreeItemData( l_pGeometry ) );
				m_pTreeGeometries->AppendItem( l_idGeometry, l_pGeometry->IsVisible() ? _( "Visible" ) : _( "Hidden" ), eTREE_ID_VISIBLE, eTREE_ID_VISIBLE_SEL, new wxGeometryTreeItemData( l_pGeometry ) );
				std::for_each( l_pGeometry->GetMesh()->Begin(), l_pGeometry->GetMesh()->End(), [&]( SubmeshSPtr p_pSubmesh )
				{
					wxString l_strSubmesh = _( "Submesh " );
					l_strSubmesh << l_iCount++;
					l_idSubmesh = m_pTreeGeometries->AppendItem( l_idGeometry, l_strSubmesh, eTREE_ID_SUBMESH, eTREE_ID_SUBMESH_SEL, new wxSubmeshTreeItemData( l_pGeometry, p_pSubmesh ) );
					m_setSubmeshesInTree.insert( l_idSubmesh );
				} );
				m_setGeometriesInTree.insert( l_idGeometry );
			} );
		}

		wxBoxSizer * l_pSizer = new wxBoxSizer( wxVERTICAL );
		l_pSizer->Add( m_pTreeGeometries, wxSizerFlags( 1 ).Expand().ReserveSpaceEvenIfHidden() );
		l_pSizer->Add( m_pComboMaterials, wxSizerFlags( 0 ).Expand().ReserveSpaceEvenIfHidden() );
		l_pSizer->Add( m_pButtonDeleteSelected, wxSizerFlags( 0 ).Expand().ReserveSpaceEvenIfHidden() );
		SetSizer( l_pSizer );
		l_pSizer->SetSizeHints( this );
	}

	wxGeometriesListFrame::~wxGeometriesListFrame()
	{
	}

	BEGIN_EVENT_TABLE( wxGeometriesListFrame, wxPanel )
		EVT_CLOSE(	wxGeometriesListFrame::OnClose	)
		EVT_BUTTON(	eID_BUTTON_DELETE,		wxGeometriesListFrame::OnDeleteItem	)
		EVT_TREE_ITEM_EXPANDED(	wxID_ANY,				wxGeometriesListFrame::OnExpandItem	)
		EVT_TREE_ITEM_COLLAPSED(	wxID_ANY,				wxGeometriesListFrame::OnCollapseItem	)
		EVT_TREE_ITEM_ACTIVATED(	wxID_ANY,				wxGeometriesListFrame::OnActivateItem	)
		EVT_TREE_SEL_CHANGED(	wxID_ANY,				wxGeometriesListFrame::OnChangeItem	)
		EVT_COMBOBOX(	eID_COMBO_MATERIALS,	wxGeometriesListFrame::OnComboMaterials	)
	END_EVENT_TABLE()

	void wxGeometriesListFrame::OnClose( wxCloseEvent & p_event )
	{
		wxAuiManager * l_pMgr = wxAuiManager::GetManager( GetParent() );

		if ( l_pMgr )
		{
			//		l_pMgr->DetachPane( this );
		}

		p_event.Skip();
	}

	void wxGeometriesListFrame::OnDeleteItem( wxCommandEvent & p_event )
	{
		wxArrayTreeItemIds l_arraySelected;
		m_pTreeGeometries->GetSelections( l_arraySelected );
		std::for_each( l_arraySelected.begin(), l_arraySelected.end(), [&]( wxTreeItemId p_selected )
		{
			GeometrySPtr l_pGeometry = m_pScene->GetGeometry( ( wxChar const * )( m_pTreeGeometries->GetItemText( p_selected ).c_str() ) );

			if ( l_pGeometry )
			{
				m_pScene->RemoveGeometry( l_pGeometry );
			}
		} );
		std::for_each( l_arraySelected.begin(), l_arraySelected.end(), [&]( wxTreeItemId p_selected )
		{
			m_pTreeGeometries->Delete( p_selected );
		} );
		m_pTreeGeometries->Refresh();
		Refresh();
		m_selItem = wxTreeItemId();
		p_event.Skip();
	}

	void wxGeometriesListFrame::OnExpandItem( wxTreeEvent & p_event )
	{
		m_selItem = p_event.GetItem();
		m_pTreeGeometries->SetItemImage( m_selItem, eTREE_ID_GEOMETRY_OPEN );
		m_pTreeGeometries->SetItemImage( m_selItem, eTREE_ID_GEOMETRY_OPEN_SEL, wxTreeItemIcon_Selected );
		p_event.Skip();
	}

	void wxGeometriesListFrame::OnCollapseItem( wxTreeEvent & p_event )
	{
		m_selItem = p_event.GetItem();
		m_pTreeGeometries->SetItemImage( m_selItem, eTREE_ID_GEOMETRY );
		m_pTreeGeometries->SetItemImage( m_selItem, eTREE_ID_GEOMETRY_SEL, wxTreeItemIcon_Selected );
		p_event.Skip();
	}

	void wxGeometriesListFrame::OnActivateItem( wxTreeEvent & p_event )
	{
		m_selItem = p_event.GetItem();
		wxString l_strText = m_pTreeGeometries->GetItemText( m_selItem );

		if ( l_strText == _( "Visible" ) )
		{
			wxGeometryTreeItemData * l_pItemData = static_cast< wxGeometryTreeItemData * >( m_pTreeGeometries->GetItemData( m_selItem ) );
			m_pComboMaterials->Hide();
			l_pItemData->GetGeometry()->GetParent()->SetVisible( false );
			m_pTreeGeometries->SetItemText( m_selItem, _( "Hidden" ) );
			m_pTreeGeometries->SetItemImage( m_selItem, eTREE_ID_HIDDEN );
			m_pTreeGeometries->SetItemImage( m_selItem, eTREE_ID_HIDDEN_SEL, wxTreeItemIcon_Selected );
			m_pScene->InitialiseGeometries();
		}
		else if ( l_strText == _( "Hidden" ) )
		{
			wxGeometryTreeItemData * l_pItemData = static_cast< wxGeometryTreeItemData * >( m_pTreeGeometries->GetItemData( m_selItem ) );
			m_pComboMaterials->Hide();
			l_pItemData->GetGeometry()->GetParent()->SetVisible( true );
			m_pTreeGeometries->SetItemText( m_selItem, _( "Visible" ) );
			m_pTreeGeometries->SetItemImage( m_selItem, eTREE_ID_VISIBLE );
			m_pTreeGeometries->SetItemImage( m_selItem, eTREE_ID_VISIBLE_SEL, wxTreeItemIcon_Selected );
			m_pScene->InitialiseGeometries();
		}

		p_event.Skip();
	}

	void wxGeometriesListFrame::OnChangeItem( wxTreeEvent & p_event )
	{
		m_selItem = p_event.GetItem();

		if ( m_setSubmeshesInTree.find( m_selItem ) != m_setSubmeshesInTree.end() )
		{
			wxSubmeshTreeItemData * l_pItemData = static_cast< wxSubmeshTreeItemData * >( m_pTreeGeometries->GetItemData( m_selItem ) );
			String l_strMatName = l_pItemData->GetGeometry()->GetMaterial( l_pItemData->GetSubmesh() )->GetName();
			m_pComboMaterials->Clear();
			int l_iSelected = 0;
			StringArray l_arrayNames;
			m_pEngine->GetMaterialManager().GetNames( l_arrayNames );
			wxArrayString l_wxArrayNames;
			uint32_t i = 0;
			std::for_each( l_arrayNames.begin(), l_arrayNames.end(), [&]( String const & p_strName )
			{
				if ( l_strMatName == p_strName )
				{
					l_iSelected = int( i );
				}

				l_wxArrayNames.push_back( p_strName.c_str() );
				i++;
			} );
			m_pComboMaterials->Append( l_wxArrayNames );
			m_pComboMaterials->Show();
			m_pComboMaterials->SetSelection( l_iSelected );
		}
		else
		{
			m_pComboMaterials->Hide();
		}

		p_event.Skip();
	}

	void wxGeometriesListFrame::OnComboMaterials( wxCommandEvent & p_event )
	{
		wxString l_strName = m_pComboMaterials->GetString( p_event.GetInt() );
		wxSubmeshTreeItemData * l_pItemData = static_cast< wxSubmeshTreeItemData * >( m_pTreeGeometries->GetItemData( m_selItem ) );

		if ( m_pEngine->GetMaterialManager().has( ( wxChar const * )l_strName.c_str() ) )
		{
			l_pItemData->GetGeometry()->SetMaterial( l_pItemData->GetSubmesh(), m_pEngine->GetMaterialManager().find( ( wxChar const * )l_strName.c_str() ) );
			m_pScene->InitialiseGeometries();
		}

		p_event.Skip();
	}
}
