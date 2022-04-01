/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SCENE_H___
#define ___C3D_SCENE_H___

#include "Castor3D/Overlay/OverlayModule.hpp"
#include "Castor3D/Render/GlobalIllumination/GlobalIlluminationModule.hpp"
#include "Castor3D/Render/EnvironmentMap/EnvironmentMapModule.hpp"
#include "Castor3D/Model/Skeleton/SkeletonModule.hpp"
#include "Castor3D/Render/Node/RenderNodeModule.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapModule.hpp"
#include "Castor3D/Render/GlobalIllumination/VoxelConeTracing/VctConfig.hpp"
#include "Castor3D/Scene/SceneModule.hpp"
#include "Castor3D/Scene/Animation/AnimationModule.hpp"
#include "Castor3D/Scene/Background/BackgroundModule.hpp"
#include "Castor3D/Scene/Light/LightModule.hpp"
#include "Castor3D/Scene/ParticleSystem/ParticleModule.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include "Castor3D/Cache/AnimatedObjectGroupCache.hpp"
#include "Castor3D/Cache/BillboardCache.hpp"
#include "Castor3D/Cache/CacheView.hpp"
#include "Castor3D/Cache/GeometryCache.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Cache/ObjectCache.hpp"
#include "Castor3D/Cache/OverlayCache.hpp"
#include "Castor3D/Cache/SceneNodeCache.hpp"
#include "Castor3D/Cache/TargetCache.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Scene/Fog.hpp"
#include "Castor3D/Scene/Shadow.hpp"

#include <CastorUtils/Data/TextWriter.hpp>
#include <CastorUtils/Design/Named.hpp>
#include <CastorUtils/Design/Signal.hpp>
#include <CastorUtils/FileParser/FileParserModule.hpp>
#include <CastorUtils/Graphics/FontCache.hpp>
#include <CastorUtils/Graphics/Font.hpp>
#include <CastorUtils/Graphics/ImageCache.hpp>
#include <CastorUtils/Graphics/RgbColour.hpp>
#include <CastorUtils/Log/Logger.hpp>
#include <CastorUtils/Multithreading/ThreadPool.hpp>

#include <RenderGraph/FramePassTimer.hpp>
#include <RenderGraph/ResourceHandler.hpp>

#include <CastorUtils/Config/BeginExternHeaderGuard.hpp>
#include <atomic>
#include <CastorUtils/Config/EndExternHeaderGuard.hpp>

namespace castor3d
{
	C3D_API castor::String print( castor::Point3f const & obj );
	C3D_API castor::String print( castor::BoundingBox const & obj );

	class Scene
		: public castor::OwnedBy< Engine >
		, public castor::Named
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	name	The scene name
		 *\param[in]	engine	The core engine
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	name	Le nom de la scène
		 *\param[in]	engine	Le moteur
		 */
		C3D_API Scene( castor::String const & name, Engine & engine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~Scene()noexcept;
		/**
		 *\~english
		 *\brief		Initialises the scene
		 *\~french
		 *\brief		Initialise la scène
		 */
		C3D_API void initialise();
		/**
		 *\~english
		 *\brief		Computes the scene AABB.
		 *\~french
		 *\brief		Calcule l'AABB de la scène.
		 */
		C3D_API void updateBoundingBox();
		/**
		 *\~english
		 *\brief		Clears the maps, leaves the root nodes
		 *\~french
		 *\brief		Vide les maps, laisse les noeuds pères
		 */
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief		Updates the scene before render.
		 *\~french
		 *\brief		Met à jour la scène avant le rendu.
		 */
		C3D_API void update( CpuUpdater & updater );
		/**
		 *\~english
		 *\brief		Uploads the scene objects data.
		 *\~french
		 *\brief		Upload les données des objets de la scène.
		 */
		C3D_API void upload( UploadData & uploader );
		/**
		 *\~english
		 *\brief		Updates the scene device dependant stuff.
		 *\~french
		 *\brief		Met à jour les objets de scène dépendant du device.
		 */
		C3D_API void update( GpuUpdater & updater );
		/**
		 *\~english
		 *\brief		Sets the background for the scene.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit le fond de la scène.
		 *\param[in]	value	La nouvelle valeur.
		 */
		C3D_API void setBackground( SceneBackgroundUPtr value );
		/**
		 *\~english
		 *\brief		Merges the content of the given scene to this scene
		 *\param[in]	scene	The scene to merge into this one
		 *\~french
		 *\brief		Intègre à cette scène le contenu de celle donnée
		 *\param[in]	scene	La scène à intégrer
		 */
		C3D_API void merge( Scene & scene );
		/**
		 *\~english
		 *\brief		Retrieves the vertices count
		 *\return		The value
		 *\~french
		 *\brief		Récupère le nombre de sommets
		 *\return		La valeur
		 */
		C3D_API uint32_t getVertexCount()const;
		/**
		 *\~english
		 *\brief		Retrieves the faces count
		 *\return		The value
		 *\~french
		 *\brief		Récupère le nombre de faces
		 *\return		La valeur
		 */
		C3D_API uint32_t getFaceCount()const;
		/**
		 *\~english
		 *\return		The scene flags.
		 *\~french
		 *\return		Les indicateurs de la scène.
		 */
		C3D_API SceneFlags getFlags()const;
		/**
		 *\~english
		 *\return		Tells if the scene has a shadow projecting light.
		 *\~french
		 *\return		Dit si la scène a au moins une source lumineuse projetant des ombres.
		 */
		C3D_API bool hasShadows()const;
		/**
		 *\~english
		 *\return		Tells if the scene has a shadow projecting light of given type.
		 *\~french
		 *\return		Dit si la scène a au moins une source lumineuse du type donné projetant des ombres.
		 */
		C3D_API bool hasShadows( LightType lightType )const;
		/**
		 *\~english
		 *\brief		Creates a reflection map for given node.
		 *\param[in]	node	The scene node from which the reflection map is generated.
		 *\~french
		 *\brief		Crée une reflection map pour le noeud donné.
		 *\param[in]	node	Le noeud de scène depuis lequel la reflection map est générée.
		 */
		C3D_API void addEnvironmentMap( SceneNode & node );
		/**
		 *\~english
		 *\brief		Creates a reflection map for given node.
		 *\param[in]	node	The scene node from which the reflection map is generated.
		 *\~french
		 *\brief		Crée une reflection map pour le noeud donné.
		 *\param[in]	node	Le noeud de scène depuis lequel la reflection map est générée.
		 */
		C3D_API void removeEnvironmentMap( SceneNode & node );
		/**
		 *\~english
		 *\return		Tells if there is a reflection map for given node.
		 *\param[in]	node	The scene node.
		 *\~french
		 *\return		Dit s'il y a une reflection map pour le noeud donné.
		 *\param[in]	node	Le noeud de scène.
		 */
		C3D_API bool hasEnvironmentMap( SceneNode & node )const;
		/**
		 *\~english
		 *\return		Retrieves the reflection map.
		 *\remarks		Call hasEnvironmentMap before calling this function (since this one returns a reference to an existing EnvironmentMap).
		 *\~french
		 *\return		Récupère la reflection map.
		 *\remarks		Appelez hasEnvironmentMap avant d'appeler cette fonction (celle-ci retournant une référence sur une EnvironmentMap existante)
		 */
		C3D_API EnvironmentMap & getEnvironmentMap()const;
		/**
		 *\~english
		 *\return		Retrieves the reflection map index for given node.
		 *\remarks		Call hasEnvironmentMap before calling this function (since this one returns a reference to an existing EnvironmentMap).
		 *\param[in]	node	The scene node.
		 *\~french
		 *\return		Récupère l'indice de la reflection map pour le noeud donné.
		 *\remarks		Appelez hasEnvironmentMap avant d'appeler cette fonction (celle-ci retournant une référence sur une EnvironmentMap existante)
		 *\param[in]	node	Le noeud de scène.
		 */
		C3D_API uint32_t getEnvironmentMapIndex( SceneNode const & node )const;
		/**
		 *\~english
		 *\return		The background model name.
		 *\~french
		 *\return		Le nom du modèle de fond.
		 */
		C3D_API castor::String const & getBackgroundModel()const;
		/**
		 *\~english
		 *\return		The lighting models names for the current background model.
		 *\~french
		 *\return		Les noms des modèles d'éclairage pour le modèle de fond actuel.
		 */
		C3D_API castor::Vector< LightingModelID > getLightingModelsID()const;
		/**
		 *\~english
		 *\brief		Creates an animated texture and adds it to animated textures group.
		 *\param[in]	sourceInfo	The texture source info.
		 *\param[in]	config		The texture configuration.
		 *\param[in]	pass		The target material pass.
		 *\return		The animated texture.
		 *\~french
		 *\brief		Crée une texture animée et l'ajoute au group e de textures animées.
		 *\param[in]	sourceInfo	Les informations de source de la texture.
		 *\param[in]	config		La configuration de la texture.
		 *\param[in]	pass		La passe de matériau cible.
		 *\return		La texture animée.
		 */
		C3D_API AnimatedObjectRPtr addAnimatedTexture( TextureSourceInfo const & sourceInfo
			, TextureConfiguration const & config
			, Pass & pass );
		/**
		 *\~english
		 *\brief		Adds given node to dirty nodes list.
		 *\param[in]	object	The scene node.
		 *\~french
		 *\brief		Ajoute le noeud donné à la liste des noeuds à mettre à jour.
		 *\param[in]	object	Le noeud de scène.
		 */
		C3D_API void markDirty( SceneNode & object );
		/**
		 *\~english
		 *\brief		Adds given BillboardBase to dirty BillboardBase list.
		 *\param[in]	object	The scene node.
		 *\~french
		 *\brief		Ajoute le BillboardBase donné à la liste des BillboardBase à mettre à jour.
		 *\param[in]	object	Le noeud de scène.
		 */
		C3D_API void markDirty( BillboardBase & object );
		/**
		 *\~english
		 *\brief		Adds given object to dirty object list.
		 *\param[in]	object	The object.
		 *\~french
		 *\brief		Ajoute l'objet donné à la liste des objets à mettre à jour.
		 *\param[in]	object	L'objet.
		 */
		C3D_API void markDirty( MovableObject & object );
		/**
		*\~english
		*\name
		*	Getters.
		*\~french
		*\name
		*	Accesseurs.
		*/
		/**@{*/
		C3D_API BackgroundModelID getBackgroundModelId()const;
		C3D_API LightingModelID getDefaultLightingModel()const;
		C3D_API castor::String getDefaultLightingModelName()const;
		C3D_API bool needsGlobalIllumination()const;
		C3D_API bool needsGlobalIllumination( GlobalIlluminationType giType )const;
		C3D_API bool needsGlobalIllumination( LightType ltType
			, GlobalIlluminationType giType )const;
		C3D_API crg::SemaphoreWaitArray getRenderTargetsSemaphores()const;
		C3D_API uint32_t getLpvGridSize()const;
		C3D_API bool hasBindless()const;
		C3D_API ashes::DescriptorSetLayout * getBindlessTexDescriptorLayout()const;
		C3D_API ashes::DescriptorPool * getBindlessTexDescriptorPool()const;
		C3D_API ashes::DescriptorSet * getBindlessTexDescriptorSet()const;
		C3D_API ashes::Buffer< ModelBufferConfiguration > const & getModelBuffer()const;
		C3D_API ashes::Buffer< BillboardUboConfiguration > const & getBillboardsBuffer()const;
		C3D_API bool hasObjects( LightingModelID lightingModelId )const;
		C3D_API bool hasIBLSupport( LightingModelID lightingModelId )const;

		castor::BoundingBox const & getBoundingBox()const noexcept
		{
			return m_boundingBox;
		}

		SceneBackgroundRPtr getBackground()const noexcept
		{
			return m_background.get();
		}

		castor::RgbColour const & getBackgroundColour()const noexcept
		{
			return m_backgroundColour;
		}

		SceneNodeRPtr getRootNode()const noexcept
		{
			return m_rootNode;
		}

		SceneNodeRPtr getCameraRootNode()const noexcept
		{
			return m_rootCameraNode;
		}

		SceneNodeRPtr getObjectRootNode()const noexcept
		{
			return m_rootObjectNode;
		}

		castor::RgbColour const & getAmbientLight()const noexcept
		{
			return m_ambientLight;
		}

		Fog const & getFog()const noexcept
		{
			return m_fog;
		}

		Fog & getFog()noexcept
		{
			return m_fog;
		}

		FrameListener & getListener()const noexcept
		{
			CU_Require( m_listener );
			return *m_listener;
		}

		bool isInitialised()const noexcept
		{
			return m_initialised;
		}

		bool needsSubsurfaceScattering()const noexcept
		{
			return m_needsSubsurfaceScattering;
		}

		bool hasOpaqueObjects()const noexcept
		{
			return m_hasOpaqueObjects;
		}

		bool hasTransparentObjects()const noexcept
		{
			return m_hasTransparentObjects;
		}

		uint32_t getDirectionalShadowCascades()const noexcept
		{
			return m_directionalShadowCascades;
		}

		float getLpvIndirectAttenuation()const noexcept
		{
			return m_lpvIndirectAttenuation;
		}

		VctConfig const & getVoxelConeTracingConfig()const noexcept
		{
			return m_voxelConfig;
		}

		VctConfig & getVoxelConeTracingConfig()noexcept
		{
			return m_voxelConfig;
		}

		SceneRenderNodes & getRenderNodes()const noexcept
		{
			return *m_renderNodes;
		}

		LightFactory & getLightsFactory()const noexcept
		{
			return *m_lightFactory;
		}

		crg::ResourcesCache & getResources()noexcept
		{
			return m_resources;
		}

		DebugConfig & getDebugConfig()const noexcept
		{
			return m_debugConfig;
		}
		/**@}*/
		/**
		*\~english
		*name
		*	Mutators.
		*\~french
		*name
		*	Mutateurs.
		*/
		/**@{*/
		C3D_API void setDirectionalShadowCascades( uint32_t value );
		C3D_API void setLpvIndirectAttenuation( float value );
		C3D_API void setDefaultLightingModel( LightingModelID value );

		void setBackgroundColour( castor::RgbColour const & value )noexcept
		{
			m_backgroundColour = value;
		}

		void setAmbientLight( castor::RgbColour const & value )noexcept
		{
			m_ambientLight = value;
		}

		GeometryCache::ElementObsT addGeometry( GeometryCache::ElementPtrT element )
		{
			auto result = element.get();
			m_geometryCache->add( castor::move( element ) );
			return result;
		}
		/**@}*/

	private:
		void doGatherDirty( CpuUpdater::DirtyObjects & sceneObjs );
		void doUpdateSceneNodes( CpuUpdater::DirtyObjects const & sceneObjs )const;
		void doUpdateMovables( CpuUpdater & updater
			, CpuUpdater::DirtyObjects & sceneObjs );
		void doUpdateLights( CpuUpdater & updater
		, CpuUpdater::DirtyObjects const & sceneObjs );
		void doUpdateParticles( CpuUpdater & updater );
		void doUpdateParticles( GpuUpdater & updater );
		void doUpdateMaterials();
		bool doUpdateLightDependent( LightType lightType
			, bool shadowProducer
			, GlobalIlluminationType globalIllumination );
		bool doUpdateLightsDependent();
		void onMaterialChanged( Material const & material );

	public:
		//!\~english	The signal raised when the scene is updating.
		//!\~french		Le signal levé lorsque la scène se met à jour.
		mutable OnSceneUpdate onUpdate;
		//!\~english	The signal raised when the scene background has changed.
		//!\~french		Le signal levé lorsque le fond a changé.
		mutable OnBackgroundChanged onSetBackground;

	private:
		bool m_initialised{ false };
		crg::ResourcesCache m_resources;
		castor::Vector< SceneNode * > m_dirtyNodes;
		castor::Vector< BillboardBase * > m_dirtyBillboards;
		castor::Vector< MovableObject * > m_dirtyObjects;
		DECLARE_OBJECT_CACHE_MEMBER( sceneNode, SceneNode );
		SceneNodeRPtr m_rootNode;
		SceneNodeRPtr m_rootCameraNode;
		SceneNodeRPtr m_rootObjectNode;
		DECLARE_OBJECT_CACHE_MEMBER_MIN( geometry, Geometry );
		DECLARE_OBJECT_CACHE_MEMBER( camera, Camera );
		DECLARE_OBJECT_CACHE_MEMBER( light, Light );
		DECLARE_OBJECT_CACHE_MEMBER( billboard, BillboardList );
		DECLARE_OBJECT_CACHE_MEMBER( particleSystem, ParticleSystem );
		DECLARE_CACHE_MEMBER( mesh, Mesh );
		DECLARE_CACHE_MEMBER( animatedObjectGroup, AnimatedObjectGroup );
		DECLARE_CACHE_MEMBER( overlay, Overlay );
		DECLARE_CACHE_MEMBER( skeleton, Skeleton );
		DECLARE_CACHE_VIEW_MEMBER( material, Material, GpuEventType::ePreUpload );
		DECLARE_CACHE_VIEW_MEMBER( sampler, Sampler, GpuEventType::ePreUpload );
		DECLARE_CU_CACHE_VIEW_MEMBER( font, Font, CpuEventType::ePreGpuStep );
		bool m_changed{ false };
		castor::RgbColour m_ambientLight{ 0.0f, 0.0f, 0.0f };
		castor::RgbColour m_backgroundColour{ 0.0f, 0.0f, 0.0f };
		SceneBackgroundUPtr m_background;
		LightFactoryUPtr m_lightFactory;
		Fog m_fog;
		FrameListenerRPtr m_listener{};
		EnvironmentMapUPtr m_reflectionMap;
		bool m_needsSubsurfaceScattering{ false };
		bool m_hasOpaqueObjects{ false };
		bool m_hasTransparentObjects{ false };
		castor::Map< Material *, OnMaterialChangedConnection > m_materialsListeners;
		bool m_dirtyMaterials{ true };
		uint32_t m_directionalShadowCascades{ MaxDirectionalCascadesCount };
		castor::BoundingBox m_boundingBox;
		std::atomic_bool m_needsGlobalIllumination;
		castor::Array< std::atomic_bool, size_t( LightType::eCount ) > m_hasShadows;
		castor::Array< castor::Set< GlobalIlluminationType >, size_t( LightType::eCount ) > m_giTypes;
		std::atomic_bool m_hasAnyShadows;
		float m_lpvIndirectAttenuation{ 1.7f };
		VctConfig m_voxelConfig;
		SceneRenderNodesUPtr m_renderNodes;
		FramePassTimerUPtr m_timerSceneNodes;
		FramePassTimerUPtr m_timerBoundingBox;
		FramePassTimerUPtr m_timerMaterials;
		FramePassTimerUPtr m_timerLights;
		FramePassTimerUPtr m_timerParticlesCpu;
		FramePassTimerUPtr m_timerParticlesGpu;
		FramePassTimerUPtr m_timerGpuUpdate;
		FramePassTimerUPtr m_timerMovables;
		CpuFrameEvent * m_cleanBackground{};
		mutable DebugConfig m_debugConfig;

	public:
		//!\~english	The cameras root node name.
		//!\~french		Le nom du noeud de scène racine des caméras.
		C3D_API static castor::String CameraRootNode;
		//!\~english	The objects root node name.
		//!\~french		Le nom du noeud de scène racine des objets.
		C3D_API static castor::String ObjectRootNode;
		//!\~english	The root node name.
		//!\~french		Le nom du noeud de scène racine.
		C3D_API static castor::String RootNode;
	};

	struct RootContext;

	struct SceneContext
	{
		RootContext * root{};
		SceneRPtr scene{};
		SceneUPtr ownScene{};
		castor::String fontName{};
		OverlayContextUPtr overlays{};
	};

	C3D_API Engine * getEngine( SceneContext const & context );
}

namespace castor
{
	template<>
	struct ParserEnumTraits< castor3d::FogType >
	{
		static inline xchar const * const Name = cuT( "FogType" );
		static inline UInt32StrMap const Values = []()
			{
				UInt32StrMap result;
				result = castor3d::getEnumMapT< castor3d::FogType >();
				return result;
			}( );
	};
}

#endif
