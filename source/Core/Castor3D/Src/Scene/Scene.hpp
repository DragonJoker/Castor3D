/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___C3D_SCENE_H___
#define ___C3D_SCENE_H___

#include "Cache/CacheView.hpp"
#include "Cache/ObjectCache.hpp"
#include "Cache/BillboardCache.hpp"
#include "Cache/CameraCache.hpp"
#include "Cache/GeometryCache.hpp"
#include "Cache/SceneNodeCache.hpp"
#include "Cache/AnimatedObjectGroupCache.hpp"
#include "Cache/LightCache.hpp"
#include "Cache/MaterialCache.hpp"
#include "Cache/MeshCache.hpp"
#include "Cache/OverlayCache.hpp"
#include "Cache/ParticleSystemCache.hpp"
#include "Cache/SamplerCache.hpp"

#include "RenderToTexture/TextureProjection.hpp"
#include "Scene/Fog.hpp"
#include "Scene/Shadow.hpp"

#include <Log/Logger.hpp>
#include <Design/Named.hpp>
#include <Design/OwnedBy.hpp>
#include <Design/Signal.hpp>

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Scene handler class
	\remarks	A scene is a collection of lights, scene nodes and geometries.
				<br />It has at least one camera to render it
	\~french
	\brief		Classe de gestion d'un scène
	\remarks	Une scène est une collection de lumières, noeuds et géométries.
				<br />Elle a au moins une caméra permettant son rendu
	*/
	class Scene
		: public std::enable_shared_from_this< Scene >
		, public Castor::OwnedBy< Engine >
		, public Castor::Named
	{
	public:
		using OnChangedFunction = std::function< void( Scene const & ) >;
		using OnChanged = Castor::Signal< OnChangedFunction >;

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
			: public Castor::TextWriter< Scene >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			C3D_API explicit TextWriter( Castor::String const & p_tabs );
			/**
			 *\~english
			 *\brief		Writes a scene into a text file
			 *\param[in]	p_scene	the write to save
			 *\param[in]	p_file	the file to write the scene in
			 *\~french
			 *\brief		Ecrit une scène dans un fichier texte
			 *\param[in]	p_scene	La scène
			 *\param[in]	p_file	Le fichier
			 */
			C3D_API bool operator()( Scene const & p_scene, Castor::TextFile & p_file )override;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_name		The scene name
		 *\param[in]	p_engine	The core engine
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_name		Le nom de la scène
		 *\param[in]	p_engine	Le moteur
		 */
		C3D_API Scene( Castor::String const & p_name, Engine & p_engine );
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
		C3D_API void Initialise();
		/**
		 *\~english
		 *\brief		Clears the maps, leaves the root nodes
		 *\~french
		 *\brief		Vide les maps, laisse les noeuds pères
		 */
		C3D_API void Cleanup();
		/**
		 *\~english
		 *\brief		Renders the scene background (skybox or image).
		 *\param[in]	p_size		The target dimensions.
		 *\param[in]	p_camera	The current camera.
		 *\~french
		 *\brief		Rend l'arrière plan de la scène (skybox ou image).
		 *\param[in]	p_size		Les dimensions de la cible.
		 *\param[in]	p_camera	La caméra courante.
		 */
		C3D_API void RenderBackground( Castor::Size const & p_size, Camera const & p_camera );
		/**
		 *\~english
		 *\brief		Updates the scene before render.
		 *\~french
		 *\brief		Met à jour la scène avant le rendu.
		 */
		C3D_API void Update();
		/**
		 *\~english
		 *\brief		Sets the background image for the scene
		 *\param[in]	p_folder	The folder containing the image.
		 *\param[in]	p_relative	The image file path, relative to p_folder.
		 *\~french
		 *\brief		Définit l'image de fond pour la scène
		 *\param[in]	p_folder	Le dossier contenant l'image.
		 *\param[in]	p_relative	Le chemin d'accès à l'image, relatif à p_folder.
		 */
		C3D_API bool SetBackground( Castor::Path const & p_folder, Castor::Path const & p_relative );
		/**
		 *\~english
		 *\brief		Sets the skybox for the scene.
		 *\param[in]	p_skybox	The skybox.
		 *\~french
		 *\brief		Définit la skybox de la scène.
		 *\param[in]	p_skybox	La skybox.
		 */
		C3D_API bool SetForeground( SkyboxUPtr && p_skybox );
		/**
		 *\~english
		 *\brief		Imports a scene from an foreign file
		 *\param[in]	p_fileName	file to read from
		 *\param[in]	p_importer	The importer, which is in charge of loading the scene
		 *\return		\p true if successful, false if not
		 *\~french
		 *\brief		Importe une scène à partir d'un fichier
		 *\param[in]	p_fileName	L'adresse du fichier
		 *\param[in]	p_importer	L'importeur chargé de la récupération des données
		 *\return		\p false si un problème quelconque a été rencontré
		 */
		C3D_API bool ImportExternal( Castor::Path const & p_fileName, Importer & p_importer );
		/**
		 *\~english
		 *\brief		Merges the content of the given scene to this scene
		 *\param[in]	p_scene	The scene to merge into this one
		 *\~french
		 *\brief		Intègre à cette scène le contenu de celle donnée
		 *\param[in]	p_scene	La scène à intégrer
		 */
		C3D_API void Merge( SceneSPtr p_scene );
		/**
		 *\~english
		 *\brief		Retrieves the vertices count
		 *\return		The value
		 *\~french
		 *\brief		Récupère le nombre de sommets
		 *\return		La valeur
		 */
		C3D_API uint32_t GetVertexCount()const;
		/**
		 *\~english
		 *\brief		Retrieves the faces count
		 *\return		The value
		 *\~french
		 *\brief		Récupère le nombre de faces
		 *\return		La valeur
		 */
		C3D_API uint32_t GetFaceCount()const;
		/**
		 *\~english
		 *\return		The scene flags.
		 *\~french
		 *\return		Les indicateurs de la scène.
		 */
		C3D_API SceneFlags GetFlags()const;
		/**
		 *\~english
		 *\return		Tells if the scene has a shadow projecting light.
		 *\~french
		 *\return		Dit si la scène a au moins une source lumineuse projetant des ombres.
		 */
		C3D_API bool HasShadows()const;
		/**
		 *\~english
		 *\return		Creates a reflection map for given node.
		 *\param[in]	p_node	The scene node from which the reflection map is generated.
		 *\~french
		 *\return		Crée une reflection map pour le noeud donné.
		 *\param[in]	p_node	Le noeud de scène depuis lequel la reflection map est générée.
		 */
		C3D_API void CreateEnvironmentMap( SceneNode & p_node );
		/**
		 *\~english
		 *\return		Tells if there is a reflection map for given node.
		 *\param[in]	p_node	The scene node.
		 *\~french
		 *\return		Dit s'il y a une reflection map pour le noeud donné.
		 *\param[in]	p_node	Le noeud de scène.
		 */
		C3D_API bool HasEnvironmentMap( SceneNode const & p_node );
		/**
		 *\~english
		 *\remarks		Call HasEnvironmentMap before calling this function (since this one returns a reference to an existing EnvironmentMap).
		 *\return		Retrieves the reflection map for given node.
		 *\param[in]	p_node	The scene node.
		 *\~french
		 *\remarks		Appelez HasEnvironmentMap avant d'appeler cette fonction (celle-ci retournant une référence sur une EnvironmentMap existante)
		 *\return		Récupère la reflection map pour le noeud donné.
		 *\param[in]	p_node	Le noeud de scène.
		 */
		C3D_API EnvironmentMap & GetEnvironmentMap( SceneNode const & p_node );
		/**
		 *\~english
		 *\return		The reflection maps list.
		 *\~french
		 *\return		La liste des reflection maps.
		 */
		inline std::vector< std::reference_wrapper< EnvironmentMap > > & GetEnvironmentMaps()
		{
			return m_reflectionMapsArray;
		}
		/**
		 *\~english
		 *\return		The reflection maps list.
		 *\~french
		 *\return		La liste des reflection maps.
		 */
		inline std::vector< std::reference_wrapper< EnvironmentMap > > const & GetEnvironmentMaps()const
		{
			return m_reflectionMapsArray;
		}
		/**
		 *\~english
		 *\brief		Sets the background colour
		 *\param[in]	p_clrNew	The new colour
		 *\~french
		 *\brief		Définit la couleur du fond
		 *\param[in]	p_clrNew	La nouvelle couleur
		 */
		inline void SetBackgroundColour( Castor::Colour const & p_clrNew )
		{
			m_backgroundColour = p_clrNew;
		}
		/**
		 *\~english
		 *\brief		Retrieves the background colour
		 *\return		The colour
		 *\~french
		 *\brief		Récupère la couleur du fond
		 *\return		La couleur
		 */
		inline Castor::Colour const & GetBackgroundColour()const
		{
			return m_backgroundColour;
		}
		/**
		 *\~english
		 *\brief		Retrieves the root node
		 *\return		The value
		 *\~french
		 *\brief		Récupère le node racine
		 *\return		La valeur
		 */
		inline SceneNodeSPtr GetRootNode()const
		{
			return m_rootNode;
		}
		/**
		 *\~english
		 *\brief		Retrieves the cameras root node
		 *\return		The value
		 *\~french
		 *\brief		Récupère le node racine des caméras
		 *\return		La valeur
		 */
		inline SceneNodeSPtr GetCameraRootNode()const
		{
			return m_rootCameraNode;
		}
		/**
		 *\~english
		 *\brief		Retrieves the objects root node
		 *\return		The value
		 *\~french
		 *\brief		Récupère le node racine des objets
		 *\return		La valeur
		 */
		inline SceneNodeSPtr GetObjectRootNode()const
		{
			return m_rootObjectNode;
		}
		/**
		 *\~english
		 *\brief		Retrieves the scene background image
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'image de fond de la scène
		 *\return		La valeur
		 */
		inline TextureLayoutSPtr GetBackgroundImage()const
		{
			return m_backgroundImage;
		}
		/**
		 *\~english
		 *\brief		Retrieves the scene change status
		 *\return		The value
		 *\~french
		 *\brief		Récupère le statut de changement de la scène
		 *\return		La valeur
		 */
		inline bool HasChanged()const
		{
			return m_changed;
		}
		/**
		 *\~english
		 *\brief		Sets the scene changed status to \p true.
		 *\~french
		 *\brief		Définit le statut de changement de la scène to \p true.
		 */
		inline void SetChanged()
		{
			m_changed = true;
			onChanged( *this );
		}
		/**
		 *\~english
		 *\return		The ambient light colour
		 *\~french
		 *\return		La couleur de la lumière ambiante
		 */
		inline Castor::Colour const & GetAmbientLight()const
		{
			return m_ambientLight;
		}
		/**
		 *\~english
		 *\brief		Sets the ambient light colour.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\brief		Définit la couleur de la lumière ambiante.
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		inline void SetAmbientLight( Castor::Colour const & p_value )
		{
			m_ambientLight = p_value;
		}
		/**
		 *\~english
		 *\return		\p true if the skybox is defined.
		 *\~french
		 *\return		\p true si la skybox est définie.
		 */
		inline bool HasSkybox()const
		{
			return m_skybox != nullptr;
		}
		/**
		 *\~english
		 *\return		The skybox.
		 *\~french
		 *\return		La skybox.
		 */
		inline Skybox const & GetSkybox()const
		{
			REQUIRE( m_skybox );
			return *m_skybox;
		}
		/**
		 *\~english
		 *\return		The fog's parameters.
		 *\~french
		 *\return		Les paramètres du brouillard.
		 */
		inline Fog const & GetFog()const
		{
			return m_fog;
		}
		/**
		 *\~english
		 *\return		The fog's parameters.
		 *\~french
		 *\return		Les paramètres du brouillard.
		 */
		inline Fog & GetFog()
		{
			return m_fog;
		}
		/**
		 *\~english
		 *\return		The shadows parameters.
		 *\~french
		 *\return		Les paramètres des ombres.
		 */
		inline Shadow const & GetShadow()const
		{
			return m_shadow;
		}
		/**
		 *\~english
		 *\return		The shadows parameters.
		 *\~french
		 *\return		Les paramètres des ombres.
		 */
		inline Shadow & GetShadow()
		{
			return m_shadow;
		}
		/**
		 *\~english
		 *\return		The scene's frame listener.
		 *\~french
		 *\return		Le frame listener de la scène.
		 */
		inline FrameListener const & GetListener()const
		{
			REQUIRE( !m_listener.expired() );
			return *m_listener.lock();
		}
		/**
		 *\~english
		 *\return		The scene's frame listener.
		 *\~french
		 *\return		Le frame listener de la scène.
		 */
		inline FrameListener & GetListener()
		{
			REQUIRE( !m_listener.expired() );
			return *m_listener.lock();
		}
		/**
		 *\~english
		 *\return		\p true if the scene is initialised.
		 *\~french
		 *\return		\p true si la scène est initialisée.
		 */
		inline bool IsInitialised()const
		{
			return m_initialised;
		}
		/**
		 *\~english
		 *\brief		Sets the exposure value.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\brief		Définit la valeur de l'exposition.
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		inline void SetExposure( float p_value )
		{
			m_config.SetExposure( p_value );
		}
		/**
		 *\~english
		 *\brief		Sets the gamma correction value.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\brief		Définit la valeur de la correction gamma.
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		inline void SetGamma( float p_value )
		{
			m_config.SetGamma( p_value );
		}
		/**
		 *\~english
		 *\return		The HDR configuration.
		 *\~french
		 *\return		La configuration HDR.
		 */
		inline HdrConfig const & GetHdrConfig()const
		{
			return m_config;
		}

	public:
		//!\~english	The signal raised when the scene has changed.
		//!\~french		Le signal levé lorsque la scène a changé.
		OnChanged onChanged;

	private:
		//!\~english	Tells if the scene is initialised.
		//!\~french		Dit si la scène est initialisée.
		bool m_initialised{ false };
		//!\~english	The root node
		//!\~french		Le noeud père de tous les noeuds de la scène
		SceneNodeSPtr m_rootNode;
		//!\~english	The root node used only for cameras (used to ease the use of cameras)
		//!\~french		Le noeud père de tous les noeuds de caméra
		SceneNodeSPtr m_rootCameraNode;
		//!\~english	The root node for every object other than camera (used to ease the use of cameras)
		//!\~french		Le noeud père de tous les noeuds d'objet
		SceneNodeSPtr m_rootObjectNode;
		//!\~english	The scene nodes cache.
		//!\~french		Le cache de noeuds de scène.
		DECLARE_OBJECT_CACHE_MEMBER( sceneNode, SceneNode );
		//!\~english	The camera cache.
		//!\~french		Le cache de caméras.
		DECLARE_OBJECT_CACHE_MEMBER( camera, Camera );
		//!\~english	The lights cache.
		//!\~french		Le cache de lumières.
		DECLARE_OBJECT_CACHE_MEMBER( light, Light );
		//!\~english	The geometies cache.
		//!\~french		Le cache de géométries.
		DECLARE_OBJECT_CACHE_MEMBER( geometry, Geometry );
		//!\~english	The billboards cache.
		//!\~french		Le cache de billboards.
		DECLARE_OBJECT_CACHE_MEMBER( billboard, BillboardList );
		//!\~english	The particle systems cache.
		//!\~french		Le cache de systèmes de particules.
		DECLARE_OBJECT_CACHE_MEMBER( particleSystem, ParticleSystem );
		//!\~english	The meshes cache.
		//!\~french		Le cache de maillages.
		DECLARE_CACHE_MEMBER( mesh, Mesh );
		//!\~english	The animated objects groups cache.
		//!\~french		Le cache de groupes d'objets animés.
		DECLARE_CACHE_MEMBER( animatedObjectGroup, AnimatedObjectGroup );
		//!\~english	The overlays view.
		//!\~french		La vue sur le incrustations de la scène.
		DECLARE_CACHE_VIEW_MEMBER( overlay, Overlay, EventType::ePreRender );
		//!\~english	The scene materials view.
		//!\~french		La vue sur les matériaux de la scène.
		DECLARE_CACHE_VIEW_MEMBER( material, Material, EventType::ePreRender );
		//!\~english	The scene samplers view.
		//!\~french		La vue sur les échantillonneurs de la scène.
		DECLARE_CACHE_VIEW_MEMBER( sampler, Sampler, EventType::ePreRender );
		//!\~english	The scene fonts view.
		//!\~french		La vue sur les polices de la scène.
		DECLARE_CACHE_VIEW_MEMBER_CU( font, Font, EventType::ePreRender );
		//!\~english	Tells if the scene has changed, id est if a geometry has been created or added to it => Vertex buffers need to be generated
		//!\~french		Dit si la scène a changé (si des géométries ont besoin d'être initialisées, essentiellement).
		bool m_changed{ false };
		//!\~english	Ambient light color
		//!\~french		Couleur de la lumière ambiante
		Castor::Colour m_ambientLight;
		//!\~english	The scene background colour
		//!\~french		La couleur de fond de la scène
		Castor::Colour m_backgroundColour;
		//!\~english	The background image
		//!\~french		L'image de fond
		TextureLayoutSPtr m_backgroundImage;
		//!\~english	The skybox
		//!\~french		La skybox
		SkyboxUPtr m_skybox;
		//!\~english	The LightCategory factory.
		//!\~french		La fabrique de LightCategory.
		LightFactory m_lightFactory;
		//!\~english	The fog's parameters.
		//!\~french		Les paramètres de brouillard.
		Fog m_fog;
		//!\~english	The shadows parameters.
		//!\~french		Les paramètres des ombres.
		Shadow m_shadow;
		//!\~english	The pipeline used to render the background image, if any.
		//!\~french		Le pipeline utilisé pour le rendu de l'image de fond, s'il y en a une.
		std::unique_ptr< TextureProjection > m_colour;
		//!\~english	The frame listener for the scene.
		//!\~french		Le frame listener pour la scène.
		FrameListenerWPtr m_listener;
		//!\~english	The scene's reflection maps.
		//!\~french		Les reflection maps de la scène.
		std::map< SceneNode const *, std::unique_ptr< EnvironmentMap > > m_reflectionMaps;
		//!\~english	The scene's reflection maps.
		//!\~french		Les reflection maps de la scène.
		std::vector< std::reference_wrapper< EnvironmentMap > > m_reflectionMapsArray;
		//!\~english	The HDR configuration.
		//!\~french		La configuration HDR.
		HdrConfig m_config;

	public:
		//!\~english	The cameras root node name.
		//!\~french		Le nom du noeud de scène racine des caméras.
		static Castor::String CameraRootNode;
		//!\~english	The objects root node name.
		//!\~french		Le nom du noeud de scène racine des objets.
		static Castor::String ObjectRootNode;
		//!\~english	The root node name.
		//!\~french		Le nom du noeud de scène racine.
		static Castor::String RootNode;
	};
}

#endif
