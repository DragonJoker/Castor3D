#include "SceneObjectsList.hpp"

#include "CameraTreeItemData.hpp"
#include "GeometryTreeItemData.hpp"
#include "LightTreeItemData.hpp"
#include "NodeTreeItemData.hpp"
#include "OverlayTreeItemData.hpp"
#include "SubmeshTreeItemData.hpp"
#include "PropertiesHolder.hpp"

#include "ImagesLoader.hpp"

#include "xpms/node.xpm"
#include "xpms/node_sel.xpm"
#include "xpms/camera.xpm"
#include "xpms/camera_sel.xpm"
#include "xpms/directional.xpm"
#include "xpms/directional_sel.xpm"
#include "xpms/point.xpm"
#include "xpms/point_sel.xpm"
#include "xpms/spot.xpm"
#include "xpms/spot_sel.xpm"
#include "xpms/geometry.xpm"
#include "xpms/geometry_sel.xpm"
#include "xpms/submesh.xpm"
#include "xpms/submesh_sel.xpm"
#include "xpms/scene.xpm"
#include "xpms/scene_sel.xpm"
#include "xpms/panel.xpm"
#include "xpms/panel_sel.xpm"
#include "xpms/border_panel.xpm"
#include "xpms/border_panel_sel.xpm"
#include "xpms/text.xpm"
#include "xpms/text_sel.xpm"

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
	wxSceneObjectsList::wxSceneObjectsList( wxPropertiesHolder * p_propertiesHolder, wxWindow * p_pParent, wxPoint const & p_ptPos, wxSize const & p_size )
		: wxTreeCtrl( p_pParent, wxID_ANY, p_ptPos, p_size, wxTR_DEFAULT_STYLE | wxNO_BORDER )
		, m_pEngine( NULL )
		, m_propertiesHolder( p_propertiesHolder )
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
		wxImagesLoader::AddBitmap( eBMP_DIRECTIONAL_LIGHT, directional_xpm );
		wxImagesLoader::AddBitmap( eBMP_DIRECTIONAL_LIGHT_SEL, directional_sel_xpm );
		wxImagesLoader::AddBitmap( eBMP_POINT_LIGHT, point_xpm );
		wxImagesLoader::AddBitmap( eBMP_POINT_LIGHT_SEL, point_sel_xpm );
		wxImagesLoader::AddBitmap( eBMP_SPOT_LIGHT, spot_xpm );
		wxImagesLoader::AddBitmap( eBMP_SPOT_LIGHT_SEL, spot_sel_xpm );
		wxImagesLoader::AddBitmap( eBMP_SUBMESH, submesh_xpm );
		wxImagesLoader::AddBitmap( eBMP_SUBMESH_SEL, submesh_sel_xpm );
		wxImagesLoader::AddBitmap( eBMP_PANEL_OVERLAY, panel_xpm );
		wxImagesLoader::AddBitmap( eBMP_PANEL_OVERLAY_SEL, panel_sel_xpm );
		wxImagesLoader::AddBitmap( eBMP_BORDER_PANEL_OVERLAY, border_panel_xpm );
		wxImagesLoader::AddBitmap( eBMP_BORDER_PANEL_OVERLAY_SEL, border_panel_sel_xpm );
		wxImagesLoader::AddBitmap( eBMP_TEXT_OVERLAY, text_xpm );
		wxImagesLoader::AddBitmap( eBMP_TEXT_OVERLAY_SEL, text_sel_xpm );
		wxImagesLoader::WaitAsyncLoads();

		wxImage * l_icons[] =
		{
			wxImagesLoader::GetBitmap( eBMP_SCENE ),
			wxImagesLoader::GetBitmap( eBMP_SCENE_SEL ),
			wxImagesLoader::GetBitmap( eBMP_NODE ),
			wxImagesLoader::GetBitmap( eBMP_NODE_SEL ),
			wxImagesLoader::GetBitmap( eBMP_CAMERA ),
			wxImagesLoader::GetBitmap( eBMP_CAMERA_SEL ),
			wxImagesLoader::GetBitmap( eBMP_GEOMETRY ),
			wxImagesLoader::GetBitmap( eBMP_GEOMETRY_SEL ),
			wxImagesLoader::GetBitmap( eBMP_DIRECTIONAL_LIGHT ),
			wxImagesLoader::GetBitmap( eBMP_DIRECTIONAL_LIGHT_SEL ),
			wxImagesLoader::GetBitmap( eBMP_POINT_LIGHT ),
			wxImagesLoader::GetBitmap( eBMP_POINT_LIGHT_SEL ),
			wxImagesLoader::GetBitmap( eBMP_SPOT_LIGHT ),
			wxImagesLoader::GetBitmap( eBMP_SPOT_LIGHT_SEL ),
			wxImagesLoader::GetBitmap( eBMP_SUBMESH ),
			wxImagesLoader::GetBitmap( eBMP_SUBMESH_SEL ),
			wxImagesLoader::GetBitmap( eBMP_PANEL_OVERLAY ),
			wxImagesLoader::GetBitmap( eBMP_PANEL_OVERLAY_SEL ),
			wxImagesLoader::GetBitmap( eBMP_BORDER_PANEL_OVERLAY ),
			wxImagesLoader::GetBitmap( eBMP_BORDER_PANEL_OVERLAY_SEL ),
			wxImagesLoader::GetBitmap( eBMP_TEXT_OVERLAY ),
			wxImagesLoader::GetBitmap( eBMP_TEXT_OVERLAY_SEL ),
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

	wxSceneObjectsList::~wxSceneObjectsList()
	{
	}

	void wxSceneObjectsList::LoadScene( Engine * p_pEngine, SceneSPtr p_pScene )
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
					switch( l_overlay->GetType() )
					{
					case eOVERLAY_TYPE_PANEL:
						DoAddOverlay( AppendItem( l_scene, l_overlay->GetName(), eBMP_PANEL_OVERLAY, eBMP_PANEL_OVERLAY_SEL, new wxOverlayTreeItemData( l_overlay ) ), l_overlay );
						break;

					case eOVERLAY_TYPE_BORDER_PANEL:
						DoAddOverlay( AppendItem( l_scene, l_overlay->GetName(), eBMP_BORDER_PANEL_OVERLAY, eBMP_BORDER_PANEL_OVERLAY_SEL, new wxOverlayTreeItemData( l_overlay ) ), l_overlay );
						break;

					case eOVERLAY_TYPE_TEXT:
						DoAddOverlay( AppendItem( l_scene, l_overlay->GetName(), eBMP_TEXT_OVERLAY, eBMP_TEXT_OVERLAY_SEL, new wxOverlayTreeItemData( l_overlay ) ), l_overlay );
						break;
					}
				}
			}
		}

		ExpandAll();
	}

	void wxSceneObjectsList::UnloadScene()
	{
		DeleteAllItems();
	}

	void wxSceneObjectsList::DoAddGeometry( wxTreeItemId p_id, MovableObjectSPtr p_geometry )
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

	void wxSceneObjectsList::DoAddCamera( wxTreeItemId p_id, MovableObjectSPtr p_camera )
	{
		CameraSPtr l_camera = std::static_pointer_cast< Camera >( p_camera );
		wxTreeItemId l_id = AppendItem( p_id, l_camera->GetName(), eBMP_CAMERA, eBMP_CAMERA_SEL, new wxCameraTreeItemData( l_camera ) );
	}

	void wxSceneObjectsList::DoAddLight( wxTreeItemId p_id, MovableObjectSPtr p_light )
	{
		LightSPtr l_light = std::static_pointer_cast< Light >( p_light );

		switch( l_light->GetLightType() )
		{
		case eLIGHT_TYPE_DIRECTIONAL:
			AppendItem( p_id, l_light->GetName(), eBMP_DIRECTIONAL_LIGHT, eBMP_DIRECTIONAL_LIGHT_SEL, new wxLightTreeItemData( l_light ) );
			break;

		case eLIGHT_TYPE_POINT:
			AppendItem( p_id, l_light->GetName(), eBMP_POINT_LIGHT, eBMP_POINT_LIGHT_SEL, new wxLightTreeItemData( l_light ) );
			break;

		case eLIGHT_TYPE_SPOT:
			AppendItem( p_id, l_light->GetName(), eBMP_SPOT_LIGHT, eBMP_SPOT_LIGHT_SEL, new wxLightTreeItemData( l_light ) );
			break;
		}
	}

	void wxSceneObjectsList::DoAddNode( wxTreeItemId p_id, SceneNodeSPtr p_node )
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

	void wxSceneObjectsList::DoAddOverlay( wxTreeItemId p_id, Castor3D::OverlaySPtr p_overlay )
	{
		for ( auto && l_overlay: *p_overlay )
		{
			switch( l_overlay->GetType() )
			{
			case eOVERLAY_TYPE_PANEL:
				DoAddOverlay( AppendItem( p_id, l_overlay->GetName(), eBMP_PANEL_OVERLAY, eBMP_PANEL_OVERLAY_SEL, new wxOverlayTreeItemData( l_overlay ) ), l_overlay );
				break;

			case eOVERLAY_TYPE_BORDER_PANEL:
				DoAddOverlay( AppendItem( p_id, l_overlay->GetName(), eBMP_BORDER_PANEL_OVERLAY, eBMP_BORDER_PANEL_OVERLAY_SEL, new wxOverlayTreeItemData( l_overlay ) ), l_overlay );
				break;

			case eOVERLAY_TYPE_TEXT:
				DoAddOverlay( AppendItem( p_id, l_overlay->GetName(), eBMP_TEXT_OVERLAY, eBMP_TEXT_OVERLAY_SEL, new wxOverlayTreeItemData( l_overlay ) ), l_overlay );
				break;
			}
		}
	}

	BEGIN_EVENT_TABLE( wxSceneObjectsList, wxTreeCtrl )
		EVT_CLOSE( wxSceneObjectsList::OnClose )
		EVT_TREE_SEL_CHANGED( wxID_ANY, wxSceneObjectsList::OnSelectItem )
	END_EVENT_TABLE()

	void wxSceneObjectsList::OnClose( wxCloseEvent & p_event )
	{
		DeleteAllItems();
		p_event.Skip();
	}

	void wxSceneObjectsList::OnSelectItem( wxTreeEvent & p_event )
	{
		wxTreeItemPropertyData * l_data = reinterpret_cast< wxTreeItemPropertyData * >( p_event.GetClientObject() );
		m_propertiesHolder->SetPropertyData( l_data );
		p_event.Skip();
	}
}
