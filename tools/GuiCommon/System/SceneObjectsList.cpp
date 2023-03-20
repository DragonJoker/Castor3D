#include "GuiCommon/System/SceneObjectsList.hpp"

#include "GuiCommon/Properties/PropertiesContainer.hpp"
#include "GuiCommon/Properties/TreeItems/AnimatedObjectGroupTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/AnimatedObjectTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/AnimationTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/BackgroundTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/BillboardTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/BoneTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/CameraTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/ControlTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/GeometryTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/LightTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/NodeTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/OverlayTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/RenderTargetTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/RenderWindowTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/SceneTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/SkeletonAnimationTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/SkeletonNodeTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/SkeletonTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/SubmeshTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/ViewportTreeItemProperty.hpp"
#include "GuiCommon/System/ImagesLoader.hpp"
#include "GuiCommon/System/MaterialsList.hpp"

#include <wx/imaglist.h>
#include <wx/aui/framemanager.h>
#include <wx/artprov.h>

#include <Castor3D/Engine.hpp>
#include <Castor3D/Animation/Animation.hpp>
#include <Castor3D/Cache/AnimatedObjectGroupCache.hpp>
#include <Castor3D/Cache/CacheView.hpp>
#include <Castor3D/Cache/GeometryCache.hpp>
#include <Castor3D/Cache/LightCache.hpp>
#include <Castor3D/Cache/MaterialCache.hpp>
#include <Castor3D/Cache/ObjectCache.hpp>
#include <Castor3D/Cache/OverlayCache.hpp>
#include <Castor3D/Cache/TargetCache.hpp>
#include <Castor3D/Gui/ControlsManager.hpp>
#include <Castor3D/Gui/Controls/CtrlPanel.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Model/Mesh/Mesh.hpp>
#include <Castor3D/Model/Skeleton/BoneNode.hpp>
#include <Castor3D/Model/Skeleton/Skeleton.hpp>
#include <Castor3D/Model/Skeleton/Animation/SkeletonAnimation.hpp>
#include <Castor3D/Overlay/Overlay.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Render/RenderWindow.hpp>
#include <Castor3D/Render/PostEffect/PostEffect.hpp>
#include <Castor3D/Render/ToneMapping/ToneMapping.hpp>
#include <Castor3D/Scene/BillboardList.hpp>
#include <Castor3D/Scene/Camera.hpp>
#include <Castor3D/Scene/Geometry.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/SceneNode.hpp>
#include <Castor3D/Scene/Animation/AnimatedObject.hpp>
#include <Castor3D/Scene/Animation/AnimatedObjectGroup.hpp>
#include <Castor3D/Scene/Light/Light.hpp>

#include <CastorUtils/Design/ResourceCache.hpp>

namespace GuiCommon
{
	SceneObjectsList::SceneObjectsList( PropertiesContainer * propertiesHolder
		, wxWindow * parent
		, wxPoint const & ptPos
		, wxSize const & size )
		: wxTreeCtrl( parent, wxID_ANY, ptPos, size, wxTR_DEFAULT_STYLE | wxNO_BORDER )
		, m_engine( nullptr )
		, m_propertiesHolder( propertiesHolder )
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
			ImagesLoader::getBitmap( eBMP_POST_EFFECT ),
			ImagesLoader::getBitmap( eBMP_POST_EFFECT_SEL ),
			ImagesLoader::getBitmap( eBMP_TONE_MAPPING ),
			ImagesLoader::getBitmap( eBMP_TONE_MAPPING_SEL ),
			ImagesLoader::getBitmap( eBMP_SKELETON ),
			ImagesLoader::getBitmap( eBMP_SKELETON_SEL ),
			ImagesLoader::getBitmap( eBMP_BONE ),
			ImagesLoader::getBitmap( eBMP_BONE_SEL ),
			ImagesLoader::getBitmap( eBMP_BACKGROUND ),
			ImagesLoader::getBitmap( eBMP_BACKGROUND_SEL ),
		};

		auto * imageList = new wxImageList( GC_IMG_SIZE, GC_IMG_SIZE, true );

		for ( auto image : icons )
		{
			int sizeOrig = image->GetWidth();

			if ( sizeOrig != GC_IMG_SIZE )
			{
				image->Rescale( GC_IMG_SIZE, GC_IMG_SIZE, wxIMAGE_QUALITY_HIGHEST );
			}

			imageList->Add( *image );
		}

		AssignImageList( imageList );
	}

	void SceneObjectsList::doAddControl( wxTreeItemId id
		, castor3d::Control & control )
	{
		auto parentId = AppendItem( id
			, control.getName()
			, eBMP_BORDER_PANEL_OVERLAY
			, eBMP_BORDER_PANEL_OVERLAY_SEL
			, new ControlTreeItemProperty{ m_propertiesHolder->isEditable(), control } );

		if ( control.getType() == castor3d::ControlType::ePanel )
		{
			for ( auto & sub : static_cast< castor3d::PanelCtrl & >( control ).getChildren() )
			{
				if ( auto ctrl = sub.lock() )
				{
					doAddControl( parentId, *ctrl );
				}
			}
		}
	}

	void SceneObjectsList::loadScene( castor3d::Engine * engine
		, castor3d::RenderWindow & window
		, castor3d::SceneRPtr scene )
	{
		m_scene = scene;
		m_engine = engine;

		if ( scene )
		{
			auto rootId = AddRoot( make_wxString( window.getName() )
				, eBMP_RENDER_WINDOW
				, eBMP_RENDER_WINDOW_SEL
				, new RenderWindowTreeItemProperty( m_propertiesHolder->isEditable(), window ) );

			auto catId = AppendItem( rootId
				, _( "Global GUI Controls" )
				, eBMP_BORDER_PANEL_OVERLAY
				, eBMP_BORDER_PANEL_OVERLAY_SEL );

			for ( auto & control : static_cast< castor3d::ControlsManager const & >( *scene->getEngine()->getUserInputListener() ).getRootControls() )
			{
				if ( control
					&& !control->hasScene()
					&& control->getName() != "Debug/Main"
					&& control->getName() != "Debug/RenderPasses" )
				{
					doAddControl( catId, *control );
				}
			}

			wxTreeItemId sceneId = AppendItem( rootId
				, scene->getName()
				, eBMP_SCENE
				, eBMP_SCENE_SEL
				, new SceneTreeItemProperty( this, m_propertiesHolder->isEditable(), *scene ) );

			AppendItem( sceneId
				, _( "Background" )
				, eBMP_BACKGROUND
				, eBMP_BACKGROUND_SEL
				, new BackgroundTreeItemProperty( this, m_propertiesHolder->isEditable(), *scene->getBackground() ) );

			catId = AppendItem( sceneId
				, _( "Render Targets" )
				, eBMP_RENDER_TARGET
				, eBMP_RENDER_TARGET_SEL );
			scene->getEngine()->getRenderTargetCache().forEach( [this, catId]( castor3d::RenderTarget & elem )
				{
					appendRenderTarget( this
						, m_propertiesHolder->isEditable()
						, catId
						, elem );
				} );

			catId = AppendItem( sceneId
				, _( "Cameras" )
				, eBMP_CAMERA
				, eBMP_CAMERA_SEL );
			scene->getCameraCache().forEach( [this, catId]( castor3d::Camera & elem )
				{
					doAddCamera( catId, elem );
				} );

			catId = AppendItem( sceneId
				, _( "Light Sources" )
				, eBMP_DIRECTIONAL_LIGHT
				, eBMP_DIRECTIONAL_LIGHT_SEL );
			scene->getLightCache().forEach( [this, catId]( castor3d::Light & elem )
				{
					doAddLight( catId, elem );
				} );

			catId = AppendItem( sceneId
				, _( "Scene Nodes" )
				, eBMP_NODE
				, eBMP_NODE_SEL );
			auto rootNode = scene->getRootNode();

			if ( rootNode )
			{
				doAddNode( catId, *rootNode );
			}

			catId = AppendItem( sceneId
				, _( "Materials" )
				, eBMP_MATERIAL
				, eBMP_MATERIAL_SEL );
			for ( auto materialName : scene->getMaterialView() )
			{
				auto material = engine->findMaterial( materialName ).lock().get();
				MaterialsList::addMaterial( this
					, *scene
					, m_propertiesHolder->isEditable()
					, catId
					, material );
			}

			catId = AppendItem( sceneId
				, _( "Animated Object Groups" )
				, eBMP_ANIMATED_OBJECTGROUP
				, eBMP_ANIMATED_OBJECTGROUP_SEL );
			scene->getAnimatedObjectGroupCache().forEach( [this, catId]( castor3d::AnimatedObjectGroup & elem )
				{
					doAddAnimatedObjectGroup( AppendItem( catId
							, elem.getName()
							, eBMP_ANIMATED_OBJECTGROUP
							, eBMP_ANIMATED_OBJECTGROUP_SEL
							, new AnimatedObjectGroupTreeItemProperty{ m_propertiesHolder->isEditable(), elem } )
						, elem );
				} );

			catId = AppendItem( sceneId
				, _( "Overlays" )
				, eBMP_PANEL_OVERLAY
				, eBMP_PANEL_OVERLAY_SEL );

			for ( auto overlay : scene->getOverlayCache().getCategories() )
			{
				switch ( overlay->getType() )
				{
				case castor3d::OverlayType::ePanel:
					doAddOverlay( AppendItem( catId
							, overlay->getOverlayName()
							, eBMP_PANEL_OVERLAY
							, eBMP_PANEL_OVERLAY_SEL
							, new OverlayTreeItemProperty( m_propertiesHolder->isEditable(), overlay ) )
						, *overlay );
					break;
				case castor3d::OverlayType::eBorderPanel:
					doAddOverlay( AppendItem( catId
							, overlay->getOverlayName()
							, eBMP_BORDER_PANEL_OVERLAY
							, eBMP_BORDER_PANEL_OVERLAY_SEL
							, new OverlayTreeItemProperty( m_propertiesHolder->isEditable(), overlay ) )
						, *overlay );
					break;
				case castor3d::OverlayType::eText:
					doAddOverlay( AppendItem( catId
							, overlay->getOverlayName()
							, eBMP_TEXT_OVERLAY
							, eBMP_TEXT_OVERLAY_SEL
							, new OverlayTreeItemProperty( m_propertiesHolder->isEditable(), overlay ) )
						, *overlay );
					break;
				default:
					CU_Failure( "Unsupported OverlayType" );
					break;
				}
			}

			catId = AppendItem( sceneId
				, _( "Scene GUI" )
				, eBMP_BORDER_PANEL_OVERLAY
				, eBMP_BORDER_PANEL_OVERLAY_SEL );

			for ( auto & control : static_cast< castor3d::ControlsManager const & >( *scene->getEngine()->getUserInputListener() ).getRootControls() )
			{
				if ( control
					&& control->hasScene()
					&& &control->getScene() == scene )
				{
					AppendItem( catId
						, control->getName()
						, eBMP_BORDER_PANEL_OVERLAY
						, eBMP_BORDER_PANEL_OVERLAY_SEL
						, new ControlTreeItemProperty{ m_propertiesHolder->isEditable(), *control } );
				}
			}

			CollapseAll();
			Expand( rootId );
			Expand( sceneId );
		}
	}

	void SceneObjectsList::unloadScene()
	{
		DeleteAllItems();
		m_scene = {};
	}

	void SceneObjectsList::select( castor3d::GeometrySPtr geometry
		, castor3d::SubmeshSPtr submesh )
	{
		auto itg = m_ids.find( geometry.get() );

		if ( itg != m_ids.end() )
		{
			auto its = itg->second.find( submesh.get() );

			if ( its != itg->second.end() )
			{
				SelectItem( its->second );
			}
		}
	}

	void SceneObjectsList::doAddSubmesh( castor3d::GeometryRPtr geometry
		, castor3d::SubmeshRPtr submesh
		, wxTreeItemId id )
	{
		auto itg = m_ids.insert( { geometry, SubmeshIdMap{} } ).first;
		itg->second.insert( { submesh, id } );
	}

	void SceneObjectsList::doAddGeometry( wxTreeItemId id
		, castor3d::Geometry & geometry )
	{
		wxTreeItemId geometryId = AppendItem( id
			, geometry.getName()
			, eBMP_GEOMETRY
			, eBMP_GEOMETRY_SEL
			, new GeometryTreeItemProperty( m_propertiesHolder->isEditable(), geometry ) );
		auto mesh = geometry.getMesh().lock();

		if ( mesh )
		{
			int count = 0;

			for ( auto submesh : *mesh )
			{
				wxString name = _( "Submesh " );
				name << count++;
				wxTreeItemId idSubmesh = AppendItem( geometryId
					, name
					, eBMP_SUBMESH
					, eBMP_SUBMESH_SEL
					, new SubmeshTreeItemProperty( m_propertiesHolder->isEditable(), geometry, *submesh ) );
				doAddSubmesh( &geometry
					, submesh.get()
					, idSubmesh );
			}

			auto skeleton = mesh->getSkeleton();

			if ( skeleton )
			{
				wxTreeItemId idSkeleton = AppendItem( geometryId
					, mesh->getName()
					, eBMP_SKELETON
					, eBMP_SKELETON_SEL
					, new SkeletonTreeItemProperty( m_propertiesHolder->isEditable(), *skeleton ) );
				doAddSkeleton( *skeleton, idSkeleton );
			}
		}
	}

	void SceneObjectsList::doAddSkeleton( castor3d::Skeleton const & skeleton
		, wxTreeItemId idSkeleton )
	{
		for ( auto & node : skeleton.getNodes() )
		{
			if ( node->getType() == castor3d::SkeletonNodeType::eBone )
			{
				AppendItem( idSkeleton
					, node->getName()
					, eBMP_SKELETON
					, eBMP_SKELETON_SEL
					, new BoneTreeItemProperty( m_propertiesHolder->isEditable()
						, static_cast< castor3d::BoneNode & >( *node ) ) );
			}
			else
			{
				AppendItem( idSkeleton
					, node->getName()
					, eBMP_SKELETON
					, eBMP_SKELETON_SEL
					, new SkeletonNodeTreeItemProperty( m_propertiesHolder->isEditable(), *node ) );
			}
		}

		for ( auto & anim : skeleton.getAnimations() )
		{
			AppendItem( idSkeleton
				, anim.first
				, eBMP_ANIMATION
				, eBMP_ANIMATION_SEL
				, new SkeletonAnimationTreeItemProperty( m_propertiesHolder->isEditable()
					, static_cast< castor3d::SkeletonAnimation & >( *anim.second ) ) );
		}
	}

	void SceneObjectsList::doAddCamera( wxTreeItemId id
		, castor3d::Camera & camera )
	{
		wxTreeItemId cameraId = AppendItem( id
			, camera.getName()
			, eBMP_CAMERA
			, eBMP_CAMERA_SEL
			, new CameraTreeItemProperty( m_propertiesHolder->isEditable(), camera ) );
		AppendItem( cameraId
			, _( "Viewport" )
			, eBMP_VIEWPORT
			, eBMP_VIEWPORT_SEL
			, new ViewportTreeItemProperty( m_propertiesHolder->isEditable()
				, *camera.getScene()->getEngine()
				, camera.getViewport() ) );
	}

	void SceneObjectsList::doAddBillboard( wxTreeItemId id
		, castor3d::BillboardList & billboard )
	{
		AppendItem( id
			, billboard.getName()
			, eBMP_BILLBOARD
			, eBMP_BILLBOARD_SEL
			, new BillboardTreeItemProperty( m_propertiesHolder->isEditable(), billboard ) );
	}

	void SceneObjectsList::doAddLight( wxTreeItemId id
		, castor3d::Light & light )
	{
		switch ( light.getLightType() )
		{
		case castor3d::LightType::eDirectional:
			AppendItem( id
				, light.getName()
				, eBMP_DIRECTIONAL_LIGHT
				, eBMP_DIRECTIONAL_LIGHT_SEL
				, new LightTreeItemProperty( m_propertiesHolder->isEditable(), light ) );
			break;

		case castor3d::LightType::ePoint:
			AppendItem( id
				, light.getName()
				, eBMP_POINT_LIGHT
				, eBMP_POINT_LIGHT_SEL
				, new LightTreeItemProperty( m_propertiesHolder->isEditable(), light ) );
			break;

		case castor3d::LightType::eSpot:
			AppendItem( id
				, light.getName()
				, eBMP_SPOT_LIGHT
				, eBMP_SPOT_LIGHT_SEL
				, new LightTreeItemProperty( m_propertiesHolder->isEditable(), light ) );
			break;
		default:
			CU_Failure( "Unsupported LightType" );
			break;
		}
	}

	void SceneObjectsList::doAddNode( wxTreeItemId id
		, castor3d::SceneNode & node )
	{
		for ( auto & object : node.getObjects() )
		{
			switch ( object.get().getType() )
			{
			case castor3d::MovableType::eGeometry:
				doAddGeometry( id, static_cast< castor3d::Geometry & >( object.get() ) );
				break;
			case castor3d::MovableType::eCamera:
				doAddCamera( id, static_cast< castor3d::Camera & >( object.get() ) );
				break;
			case castor3d::MovableType::eLight:
				doAddLight( id, static_cast< castor3d::Light & >( object.get() ) );
				break;
			case castor3d::MovableType::eBillboard:
				doAddBillboard( id, static_cast< castor3d::BillboardList & >( object.get() ) );
				break;
			case castor3d::MovableType::eParticleEmitter:
				break;
			default:
				CU_Failure( "Unsupported MovableType" );
				break;
			}
		}

		for ( auto pair : node.getChildren() )
		{
			doAddNode( AppendItem( id
					, pair.first
					, eBMP_NODE
					, eBMP_NODE_SEL
					, new NodeTreeItemProperty( m_propertiesHolder->isEditable(), m_engine, *pair.second ) )
				, *pair.second );
		}
	}

	void SceneObjectsList::doAddAnimatedObjectGroup( wxTreeItemId id
		, castor3d::AnimatedObjectGroup & group )
	{
		for ( auto it : group.getAnimations() )
		{
			AppendItem( id
				, it.first
				, eBMP_ANIMATION
				, eBMP_ANIMATION_SEL
				, new AnimationTreeItemProperty
				{
					m_engine,
					m_propertiesHolder->isEditable(),
					group,
					it.first,
					it.second,
				} );
		}
	}

	void SceneObjectsList::doAddOverlay( wxTreeItemId id
		, castor3d::OverlayCategory & category )
	{
		for ( auto overlay : category.getOverlay() )
		{
			switch ( overlay->getType() )
			{
			case castor3d::OverlayType::ePanel:
				doAddOverlay( AppendItem( id
						, overlay->getName()
						, eBMP_PANEL_OVERLAY
						, eBMP_PANEL_OVERLAY_SEL
						, new OverlayTreeItemProperty( m_propertiesHolder->isEditable(), overlay->getCategory() ) )
					, *overlay->getCategory() );
				break;
			case castor3d::OverlayType::eBorderPanel:
				doAddOverlay( AppendItem( id
						, overlay->getName()
						, eBMP_BORDER_PANEL_OVERLAY
						, eBMP_BORDER_PANEL_OVERLAY_SEL
						, new OverlayTreeItemProperty( m_propertiesHolder->isEditable(), overlay->getCategory() ) )
					, *overlay->getCategory() );
				break;
			case castor3d::OverlayType::eText:
				doAddOverlay( AppendItem( id
						, overlay->getName()
						, eBMP_TEXT_OVERLAY
						, eBMP_TEXT_OVERLAY_SEL
						, new OverlayTreeItemProperty( m_propertiesHolder->isEditable(), overlay->getCategory() ) )
					, *overlay->getCategory() );
				break;
			default:
				CU_Failure( "Unsupported OverlayType" );
				break;
			}
		}
	}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
	BEGIN_EVENT_TABLE( SceneObjectsList, wxTreeCtrl )
		EVT_CLOSE( SceneObjectsList::onClose )
		EVT_TREE_SEL_CHANGED( wxID_ANY, SceneObjectsList::onSelectItem )
		EVT_TREE_ITEM_RIGHT_CLICK( wxID_ANY, SceneObjectsList::onMouseRButtonUp )
	END_EVENT_TABLE()
#pragma GCC diagnostic pop

	void SceneObjectsList::onClose( wxCloseEvent & event )
	{
		DeleteAllItems();
		event.Skip();
	}

	void SceneObjectsList::onSelectItem( wxTreeEvent & event )
	{
		TreeItemProperty * data = static_cast< TreeItemProperty * >( event.GetClientObject() );
		m_propertiesHolder->setPropertyData( data );
		event.Skip();
	}

	void SceneObjectsList::onMouseRButtonUp( wxTreeEvent & event )
	{
	}
}
