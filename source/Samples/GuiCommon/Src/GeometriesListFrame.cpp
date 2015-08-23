#include "GeometriesListFrame.hpp"

#include "CameraTreeItemData.hpp"
#include "GeometryTreeItemData.hpp"
#include "LightTreeItemData.hpp"
#include "NodeTreeItemData.hpp"
#include "OverlayTreeItemData.hpp"
#include "SubmeshTreeItemData.hpp"

#include "ImagesLoader.hpp"

#include "xpms/node.xpm"
#include "xpms/node_sel.xpm"
#include "xpms/camera.xpm"
#include "xpms/camera_sel.xpm"
#include "xpms/light.xpm"
#include "xpms/light_sel.xpm"
#include "xpms/geometry.xpm"
#include "xpms/geometry_sel.xpm"
#include "xpms/submesh.xpm"
#include "xpms/submesh_sel.xpm"
#include "xpms/scene.xpm"
#include "xpms/scene_sel.xpm"
#include "xpms/overlay.xpm"
#include "xpms/overlay_sel.xpm"

#include <wx/imaglist.h>
#include <wx/aui/framemanager.h>
#include <wx/artprov.h>

#include <Geometry.hpp>
#include <Engine.hpp>
#include <Material.hpp>
#include <MaterialManager.hpp>
#include <OverlayManager.hpp>
#include <Mesh.hpp>
#include <Scene.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GuiCommon
{
	static const int GC_IMG_SIZE = 16;

	wxGeometriesListFrame::wxGeometriesListFrame( wxWindow * p_propsParent, wxWindow * p_pParent, wxString const & p_strTitle, wxPoint const & p_ptPos, wxSize const & p_size )
		: wxTreeCtrl( p_pParent, wxID_ANY, p_ptPos, p_size, wxTR_DEFAULT_STYLE | wxNO_BORDER )
		, m_pEngine( NULL )
	{
		wxBusyCursor l_wait;
		wxImagesLoader::AddBitmap( eBMP_SCENE, scene_xpm );
		wxImagesLoader::AddBitmap( eBMP_SCENE_SEL, scene_sel_xpm );
		wxImagesLoader::AddBitmap( eBMP_NODE, node_xpm );
		wxImagesLoader::AddBitmap( eBMP_NODE_SEL, node_sel_xpm );
		wxImagesLoader::AddBitmap( eBMP_CAMERA, camera_xpm );
		wxImagesLoader::AddBitmap( eBMP_CAMERA_SEL, camera_sel_xpm );
		wxImagesLoader::AddBitmap( eBMP_GEOMETRY, geometry_xpm );
		wxImagesLoader::AddBitmap( eBMP_GEOMETRY_SEL, geometry_sel_xpm );
		wxImagesLoader::AddBitmap( eBMP_LIGHT, light_xpm );
		wxImagesLoader::AddBitmap( eBMP_LIGHT_SEL, light_sel_xpm );
		wxImagesLoader::AddBitmap( eBMP_SUBMESH, submesh_xpm );
		wxImagesLoader::AddBitmap( eBMP_SUBMESH_SEL, submesh_sel_xpm );
		wxImagesLoader::AddBitmap( eBMP_OVERLAY, overlay_xpm );
		wxImagesLoader::AddBitmap( eBMP_OVERLAY_SEL, overlay_sel_xpm );
		wxImagesLoader::WaitAsyncLoads();

		std::array< wxImage *, eBMP_COUNT > l_icons =
		{
			wxImagesLoader::GetBitmap( eBMP_SCENE ),
			wxImagesLoader::GetBitmap( eBMP_SCENE_SEL ),
			wxImagesLoader::GetBitmap( eBMP_NODE ),
			wxImagesLoader::GetBitmap( eBMP_NODE_SEL ),
			wxImagesLoader::GetBitmap( eBMP_CAMERA ),
			wxImagesLoader::GetBitmap( eBMP_CAMERA_SEL ),
			wxImagesLoader::GetBitmap( eBMP_GEOMETRY ),
			wxImagesLoader::GetBitmap( eBMP_GEOMETRY_SEL ),
			wxImagesLoader::GetBitmap( eBMP_LIGHT ),
			wxImagesLoader::GetBitmap( eBMP_LIGHT_SEL ),
			wxImagesLoader::GetBitmap( eBMP_SUBMESH ),
			wxImagesLoader::GetBitmap( eBMP_SUBMESH_SEL ),
			wxImagesLoader::GetBitmap( eBMP_OVERLAY ),
			wxImagesLoader::GetBitmap( eBMP_OVERLAY_SEL ),
		};

		wxImageList * l_imageList = new wxImageList( GC_IMG_SIZE, GC_IMG_SIZE, true );

		for ( auto && l_image: l_icons )
		{
			int l_sizeOrig = l_image->GetWidth();

			if ( l_sizeOrig != GC_IMG_SIZE )
			{
				l_image->Rescale( GC_IMG_SIZE, GC_IMG_SIZE, wxIMAGE_QUALITY_HIGHEST );
			}

			l_imageList->Add( wxImage( *l_image ) );
		}

		AssignImageList( l_imageList );
	}

	wxGeometriesListFrame::~wxGeometriesListFrame()
	{
	}

	void wxGeometriesListFrame::LoadScene( Engine * p_pEngine, SceneSPtr p_pScene )
	{
		m_pScene = p_pScene;
		m_pEngine = p_pEngine;

		if ( p_pScene )
		{
			wxTreeItemId l_scene = AddRoot( p_pScene->GetName(), eBMP_SCENE, eBMP_SCENE_SEL );
			SceneNodeSPtr l_rootNode = p_pScene->GetRootNode();
			DoAddNode( l_scene, l_rootNode );

			for ( auto && l_overlay: p_pEngine->GetOverlayManager() )
			{
				if ( l_overlay->GetName() != cuT( "DebugPanel" ) )
				{
					DoAddOverlay( AppendItem( l_scene, l_overlay->GetName(), eBMP_OVERLAY, eBMP_OVERLAY_SEL, new wxOverlayTreeItemData( l_overlay ) ), l_overlay );
				}
			}
		}
	}

	void wxGeometriesListFrame::UnloadScene()
	{
		DeleteAllItems();
	}

	void wxGeometriesListFrame::DoAddGeometry( wxTreeItemId p_id, MovableObjectSPtr p_geometry )
	{
		GeometrySPtr l_geometry = std::static_pointer_cast< Geometry >( p_geometry );
		wxTreeItemId l_id = AppendItem( p_id, l_geometry->GetName(), eBMP_GEOMETRY, eBMP_GEOMETRY_SEL, new wxGeometryTreeItemData( l_geometry ) );
		int l_count = 0;

		for ( auto l_submesh: *l_geometry->GetMesh() )
		{
			wxString l_name = _( "Submesh " );
			l_name << l_count++;
			wxTreeItemId l_idSubmesh = AppendItem( l_id, l_name, eBMP_SUBMESH, eBMP_SUBMESH_SEL, new wxSubmeshTreeItemData( l_geometry, l_submesh ) );
		}
	}

	void wxGeometriesListFrame::DoAddCamera( wxTreeItemId p_id, MovableObjectSPtr p_camera )
	{
		CameraSPtr l_camera = std::static_pointer_cast< Camera >( p_camera );
		wxTreeItemId l_id = AppendItem( p_id, l_camera->GetName(), eBMP_CAMERA, eBMP_CAMERA_SEL, new wxCameraTreeItemData( l_camera ) );
	}

	void wxGeometriesListFrame::DoAddLight( wxTreeItemId p_id, MovableObjectSPtr p_light )
	{
		LightSPtr l_light = std::static_pointer_cast< Light >( p_light );
		wxTreeItemId l_id = AppendItem( p_id, l_light->GetName(), eBMP_LIGHT, eBMP_LIGHT_SEL, new wxLightTreeItemData( l_light ) );
	}

	void wxGeometriesListFrame::DoAddNode( wxTreeItemId p_id, SceneNodeSPtr p_node )
	{
		for ( auto && l_pair: p_node->GetObjects() )
		{
			MovableObjectSPtr l_object = l_pair.second.lock();

			switch ( l_object->GetType() )
			{
			case eMOVABLE_TYPE_GEOMETRY:
				DoAddGeometry( p_id, l_object );
				break;

			case eMOVABLE_TYPE_CAMERA:
				DoAddCamera( p_id, l_object );
				break;

			case eMOVABLE_TYPE_LIGHT:
				DoAddLight( p_id, l_object );
				break;
			}
		}

		for ( auto && l_pair: p_node->GetChilds() )
		{
			DoAddNode( AppendItem( p_id, l_pair.first, eBMP_NODE, eBMP_NODE_SEL, new wxNodeTreeItemData( l_pair.second.lock() ) ), l_pair.second.lock() );
		}
	}

	void wxGeometriesListFrame::DoAddOverlay( wxTreeItemId p_id, Castor3D::OverlaySPtr p_overlay )
	{
		for ( auto && l_overlay: *p_overlay )
		{
			DoAddOverlay( AppendItem( p_id, l_overlay->GetName(), eBMP_OVERLAY, eBMP_OVERLAY_SEL, new wxOverlayTreeItemData( l_overlay ) ), l_overlay );
		}
	}

	BEGIN_EVENT_TABLE( wxGeometriesListFrame, wxPanel )
		EVT_CLOSE( wxGeometriesListFrame::OnClose )
		//EVT_TREE_ITEM_ACTIVATED( wxID_ANY, wxGeometriesListFrame::OnActivateItem )
		//EVT_TREE_SEL_CHANGED( wxID_ANY, wxGeometriesListFrame::OnChangeItem )
	END_EVENT_TABLE()

	void wxGeometriesListFrame::OnClose( wxCloseEvent & p_event )
	{
		DeleteAllItems();
		p_event.Skip();
	}
}
