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
#include "GuiCommon/Properties/TreeItems/MaterialTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/NodeTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/OverlayTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/PassTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/RenderTargetTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/RenderWindowTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/SceneTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/SkeletonAnimationTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/SkeletonNodeTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/SkeletonTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/StyleTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/SubmeshTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/TextureTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/ViewportTreeItemProperty.hpp"
#include "GuiCommon/System/ImagesLoader.hpp"

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
#include <Castor3D/Gui/Controls/CtrlExpandablePanel.hpp>
#include <Castor3D/Gui/Controls/CtrlFrame.hpp>
#include <Castor3D/Gui/Controls/CtrlPanel.hpp>
#include <Castor3D/Gui/Controls/CtrlStatic.hpp>
#include <Castor3D/Gui/Theme/StylesHolder.hpp>
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
			ImagesLoader::getBitmap( eBMP_SSAO_CONFIG ),
			ImagesLoader::getBitmap( eBMP_SSAO_CONFIG_SEL ),
			ImagesLoader::getBitmap( eBMP_COLLAPSE_ALL ),
			ImagesLoader::getBitmap( eBMP_EXPAND_ALL ),
			ImagesLoader::getBitmap( eBMP_CONTROLS ),
			ImagesLoader::getBitmap( eBMP_CONTROLS_SEL ),
			ImagesLoader::getBitmap( eBMP_CONTROL ),
			ImagesLoader::getBitmap( eBMP_CONTROL_SEL ),
			ImagesLoader::getBitmap( eBMP_STYLES ),
			ImagesLoader::getBitmap( eBMP_STYLES_SEL ),
			ImagesLoader::getBitmap( eBMP_STYLE ),
			ImagesLoader::getBitmap( eBMP_STYLE_SEL ),
			ImagesLoader::getBitmap( eBMP_THEME ),
			ImagesLoader::getBitmap( eBMP_THEME_SEL ),
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

			auto catId = AppendItem( sceneId
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

			CollapseAll();
			Expand( rootId );
			Expand( sceneId );
		}
	}

	void SceneObjectsList::loadSceneMaterials( castor3d::Engine * engine
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

			for ( auto materialName : scene->getMaterialView() )
			{
				auto material = engine->findMaterial( materialName );
				doAddMaterial( rootId
					, material );
			}

			CollapseAll();
			Expand( rootId );
		}
	}

	void SceneObjectsList::loadSceneOverlays( castor3d::Engine * engine
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

			for ( auto overlay : scene->getOverlayCache().getCategories() )
			{
				switch ( overlay->getType() )
				{
				case castor3d::OverlayType::ePanel:
					doAddOverlay( AppendItem( rootId
							, overlay->getOverlayName()
							, eBMP_PANEL_OVERLAY
							, eBMP_PANEL_OVERLAY_SEL
							, new OverlayTreeItemProperty( m_propertiesHolder->isEditable(), *overlay ) )
						, *overlay );
					break;
				case castor3d::OverlayType::eBorderPanel:
					doAddOverlay( AppendItem( rootId
							, overlay->getOverlayName()
							, eBMP_BORDER_PANEL_OVERLAY
							, eBMP_BORDER_PANEL_OVERLAY_SEL
							, new OverlayTreeItemProperty( m_propertiesHolder->isEditable(), *overlay ) )
						, *overlay );
					break;
				case castor3d::OverlayType::eText:
					doAddOverlay( AppendItem( rootId
							, overlay->getOverlayName()
							, eBMP_TEXT_OVERLAY
							, eBMP_TEXT_OVERLAY_SEL
							, new OverlayTreeItemProperty( m_propertiesHolder->isEditable(), *overlay ) )
						, *overlay );
					break;
				default:
					CU_Failure( "Unsupported OverlayType" );
					break;
				}
			}

			CollapseAll();
			Expand( rootId );
		}
	}

	void SceneObjectsList::loadSceneGui( castor3d::Engine * engine
		, castor3d::RenderWindow & window
		, castor3d::SceneRPtr scene )
	{
		m_scene = scene;
		m_engine = engine;

		if ( scene )
		{
			auto & controlsManager = static_cast< castor3d::ControlsManager & >( *scene->getEngine()->getUserInputListener() );

			auto rootId = AddRoot( make_wxString( window.getName() )
				, eBMP_RENDER_WINDOW
				, eBMP_RENDER_WINDOW_SEL
				, new RenderWindowTreeItemProperty( m_propertiesHolder->isEditable(), window ) );

			auto catId = AppendItem( rootId
				, _( "Global GUI Styles" )
				, eBMP_STYLES
				, eBMP_STYLES_SEL );
			doAddStyles( catId, controlsManager, nullptr );

			for ( auto & theme : controlsManager.getThemes() )
			{
				if ( theme.first != "Debug" )
				{
					auto themeId = AppendItem( catId
						, theme.first
						, eBMP_THEME
						, eBMP_THEME_SEL );
					doAddStyles( themeId, *theme.second, nullptr );
				}
			}

			catId = AppendItem( rootId
				, _( "Global GUI Controls" )
				, eBMP_CONTROLS
				, eBMP_CONTROLS_SEL );

			for ( auto & control : controlsManager.getRootControls() )
			{
				if ( control
					&& !control->hasScene()
					&& control->getName() != "Debug/Main"
					&& control->getName() != "Debug/RenderPasses" )
				{
					doAddControl( catId, control->getName(), *control, true, false );
				}
			}

			catId = AppendItem( rootId
				, _( "Scene GUI Styles" )
				, eBMP_STYLES
				, eBMP_STYLES_SEL );
			doAddStyles( catId, controlsManager, scene );

			catId = AppendItem( rootId
				, _( "Scene GUI Controls" )
				, eBMP_CONTROLS
				, eBMP_CONTROLS_SEL );

			for ( auto & control : static_cast< castor3d::ControlsManager const & >( *scene->getEngine()->getUserInputListener() ).getRootControls() )
			{
				if ( control
					&& control->hasScene()
					&& &control->getScene() == scene )
				{
					doAddControl( catId, control->getName(), *control, true, false );
				}
			}

			CollapseAll();
			Expand( rootId );
		}
	}

	void SceneObjectsList::loadSceneNodes( castor3d::Engine * engine
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
			uint32_t count{};
			auto rootNode = scene->getRootNode();

			if ( rootNode )
			{
				doAddNode( rootId, *rootNode, count );
			}

			CollapseAll();
			Expand( rootId );
		}
	}

	void SceneObjectsList::loadSceneLights( castor3d::Engine * engine
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
			uint32_t count{};

			scene->getLightCache().forEach( [this, rootId, &count]( castor3d::Light & elem )
				{
					if ( ++count <= 500u )
					{
						doAddLight( rootId, elem );
					}
				} );

			CollapseAll();
			Expand( rootId );
		}
	}

	void SceneObjectsList::loadSceneObjects( castor3d::Engine * engine
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
			uint32_t count{};

			scene->getGeometryCache().forEach( [this, rootId, &count]( castor3d::Geometry & elem )
				{
					if ( ++count <= 500u )
					{
						doAddGeometry( rootId, elem );
					}
				} );

			CollapseAll();
			Expand( rootId );
		}
	}

	void SceneObjectsList::unloadScene()
	{
		DeleteAllItems();
		m_scene = {};
	}

	void SceneObjectsList::select( castor3d::GeometryRPtr geometry
		, castor3d::Submesh const * submesh )
	{
		auto itg = m_objects.find( geometry );

		if ( itg != m_objects.end() )
		{
			auto its = itg->second.find( submesh );

			if ( its != itg->second.end() )
			{
				SelectItem( its->second );
			}
		}
	}

	void SceneObjectsList::select( castor3d::MaterialRPtr material )
	{
		auto itm = m_materials.find( material );

		if ( itm != m_materials.end() )
		{
			SelectItem( itm->second );
		}
	}

	void SceneObjectsList::doAddSubmesh( castor3d::GeometryRPtr geometry
		, castor3d::SubmeshRPtr submesh
		, wxTreeItemId id )
	{
		auto itg = m_objects.insert( { geometry, SubmeshIdMap{} } ).first;
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

		if ( auto mesh = geometry.getMesh() )
		{
			int count = 0;

			for ( auto & submesh : *mesh )
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
		, castor3d::SceneNode & node
		, uint32_t & count )
	{
		for ( auto pair : node.getChildren() )
		{
			if ( ++count <= 500u )
			{
				doAddNode( AppendItem( id
						, pair.first
						, eBMP_NODE
						, eBMP_NODE_SEL
						, new NodeTreeItemProperty( m_propertiesHolder->isEditable(), m_engine, *pair.second ) )
					, *pair.second
					, count );
			}
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
					, overlay->getCategory() );
				break;
			case castor3d::OverlayType::eBorderPanel:
				doAddOverlay( AppendItem( id
						, overlay->getName()
						, eBMP_BORDER_PANEL_OVERLAY
						, eBMP_BORDER_PANEL_OVERLAY_SEL
						, new OverlayTreeItemProperty( m_propertiesHolder->isEditable(), overlay->getCategory() ) )
					, overlay->getCategory() );
				break;
			case castor3d::OverlayType::eText:
				doAddOverlay( AppendItem( id
						, overlay->getName()
						, eBMP_TEXT_OVERLAY
						, eBMP_TEXT_OVERLAY_SEL
						, new OverlayTreeItemProperty( m_propertiesHolder->isEditable(), overlay->getCategory() ) )
					, overlay->getCategory() );
				break;
			default:
				CU_Failure( "Unsupported OverlayType" );
				break;
			}
		}
	}

	void SceneObjectsList::doAddStyles( wxTreeItemId parentId
		, castor3d::StylesHolder & styles
		, castor3d::SceneRPtr scene )
	{
		for ( auto & sub : styles.getButtonStyles() )
		{
			doAddStyle( parentId, sub.first, *sub.second, scene );
		}

		for ( auto & sub : styles.getComboBoxStyles() )
		{
			doAddStyle( parentId, sub.first, *sub.second, scene );
		}

		for ( auto & sub : styles.getEditStyles() )
		{
			doAddStyle( parentId, sub.first, *sub.second, scene );
		}

		for ( auto & sub : styles.getExpandablePanelStyles() )
		{
			doAddStyle( parentId, sub.first, *sub.second, scene );
		}

		for ( auto & sub : styles.getFrameStyles() )
		{
			doAddStyle( parentId, sub.first, *sub.second, scene );
		}

		for ( auto & sub : styles.getListBoxStyles() )
		{
			doAddStyle( parentId, sub.first, *sub.second, scene );
		}

		for ( auto & sub : styles.getPanelStyles() )
		{
			doAddStyle( parentId, sub.first, *sub.second, scene );
		}

		for ( auto & sub : styles.getSliderStyles() )
		{
			doAddStyle( parentId, sub.first, *sub.second, scene );
		}

		for ( auto & sub : styles.getStaticStyles() )
		{
			doAddStyle( parentId, sub.first, *sub.second, scene );
		}
	}

	void SceneObjectsList::doAddStyle( wxTreeItemId id
		, castor::String const & name
		, castor3d::ControlStyle & style
		, castor3d::SceneRPtr scene )
	{
		if ( ( scene && !style.hasScene() )
			|| ( style.hasScene() && scene != &style.getScene() ) )
		{
			return;
		}

		auto parentId = AppendItem( id
			, name
			, eBMP_STYLE
			, eBMP_STYLE_SEL
			, new StyleTreeItemProperty{ m_propertiesHolder->isEditable(), style } );

		if ( isStylesHolder( style ) )
		{
			castor3d::StylesHolder * holder{};

			if ( style.getType() == castor3d::ControlType::ePanel )
			{
				holder = &static_cast< castor3d::PanelStyle & >( style );
			}

			if ( holder )
			{
				doAddStyles( parentId, *holder, scene );
			}
		}
		else if ( style.getType() == castor3d::ControlType::eExpandablePanel )
		{
			auto & expandable = static_cast< castor3d::ExpandablePanelStyle & >( style );
			doAddStyle( parentId, "Header", expandable.getHeaderStyle(), scene );
			doAddStyle( parentId, "Expand", expandable.getExpandStyle(), scene );
			doAddStyle( parentId, "Content", expandable.getContentStyle(), scene );
		}
		else if ( style.getType() == castor3d::ControlType::eComboBox )
		{
			auto & combo = static_cast< castor3d::ComboBoxStyle & >( style );
			doAddStyle( parentId, "Expand", combo.getExpandStyle(), scene );
			doAddStyle( parentId, "Elements", combo.getElementsStyle(), scene );
		}
		else if ( style.getType() == castor3d::ControlType::eListBox )
		{
			auto & list = static_cast< castor3d::ListBoxStyle & >( style );
			doAddStyle( parentId, "Item", list.getItemStyle(), scene );
			doAddStyle( parentId, "Highlighted Item", list.getHighlightedItemStyle(), scene );
			doAddStyle( parentId, "Selected Item", list.getSelectedItemStyle(), scene );
		}
		else if ( style.getType() == castor3d::ControlType::eSlider )
		{
			auto & slider = static_cast< castor3d::SliderStyle & >( style );
			doAddStyle( parentId, "Line", slider.getLineStyle(), scene );
			doAddStyle( parentId, "Tick", slider.getTickStyle(), scene );
		}
	}

	void SceneObjectsList::doAddControl( wxTreeItemId id
		, castor::String const & name
		, castor3d::Control & control
		, bool full
		, bool inLayout )
	{
		auto parentId = AppendItem( id
			, name
			, eBMP_CONTROL
			, eBMP_CONTROL_SEL
			, new ControlTreeItemProperty{ m_propertiesHolder->isEditable(), control, full, inLayout } );

		if ( isLayoutControl( control ) )
		{
			auto & layout = static_cast< castor3d::LayoutControl & >( control );

			for ( auto ctrl : layout.getChildren() )
			{
				if ( ctrl )
				{
					doAddControl( parentId, ctrl->getName(), *ctrl, true, layout.getLayout() != nullptr );
				}
			}
		}
		else if ( control.getType() == castor3d::ControlType::eExpandablePanel )
		{
			auto & expandable = static_cast< castor3d::ExpandablePanelCtrl & >( control );
			doAddControl( parentId, "Header", *expandable.getHeader(), false, false );
			doAddControl( parentId, "Expand", *expandable.getExpand(), false, false );
			doAddControl( parentId, "Content", *expandable.getContent(), false, false );
		}
		else if ( control.getType() == castor3d::ControlType::eFrame )
		{
			auto & frame = static_cast< castor3d::FrameCtrl & >( control );
			doAddControl( parentId, "Content", *frame.getContent(), false, false );
		}
	}

	void SceneObjectsList::doAddMaterial( wxTreeItemId id
		, castor3d::MaterialObs material )
	{
		wxTreeItemId materialId = AppendItem( id
			, material->getName()
			, int( eBMP_MATERIAL )
			, int( eBMP_MATERIAL_SEL )
			, new MaterialTreeItemProperty{ m_propertiesHolder->isEditable(), *material } );
		uint32_t passIndex = 0;
		m_materials.emplace( material, materialId );

		for ( auto & pass : *material )
		{
			doAddPass( materialId
				, ++passIndex
				, *pass );
		}
	}

	void SceneObjectsList::doAddPass( wxTreeItemId id
		, uint32_t index
		, castor3d::Pass & pass )
	{
		wxTreeItemId passId = AppendItem( id
			, wxString( _( "Pass " ) ) << index
			, int( eBMP_PASS )
			, int( eBMP_PASS_SEL )
			, new PassTreeItemProperty{ m_propertiesHolder->isEditable()
				, pass
				, *m_scene
				, this } );
		uint32_t unitIndex = 0;

		for ( auto unit : pass )
		{
			doAddTexture( passId
				, ++unitIndex
				, pass
				, *unit );
		}
	}

	void SceneObjectsList::doAddTexture( wxTreeItemId id
		, uint32_t index
		, castor3d::Pass & pass
		, castor3d::TextureUnit & texture )
	{
		wxTreeItemId unitId = AppendItem( id
			, wxString( _( "Texture Unit " ) ) << index
			, int( eBMP_TEXTURE )
			, int( eBMP_TEXTURE_SEL )
			, new TextureTreeItemProperty{ m_propertiesHolder->isEditable()
				, pass
				, texture } );

		if ( texture.isRenderTarget() )
		{
			auto target = texture.getRenderTarget();
			appendRenderTarget( this
				, m_propertiesHolder->isEditable()
				, unitId
				, *target );
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
