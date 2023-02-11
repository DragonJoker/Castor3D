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
#include "Castor3D/Render/GlobalIllumination/VoxelConeTracing/VoxelSceneData.hpp"
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
	C3D_API std::string print( castor::Point3f const & obj );
	C3D_API std::string print( castor::BoundingBox const & obj );

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
		C3D_API ~Scene();
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
		C3D_API void setBackground( SceneBackgroundSPtr value );
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
		 *\return		Retrieves the reflection map for given node.
		 *\remarks		Call hasEnvironmentMap before calling this function (since this one returns a reference to an existing EnvironmentMap).
		 *\param[in]	node	The scene node.
		 *\~french
		 *\return		Récupère la reflection map pour le noeud donné.
		 *\remarks		Appelez hasEnvironmentMap avant d'appeler cette fonction (celle-ci retournant une référence sur une EnvironmentMap existante)
		 *\param[in]	node	Le noeud de scène.
		 */
		C3D_API EnvironmentMap & getEnvironmentMap()const;
		/**
		 *\~english
		 *\return		Retrieves the reflection map for given node.
		 *\remarks		Call hasEnvironmentMap before calling this function (since this one returns a reference to an existing EnvironmentMap).
		 *\param[in]	node	The scene node.
		 *\~french
		 *\return		Récupère la reflection map pour le noeud donné.
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
		C3D_API std::vector< LightingModelID > getLightingModelsID()const;
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
		C3D_API AnimatedObjectSPtr addAnimatedTexture( TextureSourceInfo const & sourceInfo
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

		castor::BoundingBox const & getBoundingBox()const
		{
			return m_boundingBox;
		}

		SceneBackgroundSPtr getBackground()const
		{
			return m_background;
		}

		castor::RgbColour const & getBackgroundColour()const
		{
			return m_backgroundColour;
		}

		SceneNodeRPtr getRootNode()const
		{
			return m_rootNode.get();
		}

		SceneNodeRPtr getCameraRootNode()const
		{
			return m_rootCameraNode.get();
		}

		SceneNodeRPtr getObjectRootNode()const
		{
			return m_rootObjectNode.get();
		}

		castor::RgbColour const & getAmbientLight()const
		{
			return m_ambientLight;
		}

		Fog const & getFog()const
		{
			return m_fog;
		}

		Fog & getFog()
		{
			return m_fog;
		}

		FrameListener const & getListener()const
		{
			CU_Require( !m_listener.expired() );
			return *m_listener.lock();
		}

		FrameListener & getListener()
		{
			CU_Require( !m_listener.expired() );
			return *m_listener.lock();
		}

		bool isInitialised()const
		{
			return m_initialised;
		}

		bool needsSubsurfaceScattering()const
		{
			return m_needsSubsurfaceScattering;
		}

		bool hasOpaqueObjects()const
		{
			return m_hasOpaqueObjects;
		}

		bool hasTransparentObjects()const
		{
			return m_hasTransparentObjects;
		}

		uint32_t getDirectionalShadowCascades()const
		{
			return m_directionalShadowCascades;
		}

		float getLpvIndirectAttenuation()const
		{
			return m_lpvIndirectAttenuation;
		}

		VoxelSceneData const & getVoxelConeTracingConfig()const
		{
			return m_voxelConfig;
		}

		VoxelSceneData & getVoxelConeTracingConfig()
		{
			return m_voxelConfig;
		}

		SceneRenderNodes & getRenderNodes()const
		{
			return *m_renderNodes;
		}

		LightFactory & getLightsFactory()const
		{
			return *m_lightFactory;
		}

		SceneCuller & getDummyCuller()const
		{
			return *m_dummyCuller;
		}

		SceneCuller & getStaticsDummyCuller()const
		{
			return *m_staticsDummyCuller;
		}

		SceneCuller & getDynamicsDummyCuller()const
		{
			return *m_dynamicsDummyCuller;
		}

		crg::ResourcesCache & getResources()
		{
			return m_resources;
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

		void setBackgroundColour( castor::RgbColour const & value )
		{
			m_backgroundColour = value;
		}

		void setAmbientLight( castor::RgbColour const & value )
		{
			m_ambientLight = value;
		}

		void addGeometry( GeometryCache::ElementPtrT element )
		{
			m_geometryCache->add( element );
		}
		/**@}*/

	private:
		void doGatherDirty( CpuUpdater::DirtyObjects & sceneObjs );
		void doUpdateSceneNodes( CpuUpdater & updater
			, CpuUpdater::DirtyObjects & sceneObjs );
		void doUpdateMovables( CpuUpdater & updater
			, CpuUpdater::DirtyObjects & sceneObjs );
		void doUpdateLights( CpuUpdater & updater
			, CpuUpdater::DirtyObjects & sceneObjs );
		void doUpdateParticles( CpuUpdater & updater
			, CpuUpdater::DirtyObjects & sceneObjs );
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
		std::vector< SceneNode * > m_dirtyNodes;
		std::vector< BillboardBase * > m_dirtyBillboards;
		std::vector< MovableObject * > m_dirtyObjects;
		SceneNodeSPtr m_rootNode;
		SceneNodeSPtr m_rootCameraNode;
		SceneNodeSPtr m_rootObjectNode;
		DECLARE_OBJECT_CACHE_MEMBER_MIN( geometry, Geometry );
		DECLARE_OBJECT_CACHE_MEMBER( sceneNode, SceneNode );
		DECLARE_OBJECT_CACHE_MEMBER( camera, Camera );
		DECLARE_OBJECT_CACHE_MEMBER( light, Light );
		DECLARE_OBJECT_CACHE_MEMBER( billboard, BillboardList );
		DECLARE_OBJECT_CACHE_MEMBER( particleSystem, ParticleSystem );
		DECLARE_CACHE_MEMBER( mesh, Mesh );
		DECLARE_CACHE_MEMBER( animatedObjectGroup, AnimatedObjectGroup );
		DECLARE_CACHE_MEMBER( overlay, Overlay );
		DECLARE_CACHE_MEMBER( skeleton, Skeleton );
		DECLARE_CACHE_VIEW_MEMBER( material, Material, EventType::ePreRender );
		DECLARE_CACHE_VIEW_MEMBER( sampler, Sampler, EventType::ePreRender );
		DECLARE_CU_CACHE_VIEW_MEMBER( font, Font, EventType::ePreRender );
		bool m_changed{ false };
		castor::RgbColour m_ambientLight;
		castor::RgbColour m_backgroundColour;
		SceneBackgroundSPtr m_background;
		LightFactorySPtr m_lightFactory;
		Fog m_fog;
		FrameListenerWPtr m_listener;
		std::unique_ptr< EnvironmentMap > m_reflectionMap;
		bool m_needsSubsurfaceScattering{ false };
		bool m_hasOpaqueObjects{ false };
		bool m_hasTransparentObjects{ false };
		std::map< Material *, OnMaterialChangedConnection > m_materialsListeners;
		bool m_dirtyMaterials{ true };
		uint32_t m_directionalShadowCascades{ DirectionalMaxCascadesCount };
		castor::BoundingBox m_boundingBox;
		std::atomic_bool m_needsGlobalIllumination;
		std::array< std::atomic_bool, size_t( LightType::eCount ) > m_hasShadows;
		std::array< std::set< GlobalIlluminationType >, size_t( LightType::eCount ) > m_giTypes;
		std::atomic_bool m_hasAnyShadows;
		float m_lpvIndirectAttenuation{ 1.7f };
		VoxelSceneData m_voxelConfig;
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
		SceneCullerUPtr m_dummyCuller;
		SceneCullerUPtr m_staticsDummyCuller;
		SceneCullerUPtr m_dynamicsDummyCuller;

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
}

#endif
