/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SCENE_H___
#define ___C3D_SCENE_H___

#include "Castor3D/Cache/CacheView.hpp"
#include "Castor3D/Cache/ObjectCache.hpp"
#include "Castor3D/Cache/BillboardCache.hpp"
#include "Castor3D/Cache/CameraCache.hpp"
#include "Castor3D/Cache/GeometryCache.hpp"
#include "Castor3D/Cache/SceneNodeCache.hpp"
#include "Castor3D/Cache/AnimatedObjectGroupCache.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Cache/MeshCache.hpp"
#include "Castor3D/Cache/OverlayCache.hpp"
#include "Castor3D/Cache/ParticleSystemCache.hpp"
#include "Castor3D/Cache/SamplerCache.hpp"

#include "Castor3D/Scene/Background/Background.hpp"
#include "Castor3D/Scene/Fog.hpp"
#include "Castor3D/Scene/Shadow.hpp"

#include <CastorUtils/Log/Logger.hpp>
#include <CastorUtils/Design/Named.hpp>
#include <CastorUtils/Design/OwnedBy.hpp>
#include <CastorUtils/Design/Signal.hpp>
#include <CastorUtils/Multithreading/ThreadPool.hpp>

namespace castor3d
{
	class Scene
		: public std::enable_shared_from_this< Scene >
		, public castor::OwnedBy< Engine >
		, public castor::Named
	{
	public:
		/*!
		\author		Sylvain DOREMUS
		\date		14/02/2010
		\~english
		\brief		Scene loader
		\~english
		\brief		Loader de scène
		*/
		class TextWriter
			: public castor::TextWriter< Scene >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			C3D_API explicit TextWriter( castor::String const & tabs
				, castor::Path const & materialsFile = castor::Path{} );
			/**
			 *\~english
			 *\brief		Writes a scene into a text file
			 *\param[in]	scene	the write to save
			 *\param[in]	file	the file to write the scene in
			 *\~french
			 *\brief		Ecrit une scène dans un fichier texte
			 *\param[in]	scene	La scène
			 *\param[in]	file	Le fichier
			 */
			C3D_API bool operator()( Scene const & scene, castor::TextFile & file )override;

		private:
			castor::Path const & m_materialsFile;
		};

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
		C3D_API void update();
		/**
		 *\~english
		 *\brief		Updates the scene device dependant stuff.
		 *\~french
		 *\brief		Met à jour les objets de scène dépendant du device.
		 */
		C3D_API void updateDeviceDependent( Camera const & camera );
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
		 *\brief		Imports a scene from an foreign file
		 *\param[in]	fileName	file to read from
		 *\param[in]	importer	The importer, which is in charge of loading the scene
		 *\return		\p true if successful, false if not
		 *\~french
		 *\brief		Importe une scène à partir d'un fichier
		 *\param[in]	fileName	L'adresse du fichier
		 *\param[in]	importer	L'importeur chargé de la récupération des données
		 *\return		\p false si un problème quelconque a été rencontré
		 */
		C3D_API bool importExternal( castor::Path const & fileName, Importer & importer );
		/**
		 *\~english
		 *\brief		Merges the content of the given scene to this scene
		 *\param[in]	scene	The scene to merge into this one
		 *\~french
		 *\brief		Intègre à cette scène le contenu de celle donnée
		 *\param[in]	scene	La scène à intégrer
		 */
		C3D_API void merge( SceneSPtr scene );
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
		 *\return		Creates a reflection map for given node.
		 *\param[in]	node	The scene node from which the reflection map is generated.
		 *\~french
		 *\return		Crée une reflection map pour le noeud donné.
		 *\param[in]	node	Le noeud de scène depuis lequel la reflection map est générée.
		 */
		C3D_API void createEnvironmentMap( SceneNode & node );
		/**
		 *\~english
		 *\return		Tells if there is a reflection map for given node.
		 *\param[in]	node	The scene node.
		 *\~french
		 *\return		Dit s'il y a une reflection map pour le noeud donné.
		 *\param[in]	node	Le noeud de scène.
		 */
		C3D_API bool hasEnvironmentMap( SceneNode const & node )const;
		/**
		 *\~english
		 *\remarks		Call hasEnvironmentMap before calling this function (since this one returns a reference to an existing EnvironmentMap).
		 *\return		Retrieves the reflection map for given node.
		 *\param[in]	node	The scene node.
		 *\~french
		 *\remarks		Appelez hasEnvironmentMap avant d'appeler cette fonction (celle-ci retournant une référence sur une EnvironmentMap existante)
		 *\return		Récupère la reflection map pour le noeud donné.
		 *\param[in]	node	Le noeud de scène.
		 */
		C3D_API EnvironmentMap & getEnvironmentMap( SceneNode const & node );
		/**
		 *\~english
		 *\remarks		Call hasEnvironmentMap before calling this function (since this one returns a reference to an existing EnvironmentMap).
		 *\return		Retrieves the reflection map for given node.
		 *\param[in]	node	The scene node.
		 *\~french
		 *\remarks		Appelez hasEnvironmentMap avant d'appeler cette fonction (celle-ci retournant une référence sur une EnvironmentMap existante)
		 *\return		Récupère la reflection map pour le noeud donné.
		 *\param[in]	node	Le noeud de scène.
		 */
		C3D_API EnvironmentMap const & getEnvironmentMap( SceneNode const & node )const;
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		inline SceneBackgroundSPtr getBackground()const
		{
			return m_background;
		}

		inline SceneBackground const & getColourBackground()const
		{
			return *m_colourBackground;
		}

		inline SceneBackground & getColourBackground()
		{
			return *m_colourBackground;
		}

		inline std::vector< std::reference_wrapper< EnvironmentMap > > & getEnvironmentMaps()
		{
			return m_reflectionMapsArray;
		}

		inline std::vector< std::reference_wrapper< EnvironmentMap > > const & getEnvironmentMaps()const
		{
			return m_reflectionMapsArray;
		}

		inline castor::RgbColour const & getBackgroundColour()const
		{
			return m_backgroundColour;
		}

		inline SceneNodeSPtr getRootNode()const
		{
			return m_rootNode;
		}

		inline SceneNodeSPtr getCameraRootNode()const
		{
			return m_rootCameraNode;
		}

		inline SceneNodeSPtr getObjectRootNode()const
		{
			return m_rootObjectNode;
		}

		inline bool hasChanged()const
		{
			return m_changed;
		}

		inline castor::RgbColour const & getAmbientLight()const
		{
			return m_ambientLight;
		}

		inline Fog const & getFog()const
		{
			return m_fog;
		}

		inline Fog & getFog()
		{
			return m_fog;
		}

		inline MaterialType getMaterialsType()const
		{
			return getEngine()->getMaterialsType();
		}

		inline FrameListener const & getListener()const
		{
			CU_Require( !m_listener.expired() );
			return *m_listener.lock();
		}

		inline FrameListener & getListener()
		{
			CU_Require( !m_listener.expired() );
			return *m_listener.lock();
		}

		inline bool isInitialised()const
		{
			return m_initialised;
		}

		inline bool needsSubsurfaceScattering()const
		{
			return m_needsSubsurfaceScattering;
		}

		inline bool hasOpaqueObjects()const
		{
			return m_hasOpaqueObjects;
		}

		inline bool hasTransparentObjects()const
		{
			return m_hasTransparentObjects;
		}

		inline BillboardUboPools const & getBillboardPools()const
		{
			return m_billboardPools;
		}

		inline BillboardUboPools & getBillboardPools()
		{
			return m_billboardPools;
		}

		inline uint32_t getDirectionalShadowCascades()const
		{
			return m_directionalShadowCascades;
		}

		C3D_API ashes::SemaphoreCRefArray getRenderTargetsSemaphores()const;
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
		inline void setBackgroundColour( castor::RgbColour const & value )
		{
			m_backgroundColour = value;
		}

		inline void setChanged()
		{
			m_changed = true;
			onChanged( *this );
		}

		inline void setAmbientLight( castor::RgbColour const & value )
		{
			m_ambientLight = value;
		}

		inline void setMaterialsType( MaterialType value )
		{
			getEngine()->setMaterialsType( value );
		}

		inline void setDirectionalShadowCascades( uint32_t value )
		{
			CU_Require( value <= shader::DirectionalMaxCascadesCount );
			m_directionalShadowCascades = value;
		}
		/**@}*/

	private:
		void doUpdateAnimations();
		void doUpdateMaterials();
		void onMaterialChanged( Material const & material );

	public:
		//!\~english	The signal raised when the scene has changed.
		//!\~french		Le signal levé lorsque la scène a changé.
		mutable OnSceneChanged onChanged;
		//!\~english	The signal raised when the scene is updating.
		//!\~french		Le signal levé lorsque la scène se met à jour.
		mutable OnSceneUpdate onUpdate;

	private:
		bool m_initialised{ false };
		SceneNodeSPtr m_rootNode;
		SceneNodeSPtr m_rootCameraNode;
		SceneNodeSPtr m_rootObjectNode;
		DECLARE_OBJECT_CACHE_MEMBER( sceneNode, SceneNode );
		DECLARE_OBJECT_CACHE_MEMBER( camera, Camera );
		DECLARE_OBJECT_CACHE_MEMBER( light, Light );
		DECLARE_OBJECT_CACHE_MEMBER( geometry, Geometry );
		DECLARE_OBJECT_CACHE_MEMBER( billboard, BillboardList );
		DECLARE_OBJECT_CACHE_MEMBER( particleSystem, ParticleSystem );
		DECLARE_CACHE_MEMBER( mesh, Mesh );
		DECLARE_CACHE_MEMBER( animatedObjectGroup, AnimatedObjectGroup );
		DECLARE_CACHE_VIEW_MEMBER( overlay, Overlay, EventType::ePreRender );
		DECLARE_CACHE_VIEW_MEMBER( material, Material, EventType::ePreRender );
		DECLARE_CACHE_VIEW_MEMBER( sampler, Sampler, EventType::ePreRender );
		DECLARE_CACHE_VIEW_MEMBER_CU( font, Font, EventType::ePreRender );
		BillboardUboPools m_billboardPools;
		bool m_changed{ false };
		castor::RgbColour m_ambientLight;
		castor::RgbColour m_backgroundColour;
		SceneBackgroundSPtr m_background;
		SceneBackgroundSPtr m_colourBackground;
		LightFactory m_lightFactory;
		Fog m_fog;
		FrameListenerWPtr m_listener;
		std::map< SceneNode const *, std::unique_ptr< EnvironmentMap > > m_reflectionMaps;
		std::vector< std::reference_wrapper< EnvironmentMap > > m_reflectionMapsArray;
		castor::ThreadPool m_animationUpdater;
		bool m_needsSubsurfaceScattering{ false };
		bool m_hasOpaqueObjects{ false };
		bool m_hasTransparentObjects{ false };
		std::map< MaterialSPtr, OnMaterialChangedConnection > m_materialsListeners;
		bool m_dirtyMaterials{ true };
		uint32_t m_directionalShadowCascades{ 4u };

	public:
		//!\~english	The cameras root node name.
		//!\~french		Le nom du noeud de scène racine des caméras.
		static castor::String CameraRootNode;
		//!\~english	The objects root node name.
		//!\~french		Le nom du noeud de scène racine des objets.
		static castor::String ObjectRootNode;
		//!\~english	The root node name.
		//!\~french		Le nom du noeud de scène racine.
		static castor::String RootNode;
	};
}

#endif
