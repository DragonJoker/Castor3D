#include "SceneObjectsList.hpp"

#include "AnimatedObjectGroupTreeItemProperty.hpp"
#include "AnimatedObjectTreeItemProperty.hpp"
#include "AnimationTreeItemProperty.hpp"
#include "BillboardTreeItemProperty.hpp"
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

#include <wx/imaglist.h>
#include <wx/aui/framemanager.h>
#include <wx/artprov.h>

#include <Engine.hpp>
#include <Material/Material.hpp>
#include <Mesh/Mesh.hpp>
#include <Overlay/Overlay.hpp>
#include <Render/RenderWindow.hpp>
#include <Scene/BillboardList.hpp>
#include <Scene/Camera.hpp>
#include <Scene/Geometry.hpp>
#include <Scene/Scene.hpp>
#include <Scene/Animation/AnimatedObjectGroup.hpp>
#include <Scene/Light/Light.hpp>

#include <Scene/BillboardList.hpp>

#include <Animation/Animation.hpp>
#include <Scene/Animation/AnimatedObject.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GuiCommon
{
	SceneObjectsList::SceneObjectsList( PropertiesHolder * p_propertiesHolder, wxWindow * p_parent, wxPoint const & p_ptPos, wxSize const & p_size )
		: wxTreeCtrl( p_parent, wxID_ANY, p_ptPos, p_size, wxTR_DEFAULT_STYLE | wxNO_BORDER )
		, m_engine( nullptr )
		, m_propertiesHolder( p_propertiesHolder )
	{
		wxImage * l_icons[] =
		{
			ImagesLoader::GetBitmap( eBMP_ANIMATED_OBJECTGROUP ),
			ImagesLoader::GetBitmap( eBMP_ANIMATED_OBJECTGROUP_SEL ),
			ImagesLoader::GetBitmap( eBMP_ANIMATED_OBJECT ),
			ImagesLoader::GetBitmap( eBMP_ANIMATED_OBJECT_SEL ),
			ImagesLoader::GetBitmap( eBMP_ANIMATION ),
			ImagesLoader::GetBitmap( eBMP_ANIMATION_SEL ),
			ImagesLoader::GetBitmap( eBMP_SCENE ),
			ImagesLoader::GetBitmap( eBMP_SCENE_SEL ),
			ImagesLoader::GetBitmap( eBMP_VIEWPORT ),
			ImagesLoader::GetBitmap( eBMP_VIEWPORT_SEL ),
			ImagesLoader::GetBitmap( eBMP_RENDER_TARGET ),
			ImagesLoader::GetBitmap( eBMP_RENDER_TARGET_SEL ),
			ImagesLoader::GetBitmap( eBMP_RENDER_WINDOW ),
			ImagesLoader::GetBitmap( eBMP_RENDER_WINDOW_SEL ),
			ImagesLoader::GetBitmap( eBMP_FRAME_VARIABLE ),
			ImagesLoader::GetBitmap( eBMP_FRAME_VARIABLE_SEL ),
			ImagesLoader::GetBitmap( eBMP_FRAME_VARIABLE_BUFFER ),
			ImagesLoader::GetBitmap( eBMP_FRAME_VARIABLE_BUFFER_SEL ),
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
			ImagesLoader::GetBitmap( eBMP_MATERIAL ),
			ImagesLoader::GetBitmap( eBMP_MATERIAL_SEL ),
			ImagesLoader::GetBitmap( eBMP_PASS ),
			ImagesLoader::GetBitmap( eBMP_PASS_SEL ),
			ImagesLoader::GetBitmap( eBMP_TEXTURE ),
			ImagesLoader::GetBitmap( eBMP_TEXTURE_SEL ),
			ImagesLoader::GetBitmap( eBMP_BILLBOARD ),
			ImagesLoader::GetBitmap( eBMP_BILLBOARD_SEL ),
		};

		wxImageList * l_imageList = new wxImageList( GC_IMG_SIZE, GC_IMG_SIZE, true );

		for ( auto l_image : l_icons )
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
			p_scene->GetEngine()->GetRenderWindowCache().lock();

			for ( auto l_it : p_scene->GetEngine()->GetRenderWindowCache() )
			{
				DoAddRenderWindow( l_scene, l_it.second );
			}

			p_scene->GetEngine()->GetRenderWindowCache().unlock();
			SceneNodeSPtr l_rootNode = p_scene->GetRootNode();

			if ( l_rootNode )
			{
				DoAddNode( l_scene, l_rootNode );
			}

			p_scene->GetAnimatedObjectGroupCache().lock();

			for ( auto l_it : p_scene->GetAnimatedObjectGroupCache() )
			{
				DoAddAnimatedObjectGroup( AppendItem( l_scene, l_it.first, eBMP_ANIMATED_OBJECTGROUP, eBMP_ANIMATED_OBJECTGROUP_SEL, new AnimatedObjectGroupTreeItemProperty( m_propertiesHolder->IsEditable(), l_it.second ) ), l_it.second );
			}

			p_scene->GetAnimatedObjectGroupCache().unlock();

			for ( auto l_overlay : p_engine->GetOverlayCache() )
			{
				if ( l_overlay->GetOverlayName().find( cuT( "DebugPanel" ) ) != 0 )
				{
					switch ( l_overlay->GetType() )
					{
					case OverlayType::ePanel:
						DoAddOverlay( AppendItem( l_scene, l_overlay->GetOverlayName(), eBMP_PANEL_OVERLAY, eBMP_PANEL_OVERLAY_SEL, new OverlayTreeItemProperty( m_propertiesHolder->IsEditable(), l_overlay ) ), l_overlay );
						break;

					case OverlayType::eBorderPanel:
						DoAddOverlay( AppendItem( l_scene, l_overlay->GetOverlayName(), eBMP_BORDER_PANEL_OVERLAY, eBMP_BORDER_PANEL_OVERLAY_SEL, new OverlayTreeItemProperty( m_propertiesHolder->IsEditable(), l_overlay ) ), l_overlay );
						break;

					case OverlayType::eText:
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

	void SceneObjectsList::DoAddGeometry( wxTreeItemId p_id, Geometry & p_geometry )
	{
		wxTreeItemId l_id = AppendItem( p_id, p_geometry.GetName(), eBMP_GEOMETRY, eBMP_GEOMETRY_SEL, new GeometryTreeItemProperty( m_propertiesHolder->IsEditable(), p_geometry ) );
		int l_count = 0;

		if ( p_geometry.GetMesh() )
		{
			for ( auto l_submesh : *p_geometry.GetMesh() )
			{
				wxString l_name = _( "Submesh " );
				l_name << l_count++;
				wxTreeItemId l_idSubmesh = AppendItem( l_id, l_name, eBMP_SUBMESH, eBMP_SUBMESH_SEL, new SubmeshTreeItemProperty( m_propertiesHolder->IsEditable(), p_geometry, *l_submesh ) );
			}
		}
	}

	void SceneObjectsList::DoAddCamera( wxTreeItemId p_id, Camera & p_camera )
	{
		wxTreeItemId l_id = AppendItem( p_id, p_camera.GetName(), eBMP_CAMERA, eBMP_CAMERA_SEL, new CameraTreeItemProperty( m_propertiesHolder->IsEditable(), p_camera ) );
		AppendItem( l_id, _( "Viewport" ), eBMP_VIEWPORT, eBMP_VIEWPORT_SEL, new ViewportTreeItemProperty( m_propertiesHolder->IsEditable(), *p_camera.GetScene()->GetEngine(), p_camera.GetViewport() ) );
	}

	void SceneObjectsList::DoAddBillboard( wxTreeItemId p_id, BillboardList & p_billboard )
	{
		wxTreeItemId l_id = AppendItem( p_id, p_billboard.GetName(), eBMP_BILLBOARD, eBMP_BILLBOARD_SEL, new BillboardTreeItemProperty( m_propertiesHolder->IsEditable(), p_billboard ) );
	}

	void SceneObjectsList::DoAddLight( wxTreeItemId p_id, Light & p_light )
	{
		switch ( p_light.GetLightType() )
		{
		case LightType::eDirectional:
			AppendItem( p_id, p_light.GetName(), eBMP_DIRECTIONAL_LIGHT, eBMP_DIRECTIONAL_LIGHT_SEL, new LightTreeItemProperty( m_propertiesHolder->IsEditable(), p_light ) );
			break;

		case LightType::ePoint:
			AppendItem( p_id, p_light.GetName(), eBMP_POINT_LIGHT, eBMP_POINT_LIGHT_SEL, new LightTreeItemProperty( m_propertiesHolder->IsEditable(), p_light ) );
			break;

		case LightType::eSpot:
			AppendItem( p_id, p_light.GetName(), eBMP_SPOT_LIGHT, eBMP_SPOT_LIGHT_SEL, new LightTreeItemProperty( m_propertiesHolder->IsEditable(), p_light ) );
			break;
		}
	}

	void SceneObjectsList::DoAddNode( wxTreeItemId p_id, SceneNodeSPtr p_node )
	{
		for ( auto & l_object : p_node->GetObjects() )
		{
			switch ( l_object.get().GetType() )
			{
			case MovableType::eGeometry:
				DoAddGeometry( p_id, static_cast< Geometry & >( l_object.get() ) );
				break;

			case MovableType::eCamera:
				DoAddCamera( p_id, static_cast< Camera & >( l_object.get() ) );
				break;

			case MovableType::eLight:
				DoAddLight( p_id, static_cast< Light & >( l_object.get() ) );
				break;

			case MovableType::eBillboard:
				DoAddBillboard( p_id, static_cast< BillboardList & >( l_object.get() ) );
				break;
			}
		}

		for ( auto l_pair : p_node->GetChilds() )
		{
			DoAddNode( AppendItem( p_id, l_pair.first, eBMP_NODE, eBMP_NODE_SEL, new NodeTreeItemProperty( m_propertiesHolder->IsEditable(), m_engine, l_pair.second.lock() ) ), l_pair.second.lock() );
		}
	}

	void SceneObjectsList::DoAddAnimatedObjectGroup( wxTreeItemId p_id, Castor3D::AnimatedObjectGroupSPtr p_group )
	{
		for ( auto l_it : p_group->GetAnimations() )
		{
			AppendItem( p_id, l_it.first, eBMP_ANIMATION, eBMP_ANIMATION_SEL, new AnimationTreeItemProperty( m_engine, m_propertiesHolder->IsEditable(), p_group, l_it.first, l_it.second ) );
		}
	}

	void SceneObjectsList::DoAddOverlay( wxTreeItemId p_id, Castor3D::OverlayCategorySPtr p_overlay )
	{
		for ( auto l_overlay : p_overlay->GetOverlay() )
		{
			switch ( l_overlay->GetType() )
			{
			case OverlayType::ePanel:
				DoAddOverlay( AppendItem( p_id, l_overlay->GetName(), eBMP_PANEL_OVERLAY, eBMP_PANEL_OVERLAY_SEL, new OverlayTreeItemProperty( m_propertiesHolder->IsEditable(), l_overlay->GetCategory() ) ), l_overlay->GetCategory() );
				break;

			case OverlayType::eBorderPanel:
				DoAddOverlay( AppendItem( p_id, l_overlay->GetName(), eBMP_BORDER_PANEL_OVERLAY, eBMP_BORDER_PANEL_OVERLAY_SEL, new OverlayTreeItemProperty( m_propertiesHolder->IsEditable(), l_overlay->GetCategory() ) ), l_overlay->GetCategory() );
				break;

			case OverlayType::eText:
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
