#include "GuiCommon/System/SceneObjectsTree.hpp"

#include "GuiCommon/Properties/PropertiesContainer.hpp"
#include "GuiCommon/Properties/TreeItems/AnimatedObjectTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/BackgroundTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/CameraTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/RenderTargetTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/RenderWindowTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/SceneTreeItemProperty.hpp"
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
#include <Castor3D/Scene/ParticleSystem/ParticleSystem.hpp>

#include <CastorUtils/Design/ResourceCache.hpp>

namespace GuiCommon
{
	SceneObjectsTree::SceneObjectsTree( PropertiesContainer * propertiesHolder
		, wxWindow * parent
		, wxPoint const & pos
		, wxSize const & size )
		: wxTreeCtrl{ parent, wxID_ANY, pos, size, wxTR_HAS_BUTTONS | wxTR_SINGLE | wxNO_BORDER }
		, m_propertiesHolder{ propertiesHolder }
		, m_images{ GC_IMG_SIZE, GC_IMG_SIZE, true }
	{
		for ( auto const & [id, image] : ImagesLoader::getBitmaps() )
		{
			if ( int sizeOrig = image->GetWidth();
				sizeOrig != GC_IMG_SIZE )
			{
				image->Rescale( GC_IMG_SIZE, GC_IMG_SIZE, wxIMAGE_QUALITY_HIGHEST );
			}

			m_images.Add( *image );
		}

		SetImageList( &m_images );
	}

	void SceneObjectsTree::loadScene( castor3d::Engine * engine
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
				, new DataType{ std::make_unique< RenderWindowTreeItemProperty >( m_propertiesHolder->isEditable(), window ) } );

			auto sceneId = AppendItem( rootId
				, make_wxString( scene->getName() )
				, eBMP_SCENE
				, eBMP_SCENE_SEL
				, new DataType{ std::make_unique< SceneTreeItemProperty >( m_propertiesHolder->isEditable(), *scene ) } );

			AppendItem( sceneId
				, _( "Background" )
				, eBMP_BACKGROUND
				, eBMP_BACKGROUND_SEL
				, new DataType{ std::make_unique< BackgroundTreeItemProperty >( m_propertiesHolder->isEditable(), *scene->getBackground() ) } );

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
			m_animatedObjectGroupProperties = std::make_unique< AnimatedObjectGroupTreeItemProperty >( m_propertiesHolder->isEditable(), m_engine );
			m_animationProperties = std::make_unique< AnimationTreeItemProperty >( m_propertiesHolder->isEditable(), m_engine );
			scene->getAnimatedObjectGroupCache().forEach( [this, catId]( castor3d::AnimatedObjectGroup & elem )
				{
					doAddAnimatedObjectGroup( AppendItem( catId
							, elem.getName()
							, eBMP_ANIMATED_OBJECTGROUP
							, eBMP_ANIMATED_OBJECTGROUP_SEL
							, new DataType{ ObjectType::eAnimatedObjectGroup, &elem } )
						, elem );
				} );

			CollapseAll();
			Expand( rootId );
			Expand( sceneId );
		}
	}

	void SceneObjectsTree::loadSceneMaterials( castor3d::Engine * engine
		, castor3d::SceneRPtr scene )
	{
		m_scene = scene;
		m_engine = engine;

		if ( scene )
		{
			auto rootId = AddRoot( _( "Materials" )
				, eBMP_MATERIAL
				, eBMP_MATERIAL_SEL );
			m_materialProperties = std::make_unique< MaterialTreeItemProperty >( m_propertiesHolder->isEditable(), m_engine );
			m_passProperties = std::make_unique< PassTreeItemProperty >( m_propertiesHolder->isEditable(), *m_scene, this );
			m_textureProperties = std::make_unique< TextureTreeItemProperty >( m_propertiesHolder->isEditable(), m_engine );

			for ( auto const & materialName : scene->getMaterialView() )
			{
				auto material = engine->findMaterial( materialName );
				doAddMaterial( rootId
					, material );
			}

			CollapseAll();
			Expand( rootId );
		}
	}

	void SceneObjectsTree::loadSceneOverlays( castor3d::Engine * engine
		, castor3d::SceneRPtr scene )
	{
		m_scene = scene;
		m_engine = engine;

		if ( scene )
		{
			m_overlayProperties = std::make_unique< OverlayTreeItemProperty >( m_propertiesHolder->isEditable(), m_engine );
			auto rootId = AddRoot( _( "Overlays" )
				, eBMP_BORDER_PANEL_OVERLAY
				, eBMP_BORDER_PANEL_OVERLAY_SEL );

			for ( auto overlay : scene->getOverlayCache().getCategories() )
			{
				switch ( overlay->getType() )
				{
				case castor3d::OverlayType::ePanel:
					doAddOverlay( AppendItem( rootId
							, overlay->getOverlayName()
							, eBMP_PANEL_OVERLAY
							, eBMP_PANEL_OVERLAY_SEL
							, new DataType{ ObjectType::eOverlay, overlay } )
						, *overlay );
					break;
				case castor3d::OverlayType::eBorderPanel:
					doAddOverlay( AppendItem( rootId
							, overlay->getOverlayName()
							, eBMP_BORDER_PANEL_OVERLAY
							, eBMP_BORDER_PANEL_OVERLAY_SEL
							, new DataType{ ObjectType::eOverlay, overlay } )
						, *overlay );
					break;
				case castor3d::OverlayType::eText:
					doAddOverlay( AppendItem( rootId
							, overlay->getOverlayName()
							, eBMP_TEXT_OVERLAY
							, eBMP_TEXT_OVERLAY_SEL
							, new DataType{ ObjectType::eOverlay, overlay } )
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

	void SceneObjectsTree::loadSceneGui( castor3d::Engine * engine
		, castor3d::SceneRPtr scene )
	{
		m_scene = scene;
		m_engine = engine;

		if ( scene )
		{
			m_styleProperties = std::make_unique< StyleTreeItemProperty >( m_propertiesHolder->isEditable(), m_engine );
			m_controlProperties = std::make_unique< ControlTreeItemProperty >( m_propertiesHolder->isEditable(), m_engine );
			auto & controlsManager = static_cast< castor3d::ControlsManager const & >( *scene->getEngine()->getUserInputListener() );
			auto rootId = AddRoot( _( "GUI" )
				, eBMP_STYLES
				, eBMP_STYLES_SEL );
			auto catId = AppendItem( rootId
				, _( "Global GUI Styles" )
				, eBMP_STYLES
				, eBMP_STYLES_SEL );
			doAddStyles( catId, controlsManager, nullptr );

			for ( auto const & [name, theme] : controlsManager.getThemes() )
			{
				if ( name != "Debug" )
				{
					auto themeId = AppendItem( catId
						, name
						, eBMP_THEME
						, eBMP_THEME_SEL );
					doAddStyles( themeId, *theme, nullptr );
				}
			}

			catId = AppendItem( rootId
				, _( "Global GUI Controls" )
				, eBMP_CONTROLS
				, eBMP_CONTROLS_SEL );

			for ( auto const & control : controlsManager.getRootControls() )
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

			for ( auto const & control : static_cast< castor3d::ControlsManager const & >( *scene->getEngine()->getUserInputListener() ).getRootControls() )
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

	void SceneObjectsTree::loadSceneNodes( castor3d::Engine * engine
		, castor3d::SceneRPtr scene )
	{
		m_scene = scene;
		m_engine = engine;

		if ( scene )
		{
			m_nodeProperties = std::make_unique< NodeTreeItemProperty >( m_propertiesHolder->isEditable(), m_engine );
			auto rootId = AddRoot( _( "Nodes" )
				, eBMP_NODE
				, eBMP_NODE_SEL );

			if ( auto rootNode = scene->getRootNode() )
			{
				doAddNode( rootId, *rootNode );
			}

			CollapseAll();
			Expand( rootId );
		}
	}

	void SceneObjectsTree::loadSceneLights( castor3d::Engine * engine
		, castor3d::SceneRPtr scene )
	{
		m_scene = scene;
		m_engine = engine;

		if ( scene )
		{
			m_lightProperties = std::make_unique< LightTreeItemProperty >( m_propertiesHolder->isEditable(), m_engine );

			auto rootId = AddRoot( _( "Lights" )
				, eBMP_DIRECTIONAL_LIGHT
				, eBMP_DIRECTIONAL_LIGHT_SEL );
			doLoadSceneLights( rootId
				, _( "Directional Lights" )
				, castor3d::LightType::eDirectional
				, eBMP_DIRECTIONAL_LIGHT
				, eBMP_DIRECTIONAL_LIGHT_SEL );
			doLoadSceneLights( rootId
				, _( "Point Lights" )
				, castor3d::LightType::ePoint
				, eBMP_POINT_LIGHT
				, eBMP_POINT_LIGHT_SEL );
			doLoadSceneLights( rootId
				, _( "Spot Lights" )
				, castor3d::LightType::eSpot
				, eBMP_SPOT_LIGHT
				, eBMP_SPOT_LIGHT_SEL );
			CollapseAll();
			Expand( rootId );
		}
	}

	void SceneObjectsTree::loadSceneObjects( castor3d::Engine * engine
		, castor3d::SceneRPtr scene )
	{
		m_scene = scene;
		m_engine = engine;

		if ( scene )
		{
			m_geometryProperties = std::make_unique< GeometryTreeItemProperty >( m_propertiesHolder->isEditable(), m_engine );
			m_billboardsProperties = std::make_unique< BillboardTreeItemProperty >( m_propertiesHolder->isEditable(), m_engine );
			m_particlesProperties = std::make_unique< ParticleSystemTreeItemProperty >( m_propertiesHolder->isEditable(), m_engine );
			m_submeshProperties = std::make_unique< SubmeshTreeItemProperty >( m_propertiesHolder->isEditable(), m_engine );
			m_skeletonProperties = std::make_unique< SkeletonTreeItemProperty >( m_propertiesHolder->isEditable(), m_engine );
			m_skeletonBoneProperties = std::make_unique< BoneTreeItemProperty >( m_propertiesHolder->isEditable(), m_engine );
			m_skeletonNodeProperties = std::make_unique< SkeletonNodeTreeItemProperty >( m_propertiesHolder->isEditable(), m_engine );
			m_skeletonAnimationProperties = std::make_unique< SkeletonAnimationTreeItemProperty >( m_propertiesHolder->isEditable(), m_engine );

			auto rootId = AddRoot( _( "Objects" )
				, eBMP_GEOMETRY
				, eBMP_GEOMETRY_SEL );

			auto geometriesId = AppendItem( rootId
				, _( "Geometries" )
				, eBMP_GEOMETRY
				, eBMP_GEOMETRY_SEL );
			scene->getGeometryCache().forEach( [this, geometriesId]( castor3d::Geometry & elem )
				{
					doAddGeometry( geometriesId, elem );
				} );

			auto billboardsId = AppendItem( rootId
				, _( "Billboards" )
				, eBMP_BILLBOARD
				, eBMP_BILLBOARD_SEL );
			scene->getBillboardListCache().forEach( [this, billboardsId]( castor3d::BillboardList & elem )
				{
					doAddBillboard( billboardsId, elem );
				} );

			auto particlesId = AppendItem( rootId
				, _( "Particles" )
				, eBMP_PARTICLE
				, eBMP_PARTICLE_SEL );
			scene->getParticleSystemCache().forEach( [this, particlesId]( castor3d::ParticleSystem & elem )
				{
					doAddParticleSystem( particlesId, elem );
				} );

			CollapseAll();
			Expand( rootId );
			Expand( geometriesId );
			Expand( billboardsId );
			Expand( particlesId );
		}
	}

	void SceneObjectsTree::unloadScene()
	{
		DeleteAllItems();
		m_scene = {};
	}

	void SceneObjectsTree::select( castor3d::GeometryRPtr geometry
		, castor3d::Submesh const * submesh )
	{
		if ( auto itg = m_objects.find( geometry );
			itg != m_objects.end() )
		{
			if ( auto its = itg->second.find( submesh );
				its != itg->second.end() )
			{
				SelectItem( its->second );
			}
		}
	}

	void SceneObjectsTree::select( castor3d::MaterialRPtr material )
	{
		if ( auto itm = m_materials.find( material );
			itm != m_materials.end() )
		{
			SelectItem( itm->second );
		}
	}

	void SceneObjectsTree::doAddSubmesh( wxTreeItemId id
		, castor3d::GeometryRPtr geometry
		, castor3d::Submesh const * submesh )
	{
		auto itg = m_objects.try_emplace( geometry ).first;
		itg->second.try_emplace( submesh, id );
	}

	void SceneObjectsTree::doAddGeometry( wxTreeItemId id
		, castor3d::Geometry & geometry )
	{
		auto geometryId = AppendItem( id
			, geometry.getName()
			, eBMP_GEOMETRY
			, eBMP_GEOMETRY_SEL
			, new DataType{ ObjectType::eGeometry, &geometry } );

		if ( auto const & mesh = geometry.getMesh() )
		{
			int count = 0;

			for ( auto const & submesh : *mesh )
			{
				wxString name = _( "Submesh " );
				name << count++;
				auto idSubmesh = AppendItem( geometryId
					, name
					, eBMP_SUBMESH
					, eBMP_SUBMESH_SEL
					, new DataType{ geometry, *submesh } );
				doAddSubmesh( idSubmesh
					, &geometry
					, submesh.get() );
			}

			if ( auto skeleton = mesh->getSkeleton();
				skeleton )
			{
				auto idSkeleton = AppendItem( geometryId
					, mesh->getName()
					, eBMP_SKELETON
					, eBMP_SKELETON_SEL
					, new DataType{ ObjectType::eSkeleton, skeleton } );
				doAddSkeleton( idSkeleton , *skeleton );
			}
		}
	}

	void SceneObjectsTree::doAddSkeleton( wxTreeItemId idSkeleton
		, castor3d::Skeleton const & skeleton )
	{
		for ( auto const & node : skeleton.getNodes() )
		{
			if ( node->getType() == castor3d::SkeletonNodeType::eBone )
			{
				AppendItem( idSkeleton
					, node->getName()
					, eBMP_SKELETON
					, eBMP_SKELETON_SEL
					, new DataType{ ObjectType::eSkeletonBone, &static_cast< castor3d::BoneNode & >( *node ) } );
			}
			else
			{
				AppendItem( idSkeleton
					, node->getName()
					, eBMP_SKELETON
					, eBMP_SKELETON_SEL
					, new DataType{ ObjectType::eSkeletonNode, node.get() } );
			}
		}

		for ( auto & anim : skeleton.getAnimations() )
		{
			AppendItem( idSkeleton
				, anim.first
				, eBMP_ANIMATION
				, eBMP_ANIMATION_SEL
				, new DataType{ ObjectType::eSkeletonAnimation, &static_cast< castor3d::SkeletonAnimation & >( *anim.second ) } );
		}
	}

	void SceneObjectsTree::doAddCamera( wxTreeItemId id
		, castor3d::Camera & camera )
	{
		auto cameraId = AppendItem( id
			, camera.getName()
			, eBMP_CAMERA
			, eBMP_CAMERA_SEL
			, new DataType{ std::make_unique< CameraTreeItemProperty >( m_propertiesHolder->isEditable(), camera ) } );
		AppendItem( cameraId
			, _( "Viewport" )
			, eBMP_VIEWPORT
			, eBMP_VIEWPORT_SEL
			, new DataType{ std::make_unique< ViewportTreeItemProperty >( m_propertiesHolder->isEditable()
				, *camera.getScene()->getEngine()
				, camera.getViewport() ) } );
	}

	void SceneObjectsTree::doAddBillboard( wxTreeItemId id
		, castor3d::BillboardList & billboard )
	{
		AppendItem( id
			, billboard.getName()
			, eBMP_BILLBOARD
			, eBMP_BILLBOARD_SEL
			, new DataType{ ObjectType::eBillboards, &billboard } );
	}

	void SceneObjectsTree::doAddParticleSystem( wxTreeItemId id
		, castor3d::ParticleSystem & particleSystem )
	{
		AppendItem( id
			, particleSystem.getName()
			, eBMP_PARTICLE
			, eBMP_PARTICLE_SEL
			, new DataType{ ObjectType::eParticleSystem, &particleSystem } );
	}

	void SceneObjectsTree::doAddNode( wxTreeItemId id
		, castor3d::SceneNode & node )
	{
		for ( auto const & [name, child] : node.getChildren() )
		{
			doAddNode( AppendItem( id
					, name
					, eBMP_NODE
					, eBMP_NODE_SEL
					, new DataType{ ObjectType::eSceneNode, child } )
				, *child );
		}
	}

	void SceneObjectsTree::doAddAnimatedObjectGroup( wxTreeItemId id
		, castor3d::AnimatedObjectGroup & group )
	{
		for ( auto const & [name, anim] : group.getAnimations() )
		{
			AppendItem( id
				, name
				, eBMP_ANIMATION
				, eBMP_ANIMATION_SEL
				, new DataType{ group, anim } );
		}
	}

	void SceneObjectsTree::doAddOverlay( wxTreeItemId id
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
						, new DataType{ ObjectType::eOverlay, &overlay->getCategory() } )
					, overlay->getCategory() );
				break;
			case castor3d::OverlayType::eBorderPanel:
				doAddOverlay( AppendItem( id
						, overlay->getName()
						, eBMP_BORDER_PANEL_OVERLAY
						, eBMP_BORDER_PANEL_OVERLAY_SEL
						, new DataType{ ObjectType::eOverlay, &overlay->getCategory() } )
					, overlay->getCategory() );
				break;
			case castor3d::OverlayType::eText:
				doAddOverlay( AppendItem( id
						, overlay->getName()
						, eBMP_TEXT_OVERLAY
						, eBMP_TEXT_OVERLAY_SEL
						, new DataType{ ObjectType::eOverlay, &overlay->getCategory() } )
					, overlay->getCategory() );
				break;
			default:
				CU_Failure( "Unsupported OverlayType" );
				break;
			}
		}
	}

	void SceneObjectsTree::doAddStyles( wxTreeItemId parentId
		, castor3d::StylesHolder const & styles
		, castor3d::SceneRPtr scene )
	{
		for ( auto const & [name, style] : styles.getButtonStyles() )
		{
			doAddStyle( parentId, name, *style, scene );
		}

		for ( auto const & [name, style] : styles.getComboBoxStyles() )
		{
			doAddStyle( parentId, name, *style, scene );
		}

		for ( auto const & [name, style] : styles.getEditStyles() )
		{
			doAddStyle( parentId, name, *style, scene );
		}

		for ( auto const & [name, style] : styles.getExpandablePanelStyles() )
		{
			doAddStyle( parentId, name, *style, scene );
		}

		for ( auto const & [name, style] : styles.getFrameStyles() )
		{
			doAddStyle( parentId, name, *style, scene );
		}

		for ( auto const & [name, style] : styles.getListBoxStyles() )
		{
			doAddStyle( parentId, name, *style, scene );
		}

		for ( auto const & [name, style] : styles.getPanelStyles() )
		{
			doAddStyle( parentId, name, *style, scene );
		}

		for ( auto const & [name, style] : styles.getSliderStyles() )
		{
			doAddStyle( parentId, name, *style, scene );
		}

		for ( auto const & [name, style] : styles.getStaticStyles() )
		{
			doAddStyle( parentId, name, *style, scene );
		}
	}

	void SceneObjectsTree::doAddStyle( wxTreeItemId id
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
			, new DataType{ ObjectType::eStyle, &style } );

		if ( isStylesHolder( style ) )
		{
			castor3d::StylesHolder const * holder{};

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
			doAddStyle( parentId, cuT( "Header" ), expandable.getHeaderStyle(), scene );
			doAddStyle( parentId, cuT( "Expand" ), expandable.getExpandStyle(), scene );
			doAddStyle( parentId, cuT( "Content" ), expandable.getContentStyle(), scene );
		}
		else if ( style.getType() == castor3d::ControlType::eComboBox )
		{
			auto & combo = static_cast< castor3d::ComboBoxStyle & >( style );
			doAddStyle( parentId, cuT( "Expand" ), combo.getExpandStyle(), scene );
			doAddStyle( parentId, cuT( "Elements" ), combo.getElementsStyle(), scene );
		}
		else if ( style.getType() == castor3d::ControlType::eListBox )
		{
			auto & list = static_cast< castor3d::ListBoxStyle & >( style );
			doAddStyle( parentId, cuT( "Item" ), list.getItemStyle(), scene );
			doAddStyle( parentId, cuT( "Highlighted Item" ), list.getHighlightedItemStyle(), scene );
			doAddStyle( parentId, cuT( "Selected Item" ), list.getSelectedItemStyle(), scene );
		}
		else if ( style.getType() == castor3d::ControlType::eSlider )
		{
			auto & slider = static_cast< castor3d::SliderStyle & >( style );
			doAddStyle( parentId, cuT( "Line" ), slider.getLineStyle(), scene );
			doAddStyle( parentId, cuT( "Tick" ), slider.getTickStyle(), scene );
		}
	}

	void SceneObjectsTree::doAddControl( wxTreeItemId id
		, castor::String const & name
		, castor3d::Control & control
		, bool full
		, bool inLayout )
	{
		auto parentId = AppendItem( id
			, name
			, eBMP_CONTROL
			, eBMP_CONTROL_SEL
			, new DataType{ control, full, inLayout } );

		if ( isLayoutControl( control ) )
		{
			auto & layout = static_cast< castor3d::LayoutControl const & >( control );

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
			auto & expandable = static_cast< castor3d::ExpandablePanelCtrl const & >( control );
			doAddControl( parentId, cuT( "Header" ), *expandable.getHeader(), false, false );
			doAddControl( parentId, cuT( "Expand" ), *expandable.getExpand(), false, false );
			doAddControl( parentId, cuT( "Content" ), *expandable.getContent(), false, false );
		}
		else if ( control.getType() == castor3d::ControlType::eFrame )
		{
			auto & frame = static_cast< castor3d::FrameCtrl const & >( control );
			doAddControl( parentId, cuT( "Content" ), *frame.getContent(), false, false );
		}
	}

	void SceneObjectsTree::doAddMaterial( wxTreeItemId id
		, castor3d::MaterialObs material )
	{
		auto materialId = AppendItem( id
			, material->getName()
			, int( eBMP_MATERIAL )
			, int( eBMP_MATERIAL_SEL )
			, new DataType{ ObjectType::eMaterial, material } );
		uint32_t passIndex = 0;
		m_materials.try_emplace( material, materialId );

		for ( auto const & pass : *material )
		{
			++passIndex;
			doAddPass( materialId
				, passIndex
				, *pass );
		}
	}

	void SceneObjectsTree::doAddPass( wxTreeItemId id
		, uint32_t index
		, castor3d::Pass & pass )
	{
		auto passId = AppendItem( id
			, wxString( _( "Pass " ) ) << index
			, int( eBMP_PASS )
			, int( eBMP_PASS_SEL )
			, new DataType{ ObjectType::ePass, &pass } );
		uint32_t unitIndex = 0;

		for ( auto unit : pass )
		{
			++unitIndex;
			doAddTexture( passId
				, unitIndex
				, pass
				, *unit );
		}
	}

	void SceneObjectsTree::doAddTexture( wxTreeItemId id
		, uint32_t index
		, castor3d::Pass & pass
		, castor3d::TextureUnit & texture )
	{
		auto unitId = AppendItem( id
			, wxString( _( "Texture Unit " ) ) << index
			, int( eBMP_TEXTURE )
			, int( eBMP_TEXTURE_SEL )
			, new DataType{ pass, texture } );

		if ( texture.isRenderTarget() )
		{
			auto target = texture.getRenderTarget();
			appendRenderTarget( this
				, m_propertiesHolder->isEditable()
				, unitId
				, *target );
		}
	}

	void SceneObjectsTree::doLoadSceneLights( wxTreeItemId id
		, wxString const & name
		, castor3d::LightType type
		, int icon
		, int iconSel )
	{
		if ( auto lights = m_scene->getLightCache().getLights( type );
			!lights.empty() )
		{
			auto lightsId = AppendItem( id
				, name
				, icon
				, iconSel );

			for ( auto light : lights )
			{
				AppendItem( lightsId
					, light->getName()
					, icon
					, iconSel
					, new DataType{ ObjectType::eLight, light } );
			}
		}
	}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
	BEGIN_EVENT_TABLE( SceneObjectsTree, wxTreeCtrl )
		EVT_CLOSE( SceneObjectsTree::onClose )
		EVT_TREE_SEL_CHANGED( wxID_ANY, SceneObjectsTree::onSelectItem )
	END_EVENT_TABLE()
#pragma GCC diagnostic pop

	void SceneObjectsTree::onClose( wxCloseEvent & event )
	{
		DeleteAllItems();
		event.Skip();
	}

	void SceneObjectsTree::onSelectItem( wxTreeEvent & event )
	{
		if ( auto data = static_cast< DataType * >( GetItemData( event.GetItem() ) ) )
		{
			switch ( data->getType() )
			{
			case ObjectType::eTreeItemProp:
				m_propertiesHolder->setPropertyData( data->getProperties() );
				break;
			case ObjectType::eMaterial:
				m_materialProperties->setData( data->getObject< castor3d::Material >() );
				m_propertiesHolder->setPropertyData( m_materialProperties.get() );
				break;
			case ObjectType::ePass:
				m_passProperties->setData( data->getObject< castor3d::Pass >() );
				m_propertiesHolder->setPropertyData( m_passProperties.get() );
				break;
			case ObjectType::eSceneNode:
				m_nodeProperties->setData( data->getObject< castor3d::SceneNode >() );
				m_propertiesHolder->setPropertyData( m_nodeProperties.get() );
				break;
			case ObjectType::eLight:
				m_lightProperties->setData( data->getObject< castor3d::Light >() );
				m_propertiesHolder->setPropertyData( m_lightProperties.get() );
				break;
			case ObjectType::eOverlay:
				m_overlayProperties->setData( data->getObject< castor3d::OverlayCategory >() );
				m_propertiesHolder->setPropertyData( m_overlayProperties.get() );
				break;
			case ObjectType::eStyle:
				m_styleProperties->setData( data->getObject< castor3d::ControlStyle >() );
				m_propertiesHolder->setPropertyData( m_styleProperties.get() );
				break;
			case ObjectType::eGeometry:
				m_geometryProperties->setData( data->getObject< castor3d::Geometry >() );
				m_propertiesHolder->setPropertyData( m_geometryProperties.get() );
				break;
			case ObjectType::eBillboards:
				m_billboardsProperties->setData( data->getObject< castor3d::BillboardList >() );
				m_propertiesHolder->setPropertyData( m_billboardsProperties.get() );
				break;
			case ObjectType::eParticleSystem:
				m_particlesProperties->setData( data->getObject< castor3d::ParticleSystem >() );
				m_propertiesHolder->setPropertyData( m_particlesProperties.get() );
				break;
			case ObjectType::eSkeleton:
				m_skeletonProperties->setData( data->getObject< castor3d::Skeleton >() );
				m_propertiesHolder->setPropertyData( m_skeletonProperties.get() );
				break;
			case ObjectType::eSkeletonBone:
				m_skeletonBoneProperties->setData( data->getObject< castor3d::BoneNode >() );
				m_propertiesHolder->setPropertyData( m_skeletonBoneProperties.get() );
				break;
			case ObjectType::eSkeletonNode:
				m_skeletonNodeProperties->setData( data->getObject< castor3d::SkeletonNode >() );
				m_propertiesHolder->setPropertyData( m_skeletonNodeProperties.get() );
				break;
			case ObjectType::eSkeletonAnimation:
				m_skeletonAnimationProperties->setData( data->getObject< castor3d::SkeletonAnimation >() );
				m_propertiesHolder->setPropertyData( m_skeletonAnimationProperties.get() );
				break;
			case ObjectType::eAnimatedObjectGroup:
				m_animatedObjectGroupProperties->setData( data->getObject< castor3d::AnimatedObjectGroup >() );
				m_propertiesHolder->setPropertyData( m_animatedObjectGroupProperties.get() );
				break;
			case ObjectType::eTexture:
				m_textureProperties->setData( *std::get< 0 >( data->getPassTexture() )
					, *std::get< 1 >( data->getPassTexture() ) );
				m_propertiesHolder->setPropertyData( m_textureProperties.get() );
				break;
			case ObjectType::eControl:
				m_controlProperties->setData( *std::get< 0 >( data->getControlData() )
					, std::get< 1 >( data->getControlData() )
					, std::get< 2 >( data->getControlData() ) );
				m_propertiesHolder->setPropertyData( m_controlProperties.get() );
				break;
			case ObjectType::eAnimation:
				m_animationProperties->setData( *std::get< 0 >( data->getAnimationData() )
					, std::get< 1 >( data->getAnimationData() ) );
				m_propertiesHolder->setPropertyData( m_animationProperties.get() );
				break;
			case ObjectType::eSubmesh:
				m_submeshProperties->setData( *std::get< 0 >( data->getSubmeshData() )
					, *std::get< 1 >( data->getSubmeshData() ) );
				m_propertiesHolder->setPropertyData( m_submeshProperties.get() );
				break;
			}
		}
		else
		{
			m_propertiesHolder->setPropertyData( nullptr );
		}

		event.Skip();
	}
}
