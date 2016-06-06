/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___C3D_SCENE_H___
#define ___C3D_SCENE_H___

#include "Castor3DPrerequisites.hpp"
#include "SceneNode.hpp"

#include <Logger.hpp>
#include <OwnedBy.hpp>

#define DECLARE_MANAGER_MEMBER( memberName, className )\
	public:\
		inline className##Manager & Get##className##Manager()\
		{\
			return *m_##memberName##Manager;\
		}\
		inline className##Manager const & Get##className##Manager()const\
		{\
			return *m_##memberName##Manager;\
		}\
	private:\
		className##ManagerUPtr m_##memberName##Manager

#define DECLARE_MANAGER_VIEW_MEMBER( memberName, className, eventType )\
	public:\
		inline ManagerView< className, className##Manager, eventType > & Get##className##View()\
		{\
			return *m_##memberName##ManagerView;\
		}\
		inline ManagerView< className, className##Manager, eventType > const & Get##className##View()const\
		{\
			return *m_##memberName##ManagerView;\
		}\
	private:\
		std::unique_ptr< ManagerView< className, className##Manager, eventType > > m_##memberName##ManagerView

#define DECLARE_MANAGER_VIEW_MEMBER_CU( memberName, className, eventType )\
	public:\
		inline ManagerView< Castor::className, Castor::className##Manager, eventType > & Get##className##View()\
		{\
			return *m_##memberName##ManagerView;\
		}\
		inline ManagerView< Castor::className, Castor::className##Manager, eventType > const & Get##className##View()const\
		{\
			return *m_##memberName##ManagerView;\
		}\
	private:\
		std::unique_ptr< ManagerView< Castor::className, Castor::className##Manager, eventType > > m_##memberName##ManagerView

#define DECLARE_MANAGER_VIEW_MEMBER_EX( memberName, mgrName, className, eventType )\
	public:\
		inline ManagerView< className, mgrName##Manager, eventType > & Get##className##View()\
		{\
			return *m_##memberName##ManagerView;\
		}\
		inline ManagerView< className, mgrName##Manager, eventType > const & Get##className##View()const\
		{\
			return *m_##memberName##ManagerView;\
		}\
	private:\
		std::unique_ptr< ManagerView< className, mgrName##Manager, eventType > > m_##memberName##ManagerView

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
		 *\brief		Renders the scene background.
		 *\param[in]	p_size		The target dimensions.
		 *\param[in]	p_pipeline	The render pipeline.
		 *\~french
		 *\brief		Rend le fond de la scène.
		 *\param[in]	p_size		Les dimensions de la cible.
		 *\param[in]	p_pipeline	Le pipeline de rend.
		 */
		C3D_API void RenderBackground( Castor::Size const & p_size, Pipeline & p_pipeline );
		/**
		 *\~english
		 *\brief		Renders the scene foreground (skybox).
		 *\param[in]	p_size		The target dimensions.
		 *\param[in]	p_pipeline	The render pipeline.
		 *\~french
		 *\brief		Rend le devant de la scène (skybox).
		 *\param[in]	p_size		Les dimensions de la cible.
		 *\param[in]	p_pipeline	Le pipeline de rend.
		 */
		C3D_API void RenderForeground( Castor::Size const & p_size, Camera const & p_camera, Pipeline & p_pipeline );
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
		C3D_API bool SetForeground( SkyboxSPtr p_skybox );
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
		 *\brief		Mesh import Function.
		 *\param[in]	p_fileName		The location of the file to import.
		 *\param[in]	p_importer		The importer.
		 *\param[in]	p_parameters	Import configuration parameters.
		 *\return		The imported Mesh
		 *\~french
		 *\brief		Fonction d'import de Mesh.
		 *\param[in]	p_fileName		Le chemin vers le fichier à importer.
		 *\param[in]	p_importer		L'importeur.
		 *\param[in]	p_parameters	Paramètres de configuration de l'import.
		 *\return		Le Mesh importé
		 */
		C3D_API MeshSPtr ImportMesh( Castor::Path const & p_fileName, Importer & p_importer, Parameters const & p_parameters );
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
		 *\brief		Sets the background colour
		 *\param[in]	p_clrNew	The new colour
		 *\~french
		 *\brief		Définit la couleur du fond
		 *\param[in]	p_clrNew	La nouvelle couleur
		 */
		void SetBackgroundColour( Castor::Colour const & p_clrNew )
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
		Castor::Colour const & GetBackgroundColour()const
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
		 *\return		The skybox.
		 *\~french
		 *\return		La skybox.
		 */
		inline SkyboxSPtr GetSkybox()const
		{
			return m_skybox;
		}

		//@}

	private:
		//!\~english	The root node
		//!\~french		Le noeud père de tous les noeuds de la scène
		SceneNodeSPtr m_rootNode;
		//!\~english	The root node used only for cameras (used to ease the use of cameras)
		//!\~french		Le noeud père de tous les noeuds de caméra
		SceneNodeSPtr m_rootCameraNode;
		//!\~english	The root node for every object other than camera (used to ease the use of cameras)
		//!\~french		Le noeud père de tous les noeuds d'objet
		SceneNodeSPtr m_rootObjectNode;
		//!\~english	The scene nodes manager.
		//!\~french		Le gestionnaire de noeuds de scène.
		DECLARE_MANAGER_MEMBER( sceneNode, SceneNode );
		//!\~english	The camera manager.
		//!\~french		Le gestionnaire de caméras.
		DECLARE_MANAGER_MEMBER( camera, Camera );
		//!\~english	The lights manager.
		//!\~french		Le gestionnaire de lumières.
		DECLARE_MANAGER_MEMBER( light, Light );
		//!\~english	The geometies manager.
		//!\~french		Le gestionnaire de géométries.
		DECLARE_MANAGER_MEMBER( geometry, Geometry );
		//!\~english	The billboards manager.
		//!\~french		Le gestionnaire de billboards.
		DECLARE_MANAGER_MEMBER( billboard, Billboard );
		//!\~english	The animated objects groups manager.
		//!\~french		Le gestionnaire de groupes d'objets animés.
		DECLARE_MANAGER_MEMBER( animatedObjectGroup, AnimatedObjectGroup );
		//!\~english	The overlays view.
		//!\~french		La vue sur le incrustations de la scène.
		DECLARE_MANAGER_VIEW_MEMBER( overlay, Overlay, eEVENT_TYPE_PRE_RENDER );
		//!\~english	The scene meshes view.
		//!\~french		La vue sur les maillages de la scène.
		DECLARE_MANAGER_VIEW_MEMBER( mesh, Mesh, eEVENT_TYPE_PRE_RENDER );
		//!\~english	The scene materials view.
		//!\~french		La vue sur les matériaux de la scène.
		DECLARE_MANAGER_VIEW_MEMBER( material, Material, eEVENT_TYPE_PRE_RENDER );
		//!\~english	The scene samplers view.
		//!\~french		La vue sur les échantillonneurs de la scène.
		DECLARE_MANAGER_VIEW_MEMBER( sampler, Sampler, eEVENT_TYPE_PRE_RENDER );
		//!\~english	The scene render windows view.
		//!\~french		La vue sur les fenêtres de rendu de la scène.
		DECLARE_MANAGER_VIEW_MEMBER_EX( window, Window, RenderWindow, eEVENT_TYPE_POST_RENDER );
		//!\~english	The scene fonts view.
		//!\~french		La vue sur les polices de la scène.
		DECLARE_MANAGER_VIEW_MEMBER_CU( font, Font, eEVENT_TYPE_PRE_RENDER );
		//!\~english	Tells if the scene has changed, id est if a geometry has been created or added to it => Vertex buffers need to be generated
		//!\~french		Dit si la scène a changé (si des géométries ont besoin d'être initialisées, essentiellement).
		bool m_changed;
		//!\~english	Ambient light color
		//!\~french		Couleur de la lumière ambiante
		Castor::Colour m_ambientLight;
		//!\~english	The mutex, to make the Scene threadsafe
		//!\~french		Le mutex protégeant les données de la scène
		mutable std::recursive_mutex m_mutex;
		//!\~english	The overlays array
		//!\~french		Le tableau d'overlays
		OverlayPtrArray m_overlays;
		//!\~english	The scene background colour
		//!\~french		La couleur de fond de la scène
		Castor::Colour m_backgroundColour;
		//!\~english	The background image
		//!\~french		L'image de fond
		TextureLayoutSPtr m_backgroundImage;
		//!\~english	The skybox
		//!\~french		La skybox
		SkyboxSPtr m_skybox;

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

#undef DECLARE_MANAGER_MEMBER

#endif
