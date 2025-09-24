/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_GEOMETRIES_LIST_FRAME_H___
#define ___GUICOMMON_GEOMETRIES_LIST_FRAME_H___

#include "GuiCommon/Properties/TreeItems/AnimatedObjectGroupTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/AnimationTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/BillboardTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/BoneTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/ControlTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/GeometryTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/LightTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/LightGroupTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/MaterialTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/NodeTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/OverlayTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/ParticleSystemTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/PassTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/SkeletonAnimationTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/SkeletonNodeTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/SkeletonTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/StyleTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/SubmeshTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/TextureTreeItemProperty.hpp"

#include <Castor3D/Gui/GuiModule.hpp>
#include <CastorUtils/Design/Signal.hpp>

#include <wx/imaglist.h>
#include <wx/panel.h>
#include <wx/treectrl.h>

namespace GuiCommon
{
	class SceneObjectsTree final
		: public wxTreeCtrl
	{
	public:
		using SelectLightFunc = std::function< void( c3d::LightInstance * ) >;
		using SelectSubmeshFunc = std::function< void( c3d::Geometry *, c3d::Submesh const * ) >;
		using SelectNodeFunc = std::function< void( c3d::SceneNode * ) >;
		using SelectLightSignal = c3d::SignalT< SelectLightFunc >;
		using SelectSubmeshSignal = c3d::SignalT< SelectSubmeshFunc >;
		using SelectNodeSignal = c3d::SignalT< SelectNodeFunc >;
		using SelectLightConnection = SelectLightSignal::connection;
		using SelectSubmeshConnection = SelectSubmeshSignal::connection;
		using SelectNodeConnection = SelectNodeSignal::connection;

		enum class ObjectType : uint8_t
		{
			TreeItemProperty,
			Material,
			Pass,
			Texture,
			SceneNode,
			Light,
			LightGroup,
			LightInstance,
			OverlayCategory,
			ControlStyle,
			Control,
			AnimatedObjectGroup,
			Animation,
			Geometry,
			BillboardList,
			ParticleSystem,
			Submesh,
			Skeleton,
			BoneNode,
			SkeletonNode,
			SkeletonAnimation,
		};

		class DataType
			: public wxTreeItemData
		{
			using PassTexture = std::tuple< c3d::Pass *, c3d::TextureUnit * >;
			using ControlData = std::tuple< c3d::Control *, bool, bool >;
			using AnimationData = std::tuple< c3d::AnimatedObjectGroup *, c3d::GroupAnimation >;
			using SubmeshData = std::tuple< c3d::Geometry *, c3d::Submesh * >;

			using Object = std::variant< std::unique_ptr< TreeItemProperty >
				, c3d::Material *
				, c3d::Pass *
				, std::unique_ptr< PassTexture >
				, c3d::SceneNode *
				, c3d::Light *
				, c3d::LightGroup *
				, c3d::LightInstance *
				, c3d::OverlayCategory *
				, c3d::ControlStyle *
				, c3d::Control *
				, c3d::AnimatedObjectGroup *
				, c3d::Animation *
				, c3d::Geometry *
				, c3d::BillboardList *
				, c3d::ParticleSystem *
				, c3d::Submesh *
				, c3d::Skeleton *
				, c3d::BoneNode *
				, c3d::SkeletonNode *
				, c3d::SkeletonAnimation * >;

		public:
			explicit DataType( std::unique_ptr< TreeItemProperty > prop )
				: m_type{ ObjectType::TreeItemProperty }
				, m_prop{ std::move( prop ) }
			{
			}

			explicit DataType( c3d::MaterialObs object )
				: m_type{ ObjectType::Material }
				, m_object{ object }
			{
			}

			explicit DataType( c3d::Pass * object )
				: m_type{ ObjectType::Pass }
				, m_object{ object }
			{
			}

			explicit DataType( c3d::Pass & pass
				, c3d::TextureUnit & texture )
				: m_type{ ObjectType::Texture }
				, m_passTexture{ std::make_unique< PassTexture >( &pass, &texture ) }
			{
			}

			explicit DataType( c3d::SceneNode * object )
				: m_type{ ObjectType::SceneNode }
				, m_object{ object }
			{
			}

			explicit DataType( c3d::Light * object )
				: m_type{ ObjectType::Light }
				, m_object{ object }
			{
			}

			explicit DataType( c3d::LightGroup * object )
				: m_type{ ObjectType::LightGroup }
				, m_object{ object }
			{
			}

			explicit DataType( c3d::LightInstance * object )
				: m_type{ ObjectType::LightInstance }
				, m_object{ object }
			{
			}

			explicit DataType( c3d::OverlayCategory * object )
				: m_type{ ObjectType::OverlayCategory }
				, m_object{ object }
			{
			}

			explicit DataType( c3d::ControlStyle * object )
				: m_type{ ObjectType::ControlStyle }
				, m_object{ object }
			{
			}

			explicit DataType( c3d::Control & control
				, bool full
				, bool inLayout )
				: m_type{ ObjectType::Control }
				, m_controlData{ std::make_unique< ControlData >( &control, full, inLayout ) }
			{
			}

			explicit DataType( c3d::AnimatedObjectGroup * object )
				: m_type{ ObjectType::AnimatedObjectGroup }
				, m_object{ object }
			{
			}

			explicit DataType( c3d::AnimatedObjectGroup & group
				, c3d::GroupAnimation anim )
				: m_type{ ObjectType::Animation }
				, m_animationlData{ std::make_unique< AnimationData >( &group, std::move( anim ) ) }
			{
			}

			explicit DataType( c3d::Geometry * object )
				: m_type{ ObjectType::Geometry }
				, m_object{ object }
			{
			}

			explicit DataType( c3d::BillboardList * object )
				: m_type{ ObjectType::BillboardList }
				, m_object{ object }
			{
			}

			explicit DataType( c3d::ParticleSystem * object )
				: m_type{ ObjectType::ParticleSystem }
				, m_object{ object }
			{
			}

			explicit DataType( c3d::Geometry & geometry
				, c3d::Submesh & submesh )
				: m_type{ ObjectType::Submesh }
				, m_submeshData{ std::make_unique< SubmeshData >( &geometry, &submesh ) }
			{
			}

			explicit DataType( c3d::Skeleton * object )
				: m_type{ ObjectType::Skeleton }
				, m_object{ object }
			{
			}

			explicit DataType( c3d::BoneNode * object )
				: m_type{ ObjectType::BoneNode }
				, m_object{ object }
			{
			}

			explicit DataType( c3d::SkeletonNode * object )
				: m_type{ ObjectType::SkeletonNode }
				, m_object{ object }
			{
			}

			explicit DataType( c3d::SkeletonAnimation * object )
				: m_type{ ObjectType::SkeletonAnimation }
				, m_object{ object }
			{
			}

			ObjectType getType()const noexcept
			{
				return m_type;
			}

			TreeItemProperty * getProperties()const noexcept
			{
				return m_prop.get();
			}

			PassTexture const & getPassTexture()const noexcept
			{
				return *m_passTexture;
			}

			ControlData const & getControlData()const noexcept
			{
				return *m_controlData;
			}

			AnimationData const & getAnimationData()const noexcept
			{
				return *m_animationlData;
			}

			SubmeshData const & getSubmeshData()const noexcept
			{
				return *m_submeshData;
			}

			template< ObjectType TypeT >
			auto & getObject()const noexcept
			{
				return *std::get< uint32_t( TypeT ) >( m_object );
			}

		private:
			ObjectType m_type;
			Object m_object{};
			std::unique_ptr< TreeItemProperty > m_prop;
			std::unique_ptr< PassTexture > m_passTexture;
			std::unique_ptr< ControlData > m_controlData;
			std::unique_ptr< AnimationData > m_animationlData;
			std::unique_ptr< SubmeshData > m_submeshData;
		};

	private:
		using SubmeshIdMap = c3d::Map< c3d::Submesh const *, wxTreeItemId >;
		using GeometrySubmeshIdMap = c3d::Map< c3d::Geometry const *, SubmeshIdMap >;
		using MaterialIdMap = c3d::Map< c3d::MaterialRPtr, wxTreeItemId >;

	public:
		SceneObjectsTree( ImagesLoader & imagesLoader
			, PropertiesContainer * propertiesHolder
			, wxWindow * parent
			, wxPoint const & ptPos = wxDefaultPosition
			, wxSize const & size = wxDefaultSize );

		void loadScene( c3d::Engine * engine
			, c3d::RenderWindow & window
			, c3d::SceneRPtr scene );
		void loadSceneMaterials( c3d::Engine * engine
			, c3d::SceneRPtr scene );
		void loadSceneOverlays( c3d::Engine * engine
			, c3d::SceneRPtr scene );
		void loadSceneGui( c3d::Engine * engine
			, c3d::SceneRPtr scene );
		void loadSceneNodes( c3d::Engine * engine
			, c3d::SceneRPtr scene );
		void loadSceneLights( c3d::Engine * engine
			, c3d::SceneRPtr scene );
		void loadSceneObjects( c3d::Engine * engine
			, c3d::SceneRPtr scene );
		void unloadScene();
		void select( c3d::Geometry const * geometry
			, c3d::Submesh const * submesh );
		void select( c3d::MaterialRPtr material );

		SelectLightSignal onSelectLight;
		SelectSubmeshSignal onSelectSubmesh;
		SelectNodeSignal onSelectNode;

		ImagesLoader & getImagesLoader()const noexcept
		{
			return m_imagesLoader;
		}


		template< typename IdT >
		wxTreeItemId AddRootT( const wxString & text,
			IdT image, IdT selectedImage,
			wxTreeItemData * data = NULL )
		{
			return AddRoot( text, int( image ), int( selectedImage ), data );
		}

		template< typename IdT >
		wxTreeItemId AppendItemT( const wxTreeItemId & parent,
			const wxString & text,
			IdT image, IdT selImage,
			wxTreeItemData * data = NULL )
		{
			return AppendItem( parent, text, int( image ), int( selImage ), data );
		}

	private:
		void doAddSubmesh( wxTreeItemId id
			, c3d::Geometry const * geometry
			, c3d::Submesh const * submesh );
		void doAddSkeleton( wxTreeItemId id
			, c3d::Skeleton const & skeleton );
		void doAddGeometry( wxTreeItemId id
			, c3d::Geometry & geometry );
		void doAddCamera( wxTreeItemId id
			, c3d::Camera & camera );
		void doAddBillboard( wxTreeItemId id
			, c3d::BillboardList & billboard );
		void doAddParticleSystem( wxTreeItemId id
			, c3d::ParticleSystem & particleSystem );
		void doAddNode( wxTreeItemId id
			, c3d::SceneNode const & node );
		void doAddAnimatedObjectGroup( wxTreeItemId id
			, c3d::AnimatedObjectGroup & group );
		void doAddOverlay( wxTreeItemId id
			, c3d::OverlayCategory & overlay );
		void doAddStyles( wxTreeItemId id
			, c3d::StylesHolder const & styles
			, c3d::SceneRPtr scene );
		void doAddStyle( wxTreeItemId id
			, c3d::String const & name
			, c3d::ControlStyle & style
			, c3d::SceneRPtr scene );
		void doAddControl( wxTreeItemId id
			, c3d::String const & name
			, c3d::Control & control
			, bool full
			, bool inLayout );
		void doAddMaterial( wxTreeItemId id
			, c3d::MaterialObs material );
		void doAddPass( wxTreeItemId id
			, uint32_t index
			, c3d::Pass & pass );
		void doAddTexture( wxTreeItemId id
			, uint32_t index
			, c3d::Pass & pass
			, c3d::TextureUnit & texture );
		void doLoadSceneLights( wxTreeItemId id
			, wxString const & name
			, c3d::LightType type
			, eBMP icon
			, eBMP iconSel );
		void doLoadSceneLightGroups( wxTreeItemId id
			, wxString const & name
			, c3d::LightType type
			, eBMP icon
			, eBMP iconSel );

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-override"
		DECLARE_EVENT_TABLE()
#pragma clang diagnostic pop
		void onClose( wxCloseEvent & event );
		void onSelectItem( wxTreeEvent & event );

	private:
		c3d::SceneRPtr m_scene{};
		c3d::Engine * m_engine{};
		ImagesLoader & m_imagesLoader;
		PropertiesContainer * m_propertiesHolder{};
		wxImageList m_images{};
		GeometrySubmeshIdMap m_objects{};
		MaterialIdMap m_materials{};
		std::unique_ptr< MaterialTreeItemProperty > m_materialProperties;
		std::unique_ptr< PassTreeItemProperty > m_passProperties;
		std::unique_ptr< TextureTreeItemProperty > m_textureProperties;
		std::unique_ptr< NodeTreeItemProperty > m_nodeProperties;
		std::unique_ptr< LightTreeItemProperty > m_lightProperties;
		std::unique_ptr< LightGroupTreeItemProperty > m_lightGroupProperties;
		std::unique_ptr< OverlayTreeItemProperty > m_overlayProperties;
		std::unique_ptr< StyleTreeItemProperty > m_styleProperties;
		std::unique_ptr< ControlTreeItemProperty > m_controlProperties;
		std::unique_ptr< AnimatedObjectGroupTreeItemProperty > m_animatedObjectGroupProperties;
		std::unique_ptr< AnimationTreeItemProperty > m_animationProperties;
		std::unique_ptr< GeometryTreeItemProperty > m_geometryProperties;
		std::unique_ptr< BillboardTreeItemProperty > m_billboardsProperties;
		std::unique_ptr< ParticleSystemTreeItemProperty > m_particlesProperties;
		std::unique_ptr< SubmeshTreeItemProperty > m_submeshProperties;
		std::unique_ptr< SkeletonTreeItemProperty > m_skeletonProperties;
		std::unique_ptr< BoneTreeItemProperty > m_skeletonBoneProperties;
		std::unique_ptr< SkeletonNodeTreeItemProperty > m_skeletonNodeProperties;
		std::unique_ptr< SkeletonAnimationTreeItemProperty > m_skeletonAnimationProperties;
	};
}

#endif
