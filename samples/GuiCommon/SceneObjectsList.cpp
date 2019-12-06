#include "GuiCommon/SceneObjectsList.hpp"

#include "GuiCommon/AnimatedObjectGroupTreeItemProperty.hpp"
#include "GuiCommon/AnimatedObjectTreeItemProperty.hpp"
#include "GuiCommon/AnimationTreeItemProperty.hpp"
#include "GuiCommon/BackgroundTreeItemProperty.hpp"
#include "GuiCommon/BillboardTreeItemProperty.hpp"
#include "GuiCommon/BoneTreeItemProperty.hpp"
#include "GuiCommon/CameraTreeItemProperty.hpp"
#include "GuiCommon/GeometryTreeItemProperty.hpp"
#include "GuiCommon/LightTreeItemProperty.hpp"
#include "GuiCommon/NodeTreeItemProperty.hpp"
#include "GuiCommon/OverlayTreeItemProperty.hpp"
#include "GuiCommon/PostEffectTreeItemProperty.hpp"
#include "GuiCommon/PropertiesContainer.hpp"
#include "GuiCommon/RenderTargetTreeItemProperty.hpp"
#include "GuiCommon/RenderWindowTreeItemProperty.hpp"
#include "GuiCommon/SceneTreeItemProperty.hpp"
#include "GuiCommon/SkeletonAnimationTreeItemProperty.hpp"
#include "GuiCommon/SkeletonTreeItemProperty.hpp"
#include "GuiCommon/SubmeshTreeItemProperty.hpp"
#include "GuiCommon/ToneMappingTreeItemProperty.hpp"
#include "GuiCommon/ViewportTreeItemProperty.hpp"

#include "GuiCommon/ImagesLoader.hpp"

#include <wx/imaglist.h>
#include <wx/aui/framemanager.h>
#include <wx/artprov.h>

#include <Castor3D/Engine.hpp>
#include <Castor3D/Animation/Animation.hpp>
#include <Castor3D/Animation/Skeleton/SkeletonAnimation.hpp>
#include <Castor3D/HDR/ToneMapping.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Mesh/Mesh.hpp>
#include <Castor3D/Mesh/Skeleton/Skeleton.hpp>
#include <Castor3D/Overlay/Overlay.hpp>
#include <Castor3D/Render/RenderWindow.hpp>
#include <Castor3D/Scene/BillboardList.hpp>
#include <Castor3D/Scene/Camera.hpp>
#include <Castor3D/Scene/Geometry.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/Animation/AnimatedObject.hpp>
#include <Castor3D/Scene/Animation/AnimatedObjectGroup.hpp>
#include <Castor3D/Scene/Light/Light.hpp>


using namespace castor3d;
using namespace castor;

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

			imageList->Add( wxImage( *image ) );
		}

		AssignImageList( imageList );
	}

	SceneObjectsList::~SceneObjectsList()
	{
	}

	void SceneObjectsList::loadScene( Engine * engine
		, SceneSPtr scene )
	{
		m_scene = scene;
		m_engine = engine;

		if ( scene )
		{
			wxTreeItemId sceneId = AddRoot( scene->getName()
				, eBMP_SCENE
				, eBMP_SCENE_SEL
				, new SceneTreeItemProperty( this, m_propertiesHolder->IsEditable(), *scene ) );

			AppendItem( sceneId
				, _( "Background" )
				, eBMP_BACKGROUND
				, eBMP_BACKGROUND_SEL
				, new BackgroundTreeItemProperty( this, m_propertiesHolder->IsEditable(), *scene->getBackground() ) );

			scene->getEngine()->getRenderWindowCache().lock();

			for ( auto it : scene->getEngine()->getRenderWindowCache() )
			{
				doAddRenderWindow( sceneId, it.second );
			}

			scene->getEngine()->getRenderWindowCache().unlock();
			SceneNodeSPtr rootNode = scene->getRootNode();

			if ( rootNode )
			{
				doAddNode( sceneId, rootNode );
			}

			scene->getAnimatedObjectGroupCache().lock();

			for ( auto it : scene->getAnimatedObjectGroupCache() )
			{
				doAddAnimatedObjectGroup( AppendItem( sceneId
						, it.first
						, eBMP_ANIMATED_OBJECTGROUP
						, eBMP_ANIMATED_OBJECTGROUP_SEL
						, new AnimatedObjectGroupTreeItemProperty( m_propertiesHolder->IsEditable(), it.second ) )
					, it.second );
			}

			scene->getAnimatedObjectGroupCache().unlock();

			for ( auto overlay : engine->getOverlayCache() )
			{
				if ( overlay->getOverlayName().find( cuT( "MainDebugPanel" ) ) != 0
					&& overlay->getOverlayName().find( cuT( "RenderPassOverlays" ) ) != 0
					&& overlay->getOverlayName().find( cuT( "DebugPanels" ) ) != 0
					&& overlay->getOverlayName().find( cuT( "DebugPanel" ) ) != 0 )
				{
					switch ( overlay->getType() )
					{
					case OverlayType::ePanel:
						doAddOverlay( AppendItem( sceneId
								, overlay->getOverlayName()
								, eBMP_PANEL_OVERLAY
								, eBMP_PANEL_OVERLAY_SEL
								, new OverlayTreeItemProperty( m_propertiesHolder->IsEditable(), overlay ) )
							, overlay );
						break;

					case OverlayType::eBorderPanel:
						doAddOverlay( AppendItem( sceneId
								, overlay->getOverlayName()
								, eBMP_BORDER_PANEL_OVERLAY
								, eBMP_BORDER_PANEL_OVERLAY_SEL
								, new OverlayTreeItemProperty( m_propertiesHolder->IsEditable(), overlay ) )
							, overlay );
						break;

					case OverlayType::eText:
						doAddOverlay( AppendItem( sceneId
								, overlay->getOverlayName()
								, eBMP_TEXT_OVERLAY
								, eBMP_TEXT_OVERLAY_SEL
								, new OverlayTreeItemProperty( m_propertiesHolder->IsEditable(), overlay ) )
							, overlay );
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
		m_scene.reset();
	}

	void SceneObjectsList::select( castor3d::GeometrySPtr geometry
		, castor3d::SubmeshSPtr submesh )
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

	void SceneObjectsList::doAddSubmesh( GeometrySPtr geometry
		, SubmeshSPtr submesh
		, wxTreeItemId id )
	{
		auto itg = m_ids.insert( { geometry, SubmeshIdMap{} } ).first;
		itg->second.insert( { submesh, id } );
	}

	void SceneObjectsList::doAddRenderWindow( wxTreeItemId id
		, RenderWindowSPtr window )
	{
		wxTreeItemId windowId = AppendItem( id
			, window->getName()
			, eBMP_RENDER_WINDOW
			, eBMP_RENDER_WINDOW_SEL
			, new RenderWindowTreeItemProperty( m_propertiesHolder->IsEditable(), window ) );
		RenderTargetSPtr target = window->getRenderTarget();

		if ( target )
		{
			wxString name = _( "Render Target" );
			auto targetId = AppendItem( windowId
				, name
				, eBMP_RENDER_TARGET
				, eBMP_RENDER_TARGET_SEL
				, new RenderTargetTreeItemProperty( m_propertiesHolder->IsEditable(), target ) );

			for ( auto postEffect : target->getHDRPostEffects() )
			{
				wxTreeItemId idPostEffect = AppendItem( targetId
					, _( "HDR - " ) + make_wxString( postEffect->getFullName() )
					, eBMP_POST_EFFECT
					, eBMP_POST_EFFECT_SEL
					, new PostEffectTreeItemProperty( m_propertiesHolder->IsEditable()
						, *postEffect
						, this ) );
			}

			for ( auto postEffect : target->getSRGBPostEffects() )
			{
				wxTreeItemId idPostEffect = AppendItem( targetId
					, _( "SRGB - " ) + make_wxString( postEffect->getFullName() )
					, eBMP_POST_EFFECT
					, eBMP_POST_EFFECT_SEL
					, new PostEffectTreeItemProperty( m_propertiesHolder->IsEditable()
						, *postEffect
						, this ) );
			}

			auto toneMapping = target->getToneMapping();

			if ( toneMapping )
			{
				wxTreeItemId idToneMapping = AppendItem( targetId
					, make_wxString( toneMapping->getFullName() )
					, eBMP_TONE_MAPPING
					, eBMP_TONE_MAPPING_SEL
					, new ToneMappingTreeItemProperty( m_propertiesHolder->IsEditable()
						, *toneMapping
						, this ) );
			}
		}
	}

	void SceneObjectsList::doAddGeometry( wxTreeItemId id
		, Geometry & geometry )
	{
		wxTreeItemId geometryId = AppendItem( id
			, geometry.getName()
			, eBMP_GEOMETRY
			, eBMP_GEOMETRY_SEL
			, new GeometryTreeItemProperty( m_propertiesHolder->IsEditable(), geometry ) );
		auto mesh = geometry.getMesh();

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
					, new SubmeshTreeItemProperty( m_propertiesHolder->IsEditable(), geometry, *submesh ) );
				doAddSubmesh( std::static_pointer_cast< Geometry >( geometry.shared_from_this() )
					, submesh
					, idSubmesh );
			}

			auto skeleton = mesh->getSkeleton();

			if ( skeleton )
			{
				wxTreeItemId idSkeleton = AppendItem( geometryId
					, mesh->getName()
					, eBMP_SKELETON
					, eBMP_SKELETON_SEL
					, new SkeletonTreeItemProperty( m_propertiesHolder->IsEditable(), *skeleton ) );
				doAddSkeleton( *skeleton, idSkeleton );
			}
		}
	}

	void SceneObjectsList::doAddSkeleton( castor3d::Skeleton const & skeleton
		, wxTreeItemId idSkeleton )
	{
		for ( auto bone : skeleton )
		{
			wxTreeItemId idBone = AppendItem( idSkeleton
				, bone->getName()
				, eBMP_SKELETON
				, eBMP_SKELETON_SEL
				, new BoneTreeItemProperty( m_propertiesHolder->IsEditable(), *bone ) );
		}

		for ( auto & anim : skeleton.getAnimations() )
		{
			wxTreeItemId idAnimation = AppendItem( idSkeleton
				, anim.first
				, eBMP_ANIMATION
				, eBMP_ANIMATION_SEL
				, new SkeletonAnimationTreeItemProperty( m_propertiesHolder->IsEditable()
					, static_cast< SkeletonAnimation & >( *anim.second ) ) );
		}
	}

	void SceneObjectsList::doAddCamera( wxTreeItemId id
		, Camera & camera )
	{
		wxTreeItemId cameraId = AppendItem( id
			, camera.getName()
			, eBMP_CAMERA
			, eBMP_CAMERA_SEL
			, new CameraTreeItemProperty( m_propertiesHolder->IsEditable(), camera ) );
		AppendItem( cameraId
			, _( "Viewport" )
			, eBMP_VIEWPORT
			, eBMP_VIEWPORT_SEL
			, new ViewportTreeItemProperty( m_propertiesHolder->IsEditable()
				, *camera.getScene()->getEngine()
				, camera.getViewport() ) );
	}

	void SceneObjectsList::doAddBillboard( wxTreeItemId id
		, BillboardList & billboard )
	{
		wxTreeItemId billboardId = AppendItem( id
			, billboard.getName()
			, eBMP_BILLBOARD
			, eBMP_BILLBOARD_SEL
			, new BillboardTreeItemProperty( m_propertiesHolder->IsEditable(), billboard ) );
	}

	void SceneObjectsList::doAddLight( wxTreeItemId id
		, Light & light )
	{
		switch ( light.getLightType() )
		{
		case LightType::eDirectional:
			AppendItem( id
				, light.getName()
				, eBMP_DIRECTIONAL_LIGHT
				, eBMP_DIRECTIONAL_LIGHT_SEL
				, new LightTreeItemProperty( m_propertiesHolder->IsEditable(), light ) );
			break;

		case LightType::ePoint:
			AppendItem( id
				, light.getName()
				, eBMP_POINT_LIGHT
				, eBMP_POINT_LIGHT_SEL
				, new LightTreeItemProperty( m_propertiesHolder->IsEditable(), light ) );
			break;

		case LightType::eSpot:
			AppendItem( id
				, light.getName()
				, eBMP_SPOT_LIGHT
				, eBMP_SPOT_LIGHT_SEL
				, new LightTreeItemProperty( m_propertiesHolder->IsEditable(), light ) );
			break;
		}
	}

	void SceneObjectsList::doAddNode( wxTreeItemId id
		, SceneNodeSPtr node )
	{
		for ( auto & object : node->getObjects() )
		{
			switch ( object.get().getType() )
			{
			case MovableType::eGeometry:
				doAddGeometry( id, static_cast< Geometry & >( object.get() ) );
				break;

			case MovableType::eCamera:
				doAddCamera( id, static_cast< Camera & >( object.get() ) );
				break;

			case MovableType::eLight:
				doAddLight( id, static_cast< Light & >( object.get() ) );
				break;

			case MovableType::eBillboard:
				doAddBillboard( id, static_cast< BillboardList & >( object.get() ) );
				break;
			}
		}

		for ( auto pair : node->getChildren() )
		{
			doAddNode( AppendItem( id
					, pair.first
					, eBMP_NODE
					, eBMP_NODE_SEL
					, new NodeTreeItemProperty( m_propertiesHolder->IsEditable(), m_engine, pair.second.lock() ) )
				, pair.second.lock() );
		}
	}

	void SceneObjectsList::doAddAnimatedObjectGroup( wxTreeItemId id
		, castor3d::AnimatedObjectGroupSPtr group )
	{
		for ( auto it : group->getAnimations() )
		{
			AppendItem( id
				, it.first
				, eBMP_ANIMATION
				, eBMP_ANIMATION_SEL
				, new AnimationTreeItemProperty( m_engine
					, m_propertiesHolder->IsEditable()
					, group
					, it.first
					, it.second ) );
		}
	}

	void SceneObjectsList::doAddOverlay( wxTreeItemId id
		, castor3d::OverlayCategorySPtr overlay )
	{
		for ( auto overlay : overlay->getOverlay() )
		{
			switch ( overlay->getType() )
			{
			case OverlayType::ePanel:
				doAddOverlay( AppendItem( id
						, overlay->getName()
						, eBMP_PANEL_OVERLAY
						, eBMP_PANEL_OVERLAY_SEL
						, new OverlayTreeItemProperty( m_propertiesHolder->IsEditable(), overlay->getCategory() ) )
					, overlay->getCategory() );
				break;

			case OverlayType::eBorderPanel:
				doAddOverlay( AppendItem( id
						, overlay->getName()
						, eBMP_BORDER_PANEL_OVERLAY
						, eBMP_BORDER_PANEL_OVERLAY_SEL
						, new OverlayTreeItemProperty( m_propertiesHolder->IsEditable(), overlay->getCategory() ) )
					, overlay->getCategory() );
				break;

			case OverlayType::eText:
				doAddOverlay( AppendItem( id
						, overlay->getName()
						, eBMP_TEXT_OVERLAY
						, eBMP_TEXT_OVERLAY_SEL
						, new OverlayTreeItemProperty( m_propertiesHolder->IsEditable(), overlay->getCategory() ) )
					, overlay->getCategory() );
				break;
			}
		}
	}

	BEGIN_EVENT_TABLE( SceneObjectsList, wxTreeCtrl )
		EVT_CLOSE( SceneObjectsList::onClose )
		EVT_TREE_SEL_CHANGED( wxID_ANY, SceneObjectsList::onSelectItem )
		EVT_TREE_ITEM_RIGHT_CLICK( wxID_ANY, SceneObjectsList::onMouseRButtonUp )
	END_EVENT_TABLE()

	void SceneObjectsList::onClose( wxCloseEvent & p_event )
	{
		DeleteAllItems();
		p_event.Skip();
	}

	void SceneObjectsList::onSelectItem( wxTreeEvent & p_event )
	{
		TreeItemProperty * data = reinterpret_cast< TreeItemProperty * >( p_event.GetClientObject() );
		m_propertiesHolder->setPropertyData( data );
		p_event.Skip();
	}

	void SceneObjectsList::onMouseRButtonUp( wxTreeEvent & p_event )
	{
	}
}
