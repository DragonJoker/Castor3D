#include "SceneObjectsList.hpp"

#include "CameraTreeItemProperty.hpp"
#include "GeometryTreeItemProperty.hpp"
#include "LightTreeItemProperty.hpp"
#include "NodeTreeItemProperty.hpp"
#include "OverlayTreeItemProperty.hpp"
#include "SceneTreeItemProperty.hpp"
#include "RenderTargetTreeItemProperty.hpp"
#include "RenderWindowTreeItemProperty.hpp"
#include "SubmeshTreeItemProperty.hpp"
#include "PropertiesHolder.hpp"
#include "ViewportTreeItemProperty.hpp"

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
#include "xpms/viewport.xpm"
#include "xpms/viewport_sel.xpm"
#include "xpms/render_window.xpm"
#include "xpms/render_window_sel.xpm"
#include "xpms/render_target.xpm"
#include "xpms/render_target_sel.xpm"

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
	SceneObjectsList::SceneObjectsList( PropertiesHolder * p_propertiesHolder, wxWindow * p_parent, wxPoint const & p_ptPos, wxSize const & p_size )
		: wxTreeCtrl( p_parent, wxID_ANY, p_ptPos, p_size, wxTR_DEFAULT_STYLE | wxNO_BORDER )
		, m_engine( NULL )
		, m_propertiesHolder( p_propertiesHolder )
	{
		wxBusyCursor l_wait;
		ImagesLoader::AddBitmap( eBMP_SCENE, scene_xpm );
		ImagesLoader::AddBitmap( eBMP_SCENE_SEL, scene_sel_xpm );
		ImagesLoader::AddBitmap( eBMP_NODE, node_xpm );
		ImagesLoader::AddBitmap( eBMP_NODE_SEL, node_sel_xpm );
		ImagesLoader::AddBitmap( eBMP_CAMERA, camera_xpm );
		ImagesLoader::AddBitmap( eBMP_CAMERA_SEL, camera_sel_xpm );
		ImagesLoader::AddBitmap( eBMP_VIEWPORT, viewport_xpm );
		ImagesLoader::AddBitmap( eBMP_VIEWPORT_SEL, viewport_sel_xpm );
		ImagesLoader::AddBitmap( eBMP_GEOMETRY, geometry_xpm );
		ImagesLoader::AddBitmap( eBMP_GEOMETRY_SEL, geometry_sel_xpm );
		ImagesLoader::AddBitmap( eBMP_DIRECTIONAL_LIGHT, directional_xpm );
		ImagesLoader::AddBitmap( eBMP_DIRECTIONAL_LIGHT_SEL, directional_sel_xpm );
		ImagesLoader::AddBitmap( eBMP_POINT_LIGHT, point_xpm );
		ImagesLoader::AddBitmap( eBMP_POINT_LIGHT_SEL, point_sel_xpm );
		ImagesLoader::AddBitmap( eBMP_SPOT_LIGHT, spot_xpm );
		ImagesLoader::AddBitmap( eBMP_SPOT_LIGHT_SEL, spot_sel_xpm );
		ImagesLoader::AddBitmap( eBMP_SUBMESH, submesh_xpm );
		ImagesLoader::AddBitmap( eBMP_SUBMESH_SEL, submesh_sel_xpm );
		ImagesLoader::AddBitmap( eBMP_PANEL_OVERLAY, panel_xpm );
		ImagesLoader::AddBitmap( eBMP_PANEL_OVERLAY_SEL, panel_sel_xpm );
		ImagesLoader::AddBitmap( eBMP_BORDER_PANEL_OVERLAY, border_panel_xpm );
		ImagesLoader::AddBitmap( eBMP_BORDER_PANEL_OVERLAY_SEL, border_panel_sel_xpm );
		ImagesLoader::AddBitmap( eBMP_TEXT_OVERLAY, text_xpm );
		ImagesLoader::AddBitmap( eBMP_TEXT_OVERLAY_SEL, text_sel_xpm );
		ImagesLoader::AddBitmap( eBMP_RENDER_TARGET, render_target_xpm );
		ImagesLoader::AddBitmap( eBMP_RENDER_TARGET_SEL, render_target_sel_xpm );
		ImagesLoader::AddBitmap( eBMP_RENDER_WINDOW, render_window_xpm );
		ImagesLoader::AddBitmap( eBMP_RENDER_WINDOW_SEL, render_window_sel_xpm );
		ImagesLoader::WaitAsyncLoads();

		wxImage * l_icons[] =
		{
			ImagesLoader::GetBitmap( eBMP_SCENE ),
			ImagesLoader::GetBitmap( eBMP_SCENE_SEL ),
			ImagesLoader::GetBitmap( eBMP_VIEWPORT ),
			ImagesLoader::GetBitmap( eBMP_VIEWPORT_SEL ),
			ImagesLoader::GetBitmap( eBMP_RENDER_TARGET ),
			ImagesLoader::GetBitmap( eBMP_RENDER_TARGET_SEL ),
			ImagesLoader::GetBitmap( eBMP_RENDER_WINDOW ),
			ImagesLoader::GetBitmap( eBMP_RENDER_WINDOW_SEL ),
			ImagesLoader::GetBitmap( eBMP_NODE ),
			ImagesLoader::GetBitmap( eBMP_NODE_SEL ),
			ImagesLoader::GetBitmap( eBMP_CAMERA ),
			ImagesLoader::GetBitmap( eBMP_CAMERA_SEL ),
			ImagesLoader::GetBitmap( eBMP_GEOMETRY ),
			ImagesLoader::GetBitmap( eBMP_GEOMETRY_SEL ),
			ImagesLoader::GetBitmap( eBMP_DIRECTIONAL_LIGHT ),
			ImagesLoader::GetBitmap( eBMP_DIRECTIONAL_LIGHT_SEL ),
			ImagesLoader::GetBitmap( eBMP_POINT_LIGHT ),
			ImagesLoader::GetBitmap( eBMP_POINT_LIGHT_SEL ),
			ImagesLoader::GetBitmap( eBMP_SPOT_LIGHT ),
			ImagesLoader::GetBitmap( eBMP_SPOT_LIGHT_SEL ),
			ImagesLoader::GetBitmap( eBMP_SUBMESH ),
			ImagesLoader::GetBitmap( eBMP_SUBMESH_SEL ),
			ImagesLoader::GetBitmap( eBMP_PANEL_OVERLAY ),
			ImagesLoader::GetBitmap( eBMP_PANEL_OVERLAY_SEL ),
			ImagesLoader::GetBitmap( eBMP_BORDER_PANEL_OVERLAY ),
			ImagesLoader::GetBitmap( eBMP_BORDER_PANEL_OVERLAY_SEL ),
			ImagesLoader::GetBitmap( eBMP_TEXT_OVERLAY ),
			ImagesLoader::GetBitmap( eBMP_TEXT_OVERLAY_SEL ),
		};

		wxImageList * l_imageList = new wxImageList( GC_IMG_SIZE, GC_IMG_SIZE, true );

		for ( auto && l_image : l_icons )
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

	SceneObjectsList::~SceneObjectsList()
	{
	}

	void SceneObjectsList::LoadScene( Engine * p_engine, SceneSPtr p_scene )
	{
		m_pScene = p_scene;
		m_engine = p_engine;

		if ( p_scene )
		{
			wxTreeItemId l_scene = AddRoot( p_scene->GetName(), eBMP_SCENE, eBMP_SCENE_SEL, new SceneTreeItemProperty( m_propertiesHolder->IsEditable(), p_scene ) );

			for ( auto && l_it = p_scene->GetEngine()->RenderWindowsBegin(); l_it != p_scene->GetEngine()->RenderWindowsEnd(); ++l_it )
			{
				DoAddRenderWindow( l_scene, l_it->second );
			}

			SceneNodeSPtr l_rootNode = p_scene->GetRootNode();

			if ( l_rootNode )
			{
				DoAddNode( l_scene, l_rootNode );
			}

			for ( auto && l_overlay : p_engine->GetOverlayManager() )
			{
				if ( l_overlay->GetOverlayName() != cuT( "DebugPanel" ) )
				{
					switch ( l_overlay->GetType() )
					{
					case eOVERLAY_TYPE_PANEL:
						DoAddOverlay( AppendItem( l_scene, l_overlay->GetOverlayName(), eBMP_PANEL_OVERLAY, eBMP_PANEL_OVERLAY_SEL, new OverlayTreeItemProperty( m_propertiesHolder->IsEditable(), l_overlay ) ), l_overlay );
						break;

					case eOVERLAY_TYPE_BORDER_PANEL:
						DoAddOverlay( AppendItem( l_scene, l_overlay->GetOverlayName(), eBMP_BORDER_PANEL_OVERLAY, eBMP_BORDER_PANEL_OVERLAY_SEL, new OverlayTreeItemProperty( m_propertiesHolder->IsEditable(), l_overlay ) ), l_overlay );
						break;

					case eOVERLAY_TYPE_TEXT:
						DoAddOverlay( AppendItem( l_scene, l_overlay->GetOverlayName(), eBMP_TEXT_OVERLAY, eBMP_TEXT_OVERLAY_SEL, new OverlayTreeItemProperty( m_propertiesHolder->IsEditable(), l_overlay ) ), l_overlay );
						break;
					}
				}
			}
		}

		ExpandAll();
	}

	void SceneObjectsList::UnloadScene()
	{
		DeleteAllItems();
	}

	void SceneObjectsList::DoAddRenderWindow( wxTreeItemId p_id, RenderWindowSPtr p_window )
	{
		wxTreeItemId l_id = AppendItem( p_id, p_window->GetName(), eBMP_RENDER_WINDOW, eBMP_RENDER_WINDOW_SEL, new RenderWindowTreeItemProperty( m_propertiesHolder->IsEditable(), p_window ) );
		RenderTargetSPtr l_target = p_window->GetRenderTarget();

		if ( l_target )
		{
			wxString l_name = _( "Render Target" );
			AppendItem( l_id, l_name, eBMP_RENDER_TARGET, eBMP_RENDER_TARGET_SEL, new RenderTargetTreeItemProperty( m_propertiesHolder->IsEditable(), l_target ) );
		}
	}

	void SceneObjectsList::DoAddGeometry( wxTreeItemId p_id, MovableObjectSPtr p_geometry )
	{
		GeometrySPtr l_geometry = std::static_pointer_cast< Geometry >( p_geometry );
		wxTreeItemId l_id = AppendItem( p_id, l_geometry->GetName(), eBMP_GEOMETRY, eBMP_GEOMETRY_SEL, new GeometryTreeItemProperty( m_propertiesHolder->IsEditable(), l_geometry ) );
		int l_count = 0;

		for ( auto l_submesh : *l_geometry->GetMesh() )
		{
			wxString l_name = _( "Submesh " );
			l_name << l_count++;
			wxTreeItemId l_idSubmesh = AppendItem( l_id, l_name, eBMP_SUBMESH, eBMP_SUBMESH_SEL, new SubmeshTreeItemProperty( m_propertiesHolder->IsEditable(), l_geometry, l_submesh ) );
		}
	}

	void SceneObjectsList::DoAddCamera( wxTreeItemId p_id, MovableObjectSPtr p_camera )
	{
		CameraSPtr l_camera = std::static_pointer_cast< Camera >( p_camera );
		wxTreeItemId l_id = AppendItem( p_id, l_camera->GetName(), eBMP_CAMERA, eBMP_CAMERA_SEL, new CameraTreeItemProperty( m_propertiesHolder->IsEditable(), l_camera ) );
		AppendItem( l_id, _( "Viewport" ), eBMP_VIEWPORT, eBMP_VIEWPORT_SEL, new ViewportTreeItemProperty( m_propertiesHolder->IsEditable(), l_camera->GetViewport() ) );
	}

	void SceneObjectsList::DoAddLight( wxTreeItemId p_id, MovableObjectSPtr p_light )
	{
		LightSPtr l_light = std::static_pointer_cast< Light >( p_light );

		switch ( l_light->GetLightType() )
		{
		case eLIGHT_TYPE_DIRECTIONAL:
			AppendItem( p_id, l_light->GetName(), eBMP_DIRECTIONAL_LIGHT, eBMP_DIRECTIONAL_LIGHT_SEL, new LightTreeItemProperty( m_propertiesHolder->IsEditable(), l_light ) );
			break;

		case eLIGHT_TYPE_POINT:
			AppendItem( p_id, l_light->GetName(), eBMP_POINT_LIGHT, eBMP_POINT_LIGHT_SEL, new LightTreeItemProperty( m_propertiesHolder->IsEditable(), l_light ) );
			break;

		case eLIGHT_TYPE_SPOT:
			AppendItem( p_id, l_light->GetName(), eBMP_SPOT_LIGHT, eBMP_SPOT_LIGHT_SEL, new LightTreeItemProperty( m_propertiesHolder->IsEditable(), l_light ) );
			break;
		}
	}

	void SceneObjectsList::DoAddNode( wxTreeItemId p_id, SceneNodeSPtr p_node )
	{
		for ( auto && l_pair : p_node->GetObjects() )
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

		for ( auto && l_pair : p_node->GetChilds() )
		{
			DoAddNode( AppendItem( p_id, l_pair.first, eBMP_NODE, eBMP_NODE_SEL, new NodeTreeItemProperty( m_propertiesHolder->IsEditable(), m_engine, l_pair.second.lock() ) ), l_pair.second.lock() );
		}
	}

	void SceneObjectsList::DoAddOverlay( wxTreeItemId p_id, Castor3D::OverlayCategorySPtr p_overlay )
	{
		for ( auto && l_overlay : p_overlay->GetOverlay() )
		{
			switch ( l_overlay->GetType() )
			{
			case eOVERLAY_TYPE_PANEL:
				DoAddOverlay( AppendItem( p_id, l_overlay->GetName(), eBMP_PANEL_OVERLAY, eBMP_PANEL_OVERLAY_SEL, new OverlayTreeItemProperty( m_propertiesHolder->IsEditable(), l_overlay->GetCategory() ) ), l_overlay->GetCategory() );
				break;

			case eOVERLAY_TYPE_BORDER_PANEL:
				DoAddOverlay( AppendItem( p_id, l_overlay->GetName(), eBMP_BORDER_PANEL_OVERLAY, eBMP_BORDER_PANEL_OVERLAY_SEL, new OverlayTreeItemProperty( m_propertiesHolder->IsEditable(), l_overlay->GetCategory() ) ), l_overlay->GetCategory() );
				break;

			case eOVERLAY_TYPE_TEXT:
				DoAddOverlay( AppendItem( p_id, l_overlay->GetName(), eBMP_TEXT_OVERLAY, eBMP_TEXT_OVERLAY_SEL, new OverlayTreeItemProperty( m_propertiesHolder->IsEditable(), l_overlay->GetCategory() ) ), l_overlay->GetCategory() );
				break;
			}
		}
	}

	BEGIN_EVENT_TABLE( SceneObjectsList, wxTreeCtrl )
		EVT_CLOSE( SceneObjectsList::OnClose )
		EVT_TREE_SEL_CHANGED( wxID_ANY, SceneObjectsList::OnSelectItem )
		EVT_TREE_ITEM_RIGHT_CLICK( wxID_ANY, SceneObjectsList::OnMouseRButtonUp )
	END_EVENT_TABLE()

	void SceneObjectsList::OnClose( wxCloseEvent & p_event )
	{
		DeleteAllItems();
		p_event.Skip();
	}

	void SceneObjectsList::OnSelectItem( wxTreeEvent & p_event )
	{
		TreeItemProperty * l_data = reinterpret_cast< TreeItemProperty * >( p_event.GetClientObject() );
		m_propertiesHolder->SetPropertyData( l_data );
		p_event.Skip();
	}

	void SceneObjectsList::OnMouseRButtonUp( wxTreeEvent & p_event )
	{
	}
}
