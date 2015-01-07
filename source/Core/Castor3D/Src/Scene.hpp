/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

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

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Scene handler class
	\remark		A scene is a collection of lights, scene nodes and geometries.
				<br />It has at least one camera to render it
	\~french
	\brief		Classe de gestion d'un scène
	\remark		Une scène est une collection de lumières, noeuds et géométries.
				<br />Elle a au moins une caméra permettant son rendu
	*/
	class C3D_API Scene
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
		class C3D_API TextLoader
			:	public Castor::Loader< Scene, Castor::eFILE_TYPE_TEXT, Castor::TextFile >
			,	public Castor::NonCopyable
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			TextLoader( Castor::File::eENCODING_MODE p_eEncodingMode = Castor::File::eENCODING_MODE_ASCII );
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
			virtual bool operator()( Scene const & p_scene, Castor::TextFile & p_file );
		};
		/*!
		\author		Sylvain DOREMUS
		\date		08/04/2014
		\~english
		\brief		Sampler loader
		\~english
		\brief		Loader de Sampler
		*/
		class C3D_API BinaryParser
			:	public Castor3D::BinaryParser< Scene >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\param[in]	p_path	The current folder path
			 *\~french
			 *\brief		Constructeur
			 *\param[in]	p_path	Le chemin d'accès au dossier courant
			 */
			BinaryParser( Castor::Path const & p_path );
			/**
			 *\~english
			 *\brief		Function used to fill the chunk from specific data
			 *\param[in]	p_obj	The object to write
			 *\param[out]	p_chunk	The chunk to fill
			 *\return		\p false if any error occured
			 *\~french
			 *\brief		Fonction utilisée afin de remplir le chunk de données spécifiques
			 *\param[in]	p_obj	L'objet à écrire
			 *\param[out]	p_chunk	Le chunk à remplir
			 *\return		\p false si une erreur quelconque est arrivée
			 */
			virtual bool Fill( Scene const & p_obj, BinaryChunk & p_chunk )const;
			/**
			 *\~english
			 *\brief		Function used to retrieve specific data from the chunk
			 *\param[out]	p_obj	The object to read
			 *\param[in]	p_chunk	The chunk containing data
			 *\return		\p false if any error occured
			 *\~french
			 *\brief		Fonction utilisée afin de récupérer des données spécifiques à partir d'un chunk
			 *\param[out]	p_obj	L'objet à lire
			 *\param[in]	p_chunk	Le chunk contenant les données
			 *\return		\p false si une erreur quelconque est arrivée
			 */
			virtual bool Parse( Scene & p_obj, BinaryChunk & p_chunk )const;
		};

	private:
		/*!
		\author 	Sylvain DOREMUS
		\date
		\~english
		\brief		Helper structure used to sort submeshes
		\~french
		\brief		Structure d'aide utilisée lors du tri des sous-maillages
		\remark
		*/
		struct stRENDER_NODE
		{
			//!\~english The parent mesh node	\~french Le node du mesh parent
			SceneNodeRPtr m_pNode;
			//!\~english The geometry instanciating the submesh	\~french La géométrie instanciant le submesh
			GeometrySPtr m_pGeometry;
			//!\~english The submesh	\~french Le sous-maillage
			SubmeshSPtr m_pSubmesh;
			//!\~english The material	\~french Le matériau
			MaterialSPtr m_pMaterial;
		};
		//!\~english Multimap of stRENDER_NODEs sorted by distance	\~french Multimap de stRENDER_NODEs triés par distance
		DECLARE_MULTIMAP( double, stRENDER_NODE, RenderNodeByDistance );
		//!\~english stRENDER_NODE array	\~french tableau de stRENDER_NODE
		DECLARE_VECTOR( stRENDER_NODE, RenderNode );
		//!\~english SceneNode array	\~french Tableau de SceneNode
		DECLARE_VECTOR( SceneNode *, SceneNode );
		//!\~english Submesh multimap, with their nodes 	\~french Multimap de sous-maillages, avec leurs SceneNodes
		DECLARE_MAP( SubmeshSPtr, RenderNodeArray, SubmeshNodes );
		//!\~english Pass sorted SubmeshNodeMMap map	\~french Map de SubmeshNodeMMap, triés par passe
		DECLARE_MAP( MaterialSPtr, SubmeshNodesMap, SubmeshNodesByMaterial );

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pEngine		The core engine
		 *\param[in]	p_lightFactory	The factory used to create the lights
		 *\param[in]	p_name			The scene name
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pEngine		Le moteur
		 *\param[in]	p_lightFactory	La fabrique utilisée pour créer les lumières
		 *\param[in]	p_name			Le nom de la scène
		 */
		Scene( Engine * p_pEngine, LightFactory & p_lightFactory, Castor::String const & p_name = Castor::cuEmptyString );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~Scene();
		/**
		 *\~english
		 *\brief		Clears the maps, leaves the root nodes
		 *\~french
		 *\brief		Vide les maps, laisse les noeuds pères
		 */
		void ClearScene();
		/**
		 *\~english
		 *\brief		Renders the scene background
		 *\param[in]	p_camera	The camera from which the render is made
		 *\~french
		 *\brief		Rend le fond de la scène
		 *\param[in]	p_camera	La caméra utilisée pour le rendu
		 */
		void RenderBackground( Camera const & p_camera );
		/**
		 *\~english
		 *\brief		Renders the scene in a given display mode
		 *\param[in]	p_displayMode	The mode in which the display must be made
		 *\param[in]	p_dFrameTime	The time elapsed since the last frame was rendered
		 *\param[in]	p_camera		The camera from which the render is made
		 *\~french
		 *\brief		Rend la scène dans un mode d'affichage donné
		 *\param[in]	p_displayMode	Le mode d'affichage
		 *\param[in]	p_dFrameTime	Le temps écoulé depuis le rendu de la frame précédente
		 *\param[in]	p_camera		La caméra utilisée pour le rendu
		 */
		void Render( eTOPOLOGY p_displayMode, double p_dFrameTime, Camera const & p_camera );
		/**
		 *\~english
		 *\brief		Sets the background image for the scene
		 *\param[in]	p_pathFile	The image file path
		 *\~french
		 *\brief		Définit l'image de fond pour la scène
		 *\param[in]	p_pathFile	Le chemin d'accès à l'image
		 */
		bool SetBackgroundImage( Castor::Path const & p_pathFile );
		/**
		 *\~english
		 *\brief		Creates a scene node in the scene, attached to the root node if th given parent is nullptr
		 *\param[in]	p_name		The node name, default is empty
		 *\param[in]	p_parent	The parent node, if nullptr, the created node will be attached to root
		 *\~french
		 *\brief		Crée un SceneNode
		 *\remark		Si le parent donné est nul, le SceneNode créé sera attaché au root node
		 *\param[in]	p_name		Le nom du node
		 *\param[in]	p_parent	Le parent du node
		 */
		SceneNodeSPtr CreateSceneNode( Castor::String const & p_name, SceneNode * p_parent = NULL );
		/**
		 *\~english
		 *\brief		Creates a scene node in the scene, attached to the root node if th given parent is nullptr
		 *\param[in]	p_name		The node name, default is empty
		 *\param[in]	p_parent	The parent node, if nullptr, the created node will be attached to root
		 *\~french
		 *\brief		Crée un SceneNode
		 *\remark		Si le parent donné est nul, le SceneNode créé sera attaché au root node
		 *\param[in]	p_name		Le nom du node
		 *\param[in]	p_parent	Le parent du node
		 */
		SceneNodeSPtr CreateSceneNode( Castor::String const & p_name, SceneNodeSPtr p_parent );
		/**
		 *\~english
		 *\brief		Creates a geometry, given a MeshType and the geometry definitions
		 *\param[in]	p_name		The geometry name
		 *\param[in]	p_type		The geometry mesh type (plane, cube, torus...)
		 *\param[in]	p_meshName	The mesh name, creates a new mesh if it doesn't exist
		 *\param[in]	p_faces		The faces numbers
		 *\param[in]	p_size		The geometry dimensions
		 *\~french
		 *\brief		Crée une géométrie
		 *\param[in]	p_name		Le nom de la géométrie
		 *\param[in]	p_type		Le type de mesh
		 *\param[in]	p_meshName	Le nom du mesh, un nouveau mesh est créé si celui donné n'existe pas
		 *\param[in]	p_faces		Les nombres de faces
		 *\param[in]	p_size		Les dimensions
		 */
		GeometrySPtr CreateGeometry( Castor::String const & p_name, eMESH_TYPE p_type, Castor::String const & p_meshName, UIntArray p_faces, RealArray p_size );
		/**
		 *\~english
		 *\brief		Creates a geometry, with no mesh
		 *\remark		The geometry is not added to the geometries of the scene. Call AddPrimitive to do that
		 *\param[in]	p_name	The primitive name
		 *\~french
		 *\brief		Crée une géométrie
		 *\remark		La géométrie n'est pas ajoutée aux géoméétries de la scène, il faut donc appeler AddPrimitive pour ce faire.
		 *\param[in]	p_name	Le nom de la géométrie
		 */
		GeometrySPtr CreateGeometry( Castor::String const & p_name );
		/**
		 *\~english
		 *\brief		Creates a camera
		 *\param[in]	p_name		The camera name
		 *\param[in]	p_ww, p_wh	The window size
		 *\param[in]	p_pNode		The camera's parent node
		 *\param[in]	p_type		The viewport projection type
		 *\~french
		 *\brief		Crée une caméra
		 *\param[in]	p_name		Le nom de la caméra
		 *\param[in]	p_ww, p_wh	Les dimensions de la fenêtre
		 *\param[in]	p_pNode		Le node auquel attacher la caméra
		 *\param[in]	p_type		Le type de projection du viewport
		 */
		CameraSPtr CreateCamera( Castor::String const & p_name, int p_ww, int p_wh, SceneNodeSPtr p_pNode, eVIEWPORT_TYPE p_type );
		/**
		 *\~english
		 *\brief		Creates a camera
		 *\param[in]	p_name		The camera name
		 *\param[in]	p_pNode		The camera's parent node
		 *\param[in]	p_pViewport	The viewport
		 *\~french
		 *\brief		Crée une caméra
		 *\param[in]	p_name		Le nom de la caméra
		 *\param[in]	p_pNode		Le node auquel attacher la caméra
		 *\param[in]	p_pViewport	Le viewport
		 */
		CameraSPtr CreateCamera( Castor::String const & p_name, SceneNodeSPtr p_pNode, ViewportSPtr p_pViewport );
		/**
		 *\~english
		 *\brief		Creates a light
		 *\param[in]	p_name			The light name
		 *\param[in]	p_pNode			The light's parent node
		 *\param[in]	p_eLightType	The light type
		 *\~french
		 *\brief		Crée une lumière
		 *\param[in]	p_name			Le nom de la lumière
		 *\param[in]	p_pNode			Le node auquel attacher la lumière
		 *\param[in]	p_eLightType	Le type de la lumière
		 */
		LightSPtr CreateLight( Castor::String const & p_name, SceneNodeSPtr p_pNode, eLIGHT_TYPE p_eLightType );
		/**
		 *\~english
		 *\brief		Creates an animated object group
		 *\param[in]	p_name	The group name
		 *\~french
		 *\brief		Crée un groupe d'objets animés
		 *\param[in]	p_name	Le nom du groupe
		 */
		AnimatedObjectGroupSPtr CreateAnimatedObjectGroup( Castor::String const & p_name );
		/**
		 *\~english
		 *\brief		Initialises geometries
		 *\~french
		 *\brief		Initialise les géométries
		 */
		void InitialiseGeometries();
		/**
		 *\~english
		 *\brief		Adds a node to the scene
		 *\param[in]	p_node	The node to add
		 *\~french
		 *\brief		Ajoute un node à la scène
		 *\param[in]	p_node	Le node
		 */
		void AddNode( SceneNodeSPtr p_node );
		/**
		 *\~english
		 *\brief		Adds a light to the scene
		 *\param[in]	p_light	The light to add
		 *\~french
		 *\brief		Ajoute une lumière à la scène
		 *\param[in]	p_light	La lumière
		 */
		void AddLight( LightSPtr p_light );
		/**
		 *\~english
		 *\brief		Adds a geometry to the scene
		 *\param[in]	p_geometry	The geometry to add
		 *\~french
		 *\brief		Ajoute une géométrie à la scène
		 *\param[in]	p_geometry	La géométrie
		 */
		void AddGeometry( GeometrySPtr p_geometry );
		/**
		 *\~english
		 *\brief		Adds a billboards list to the scene
		 *\param[in]	p_pList	The billboards list to add
		 *\~french
		 *\brief		Ajoute une liste de billboards à la scène
		 *\param[in]	p_pList	La liste de billboards
		 */
		void AddBillboards( BillboardListSPtr p_pList );
		/**
		 *\~english
		 *\brief		Adds an animated object group
		 *\param[in]	p_pGroup	The animated object group
		 *\~french
		 *\brief		Ajoute un groupe d'objets animés
		 *\param[in]	p_pGroup	Le groupe d'objets animés
		 */
		void AddAnimatedObjectGroup( AnimatedObjectGroupSPtr p_pGroup );
		/**
		 *\~english
		 *\brief		Retrieves the node with the given name
		 *\param[in]	p_name	The name of the node
		 *\return		The named node, nullptr if not found
		 *\~french
		 *\brief		Récupère le node ayant le nom donné
		 *\param[in]	p_name	Le nom
		 *\return		Le node, nullptr si non trouvé
		 */
		SceneNodeSPtr GetNode( Castor::String const & p_name )const;
		/**
		 *\~english
		 *\brief		Retrieves the geometry with the given name
		 *\param[in]	p_name	the name of the geometry
		 *\return		The named geometry, nullptr if not found
		 *\~french
		 *\brief		Récupère la géométrie ayant le nom donné
		 *\param[in]	p_name	Le nom
		 *\return		La géométrie, nullptr si non trouvée
		 */
		GeometrySPtr GetGeometry( Castor::String const & p_name )const;
		/**
		 *\~english
		 *\brief		Retrieves the light with the given name
		 *\param[in]	p_name	the name of the light
		 *\return		The named light, nullptr if not found
		 *\~french
		 *\brief		Récupère la lumière ayant le nom donné
		 *\param[in]	p_name	Le nom
		 *\return		La lumière, nullptr si non trouvée
		 */
		LightSPtr GetLight( Castor::String const & p_name )const;
		/**
		 *\~english
		 *\brief		Retrieves the billboards list with the given name
		 *\param[in]	p_name	the name of the billboards list
		 *\return		The named billboards list, nullptr if not found
		 *\~french
		 *\brief		Récupère la liste de billboards ayant le nom donné
		 *\param[in]	p_name	Le nom
		 *\return		La liste de billboards, nullptr si non trouvée
		 */
		BillboardListSPtr GetBillboards( Castor::String const & p_name )const;
		/**
		 *\~english
		 *\brief		Retrieves an animated object group with the given name
		 *\param[in]	p_name	The name
		 *\return		The animated object group
		 *\~french
		 *\brief		Récupère un groupe d'objets animés ayant le nom donné
		 *\param[in]	p_name	Le nom
		 *\return		Le groupe d'objets animés
		 */
		AnimatedObjectGroupSPtr GetAnimatedObjectGroup( Castor::String const & p_name )const;
		/**
		 *\~english
		 *\brief		Retrieves the camera with the given name
		 *\param[in]	p_name	The name of the camera
		 *\return		The named camera, nullptr if not found
		 *\~french
		 *\brief		Récupère la caméra ayant le nom donné
		 *\param[in]	p_name	Le nom
		 *\return		La caméra, nullptr si non trouvée
		 */
		CameraSPtr GetCamera( Castor::String const & p_name )const;
		/**
		 *\~english
		 *\brief		Removes the light given in argument from the scene
		 *\param[in]	p_pLight	The light
		 *\~french
		 *\brief		Enlève la lumière donnée de la scène
		 *\param[in]	p_pLight	La lumière
		 */
		void RemoveLight( LightSPtr p_pLight );
		/**
		 *\~english
		 *\brief		Removes the node given in argument from the scene
		 *\param[in]	p_pNode	The node
		 *\~french
		 *\brief		Enlève le node donné de la scène
		 *\param[in]	p_pNode	La lumière
		 */
		void RemoveNode( SceneNodeSPtr p_pNode );
		/**
		 *\~english
		 *\brief		Removes the geometry given in argument from the scene and deletes it
		 *\param[in]	p_pGeometry	The geometry
		 *\~french
		 *\brief		Enlève la géométrie donnée de la scène
		 *\param[in]	p_pGeometry	La géométrie
		 */
		void RemoveGeometry( GeometrySPtr p_pGeometry );
		/**
		 *\~english
		 *\brief		Removes the billboards list given in argument from the scene and deletes it
		 *\param[in]	p_pGeometry	The billboards list
		 *\~french
		 *\brief		Enlève la liste de billboards donnée de la scène
		 *\param[in]	p_pGeometry	La liste de billboards
		 */
		void RemoveBillboards( BillboardListSPtr p_pList );
		/**
		 *\~english
		 *\brief		Removes the camera given in argument from the scene
		 *\param[in]	p_pCamera	The camera
		 *\~french
		 *\brief		Enlève la caméra donnée de la scène
		 *\param[in]	p_pCamera	La caméra
		 */
		void RemoveCamera( CameraSPtr p_pCamera );
		/**
		 *\~english
		 *\brief		Removes an animated object group from the scene
		 *\param[in]	p_pGroup	The animated object group
		 *\~french
		 *\brief		Enlève un groupe d'objets animés de la scène
		 *\param[in]	p_pGroup	Le groupe d'objets animés
		 */
		void RemoveAnimatedObjectGroup( AnimatedObjectGroupSPtr p_pGroup );
		/**
		 *\~english
		 *\brief		Removes all the lights from the scene
		 *\~french
		 *\brief		Enlève toutes les lumières de la scène
		 */
		void RemoveAllLights();
		/**
		 *\~english
		 *\brief		Removes all the nodes from the scene
		 *\~french
		 *\brief		Enlève tous les nodes de la scène
		 */
		void RemoveAllNodes();
		/**
		 *\~english
		 *\brief		Removes all the geometries from the scene
		 *\~french
		 *\brief		Enlève totues les géométries de la scène
		 */
		void RemoveAllGeometries();
		/**
		 *\~english
		 *\brief		Removes all the billboards lists from the scene
		 *\~french
		 *\brief		Enlève totues les listes de billboards de la scène
		 */
		void RemoveAllBillboards();
		/**
		 *\~english
		 *\brief		Removes all the cameras from the scene
		 *\~french
		 *\brief		Enlève toutes les caméras de la scène
		 */
		void RemoveAllCameras();
		/**
		 *\~english
		 *\brief		Removes all the billboards lists from the scene
		 *\~french
		 *\brief		Enlève totues les listes de billboards de la scène
		 */
		void RemoveAllAnimatedObjectGroups();
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
		bool ImportExternal( Castor::String const & p_fileName, Importer & p_importer );
		/**
		 *\~english
		 *\brief		Selects the nearest element in the ray's way
		 *\param[in]	p_ray		The ray
		 *\param[out]	p_geo		The nearest met geometry
		 *\param[out]	p_submesh	The nearest met submesh
		 *\param[out]	p_face		The nearest met Face
		 *\param[out]	p_vertex	The nearest met Vertex
		 *\~french
		 *\brief		Sélectionne l'élément le plus proche touché par le rayon
		 *\param[in]	p_ray		Le rayon
		 *\param[out]	p_geo		La géométrie la plus proche
		 *\param[out]	p_submesh	Le submesh le plus proche, dans la géométrie
		 *\param[out]	p_face		La face la plus proche, dans le submesh
		 *\param[out]	p_vertex	Le vertex le plus proche, dans la face
		 */
		void Select( Ray * p_ray, GeometrySPtr & p_geo, SubmeshSPtr & p_submesh, FaceSPtr * p_face, Vertex * p_vertex );
		/**
		 *\~english
		 *\brief		Merges the content of the given scene to this scene
		 *\param[in]	p_pScene	The scene to merge into this one
		 *\~french
		 *\brief		Intègre à cette scène le contenu de celle donnée
		 *\param[in]	p_pScene	La scène à intégrer
		 */
		void Merge( SceneSPtr p_pScene );
		/**
		 *\~english
		 *\brief		Clears the overlay list
		 *\~french
		 *\brief		Vide la liste des overlays contenus dans la scène
		 */
		void ClearOverlays();
		/**
		 *\~english
		 *\brief		Adds an overlay to the list
		 *\param[in]	p_pOverlay	The overlay to add
		 *\~french
		 *\brief		Ajoute un overlay à ceux déjà présents
		 *\param[in]	p_pOverlay	L'overlay
		 *\return
		 */
		void AddOverlay( OverlaySPtr p_pOverlay );
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
			m_clrBackground = p_clrNew;
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
			return m_clrBackground;
		}
		/**
		 *\~english
		 *\brief		Retrieves the scene name
		 *\return		The value
		 *\~french
		 *\brief		Récupère le nom de la scène
		 *\return		La valeur
		 */
		inline Castor::String const & GetName()const
		{
			return m_strName;
		}
		/**
		 *\~english
		 *\brief		Sets the scene name
		 *\param[in]	p_name	The value
		 *\~french
		 *\brief		Définit le nom de la scène
		 *\param[in]	p_name	La valeur
		 */
		inline void SetName( Castor::String const & p_name )
		{
			m_strName = p_name;
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
		inline TextureBaseSPtr GetBackgroundImage()const
		{
			return m_pBackgroundImage;
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
		 *\brief		Retrieves the ambient light colour
		 *\return		The value
		 *\~french
		 *\brief		Récupère la couleur de la lumière ambiante
		 *\return		La valeur
		 */
		inline Castor::Colour const & GetAmbientLight()const
		{
			return m_clAmbientLight;
		}
		/**
		 *\~english
		 *\brief		Retrieves the scene nodes count
		 *\return		The value
		 *\~french
		 *\brief		Récupère le nombre de SceneNodes
		 *\return		La valeur
		 */
		inline uint32_t GetNodesCount()const
		{
			return uint32_t( m_addedNodes.size() );
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the scene nodes map begin
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur le début de la map des SceneNodes
		 *\return		La valeur
		 */
		inline SceneNodePtrStrMap::iterator NodesBegin()
		{
			return m_addedNodes.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the scene nodes map begin
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur le début de la map des SceneNodes
		 *\return		La valeur
		 */
		inline SceneNodePtrStrMap::const_iterator NodesBegin()const
		{
			return m_addedNodes.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the scene nodes map end
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur la fin de la map des SceneNodes
		 *\return		La valeur
		 */
		inline SceneNodePtrStrMap::iterator NodesEnd()
		{
			return m_addedNodes.end();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the scene nodes map end
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur la fin de la map des SceneNodes
		 *\return		La valeur
		 */
		inline SceneNodePtrStrMap::const_iterator NodesEnd()const
		{
			return m_addedNodes.end();
		}
		/**
		 *\~english
		 *\brief		Retrieves the lights count
		 *\return		The value
		 *\~french
		 *\brief		Récupère le nombre de lumières
		 *\return		La valeur
		 */
		inline uint32_t GetLightsCount()const
		{
			return uint32_t( m_mapLights.size() );
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the lights map begin
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur le début de la map des lumières
		 *\return		La valeur
		 */
		inline LightPtrIntMap::iterator LightsBegin()
		{
			return m_mapLights.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the lights map begin
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur le début de la map des lumières
		 *\return		La valeur
		 */
		inline LightPtrIntMap::const_iterator LightsBegin()const
		{
			return m_mapLights.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the lights map end
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur la fin de la map des lumières
		 *\return		La valeur
		 */
		inline LightPtrIntMap::iterator LightsEnd()
		{
			return m_mapLights.end();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the lights map end
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur la fin de la map des lumières
		 *\return		La valeur
		 */
		inline LightPtrIntMap::const_iterator LightsEnd()const
		{
			return m_mapLights.end();
		}
		/**
		 *\~english
		 *\brief		Retrieves the geometries count
		 *\return		The value
		 *\~french
		 *\brief		Récupère le nombre de géométries
		 *\return		La valeur
		 */
		inline uint32_t GetGeometriesCount()const
		{
			return uint32_t( m_addedPrimitives.size() );
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the geometries map begin
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur le début de la map des géométries
		 *\return		La valeur
		 */
		inline GeometryPtrStrMap::iterator GeometriesBegin()
		{
			return m_addedPrimitives.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the geometries map begin
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur le début de la map des géométries
		 *\return		La valeur
		 */
		inline GeometryPtrStrMap::const_iterator GeometriesBegin()const
		{
			return m_addedPrimitives.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the geometries map end
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur la fin de la map des géométries
		 *\return		La valeur
		 */
		inline GeometryPtrStrMap::iterator GeometriesEnd()
		{
			return m_addedPrimitives.end();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the geometries map end
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur la fin de la map des géométries
		 *\return		La valeur
		 */
		inline GeometryPtrStrMap::const_iterator GeometriesEnd()const
		{
			return m_addedPrimitives.end();
		}
		/**
		 *\~english
		 *\brief		Retrieves the cameras count
		 *\return		The value
		 *\~french
		 *\brief		Récupère le nombre de caméras
		 *\return		La valeur
		 */
		inline uint32_t GetCamerasCount()const
		{
			return uint32_t( m_addedCameras.size() );
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the cameras map begin
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur le début de la map des caméras
		 *\return		La valeur
		 */
		inline CameraPtrStrMap::iterator CamerasBegin()
		{
			return m_addedCameras.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the cameras map begin
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur le début de la map des caméras
		 *\return		La valeur
		 */
		inline CameraPtrStrMap::const_iterator CamerasBegin()const
		{
			return m_addedCameras.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the cameras map end
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur la fin de la map des caméras
		 *\return		La valeur
		 */
		inline CameraPtrStrMap::iterator CamerasEnd()
		{
			return m_addedCameras.end();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the cameras map end
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur la fin de la map des caméras
		 *\return		La valeur
		 */
		inline CameraPtrStrMap::const_iterator CamerasEnd()const
		{
			return m_addedCameras.end();
		}
		/**
		 *\~english
		 *\brief		Retrieves the core engine
		 *\return		The value
		 *\~french
		 *\brief		Récupère le moteur
		 *\return		La valeur
		 */
		inline Engine * GetEngine()const
		{
			return m_pEngine;
		}
		/**
		 *\~english
		 *\brief		Sets the ambient light colour
		 *\param[in]	The new value
		 *\~french
		 *\brief		Définit la couleur de la lumière ambiante
		 *\param[in]	La nouvelle valeur
		 */
		inline void SetAmbientLight( Castor::Colour const & val )
		{
			m_clAmbientLight = val;
		}
		//@}

	private:
		void DoDeleteToDelete();
		void DoUpdateAnimations();
		void DoSortByAlpha();
		void DoRenderSubmeshesNonInstanced( Camera const & p_camera, Pipeline & p_pipeline, eTOPOLOGY p_displayMode, RenderNodeArrayConstIt p_begin, RenderNodeArrayConstIt p_end );
		void DoRenderSubmeshesInstanced( Camera const & p_camera, Pipeline & p_pipeline, eTOPOLOGY p_displayMode, SubmeshNodesByMaterialMapConstIt p_begin, SubmeshNodesByMaterialMapConstIt p_end );
		void DoRenderAlphaSortedSubmeshes( Pipeline & p_pipeline, eTOPOLOGY p_displayMode, RenderNodeByDistanceMMapConstIt p_begin, RenderNodeByDistanceMMapConstIt p_end );
		void DoResortAlpha( Camera const & p_camera, RenderNodeArrayIt p_begin, RenderNodeArrayIt p_end, RenderNodeByDistanceMMap & p_map, int p_sign );
		void DoRenderSubmeshInstancedMultiple( Pipeline & p_pipeline, RenderNodeArray const & p_nodes, eTOPOLOGY p_eTopology );
		void DoRenderSubmeshInstancedSingle( Pipeline & p_pipeline, stRENDER_NODE const & p_node, eTOPOLOGY p_eTopology );
		void DoRenderSubmeshNonInstanced( Pipeline & p_pipeline, stRENDER_NODE const & p_node, eTOPOLOGY p_eTopology );
		void DoRenderSubmesh( Pipeline & p_pipeline, stRENDER_NODE const & p_node, eTOPOLOGY p_eTopology );
		void DoRenderBillboards( Pipeline & p_pipeline, BillboardListStrMapIt p_itBegin, BillboardListStrMapIt p_itEnd );

		template< typename MapType >
		void DoMerge( SceneSPtr p_pScene, MapType & p_map, MapType & p_myMap )
		{
			Castor::String l_strName;

			for ( typename MapType::iterator l_it = p_map.begin(); l_it != p_map.end(); ++l_it )
			{
				if ( l_it->second->GetParent()->GetName() == cuT( "CameraRootNode" ) )
				{
					l_it->second->Detach();
					l_it->second->AttachTo( m_rootCameraNode.get() );
				}
				else if ( l_it->second->GetParent()->GetName() == cuT( "ObjectRootNode" ) )
				{
					l_it->second->Detach();
					l_it->second->AttachTo( m_rootObjectNode.get() );
				}

				l_strName = l_it->first;

				while ( p_myMap.find( l_strName ) != p_myMap.end() )
				{
					l_strName = p_pScene->GetName() + cuT( "_" ) + l_strName;
				}

				l_it->second->SetName( l_strName );
				p_myMap.insert( std::make_pair( l_strName, l_it->second ) );
			}

			p_map.clear();
		}

		template< typename MapType >
		void DoRemoveNodesAndUnattached( MapType & p_map )
		{
			std::set< Castor::String > l_setNodes;

			for ( typename MapType::iterator l_it = p_map.begin(); l_it != p_map.end(); ++l_it )
			{
				if ( l_it->second->GetParent() && l_setNodes.find( l_it->second->GetParent()->GetName() ) == l_setNodes.end() )
				{
					l_setNodes.insert( l_it->second->GetParent()->GetName() );
				}
			}

			SceneNodePtrStrMap::iterator l_itNode = m_addedNodes.begin();

			while ( l_itNode != m_addedNodes.end() )
			{
				if ( l_setNodes.find( l_itNode->first ) == l_setNodes.end() )
				{
					l_itNode->second->Detach();
					l_itNode->second.reset();
					m_addedNodes.erase( l_itNode );
					l_itNode = m_addedNodes.begin();
				}
				else
				{
					++l_itNode;
				}
			}

			for ( typename MapType::iterator l_it = p_map.begin(); l_it != p_map.end(); ++l_it )
			{
				if ( !l_it->second->GetParent() )
				{
					p_map.erase( l_it );
					l_it = p_map.begin();
				}
			}
		}
		template< class MapType, class ArrayType >
		void DoRemoveAll( MapType & p_map, ArrayType & p_array )
		{
			CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
			typename MapType::iterator l_it = p_map.begin();

			while ( p_map.size() > 0 )
			{
				l_it->second->Detach();
				p_array.push_back( l_it->second );
				l_it->second.reset();
				p_map.erase( l_it );
				l_it = p_map.begin();
			}
		}
		template< class ObjectType, typename KeyType >
		bool DoCheckObject( KeyType const & p_key, std::map< KeyType, std::shared_ptr< ObjectType > > & p_map, Castor::String const & p_strType )
		{
			bool l_bReturn = false;

			if ( !p_key.empty() )
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				typename std::map< KeyType, std::shared_ptr< ObjectType > >::const_iterator l_it = p_map.find( p_key );

				if ( l_it == p_map.end() )
				{
					l_bReturn = true;
				}
				else
				{
					Castor::String l_strLog = cuT( "Scene::CreateObject - Can't create " ) + p_strType + cuT( " [" ) + p_key + cuT( "] - A " ) + p_strType + cuT( " with that name already exists" );
					Castor::Logger::LogWarning( l_strLog );
				}
			}
			else
			{
				Castor::Logger::LogWarning( cuT( "Scene::CreateObject - The object name is void" ) );
			}

			return l_bReturn;
		}
		template< class ObjectType, typename KeyType >
		bool DoAddObject( std::shared_ptr< ObjectType > p_pObject, std::map< KeyType, std::shared_ptr< ObjectType > > & p_map, Castor::String const & p_strType )
		{
			bool l_bReturn = false;

			if ( p_pObject )
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				typename std::map< KeyType, std::shared_ptr< ObjectType > >::const_iterator l_it = p_map.find( p_pObject->GetName() );

				if ( l_it == p_map.end() )
				{
					p_map.insert( std::make_pair( p_pObject->GetName(), p_pObject ) );
					l_bReturn = true;
				}
				else
				{
					Castor::String l_strLog = cuT( "Scene::AddObject - Can't add " ) + p_strType + cuT( " [" ) + p_pObject->GetName() + cuT( "] - A " ) + p_strType + cuT( " with that name already exists" );
					Castor::Logger::LogWarning( l_strLog );
				}
			}
			else
			{
				Castor::Logger::LogWarning( cuT( "Scene::AddObject - The given object is null" ) );
			}

			return l_bReturn;
		}
		template< class ObjectType, typename KeyType >
		std::shared_ptr< ObjectType > DoGetObject( std::map< KeyType, std::shared_ptr< ObjectType > > const & p_map, KeyType const & p_key )const
		{
			CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
			typename std::shared_ptr< ObjectType > l_pReturn;
			typename std::map< KeyType, std::shared_ptr< ObjectType > >::const_iterator l_it = p_map.find( p_key );

			if ( l_it != p_map.end() )
			{
				l_pReturn = l_it->second;
			}

			return l_pReturn;
		}
		template< class ObjectType, typename KeyType >
		void DoRemoveObject( std::shared_ptr< ObjectType > p_pObject, std::map< KeyType, std::shared_ptr< ObjectType > > & p_map, std::vector< std::shared_ptr< ObjectType > > & p_array )
		{
			if ( p_pObject )
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				typename std::map< KeyType, std::shared_ptr< ObjectType > >::const_iterator l_it = p_map.find( p_pObject->GetName() );

				if ( l_it != p_map.end() )
				{
					p_map.erase( l_it );
				}

				p_array.push_back( p_pObject );
			}
			else
			{
				Castor::Logger::LogWarning( cuT( "Scene::RemoveObject - The given object is null" ) );
			}
		}

	private:
		//!\~english The scene name	\~french Le nom de la scène
		Castor::String m_strName;
		//!\~english The root node	\~french Le noeud père de tous les noeuds de la scène
		SceneNodeSPtr m_rootNode;
		//!\~english The root node used only for cameras (used to ease the use of cameras)	\~french Le noeud père de tous les noeuds de caméra
		SceneNodeSPtr m_rootCameraNode;
		//!\~english The root node for every object other than camera (used to ease the use of cameras)	\~french Le noeud père de tous les noeuds d'objet
		SceneNodeSPtr m_rootObjectNode;
		//!\~english The nodes map	\~french La map des noeuds
		SceneNodePtrStrMap m_addedNodes;
		//!\~english Added cameras	\~french Les caméras
		CameraPtrStrMap m_addedCameras;
		//!\~english Added lights	\~french Les lumières
		LightPtrStrMap m_addedLights;
		//!\~english Added primitives	\~french Les objets
		GeometryPtrStrMap m_addedPrimitives;
		//!\~english The lights to delete array	\~french Le tableau de lumières à détruire
		LightPtrArray m_arrayLightsToDelete;
		//!\~english The nodes to delete array	\~french Le tableau de noeuds à détruire
		SceneNodePtrArray m_arrayNodesToDelete;
		//!\~english The geometries to delete array	\~french Le tableau d'objets à détruire
		GeometryPtrArray m_arrayPrimitivesToDelete;
		//!\~english The cameras to delete array	\~french Le tableau de caméras à détruire
		CameraPtrArray m_arrayCamerasToDelete;
		//!\~english The newly added geometries, it is used to make the vertex buffer of them, then they are removed from the map.	\~french Les géométries nouvellement ajoutées.
		GeometryPtrStrMap m_newlyAddedPrimitives;
		//!\~english The added animated object groups, it is used to make the vertex buffer of them, then they are removed from the map.	\~french Les groupes d'objets animés.
		AnimatedObjectGroupPtrStrMap m_addedGroups;
		//!\~english The scene faces count	\~french Le nombre de faces dans la scène
		uint32_t m_nbFaces;
		//!\~english The scene vertices count	\~french Le nombre de vertices dans la scène
		uint32_t m_nbVertex;
		//!\~english Tells if the scene has changed, id est if a geometry has been created or added to it => Vertex buffers need to be generated	\~french Dit si la scène a changé (si des géométries ont besoin d'être initialisées, essentiellement).
		bool m_changed;
		//!\~english Ambient light color	\~french Couleur de la lumière ambiante
		Castor::Colour m_clAmbientLight;
		//!\~english The mutex, to make the Scene threadsafe	\~french Le mutex protégeant les données de la scène
		mutable std::recursive_mutex m_mutex;
		//!\~english The lights factory	\~french La fabrique de lumières
		LightFactory & m_lightFactory;
		//!\~english The overlays array	\~french Le tableau d'overlays
		OverlayPtrArray m_arrayOverlays;
		//!\~english The core engine	\~french Le moteur
		Engine * m_pEngine;
		//!\~english Lights map, ordered by index	\~french Map de lumières, triées par index
		std::map< int, LightSPtr > m_mapLights;
		//!\~english The geometries with no alpha blending, sorted by material	\~french Les géométries n'ayant pas d'alpha blend, triées par matériau
		SubmeshNodesByMaterialMap m_mapSubmeshesNoAlpha;
		//!\~english The geometries without alpha blending, unsorted	\~french Les géométries sans alpha blend, non triées
		RenderNodeArray m_arraySubmeshesNoAlpha;
		//!\~english The geometries with alpha blending, sorted by material	\~french Les géométries avec de l'alpha blend, triées par matériau
		SubmeshNodesByMaterialMap m_mapSubmeshesAlpha;
		//!\~english The geometries with alpha blending, sorted by distance to the camera	\~french Les géométries avec de l'alpha blend, triées par distance à la caméra
		RenderNodeByDistanceMMap m_mapSubmeshesAlphaSorted;
		//!\~english The geometries with alpha blending, unsorted	\~french Les géométries avec de l'alpha blend, non triées
		RenderNodeArray m_arraySubmeshesAlpha;
		//!\~english The scene background colour	\~french La couleur de fond de la scène
		Castor::Colour m_clrBackground;
		//!\~english The billboards lists	\~french Les listes de billboards
		BillboardListStrMap m_mapBillboardsLists;
		//!\~english The billboards lists to delete	\~french Les listes de billboards à détruire
		BillboardListArray m_arrayBillboardsToDelete;
		//!\~english The background image	\~french L'image de fond
		TextureBaseSPtr m_pBackgroundImage;
	};
}

#pragma warning( pop )

#endif
