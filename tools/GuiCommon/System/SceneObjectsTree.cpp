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
#include <Castor3D/Cache/GeometryCache.hpp>
#include <Castor3D/Cache/LightCache.hpp>
#include <Castor3D/Cache/LightGroupCache.hpp>
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
#include <Castor3D/Scene/Light/LightGroup.hpp>
#include <Castor3D/Scene/ParticleSystem/ParticleSystem.hpp>

#include <CastorUtils/Design/CacheView.hpp>
#include <CastorUtils/Design/ResourceCache.hpp>

namespace GuiCommon
{
	SceneObjectsTree::SceneObjectsTree( ImagesLoader & imagesLoader
		, PropertiesContainer * propertiesHolder
		, wxWindow * parent
		, wxPoint const & pos
		, wxSize const & size )
		: wxTreeCtrl{ parent, wxID_ANY, pos, size, wxTR_HAS_BUTTONS | wxTR_SINGLE | wxNO_BORDER }
		, m_imagesLoader{ imagesLoader }
		, m_propertiesHolder{ propertiesHolder }
		, m_images{ GC_IMG_SIZE, GC_IMG_SIZE, true }
	{
		for ( auto const & [id, image] : imagesLoader.getBitmaps() )
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

	void SceneObjectsTree::loadScene( c3d::Engine * engine
		, c3d::RenderWindow & window
		, c3d::SceneRPtr scene )
	{
		m_scene = scene;
		m_engine = engine;

		if ( scene )
		{
			auto rootId = AddRootT( make_wxString( window.getName() )
				, eBMP::eRenderWindow
				, eBMP::eRenderWindowSelected
				, new DataType{ std::make_unique< RenderWindowTreeItemProperty >( m_imagesLoader, m_propertiesHolder->isEditable(), window ) } );

			auto sceneId = AppendItemT( rootId
				, make_wxString( scene->getName() )
				, eBMP::eScene
				, eBMP::eSceneSelected
				, new DataType{ std::make_unique< SceneTreeItemProperty >( m_imagesLoader, m_propertiesHolder->isEditable(), *scene ) } );

			AppendItemT( sceneId
				, _( "Background" )
				, eBMP::eBackground
				, eBMP::eBackgroundSelected
				, new DataType{ std::make_unique< BackgroundTreeItemProperty >( m_imagesLoader, m_propertiesHolder->isEditable(), *scene->getBackground() ) } );

			auto catId = AppendItemT( sceneId
				, _( "Render Targets" )
				, eBMP::eRenderTarget
				, eBMP::eRenderTargetSelected );
			scene->getEngine()->getRenderTargetCache().forEach( [this, catId]( c3d::RenderTarget & elem )
				{
					appendRenderTarget( this
						, m_propertiesHolder->isEditable()
						, catId
						, elem );
				} );
			
			catId = AppendItemT( sceneId
				, _( "Cameras" )
				, eBMP::eCamera
				, eBMP::eCameraSelected );
			scene->getCameraCache().forEach( [this, catId]( c3d::Camera & elem )
				{
					doAddCamera( catId, elem );
				} );

			catId = AppendItemT( sceneId
				, _( "Animated Object Groups" )
				, eBMP::eAnimatedObjectGroup
				, eBMP::eAnimatedObjectGroupSelected );
			m_animatedObjectGroupProperties = std::make_unique< AnimatedObjectGroupTreeItemProperty >( m_imagesLoader, m_propertiesHolder->isEditable(), m_engine );
			m_animationProperties = std::make_unique< AnimationTreeItemProperty >( m_imagesLoader, m_propertiesHolder->isEditable(), m_engine );
			scene->getAnimatedObjectGroupCache().forEach( [this, catId]( c3d::AnimatedObjectGroup & elem )
				{
					if ( elem.isSerialisable() )
						doAddAnimatedObjectGroup( AppendItemT( catId
								, elem.getName()
								, eBMP::eAnimatedObjectGroup
								, eBMP::eAnimatedObjectGroupSelected
								, new DataType{ &elem } )
							, elem );
				} );

			CollapseAll();
			Expand( rootId );
			Expand( sceneId );
		}
	}

	void SceneObjectsTree::loadSceneMaterials( c3d::Engine * engine
		, c3d::SceneRPtr scene )
	{
		m_scene = scene;
		m_engine = engine;

		if ( scene )
		{
			auto rootId = AddRootT( _( "Materials" )
				, eBMP::eMaterial
				, eBMP::eMaterialSelected );
			m_materialProperties = std::make_unique< MaterialTreeItemProperty >( m_imagesLoader, m_propertiesHolder->isEditable(), m_engine );
			m_passProperties = std::make_unique< PassTreeItemProperty >( m_imagesLoader, m_propertiesHolder->isEditable(), *m_scene, this );
			m_textureProperties = std::make_unique< TextureTreeItemProperty >( m_imagesLoader, m_propertiesHolder->isEditable(), m_engine );

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

	void SceneObjectsTree::loadSceneOverlays( c3d::Engine * engine
		, c3d::SceneRPtr scene )
	{
		m_scene = scene;
		m_engine = engine;

		if ( scene )
		{
			m_overlayProperties = std::make_unique< OverlayTreeItemProperty >( m_imagesLoader, m_propertiesHolder->isEditable(), m_engine );
			auto rootId = AddRootT( _( "Overlays" )
				, eBMP::eBorderPanelOverlay
				, eBMP::eBorderPanelOverlaySelected );

			for ( auto overlay : scene->getOverlayCache().getCategories() )
			{
				switch ( overlay->getType() )
				{
				case c3d::OverlayType::ePanel:
					doAddOverlay( AppendItemT( rootId
							, overlay->getOverlayName()
							, eBMP::ePanelOverlay
							, eBMP::ePanelOverlaySelected
							, new DataType{ overlay } )
						, *overlay );
					break;
				case c3d::OverlayType::eBorderPanel:
					doAddOverlay( AppendItemT( rootId
							, overlay->getOverlayName()
							, eBMP::eBorderPanelOverlay
							, eBMP::eBorderPanelOverlaySelected
							, new DataType{ overlay } )
						, *overlay );
					break;
				case c3d::OverlayType::eText:
					doAddOverlay( AppendItemT( rootId
							, overlay->getOverlayName()
							, eBMP::eTextOverlay
							, eBMP::eTextOverlaySelected
							, new DataType{ overlay } )
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

	void SceneObjectsTree::loadSceneGui( c3d::Engine * engine
		, c3d::SceneRPtr scene )
	{
		m_scene = scene;
		m_engine = engine;

		if ( scene )
		{
			m_styleProperties = std::make_unique< StyleTreeItemProperty >( m_imagesLoader, m_propertiesHolder->isEditable(), m_engine );
			m_controlProperties = std::make_unique< ControlTreeItemProperty >( m_imagesLoader, m_propertiesHolder->isEditable(), m_engine );
			auto & controlsManager = static_cast< c3d::ControlsManager const & >( *scene->getEngine()->getUserInputListener() );
			auto rootId = AddRootT( _( "GUI" )
				, eBMP::eStyles
				, eBMP::eStylesSelected );
			auto catId = AppendItemT( rootId
				, _( "Global GUI Styles" )
				, eBMP::eStyles
				, eBMP::eStylesSelected );
			doAddStyles( catId, controlsManager, nullptr );

			for ( auto const & [name, theme] : controlsManager.getThemes() )
			{
				if ( name != "Debug" )
				{
					auto themeId = AppendItemT( catId
						, name
						, eBMP::eTheme
						, eBMP::eThemeSelected );
					doAddStyles( themeId, *theme, nullptr );
				}
			}

			catId = AppendItemT( rootId
				, _( "Global GUI Controls" )
				, eBMP::eControls
				, eBMP::eControlsSelected );

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

			catId = AppendItemT( rootId
				, _( "Scene GUI Styles" )
				, eBMP::eStyles
				, eBMP::eStylesSelected );
			doAddStyles( catId, controlsManager, scene );

			catId = AppendItemT( rootId
				, _( "Scene GUI Controls" )
				, eBMP::eControls
				, eBMP::eControlsSelected );

			for ( auto const & control : static_cast< c3d::ControlsManager const & >( *scene->getEngine()->getUserInputListener() ).getRootControls() )
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

	void SceneObjectsTree::loadSceneNodes( c3d::Engine * engine
		, c3d::SceneRPtr scene )
	{
		m_scene = scene;
		m_engine = engine;

		if ( scene )
		{
			m_nodeProperties = std::make_unique< NodeTreeItemProperty >( m_imagesLoader, m_propertiesHolder->isEditable(), m_engine );
			auto rootId = AddRootT( _( "Nodes" )
				, eBMP::eNode
				, eBMP::eNodeSelected );

			if ( auto rootNode = scene->getRootNode() )
			{
				doAddNode( rootId, *rootNode );
			}

			CollapseAll();
			Expand( rootId );
		}
	}

	void SceneObjectsTree::loadSceneLights( c3d::Engine * engine
		, c3d::SceneRPtr scene )
	{
		m_scene = scene;
		m_engine = engine;

		if ( scene )
		{
			m_nodeProperties = std::make_unique< NodeTreeItemProperty >( m_imagesLoader, m_propertiesHolder->isEditable(), m_engine );
			m_lightProperties = std::make_unique< LightTreeItemProperty >( m_imagesLoader, m_propertiesHolder->isEditable(), m_engine );
			m_lightGroupProperties = std::make_unique< LightGroupTreeItemProperty >( m_imagesLoader, m_propertiesHolder->isEditable(), m_engine );

			auto rootId = AddRootT( _( "Lights And Groups" )
				, eBMP::eDirectionalLight
				, eBMP::eDirectionalLightSelected );
			doLoadSceneLights( rootId
				, _( "Directional Lights" )
				, c3d::LightType::eDirectional
				, eBMP::eDirectionalLight
				, eBMP::eDirectionalLightSelected );
			doLoadSceneLights( rootId
				, _( "Point Lights" )
				, c3d::LightType::ePoint
				, eBMP::ePointLight
				, eBMP::ePointLightSelected );
			doLoadSceneLights( rootId
				, _( "Spot Lights" )
				, c3d::LightType::eSpot
				, eBMP::eSpotLight
				, eBMP::eSpotLightSelected );
			doLoadSceneLightGroups( rootId
				, _( "Directional LightGroups" )
				, c3d::LightType::eDirectional
				, eBMP::eDirectionalLight
				, eBMP::eDirectionalLightSelected );
			doLoadSceneLightGroups( rootId
				, _( "Point LightGroups" )
				, c3d::LightType::ePoint
				, eBMP::ePointLight
				, eBMP::ePointLightSelected );
			doLoadSceneLightGroups( rootId
				, _( "Spot LightGroups" )
				, c3d::LightType::eSpot
				, eBMP::eSpotLight
				, eBMP::eSpotLightSelected );
			CollapseAll();
			Expand( rootId );
		}
	}

	void SceneObjectsTree::loadSceneObjects( c3d::Engine * engine
		, c3d::SceneRPtr scene )
	{
		m_scene = scene;
		m_engine = engine;

		if ( scene )
		{
			m_geometryProperties = std::make_unique< GeometryTreeItemProperty >( m_imagesLoader, m_propertiesHolder->isEditable(), m_engine );
			m_billboardsProperties = std::make_unique< BillboardTreeItemProperty >( m_imagesLoader, m_propertiesHolder->isEditable(), m_engine );
			m_particlesProperties = std::make_unique< ParticleSystemTreeItemProperty >( m_imagesLoader, m_propertiesHolder->isEditable(), m_engine );
			m_submeshProperties = std::make_unique< SubmeshTreeItemProperty >( m_imagesLoader, m_propertiesHolder->isEditable(), m_engine );
			m_skeletonProperties = std::make_unique< SkeletonTreeItemProperty >( m_imagesLoader, m_propertiesHolder->isEditable(), m_engine );
			m_skeletonBoneProperties = std::make_unique< BoneTreeItemProperty >( m_imagesLoader, m_propertiesHolder->isEditable(), m_engine );
			m_skeletonNodeProperties = std::make_unique< SkeletonNodeTreeItemProperty >( m_imagesLoader, m_propertiesHolder->isEditable(), m_engine );
			m_skeletonAnimationProperties = std::make_unique< SkeletonAnimationTreeItemProperty >( m_imagesLoader, m_propertiesHolder->isEditable(), m_engine );

			auto rootId = AddRootT( _( "Objects" )
				, eBMP::eGeometry
				, eBMP::eGeometrySelected );

			auto geometriesId = AppendItemT( rootId
				, _( "Geometries" )
				, eBMP::eGeometry
				, eBMP::eGeometrySelected );
			scene->getGeometryCache().forEach( [this, geometriesId]( c3d::Geometry & elem )
				{
					doAddGeometry( geometriesId, elem );
				} );

			auto billboardsId = AppendItemT( rootId
				, _( "Billboards" )
				, eBMP::eBillboard
				, eBMP::eBillboardSelected );
			scene->getBillboardListCache().forEach( [this, billboardsId]( c3d::BillboardList & elem )
				{
					doAddBillboard( billboardsId, elem );
				} );

			auto particlesId = AppendItemT( rootId
				, _( "Particles" )
				, eBMP::eParticle
				, eBMP::eParticleSelected );
			scene->getParticleSystemCache().forEach( [this, particlesId]( c3d::ParticleSystem & elem )
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

	void SceneObjectsTree::select( c3d::Geometry const * geometry
		, c3d::Submesh const * submesh )
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

	void SceneObjectsTree::select( c3d::MaterialRPtr material )
	{
		if ( auto itm = m_materials.find( material );
			itm != m_materials.end() )
		{
			SelectItem( itm->second );
		}
	}

	void SceneObjectsTree::doAddSubmesh( wxTreeItemId id
		, c3d::Geometry const * geometry
		, c3d::Submesh const * submesh )
	{
		auto itg = m_objects.try_emplace( geometry ).first;
		itg->second.try_emplace( submesh, id );
	}

	void SceneObjectsTree::doAddGeometry( wxTreeItemId id
		, c3d::Geometry & geometry )
	{
		auto geometryId = AppendItemT( id
			, geometry.getName()
			, eBMP::eGeometry
			, eBMP::eGeometrySelected
			, new DataType{ &geometry } );

		if ( auto const & mesh = geometry.getMesh() )
		{
			int count = 0;

			for ( auto const & submesh : *mesh )
			{
				wxString name = _( "Submesh " );
				name << count;
				++count;
				auto idSubmesh = AppendItemT( geometryId
					, name
					, eBMP::eSubmesh
					, eBMP::eSubmeshSelected
					, new DataType{ geometry, *submesh } );
				doAddSubmesh( idSubmesh
					, &geometry
					, submesh.get() );
			}

			if ( auto skeleton = mesh->getSkeleton();
				skeleton )
			{
				auto idSkeleton = AppendItemT( geometryId
					, mesh->getName()
					, eBMP::eSkeleton
					, eBMP::eSkeletonSelected
					, new DataType{ skeleton } );
				doAddSkeleton( idSkeleton , *skeleton );
			}
		}
	}

	void SceneObjectsTree::doAddSkeleton( wxTreeItemId idSkeleton
		, c3d::Skeleton const & skeleton )
	{
		for ( auto const & node : skeleton.getNodes() )
		{
			if ( node->getType() == c3d::SkeletonNodeType::eBone )
			{
				AppendItemT( idSkeleton
					, node->getName()
					, eBMP::eSkeleton
					, eBMP::eSkeletonSelected
					, new DataType{ &static_cast< c3d::BoneNode & >( *node ) } );
			}
			else
			{
				AppendItemT( idSkeleton
					, node->getName()
					, eBMP::eSkeleton
					, eBMP::eSkeletonSelected
					, new DataType{ node.get() } );
			}
		}

		for ( auto const & [name, anim] : skeleton.getAnimations() )
		{
			AppendItemT( idSkeleton
				, name
				, eBMP::eAnimation
				, eBMP::eAnimationSelected
				, new DataType{ &static_cast< c3d::SkeletonAnimation & >( *anim ) } );
		}
	}

	void SceneObjectsTree::doAddCamera( wxTreeItemId id
		, c3d::Camera & camera )
	{
		auto cameraId = AppendItemT( id
			, camera.getName()
			, eBMP::eCamera
			, eBMP::eCameraSelected
			, new DataType{ std::make_unique< CameraTreeItemProperty >( m_imagesLoader, m_propertiesHolder->isEditable(), camera ) } );
		AppendItemT( cameraId
			, _( "Viewport" )
			, eBMP::eViewport
			, eBMP::eViewportSelected
			, new DataType{ std::make_unique< ViewportTreeItemProperty >( m_imagesLoader, m_propertiesHolder->isEditable()
				, *camera.getScene()->getEngine()
				, camera.getViewport() ) } );
	}

	void SceneObjectsTree::doAddBillboard( wxTreeItemId id
		, c3d::BillboardList & billboard )
	{
		AppendItemT( id
			, billboard.getName()
			, eBMP::eBillboard
			, eBMP::eBillboardSelected
			, new DataType{ &billboard } );
	}

	void SceneObjectsTree::doAddParticleSystem( wxTreeItemId id
		, c3d::ParticleSystem & particleSystem )
	{
		AppendItemT( id
			, particleSystem.getName()
			, eBMP::eParticle
			, eBMP::eParticleSelected
			, new DataType{ &particleSystem } );
	}

	void SceneObjectsTree::doAddNode( wxTreeItemId id
		, c3d::SceneNode const & node )
	{
		for ( auto const & [name, child] : node.getChildren() )
		{
			doAddNode( AppendItemT( id
					, name
					, eBMP::eNode
					, eBMP::eNodeSelected
					, new DataType{ child } )
				, *child );
		}
	}

	void SceneObjectsTree::doAddAnimatedObjectGroup( wxTreeItemId id
		, c3d::AnimatedObjectGroup & group )
	{
		for ( auto const & [name, anim] : group.getAnimations() )
		{
			AppendItemT( id
				, name
				, eBMP::eAnimation
				, eBMP::eAnimationSelected
				, new DataType{ group, anim } );
		}
	}

	void SceneObjectsTree::doAddOverlay( wxTreeItemId id
		, c3d::OverlayCategory & category )
	{
		for ( auto overlay : category.getOverlay() )
		{
			switch ( overlay->getType() )
			{
			case c3d::OverlayType::ePanel:
				doAddOverlay( AppendItemT( id
						, overlay->getName()
						, eBMP::ePanelOverlay
						, eBMP::ePanelOverlaySelected
						, new DataType{ &overlay->getCategory() } )
					, overlay->getCategory() );
				break;
			case c3d::OverlayType::eBorderPanel:
				doAddOverlay( AppendItemT( id
						, overlay->getName()
						, eBMP::eBorderPanelOverlay
						, eBMP::eBorderPanelOverlaySelected
						, new DataType{ &overlay->getCategory() } )
					, overlay->getCategory() );
				break;
			case c3d::OverlayType::eText:
				doAddOverlay( AppendItemT( id
						, overlay->getName()
						, eBMP::eTextOverlay
						, eBMP::eTextOverlaySelected
						, new DataType{ &overlay->getCategory() } )
					, overlay->getCategory() );
				break;
			default:
				CU_Failure( "Unsupported OverlayType" );
				break;
			}
		}
	}

	void SceneObjectsTree::doAddStyles( wxTreeItemId parentId
		, c3d::StylesHolder const & styles
		, c3d::SceneRPtr scene )
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
		, c3d::String const & name
		, c3d::ControlStyle & style
		, c3d::SceneRPtr scene )
	{
		if ( ( scene && !style.hasScene() )
			|| ( style.hasScene() && scene != &style.getScene() ) )
		{
			return;
		}

		auto parentId = AppendItemT( id
			, name
			, eBMP::eStyle
			, eBMP::eStyleSelected
			, new DataType{ &style } );

		if ( isStylesHolder( style ) )
		{
			c3d::StylesHolder const * holder{};

			if ( style.getType() == c3d::ControlType::ePanel )
			{
				holder = &static_cast< c3d::PanelStyle & >( style );
			}

			if ( holder )
			{
				doAddStyles( parentId, *holder, scene );
			}
		}
		else if ( style.getType() == c3d::ControlType::eExpandablePanel )
		{
			auto & expandable = static_cast< c3d::ExpandablePanelStyle & >( style );
			doAddStyle( parentId, cuT( "Header" ), expandable.getHeaderStyle(), scene );
			doAddStyle( parentId, cuT( "Expand" ), expandable.getExpandStyle(), scene );
			doAddStyle( parentId, cuT( "Content" ), expandable.getContentStyle(), scene );
		}
		else if ( style.getType() == c3d::ControlType::eComboBox )
		{
			auto & combo = static_cast< c3d::ComboBoxStyle & >( style );
			doAddStyle( parentId, cuT( "Expand" ), combo.getExpandStyle(), scene );
			doAddStyle( parentId, cuT( "Elements" ), combo.getElementsStyle(), scene );
		}
		else if ( style.getType() == c3d::ControlType::eListBox )
		{
			auto & list = static_cast< c3d::ListBoxStyle & >( style );
			doAddStyle( parentId, cuT( "Item" ), list.getItemStyle(), scene );
			doAddStyle( parentId, cuT( "Highlighted Item" ), list.getHighlightedItemStyle(), scene );
			doAddStyle( parentId, cuT( "Selected Item" ), list.getSelectedItemStyle(), scene );
		}
		else if ( style.getType() == c3d::ControlType::eSlider )
		{
			auto & slider = static_cast< c3d::SliderStyle & >( style );
			doAddStyle( parentId, cuT( "Line" ), slider.getLineStyle(), scene );
			doAddStyle( parentId, cuT( "Tick" ), slider.getTickStyle(), scene );
		}
	}

	void SceneObjectsTree::doAddControl( wxTreeItemId id
		, c3d::String const & name
		, c3d::Control & control
		, bool full
		, bool inLayout )
	{
		auto parentId = AppendItemT( id
			, name
			, eBMP::eControl
			, eBMP::eControlSelected
			, new DataType{ control, full, inLayout } );

		if ( isLayoutControl( control ) )
		{
			auto & layout = static_cast< c3d::LayoutControl const & >( control );

			for ( auto ctrl : layout.getChildren() )
			{
				if ( ctrl )
				{
					doAddControl( parentId, ctrl->getName(), *ctrl, true, layout.getLayout() != nullptr );
				}
			}
		}
		else if ( control.getType() == c3d::ControlType::eExpandablePanel )
		{
			auto & expandable = static_cast< c3d::ExpandablePanelCtrl const & >( control );
			doAddControl( parentId, cuT( "Header" ), *expandable.getHeader(), false, false );
			doAddControl( parentId, cuT( "Expand" ), *expandable.getExpand(), false, false );
			doAddControl( parentId, cuT( "Content" ), *expandable.getContent(), false, false );
		}
		else if ( control.getType() == c3d::ControlType::eFrame )
		{
			auto & frame = static_cast< c3d::FrameCtrl const & >( control );
			doAddControl( parentId, cuT( "Content" ), *frame.getContent(), false, false );
		}
	}

	void SceneObjectsTree::doAddMaterial( wxTreeItemId id
		, c3d::MaterialObs material )
	{
		auto materialId = AppendItemT( id
			, material->getName()
			, eBMP::eMaterial
			, eBMP::eMaterialSelected
			, new DataType{ material } );
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
		, c3d::Pass & pass )
	{
		auto passId = AppendItemT( id
			, wxString( _( "Pass " ) ) << index
			, eBMP::ePass
			, eBMP::ePassSelected
			, new DataType{ &pass } );
		uint32_t unitIndex = 0;

		for ( auto unit : pass )
		{
			if ( unit->getData().base->sourceInfo.isSerialisable() )
			{
				++unitIndex;
				doAddTexture( passId
					, unitIndex
					, pass
					, *unit );
			}
		}
	}

	void SceneObjectsTree::doAddTexture( wxTreeItemId id
		, uint32_t index
		, c3d::Pass & pass
		, c3d::TextureUnit & texture )
	{
		auto unitId = AppendItemT( id
			, wxString( _( "Texture Unit " ) ) << index
			, eBMP::eTexture
			, eBMP::eTextureSelected
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
		, c3d::LightType type
		, eBMP icon
		, eBMP iconSel )
	{
		if ( auto lights = m_scene->getLightCache().getLights( type );
			!lights.empty() )
		{
			auto lightsId = AppendItemT( id
				, name
				, icon
				, iconSel );

			for ( auto light : lights )
			{
				AppendItemT( lightsId
					, light->getName()
					, icon
					, iconSel
					, new DataType{ light } );
			}
		}
	}

	void SceneObjectsTree::doLoadSceneLightGroups( wxTreeItemId id
		, wxString const & name
		, c3d::LightType type
		, eBMP icon
		, eBMP iconSel )
	{
		if ( auto & lightGroups = m_scene->getLightGroupCache().getLightGroups( type );
			!lightGroups.empty() )
		{
			auto lightGroupsId = AppendItemT( id
				, name
				, icon
				, iconSel );

			for ( auto lightGroup : lightGroups )
			{
				auto groupId = AppendItemT( lightGroupsId
					, lightGroup->getName()
					, icon
					, iconSel
					, new DataType{ lightGroup } );

				for ( auto & instance : *lightGroup )
				{
					AppendItemT( groupId
						, instance->getNode().getName()
						, eBMP::eNode
						, eBMP::eNodeSelected
						, new DataType{ instance.get() } );
				}
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
		onSelectNode( nullptr );
		onSelectLight( nullptr );
		onSelectSubmesh( nullptr, nullptr );

		if ( auto data = static_cast< DataType * >( GetItemData( event.GetItem() ) ) )
		{
			switch ( data->getType() )
			{
			case ObjectType::TreeItemProperty:
				m_propertiesHolder->setPropertyData( data->getProperties() );
				break;
			case ObjectType::Material:
				m_materialProperties->setData( data->getObject< ObjectType::Material >() );
				m_propertiesHolder->setPropertyData( m_materialProperties.get() );
				break;
			case ObjectType::Pass:
				m_passProperties->setData( data->getObject< ObjectType::Pass >() );
				m_propertiesHolder->setPropertyData( m_passProperties.get() );
				break;
			case ObjectType::SceneNode:
				m_nodeProperties->setData( data->getObject< ObjectType::SceneNode >() );
				m_propertiesHolder->setPropertyData( m_nodeProperties.get() );
				onSelectNode( &data->getObject< ObjectType::SceneNode >() );
				break;
			case ObjectType::Light:
				m_lightProperties->setData( data->getObject< ObjectType::Light >() );
				m_propertiesHolder->setPropertyData( m_lightProperties.get() );
				onSelectLight( data->getObject< ObjectType::Light >().getInstance() );
				break;
			case ObjectType::LightGroup:
				m_lightGroupProperties->setData( data->getObject< ObjectType::LightGroup >() );
				m_propertiesHolder->setPropertyData( m_lightGroupProperties.get() );
				break;
			case ObjectType::LightInstance:
				m_nodeProperties->setData( data->getObject< ObjectType::LightInstance >().getNode() );
				m_propertiesHolder->setPropertyData( m_nodeProperties.get() );
				onSelectLight( &data->getObject< ObjectType::LightInstance >() );
				break;
			case ObjectType::OverlayCategory:
				m_overlayProperties->setData( data->getObject< ObjectType::OverlayCategory >() );
				m_propertiesHolder->setPropertyData( m_overlayProperties.get() );
				break;
			case ObjectType::ControlStyle:
				m_styleProperties->setData( data->getObject< ObjectType::ControlStyle >() );
				m_propertiesHolder->setPropertyData( m_styleProperties.get() );
				break;
			case ObjectType::Geometry:
				m_geometryProperties->setData( data->getObject< ObjectType::Geometry >() );
				m_propertiesHolder->setPropertyData( m_geometryProperties.get() );
				break;
			case ObjectType::BillboardList:
				m_billboardsProperties->setData( data->getObject< ObjectType::BillboardList >() );
				m_propertiesHolder->setPropertyData( m_billboardsProperties.get() );
				break;
			case ObjectType::ParticleSystem:
				m_particlesProperties->setData( data->getObject< ObjectType::ParticleSystem >() );
				m_propertiesHolder->setPropertyData( m_particlesProperties.get() );
				break;
			case ObjectType::Skeleton:
				m_skeletonProperties->setData( data->getObject< ObjectType::Skeleton >() );
				m_propertiesHolder->setPropertyData( m_skeletonProperties.get() );
				break;
			case ObjectType::BoneNode:
				m_skeletonBoneProperties->setData( data->getObject< ObjectType::BoneNode >() );
				m_propertiesHolder->setPropertyData( m_skeletonBoneProperties.get() );
				break;
			case ObjectType::SkeletonNode:
				m_skeletonNodeProperties->setData( data->getObject< ObjectType::SkeletonNode >() );
				m_propertiesHolder->setPropertyData( m_skeletonNodeProperties.get() );
				break;
			case ObjectType::SkeletonAnimation:
				m_skeletonAnimationProperties->setData( data->getObject< ObjectType::SkeletonAnimation >() );
				m_propertiesHolder->setPropertyData( m_skeletonAnimationProperties.get() );
				break;
			case ObjectType::AnimatedObjectGroup:
				m_animatedObjectGroupProperties->setData( data->getObject< ObjectType::AnimatedObjectGroup >() );
				m_propertiesHolder->setPropertyData( m_animatedObjectGroupProperties.get() );
				break;
			case ObjectType::Texture:
				m_textureProperties->setData( *std::get< 0 >( data->getPassTexture() )
					, *std::get< 1 >( data->getPassTexture() ) );
				m_propertiesHolder->setPropertyData( m_textureProperties.get() );
				break;
			case ObjectType::Control:
				m_controlProperties->setData( *std::get< 0 >( data->getControlData() )
					, std::get< 1 >( data->getControlData() )
					, std::get< 2 >( data->getControlData() ) );
				m_propertiesHolder->setPropertyData( m_controlProperties.get() );
				break;
			case ObjectType::Animation:
				m_animationProperties->setData( *std::get< 0 >( data->getAnimationData() )
					, std::get< 1 >( data->getAnimationData() ) );
				m_propertiesHolder->setPropertyData( m_animationProperties.get() );
				break;
			case ObjectType::Submesh:
				m_submeshProperties->setData( *std::get< 0 >( data->getSubmeshData() )
					, *std::get< 1 >( data->getSubmeshData() ) );
				m_propertiesHolder->setPropertyData( m_submeshProperties.get() );
				onSelectSubmesh( std::get< 0 >( data->getSubmeshData() )
						, std::get< 1 >( data->getSubmeshData() ) );
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
