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

using namespace castor3d;
using namespace castor;

namespace GuiCommon
{
	SceneObjectsList::SceneObjectsList( PropertiesHolder * p_propertiesHolder, wxWindow * p_parent, wxPoint const & p_ptPos, wxSize const & p_size )
		: wxTreeCtrl( p_parent, wxID_ANY, p_ptPos, p_size, wxTR_DEFAULT_STYLE | wxNO_BORDER )
		, m_engine( nullptr )
		, m_propertiesHolder( p_propertiesHolder )
	{
		wxImage * icons[] =
		{
			ImagesLoader::getBitmap( eBMP_ANIMATED_OBJECTGROUP ),
			ImagesLoader::getBitmap( eBMP_ANIMATED_OBJECTGROUP_SEL ),
			ImagesLoader::getBitmap( eBMP_ANIMATED_OBJECT ),
			ImagesLoader::getBitmap( eBMP_ANIMATED_OBJECT_SEL ),
			ImagesLoader::getBitmap( eBMP_ANIMATION ),
			ImagesLoader::getBitmap( eBMP_ANIMATION_SEL ),
			ImagesLoader::getBitmap( eBMP_SCENE ),
			ImagesLoader::getBitmap( eBMP_SCENE_SEL ),
			ImagesLoader::getBitmap( eBMP_VIEWPORT ),
			ImagesLoader::getBitmap( eBMP_VIEWPORT_SEL ),
			ImagesLoader::getBitmap( eBMP_RENDER_TARGET ),
			ImagesLoader::getBitmap( eBMP_RENDER_TARGET_SEL ),
			ImagesLoader::getBitmap( eBMP_RENDER_WINDOW ),
			ImagesLoader::getBitmap( eBMP_RENDER_WINDOW_SEL ),
			ImagesLoader::getBitmap( eBMP_FRAME_VARIABLE ),
			ImagesLoader::getBitmap( eBMP_FRAME_VARIABLE_SEL ),
			ImagesLoader::getBitmap( eBMP_FRAME_VARIABLE_BUFFER ),
			ImagesLoader::getBitmap( eBMP_FRAME_VARIABLE_BUFFER_SEL ),
			ImagesLoader::getBitmap( eBMP_NODE ),
			ImagesLoader::getBitmap( eBMP_NODE_SEL ),
			ImagesLoader::getBitmap( eBMP_CAMERA ),
			ImagesLoader::getBitmap( eBMP_CAMERA_SEL ),
			ImagesLoader::getBitmap( eBMP_GEOMETRY ),
			ImagesLoader::getBitmap( eBMP_GEOMETRY_SEL ),
			ImagesLoader::getBitmap( eBMP_DIRECTIONAL_LIGHT ),
			ImagesLoader::getBitmap( eBMP_DIRECTIONAL_LIGHT_SEL ),
			ImagesLoader::getBitmap( eBMP_POINT_LIGHT ),
			ImagesLoader::getBitmap( eBMP_POINT_LIGHT_SEL ),
			ImagesLoader::getBitmap( eBMP_SPOT_LIGHT ),
			ImagesLoader::getBitmap( eBMP_SPOT_LIGHT_SEL ),
			ImagesLoader::getBitmap( eBMP_SUBMESH ),
			ImagesLoader::getBitmap( eBMP_SUBMESH_SEL ),
			ImagesLoader::getBitmap( eBMP_PANEL_OVERLAY ),
			ImagesLoader::getBitmap( eBMP_PANEL_OVERLAY_SEL ),
			ImagesLoader::getBitmap( eBMP_BORDER_PANEL_OVERLAY ),
			ImagesLoader::getBitmap( eBMP_BORDER_PANEL_OVERLAY_SEL ),
			ImagesLoader::getBitmap( eBMP_TEXT_OVERLAY ),
			ImagesLoader::getBitmap( eBMP_TEXT_OVERLAY_SEL ),
			ImagesLoader::getBitmap( eBMP_MATERIAL ),
			ImagesLoader::getBitmap( eBMP_MATERIAL_SEL ),
			ImagesLoader::getBitmap( eBMP_PASS ),
			ImagesLoader::getBitmap( eBMP_PASS_SEL ),
			ImagesLoader::getBitmap( eBMP_TEXTURE ),
			ImagesLoader::getBitmap( eBMP_TEXTURE_SEL ),
			ImagesLoader::getBitmap( eBMP_BILLBOARD ),
			ImagesLoader::getBitmap( eBMP_BILLBOARD_SEL ),
		};

		wxImageList * imageList = new wxImageList( GC_IMG_SIZE, GC_IMG_SIZE, true );

		for ( auto image : icons )
		{
			int sizeOrig = image->GetWidth();

			if ( sizeOrig != GC_IMG_SIZE )
			{
				image->Rescale( GC_IMG_SIZE, GC_IMG_SIZE, wxIMAGE_QUALITY_HIGHEST );
			}

			imageList->Add( wxImage( *image ) );
		}

		AssignImageList( imageList );
	}

	SceneObjectsList::~SceneObjectsList()
	{
	}

	void SceneObjectsList::loadScene( Engine * engine, SceneSPtr p_scene )
	{
		m_scene = p_scene;
		m_engine = engine;

		if ( p_scene )
		{
			wxTreeItemId scene = AddRoot( p_scene->getName(), eBMP_SCENE, eBMP_SCENE_SEL, new SceneTreeItemProperty( m_propertiesHolder->IsEditable(), p_scene ) );
			p_scene->getEngine()->getRenderWindowCache().lock();

			for ( auto it : p_scene->getEngine()->getRenderWindowCache() )
			{
				doAddRenderWindow( scene, it.second );
			}

			p_scene->getEngine()->getRenderWindowCache().unlock();
			SceneNodeSPtr rootNode = p_scene->getRootNode();

			if ( rootNode )
			{
				doAddNode( scene, rootNode );
			}

			p_scene->getAnimatedObjectGroupCache().lock();

			for ( auto it : p_scene->getAnimatedObjectGroupCache() )
			{
				doAddAnimatedObjectGroup( AppendItem( scene, it.first, eBMP_ANIMATED_OBJECTGROUP, eBMP_ANIMATED_OBJECTGROUP_SEL, new AnimatedObjectGroupTreeItemProperty( m_propertiesHolder->IsEditable(), it.second ) ), it.second );
			}

			p_scene->getAnimatedObjectGroupCache().unlock();

			for ( auto overlay : engine->getOverlayCache() )
			{
				if ( overlay->getOverlayName().find( cuT( "DebugPanel" ) ) != 0
					&& overlay->getOverlayName().find( cuT( "RenderPassOverlays" ) ) != 0 )
				{
					switch ( overlay->getType() )
					{
					case OverlayType::ePanel:
						doAddOverlay( AppendItem( scene, overlay->getOverlayName(), eBMP_PANEL_OVERLAY, eBMP_PANEL_OVERLAY_SEL, new OverlayTreeItemProperty( m_propertiesHolder->IsEditable(), overlay ) ), overlay );
						break;

					case OverlayType::eBorderPanel:
						doAddOverlay( AppendItem( scene, overlay->getOverlayName(), eBMP_BORDER_PANEL_OVERLAY, eBMP_BORDER_PANEL_OVERLAY_SEL, new OverlayTreeItemProperty( m_propertiesHolder->IsEditable(), overlay ) ), overlay );
						break;

					case OverlayType::eText:
						doAddOverlay( AppendItem( scene, overlay->getOverlayName(), eBMP_TEXT_OVERLAY, eBMP_TEXT_OVERLAY_SEL, new OverlayTreeItemProperty( m_propertiesHolder->IsEditable(), overlay ) ), overlay );
						break;
					}
				}
			}
		}

		ExpandAll();
	}

	void SceneObjectsList::unloadScene()
	{
		DeleteAllItems();
	}

	void SceneObjectsList::select( castor3d::GeometrySPtr geometry, castor3d::SubmeshSPtr submesh )
	{
		auto itg = m_ids.find( geometry );

		if ( itg != m_ids.end() )
		{
			auto its = itg->second.find( submesh );

			if ( its != itg->second.end() )
			{
				SelectItem( its->second );
			}
		}
	}

	void SceneObjectsList::doAddSubmesh( GeometrySPtr geometry, SubmeshSPtr submesh, wxTreeItemId id )
	{
		auto itg = m_ids.insert( { geometry, SubmeshIdMap{} } ).first;
		itg->second.insert( { submesh, id } );
	}

	void SceneObjectsList::doAddRenderWindow( wxTreeItemId p_id, RenderWindowSPtr p_window )
	{
		wxTreeItemId id = AppendItem( p_id, p_window->getName(), eBMP_RENDER_WINDOW, eBMP_RENDER_WINDOW_SEL, new RenderWindowTreeItemProperty( m_propertiesHolder->IsEditable(), p_window ) );
		RenderTargetSPtr target = p_window->getRenderTarget();

		if ( target )
		{
			wxString name = _( "Render Target" );
			AppendItem( id, name, eBMP_RENDER_TARGET, eBMP_RENDER_TARGET_SEL, new RenderTargetTreeItemProperty( m_propertiesHolder->IsEditable(), target ) );
		}
	}

	void SceneObjectsList::doAddGeometry( wxTreeItemId p_id, Geometry & geometry )
	{
		wxTreeItemId id = AppendItem( p_id
			, geometry.getName()
			, eBMP_GEOMETRY
			, eBMP_GEOMETRY_SEL
			, new GeometryTreeItemProperty( m_propertiesHolder->IsEditable(), geometry ) );
		int count = 0;

		if ( geometry.getMesh() )
		{
			for ( auto submesh : *geometry.getMesh() )
			{
				wxString name = _( "Submesh " );
				name << count++;
				wxTreeItemId idSubmesh = AppendItem( id
					, name
					, eBMP_SUBMESH
					, eBMP_SUBMESH_SEL
					, new SubmeshTreeItemProperty( m_propertiesHolder->IsEditable(), geometry, *submesh ) );
				doAddSubmesh( std::static_pointer_cast< Geometry >( geometry.shared_from_this() ), submesh, idSubmesh );
			}
		}
	}

	void SceneObjectsList::doAddCamera( wxTreeItemId p_id, Camera & p_camera )
	{
		wxTreeItemId id = AppendItem( p_id, p_camera.getName(), eBMP_CAMERA, eBMP_CAMERA_SEL, new CameraTreeItemProperty( m_propertiesHolder->IsEditable(), p_camera ) );
		AppendItem( id, _( "Viewport" ), eBMP_VIEWPORT, eBMP_VIEWPORT_SEL, new ViewportTreeItemProperty( m_propertiesHolder->IsEditable(), *p_camera.getScene()->getEngine(), p_camera.getViewport() ) );
	}

	void SceneObjectsList::doAddBillboard( wxTreeItemId p_id, BillboardList & p_billboard )
	{
		wxTreeItemId id = AppendItem( p_id, p_billboard.getName(), eBMP_BILLBOARD, eBMP_BILLBOARD_SEL, new BillboardTreeItemProperty( m_propertiesHolder->IsEditable(), p_billboard ) );
	}

	void SceneObjectsList::doAddLight( wxTreeItemId p_id, Light & p_light )
	{
		switch ( p_light.getLightType() )
		{
		case LightType::eDirectional:
			AppendItem( p_id, p_light.getName(), eBMP_DIRECTIONAL_LIGHT, eBMP_DIRECTIONAL_LIGHT_SEL, new LightTreeItemProperty( m_propertiesHolder->IsEditable(), p_light ) );
			break;

		case LightType::ePoint:
			AppendItem( p_id, p_light.getName(), eBMP_POINT_LIGHT, eBMP_POINT_LIGHT_SEL, new LightTreeItemProperty( m_propertiesHolder->IsEditable(), p_light ) );
			break;

		case LightType::eSpot:
			AppendItem( p_id, p_light.getName(), eBMP_SPOT_LIGHT, eBMP_SPOT_LIGHT_SEL, new LightTreeItemProperty( m_propertiesHolder->IsEditable(), p_light ) );
			break;
		}
	}

	void SceneObjectsList::doAddNode( wxTreeItemId p_id, SceneNodeSPtr p_node )
	{
		for ( auto & object : p_node->getObjects() )
		{
			switch ( object.get().getType() )
			{
			case MovableType::eGeometry:
				doAddGeometry( p_id, static_cast< Geometry & >( object.get() ) );
				break;

			case MovableType::eCamera:
				doAddCamera( p_id, static_cast< Camera & >( object.get() ) );
				break;

			case MovableType::eLight:
				doAddLight( p_id, static_cast< Light & >( object.get() ) );
				break;

			case MovableType::eBillboard:
				doAddBillboard( p_id, static_cast< BillboardList & >( object.get() ) );
				break;
			}
		}

		for ( auto pair : p_node->getChildren() )
		{
			doAddNode( AppendItem( p_id, pair.first, eBMP_NODE, eBMP_NODE_SEL, new NodeTreeItemProperty( m_propertiesHolder->IsEditable(), m_engine, pair.second.lock() ) ), pair.second.lock() );
		}
	}

	void SceneObjectsList::doAddAnimatedObjectGroup( wxTreeItemId p_id, castor3d::AnimatedObjectGroupSPtr p_group )
	{
		for ( auto it : p_group->getAnimations() )
		{
			AppendItem( p_id, it.first, eBMP_ANIMATION, eBMP_ANIMATION_SEL, new AnimationTreeItemProperty( m_engine, m_propertiesHolder->IsEditable(), p_group, it.first, it.second ) );
		}
	}

	void SceneObjectsList::doAddOverlay( wxTreeItemId p_id, castor3d::OverlayCategorySPtr p_overlay )
	{
		for ( auto overlay : p_overlay->getOverlay() )
		{
			switch ( overlay->getType() )
			{
			case OverlayType::ePanel:
				doAddOverlay( AppendItem( p_id, overlay->getName(), eBMP_PANEL_OVERLAY, eBMP_PANEL_OVERLAY_SEL, new OverlayTreeItemProperty( m_propertiesHolder->IsEditable(), overlay->getCategory() ) ), overlay->getCategory() );
				break;

			case OverlayType::eBorderPanel:
				doAddOverlay( AppendItem( p_id, overlay->getName(), eBMP_BORDER_PANEL_OVERLAY, eBMP_BORDER_PANEL_OVERLAY_SEL, new OverlayTreeItemProperty( m_propertiesHolder->IsEditable(), overlay->getCategory() ) ), overlay->getCategory() );
				break;

			case OverlayType::eText:
				doAddOverlay( AppendItem( p_id, overlay->getName(), eBMP_TEXT_OVERLAY, eBMP_TEXT_OVERLAY_SEL, new OverlayTreeItemProperty( m_propertiesHolder->IsEditable(), overlay->getCategory() ) ), overlay->getCategory() );
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
		TreeItemProperty * data = reinterpret_cast< TreeItemProperty * >( p_event.GetClientObject() );
		m_propertiesHolder->setPropertyData( data );
		p_event.Skip();
	}

	void SceneObjectsList::OnMouseRButtonUp( wxTreeEvent & p_event )
	{
	}
}
