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
	static constexpr uint32_t maxElemCount = 500u;

	SceneObjectsList::SceneObjectsList( PropertiesContainer * propertiesHolder
		, wxWindow * parent
		, wxPoint const & ptPos
		, wxSize const & size )
		: wxTreeCtrl( parent, wxID_ANY, ptPos, size, wxTR_DEFAULT_STYLE | wxNO_BORDER )
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
			auto & controlsManager = static_cast< castor3d::ControlsManager const & >( *scene->getEngine()->getUserInputListener() );

			auto rootId = AddRoot( make_wxString( window.getName() )
				, eBMP_RENDER_WINDOW
				, eBMP_RENDER_WINDOW_SEL
				, new RenderWindowTreeItemProperty( m_propertiesHolder->isEditable(), window ) );

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
			doLoadSceneLights( rootId
				, scene
				, _( "Directional Lights" )
				, castor3d::LightType::eDirectional
				, eBMP_DIRECTIONAL_LIGHT
				, eBMP_DIRECTIONAL_LIGHT_SEL
				, count );
			doLoadSceneLights( rootId
				, scene
				, _( "Point Lights" )
				, castor3d::LightType::ePoint
				, eBMP_POINT_LIGHT
				, eBMP_POINT_LIGHT_SEL
				, count );
			doLoadSceneLights( rootId
				, scene
				, _( "Spot Lights" )
				, castor3d::LightType::eSpot
				, eBMP_SPOT_LIGHT
				, eBMP_SPOT_LIGHT_SEL
				, count );
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
					if ( ++count <= maxElemCount )
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

	void SceneObjectsList::select( castor3d::MaterialRPtr material )
	{
		if ( auto itm = m_materials.find( material );
			itm != m_materials.end() )
		{
			SelectItem( itm->second );
		}
	}

	void SceneObjectsList::doAddSubmesh( wxTreeItemId id
		, castor3d::GeometryRPtr geometry
		, castor3d::Submesh const * submesh )
	{
		auto itg = m_objects.try_emplace( geometry ).first;
		itg->second.try_emplace( submesh, id );
	}

	void SceneObjectsList::doAddGeometry( wxTreeItemId id
		, castor3d::Geometry & geometry )
	{
		wxTreeItemId geometryId = AppendItem( id
			, geometry.getName()
			, eBMP_GEOMETRY
			, eBMP_GEOMETRY_SEL
			, new GeometryTreeItemProperty( m_propertiesHolder->isEditable(), geometry ) );

		if ( auto const & mesh = geometry.getMesh() )
		{
			int count = 0;

			for ( auto const & submesh : *mesh )
			{
				wxString name = _( "Submesh " );
				name << count++;
				wxTreeItemId idSubmesh = AppendItem( geometryId
					, name
					, eBMP_SUBMESH
					, eBMP_SUBMESH_SEL
					, new SubmeshTreeItemProperty( m_propertiesHolder->isEditable(), geometry, *submesh ) );
				doAddSubmesh( idSubmesh
					, &geometry
					, submesh.get() );
			}

			if ( auto skeleton = mesh->getSkeleton();
				skeleton )
			{
				wxTreeItemId idSkeleton = AppendItem( geometryId
					, mesh->getName()
					, eBMP_SKELETON
					, eBMP_SKELETON_SEL
					, new SkeletonTreeItemProperty( m_propertiesHolder->isEditable(), *skeleton ) );
				doAddSkeleton( idSkeleton , *skeleton );
			}
		}
	}

	void SceneObjectsList::doAddSkeleton( wxTreeItemId idSkeleton
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

	void SceneObjectsList::doAddNode( wxTreeItemId id
		, castor3d::SceneNode & node
		, uint32_t & count )
	{
		for ( auto const & [name, child] : node.getChildren() )
		{
			if ( ++count <= maxElemCount )
			{
				doAddNode( AppendItem( id
						, name
						, eBMP_NODE
						, eBMP_NODE_SEL
						, new NodeTreeItemProperty( m_propertiesHolder->isEditable(), m_engine, *child ) )
					, *child
					, count );
			}
		}
	}

	void SceneObjectsList::doAddAnimatedObjectGroup( wxTreeItemId id
		, castor3d::AnimatedObjectGroup & group )
	{
		for ( auto const & [name, anim] : group.getAnimations() )
		{
			AppendItem( id
				, name
				, eBMP_ANIMATION
				, eBMP_ANIMATION_SEL
				, new AnimationTreeItemProperty
				{
					m_engine,
					m_propertiesHolder->isEditable(),
					group,
					name,
					anim,
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
			doAddControl( parentId, "Header", *expandable.getHeader(), false, false );
			doAddControl( parentId, "Expand", *expandable.getExpand(), false, false );
			doAddControl( parentId, "Content", *expandable.getContent(), false, false );
		}
		else if ( control.getType() == castor3d::ControlType::eFrame )
		{
			auto & frame = static_cast< castor3d::FrameCtrl const & >( control );
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
		m_materials.try_emplace( material, materialId );

		for ( auto const & pass : *material )
		{
			++passIndex;
			doAddPass( materialId
				, passIndex
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
			++unitIndex;
			doAddTexture( passId
				, unitIndex
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

	void SceneObjectsList::doLoadSceneLights( wxTreeItemId parentId
		, castor3d::SceneRPtr scene
		, wxString const & name
		, castor3d::LightType type
		, int icon
		, int iconSel
		, uint32_t & count )
	{
		if ( auto lights = scene->getLightCache().getLights( type );
			!lights.empty() )
		{
			auto lightsId = AppendItem( parentId
				, name
				, icon
				, iconSel );

			for ( auto light : lights )
			{
				if ( ++count <= maxElemCount )
				{
					AppendItem( lightsId
						, light->getName()
						, -1
						, -1
						, new LightTreeItemProperty{ m_propertiesHolder->isEditable(), *light } );
				}
			}

			Collapse( lightsId );
		}
	}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
	BEGIN_EVENT_TABLE( SceneObjectsList, wxTreeCtrl )
		EVT_CLOSE( SceneObjectsList::onClose )
		EVT_TREE_SEL_CHANGED( wxID_ANY, SceneObjectsList::onSelectItem )
	END_EVENT_TABLE()
#pragma GCC diagnostic pop

	void SceneObjectsList::onClose( wxCloseEvent & event )
	{
		DeleteAllItems();
		event.Skip();
	}

	void SceneObjectsList::onSelectItem( wxTreeEvent & event )
	{
		auto data = static_cast< TreeItemProperty * >( event.GetClientObject() );
		m_propertiesHolder->setPropertyData( data );
		event.Skip();
	}
}
