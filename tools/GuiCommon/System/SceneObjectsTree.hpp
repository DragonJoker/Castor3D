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

#include <wx/imaglist.h>
#include <wx/panel.h>
#include <wx/treectrl.h>

namespace GuiCommon
{
	class SceneObjectsTree final
		: public wxTreeCtrl
	{
	private:
		using SubmeshIdMap = castor::Map< castor3d::Submesh const *, wxTreeItemId >;
		using GeometrySubmeshIdMap = castor::Map< castor3d::GeometryRPtr, SubmeshIdMap >;
		using MaterialIdMap = castor::Map< castor3d::MaterialRPtr, wxTreeItemId >;

		enum class ObjectType : uint8_t
		{
			eTreeItemProp,
			eMaterial,
			ePass,
			eTexture,
			eSceneNode,
			eLight,
			eOverlay,
			eStyle,
			eControl,
			eAnimatedObjectGroup,
			eAnimation,
			eGeometry,
			eBillboards,
			eParticleSystem,
			eSubmesh,
			eSkeleton,
			eSkeletonBone,
			eSkeletonNode,
			eSkeletonAnimation,
		};

		class DataType
			: public wxTreeItemData
		{
			using PassTexture = std::tuple< castor3d::Pass *, castor3d::TextureUnit * >;
			using ControlData = std::tuple< castor3d::Control *, bool, bool >;
			using AnimationData = std::tuple< castor3d::AnimatedObjectGroup *, castor3d::GroupAnimation >;
			using SubmeshData = std::tuple< castor3d::Geometry *, castor3d::Submesh * >;

		public:
			explicit DataType( ObjectType type
				, void * object )
				: m_type{ type }
				, m_object{ object }
			{
			}

			explicit DataType( std::unique_ptr< TreeItemProperty > prop )
				: m_type{ ObjectType::eTreeItemProp }
				, m_prop{ std::move( prop ) }
			{
			}

			explicit DataType( castor3d::Pass & pass
				, castor3d::TextureUnit & texture )
				: m_type{ ObjectType::eTexture }
				, m_passTexture{ std::make_unique< PassTexture >( &pass, &texture ) }
			{
			}

			explicit DataType( castor3d::Control & control
				, bool full
				, bool inLayout )
				: m_type{ ObjectType::eControl }
				, m_controlData{ std::make_unique< ControlData >( &control, full, inLayout ) }
			{
			}

			explicit DataType( castor3d::AnimatedObjectGroup & group
				, castor3d::GroupAnimation anim )
				: m_type{ ObjectType::eAnimation }
				, m_animationlData{ std::make_unique< AnimationData >( &group, std::move( anim ) ) }
			{
			}

			explicit DataType( castor3d::Geometry & geometry
				, castor3d::Submesh & submesh )
				: m_type{ ObjectType::eSubmesh }
				, m_submeshData{ std::make_unique< SubmeshData >( &geometry, &submesh ) }
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

			template< typename ObjectT >
			ObjectT & getObject()const noexcept
			{
				return *static_cast< ObjectT * >( m_object );
			}

		private:
			ObjectType m_type;
			void * m_object{};
			std::unique_ptr< TreeItemProperty > m_prop;
			std::unique_ptr< PassTexture > m_passTexture;
			std::unique_ptr< ControlData > m_controlData;
			std::unique_ptr< AnimationData > m_animationlData;
			std::unique_ptr< SubmeshData > m_submeshData;
		};

	public:
		SceneObjectsTree( PropertiesContainer * propertiesHolder
			, wxWindow * parent
			, wxPoint const & ptPos = wxDefaultPosition
			, wxSize const & size = wxDefaultSize );

		void loadScene( castor3d::Engine * engine
			, castor3d::RenderWindow & window
			, castor3d::SceneRPtr scene );
		void loadSceneMaterials( castor3d::Engine * engine
			, castor3d::SceneRPtr scene );
		void loadSceneOverlays( castor3d::Engine * engine
			, castor3d::SceneRPtr scene );
		void loadSceneGui( castor3d::Engine * engine
			, castor3d::SceneRPtr scene );
		void loadSceneNodes( castor3d::Engine * engine
			, castor3d::SceneRPtr scene );
		void loadSceneLights( castor3d::Engine * engine
			, castor3d::SceneRPtr scene );
		void loadSceneObjects( castor3d::Engine * engine
			, castor3d::SceneRPtr scene );
		void unloadScene();
		void select( castor3d::GeometryRPtr geometry
			, castor3d::Submesh const * submesh );
		void select( castor3d::MaterialRPtr material );

	protected:
		void doAddSubmesh( wxTreeItemId id
			, castor3d::GeometryRPtr geometry
			, castor3d::Submesh const * submesh );
		void doAddSkeleton( wxTreeItemId id
			, castor3d::Skeleton const & skeleton );
		void doAddGeometry( wxTreeItemId id
			, castor3d::Geometry & geometry );
		void doAddCamera( wxTreeItemId id
			, castor3d::Camera & camera );
		void doAddBillboard( wxTreeItemId id
			, castor3d::BillboardList & billboard );
		void doAddParticleSystem( wxTreeItemId id
			, castor3d::ParticleSystem & particleSystem );
		void doAddNode( wxTreeItemId id
			, castor3d::SceneNode & node );
		void doAddAnimatedObjectGroup( wxTreeItemId id
			, castor3d::AnimatedObjectGroup & group );
		void doAddOverlay( wxTreeItemId id
			, castor3d::OverlayCategory & overlay );
		void doAddStyles( wxTreeItemId id
			, castor3d::StylesHolder const & styles
			, castor3d::SceneRPtr scene );
		void doAddStyle( wxTreeItemId id
			, castor::String const & name
			, castor3d::ControlStyle & style
			, castor3d::SceneRPtr scene );
		void doAddControl( wxTreeItemId id
			, castor::String const & name
			, castor3d::Control & control
			, bool full
			, bool inLayout );
		void doAddMaterial( wxTreeItemId id
			, castor3d::MaterialObs material );
		void doAddPass( wxTreeItemId id
			, uint32_t index
			, castor3d::Pass & pass );
		void doAddTexture( wxTreeItemId id
			, uint32_t index
			, castor3d::Pass & pass
			, castor3d::TextureUnit & texture );
		void doLoadSceneLights( wxTreeItemId id
			, wxString const & name
			, castor3d::LightType type
			, int icon
			, int iconSel );

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-override"
		DECLARE_EVENT_TABLE()
#pragma clang diagnostic pop
		void onClose( wxCloseEvent & event );
		void onSelectItem( wxTreeEvent & event );

	private:
		castor3d::SceneRPtr m_scene{};
		castor3d::Engine * m_engine{};
		PropertiesContainer * m_propertiesHolder{};
		wxImageList m_images{};
		GeometrySubmeshIdMap m_objects{};
		MaterialIdMap m_materials{};
		std::unique_ptr< MaterialTreeItemProperty > m_materialProperties;
		std::unique_ptr< PassTreeItemProperty > m_passProperties;
		std::unique_ptr< TextureTreeItemProperty > m_textureProperties;
		std::unique_ptr< NodeTreeItemProperty > m_nodeProperties;
		std::unique_ptr< LightTreeItemProperty > m_lightProperties;
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
