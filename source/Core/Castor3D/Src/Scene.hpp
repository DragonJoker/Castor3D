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
#include <OwnedBy.hpp>

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
	{
		using LightsArray = std::vector< LightSPtr >;
		using LightsMap = std::map< eLIGHT_TYPE, LightsArray >;

	public:
		/*!
		\author		Sylvain DOREMUS
		\date		14/02/2010
		\~english
		\brief		Scene loader
		\~english
		\brief		Loader de scène
		*/
		class TextLoader
			: public Castor::Loader< Scene, Castor::eFILE_TYPE_TEXT, Castor::TextFile >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			C3D_API TextLoader( Castor::File::eENCODING_MODE p_encodingMode = Castor::File::eENCODING_MODE_ASCII );
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
			C3D_API virtual bool operator()( Scene const & p_scene, Castor::TextFile & p_file );
		};
		/*!
		\author		Sylvain DOREMUS
		\date		08/04/2014
		\~english
		\brief		Sampler loader
		\~english
		\brief		Loader de Sampler
		*/
		class BinaryParser
			: public Castor3D::BinaryParser< Scene >
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
			C3D_API BinaryParser( Castor::Path const & p_path );
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
			C3D_API virtual bool Fill( Scene const & p_obj, BinaryChunk & p_chunk )const;
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
			C3D_API virtual bool Parse( Scene & p_obj, BinaryChunk & p_chunk )const;
		};

	private:
		/*!
		\author 	Sylvain DOREMUS
		\date
		\~english
		\brief		Helper structure used to sort submeshes
		\~french
		\brief		Structure d'aide utilisée lors du tri des sous-maillages
		*/
		struct stRENDER_NODE
		{
			//!\~english The parent mesh node	\~french Le node du mesh parent
			SceneNodeSPtr m_node;
			//!\~english The geometry instanciating the submesh.	\~french La géométrie instanciant le submesh.
			GeometrySPtr m_geometry;
			//!\~english The submesh.	\~french Le sous-maillage.
			SubmeshSPtr m_submesh;
			//!\~english The material.	\~french Le matériau.
			MaterialSPtr m_material;
		};
		//!\~english Multimap of stRENDER_NODEs sorted by distance	\~french Multimap de stRENDER_NODEs triés par distance
		DECLARE_MULTIMAP( double, stRENDER_NODE, RenderNodeByDistance );
		//!\~english stRENDER_NODE array	\~french tableau de stRENDER_NODE
		DECLARE_VECTOR( stRENDER_NODE, RenderNode );
		//!\~english SceneNode array	\~french Tableau de SceneNode
		DECLARE_VECTOR( SceneNode *, SceneNode );
		//!\~english Submesh multimap, with their nodes 	\~french Multimap de sous-maillages, avec leurs SceneNodes
		DECLARE_MAP( SubmeshSPtr, RenderNodeArray, SubmeshRenderNodes );
		//!\~english Pass sorted SubmeshMaterialMMap map	\~french Map de SubmeshMaterialMMap, triés par passe
		DECLARE_MAP( MaterialSPtr, SubmeshRenderNodesMap, SubmeshRenderNodesByMaterial );

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_engine		The core engine
		 *\param[in]	p_name			The scene name
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_engine		Le moteur
		 *\param[in]	p_name			Le nom de la scène
		 */
		C3D_API Scene( Engine & p_engine, Castor::String const & p_name = Castor::cuEmptyString );
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
		 *\param[in]	p_size	The target dimensions.
		 *\~french
		 *\brief		Rend le fond de la scène.
		 *\param[in]	p_size	Les dimensions de la cible.
		 */
		C3D_API void RenderBackground( Castor::Size const & p_size );
		/**
		 *\~english
		 *\brief		Updates the scene before render.
		 *\~french
		 *\brief		Met à jour la scène avant le rendu.
		 */
		C3D_API void Update();
		/**
		 *\~english
		 *\brief		Renders the scene in a given display mode
		 *\param[in]	p_technique	The current rendering technique, used to select appropriate shaders
		 *\param[in]	p_camera	The camera from which the render is made
		 *\~french
		 *\brief		Rend la scène dans un mode d'affichage donné
		 *\param[in]	p_technique	La technique de rendu courante, utilisee pour recuperer les bons shaders
		 *\param[in]	p_camera	La caméra utilisée pour le rendu
		 */
		C3D_API void Render( RenderTechniqueBase & p_technique, Camera const & p_camera );
		/**
		 *\~english
		 *\brief		Sets the background image for the scene
		 *\param[in]	p_pathFile	The image file path
		 *\~french
		 *\brief		Définit l'image de fond pour la scène
		 *\param[in]	p_pathFile	Le chemin d'accès à l'image
		 */
		C3D_API bool SetBackgroundImage( Castor::Path const & p_pathFile );
		/**
		 *\~english
		 *\brief		Creates a scene node in the scene, attached to the root node
		 *\param[in]	p_name		The node name, default is empty
		 *\return		The created node
		 *\~french
		 *\brief		Crée un SceneNode
		 *\remarks		Le SceneNode créé sera attaché au root node
		 *\param[in]	p_name		Le nom du node
		 *\return		Le noeud créé
		 */
		C3D_API SceneNodeSPtr CreateSceneNode( Castor::String const & p_name );
		/**
		 *\~english
		 *\brief		Creates a scene node in the scene, attached to the root node if th given parent is nullptr
		 *\param[in]	p_name		The node name, default is empty
		 *\param[in]	p_parent	The parent node, if nullptr, the created node will be attached to root
		 *\~french
		 *\brief		Crée un SceneNode
		 *\remarks		Si le parent donné est nul, le SceneNode créé sera attaché au root node
		 *\param[in]	p_name		Le nom du node
		 *\param[in]	p_parent	Le parent du node
		 */
		C3D_API SceneNodeSPtr CreateSceneNode( Castor::String const & p_name, SceneNodeSPtr p_parent );
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
		C3D_API GeometrySPtr CreateGeometry( Castor::String const & p_name, eMESH_TYPE p_type, Castor::String const & p_meshName, UIntArray p_faces, RealArray p_size );
		/**
		 *\~english
		 *\brief		Creates a geometry, with no mesh
		 *\remarks		The geometry is not added to the geometries of the scene. Call AddPrimitive to do that
		 *\param[in]	p_name	The primitive name
		 *\~french
		 *\brief		Crée une géométrie
		 *\remarks		La géométrie n'est pas ajoutée aux géoméétries de la scène, il faut donc appeler AddPrimitive pour ce faire.
		 *\param[in]	p_name	Le nom de la géométrie
		 */
		C3D_API GeometrySPtr CreateGeometry( Castor::String const & p_name );
		/**
		 *\~english
		 *\brief		Creates a camera
		 *\param[in]	p_name		The camera name
		 *\param[in]	p_ww, p_wh	The window size
		 *\param[in]	p_node		The camera's parent node
		 *\~french
		 *\brief		Crée une caméra
		 *\param[in]	p_name		Le nom de la caméra
		 *\param[in]	p_ww, p_wh	Les dimensions de la fenêtre
		 *\param[in]	p_node		Le node auquel attacher la caméra
		 */
		C3D_API CameraSPtr CreateCamera( Castor::String const & p_name, int p_ww, int p_wh, SceneNodeSPtr p_node );
		/**
		 *\~english
		 *\brief		Creates a camera
		 *\param[in]	p_name		The camera name
		 *\param[in]	p_node		The camera's parent node
		 *\param[in]	p_viewport	The viewport
		 *\~french
		 *\brief		Crée une caméra
		 *\param[in]	p_name		Le nom de la caméra
		 *\param[in]	p_node		Le node auquel attacher la caméra
		 *\param[in]	p_viewport	Le viewport
		 */
		C3D_API CameraSPtr CreateCamera( Castor::String const & p_name, SceneNodeSPtr p_node, Viewport const & p_viewport );
		/**
		 *\~english
		 *\brief		Creates a light
		 *\param[in]	p_name			The light name
		 *\param[in]	p_node			The light's parent node
		 *\param[in]	p_eLightType	The light type
		 *\~french
		 *\brief		Crée une lumière
		 *\param[in]	p_name			Le nom de la lumière
		 *\param[in]	p_node			Le node auquel attacher la lumière
		 *\param[in]	p_eLightType	Le type de la lumière
		 */
		C3D_API LightSPtr CreateLight( Castor::String const & p_name, SceneNodeSPtr p_node, eLIGHT_TYPE p_eLightType );
		/**
		 *\~english
		 *\brief		Creates an animated object group
		 *\param[in]	p_name	The group name
		 *\~french
		 *\brief		Crée un groupe d'objets animés
		 *\param[in]	p_name	Le nom du groupe
		 */
		C3D_API AnimatedObjectGroupSPtr CreateAnimatedObjectGroup( Castor::String const & p_name );
		/**
		 *\~english
		 *\brief		Adds a node to the scene
		 *\param[in]	p_node	The node to add
		 *\~french
		 *\brief		Ajoute un node à la scène
		 *\param[in]	p_node	Le node
		 */
		C3D_API void AddNode( SceneNodeSPtr p_node );
		/**
		 *\~english
		 *\brief		Adds a light to the scene
		 *\param[in]	p_light	The light to add
		 *\~french
		 *\brief		Ajoute une lumière à la scène
		 *\param[in]	p_light	La lumière
		 */
		C3D_API void AddLight( LightSPtr p_light );
		/**
		 *\~english
		 *\brief		Adds a geometry to the scene
		 *\param[in]	p_geometry	The geometry to add
		 *\~french
		 *\brief		Ajoute une géométrie à la scène
		 *\param[in]	p_geometry	La géométrie
		 */
		C3D_API void AddGeometry( GeometrySPtr p_geometry );
		/**
		 *\~english
		 *\brief		Adds a billboards list to the scene
		 *\param[in]	p_pList	The billboards list to add
		 *\~french
		 *\brief		Ajoute une liste de billboards à la scène
		 *\param[in]	p_pList	La liste de billboards
		 */
		C3D_API void AddBillboards( BillboardListSPtr p_pList );
		/**
		 *\~english
		 *\brief		Adds an animated object group
		 *\param[in]	p_pGroup	The animated object group
		 *\~french
		 *\brief		Ajoute un groupe d'objets animés
		 *\param[in]	p_pGroup	Le groupe d'objets animés
		 */
		C3D_API void AddAnimatedObjectGroup( AnimatedObjectGroupSPtr p_pGroup );
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
		C3D_API SceneNodeSPtr GetNode( Castor::String const & p_name )const;
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
		C3D_API GeometrySPtr GetGeometry( Castor::String const & p_name )const;
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
		C3D_API LightSPtr GetLight( Castor::String const & p_name )const;
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
		C3D_API BillboardListSPtr GetBillboards( Castor::String const & p_name )const;
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
		C3D_API AnimatedObjectGroupSPtr GetAnimatedObjectGroup( Castor::String const & p_name )const;
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
		C3D_API CameraSPtr GetCamera( Castor::String const & p_name )const;
		/**
		 *\~english
		 *\brief		Removes the light given in argument from the scene
		 *\param[in]	p_pLight	The light
		 *\~french
		 *\brief		Enlève la lumière donnée de la scène
		 *\param[in]	p_pLight	La lumière
		 */
		C3D_API void RemoveLight( LightSPtr p_pLight );
		/**
		 *\~english
		 *\brief		Removes the node given in argument from the scene
		 *\param[in]	p_node	The node
		 *\~french
		 *\brief		Enlève le node donné de la scène
		 *\param[in]	p_node	La lumière
		 */
		C3D_API void RemoveNode( SceneNodeSPtr p_node );
		/**
		 *\~english
		 *\brief		Removes the geometry given in argument from the scene and deletes it
		 *\param[in]	p_pGeometry	The geometry
		 *\~french
		 *\brief		Enlève la géométrie donnée de la scène
		 *\param[in]	p_pGeometry	La géométrie
		 */
		C3D_API void RemoveGeometry( GeometrySPtr p_pGeometry );
		/**
		 *\~english
		 *\brief		Removes the billboards list given in argument from the scene and deletes it
		 *\param[in]	p_pGeometry	The billboards list
		 *\~french
		 *\brief		Enlève la liste de billboards donnée de la scène
		 *\param[in]	p_pGeometry	La liste de billboards
		 */
		C3D_API void RemoveBillboards( BillboardListSPtr p_pList );
		/**
		 *\~english
		 *\brief		Removes the camera given in argument from the scene
		 *\param[in]	p_pCamera	The camera
		 *\~french
		 *\brief		Enlève la caméra donnée de la scène
		 *\param[in]	p_pCamera	La caméra
		 */
		C3D_API void RemoveCamera( CameraSPtr p_pCamera );
		/**
		 *\~english
		 *\brief		Removes an animated object group from the scene
		 *\param[in]	p_pGroup	The animated object group
		 *\~french
		 *\brief		Enlève un groupe d'objets animés de la scène
		 *\param[in]	p_pGroup	Le groupe d'objets animés
		 */
		C3D_API void RemoveAnimatedObjectGroup( AnimatedObjectGroupSPtr p_pGroup );
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
		C3D_API bool ImportExternal( Castor::String const & p_fileName, Importer & p_importer );
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
		C3D_API void Select( Ray * p_ray, GeometrySPtr & p_geo, SubmeshSPtr & p_submesh, FaceSPtr * p_face, Vertex * p_vertex );
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
		 *\brief		Adds an overlay to the list
		 *\param[in]	p_overlay	The overlay to add
		 *\~french
		 *\brief		Ajoute un overlay à ceux déjà présents
		 *\param[in]	p_overlay	L'overlay
		 *\return
		 */
		C3D_API void AddOverlay( OverlaySPtr p_overlay );
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
		 *\brief		Binds the scene lights.
		 *\param[in]	p_program		The shader program.
		 *\param[in]	p_sceneBuffer	The constants buffer.
		 *\~french
		 *\brief		Attache les sources lumineuses
		 *\param[in]	p_program		Le programme shader.
		 *\param[in]	p_sceneBuffer	Le tampon de constantes.
		 */
		C3D_API void BindLights( ShaderProgramBase & p_program, FrameVariableBuffer & p_sceneBuffer );
		/**
		 *\~english
		 *\brief		Unbinds the scene lights.
		 *\param[in]	p_program		The shader program.
		 *\param[in]	p_sceneBuffer	The constants buffer.
		 *\~french
		 *\brief		Détache les sources lumineuses
		 *\param[in]	p_program		Le programme shader.
		 *\param[in]	p_sceneBuffer	Le tampon de constantes.
		 */
		C3D_API void UnbindLights( ShaderProgramBase & p_program, FrameVariableBuffer & p_sceneBuffer );
		/**
		 *\~english
		 *\brief		Bind the camera.
		 *\param[in]	p_sceneBuffer	The constants buffer.
		 *\~french
		 *\brief		Attache la caméra.
		 *\param[in]	p_sceneBuffer	Le tampon de constantes.
		 */
		C3D_API void BindCamera( FrameVariableBuffer & p_sceneBuffer );
		/**
		 *\~english
		 *\brief		Renders submeshes.
		 *\param[in]	p_technique		The render technique.
		 *\param[in]	p_pipeline		The render pipeline.
		 *\param[in]	p_begin			The render nodes begin.
		 *\param[in]	p_end			The render nodes end.
		 *\~french
		 *\brief		Dessine des sous maillages.
		 *\param[in]	p_technique		La technique de rendu.
		 *\param[in]	p_pipeline		Le pipeline de rendu.
		 *\param[in]	p_begin			Le début des noeuds de rendu.
		 *\param[in]	p_end			La fin des noeuds de rendu.
		 */
		C3D_API void RenderSubmeshes( RenderTechniqueBase & p_technique, Camera const & p_camera,  Pipeline & p_pipeline, SubmeshRenderNodesByMaterialMap const & p_nodes );
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
		 *\brief		Retrieves the scene name
		 *\return		The value
		 *\~french
		 *\brief		Récupère le nom de la scène
		 *\return		La valeur
		 */
		inline Castor::String const & GetName()const
		{
			return m_name;
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
			m_name = p_name;
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
		 *\brief		Sets the scene change status to \p true.
		 *\~french
		 *\brief		Définit le statut de changement de la scène à \p true.
		 */
		inline void SetChanged()
		{
			m_changed = true;
		}
		/**
		 *\~english
		 *\brief		Retrieves the ambient light colour
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
		 *\param[in]	The new value.
		 *\~french
		 *\brief		Définit la couleur de la lumière ambiante.
		 *\param[in]	La nouvelle valeur.
		 */
		inline void SetAmbientLight( Castor::Colour const & val )
		{
			m_ambientLight = val;
		}
		/**
		 *\~english
		 *\return		The nodes map.
		 *\~french
		 *\return		La map de noeuds.
		 */
		inline SceneNodePtrStrMap const & Nodes()const
		{
			return m_nodes;
		}
		/**
		 *\~english
		 *\return		The lights map.
		 *\~french
		 *\return		La map de lumières.
		 */
		inline LightPtrStrMap const & Lights()const
		{
			return m_lights;
		}
		/**
		 *\~english
		 *\return		The lights map.
		 *\~french
		 *\return		La map de lumières.
		 */
		inline LightsMap const & TypeSortedLights()const
		{
			return m_typeSortedLights;
		}
		/**
		 *\~english
		 *\return		The geometries map.
		 *\~french
		 *\return		La map de géométries.
		 */
		inline GeometryPtrStrMap const & Geometries()const
		{
			return m_primitives;
		}
		/**
		 *\~english
		 *\return		The cameras map.
		 *\~french
		 *\return		La map de caméras.
		 */
		inline CameraPtrStrMap const & Cameras()const
		{
			return m_cameras;
		}
		/**
		 *\~english
		 *\return		The RenderNodes, material sorted.
		 *\~french
		 *\return		Les noeuds de rendu, triés par matériau.
		 */
		inline SubmeshRenderNodesByMaterialMap const & GetRenderNodes()const
		{
			return m_renderNodes;
		}

		//@}

	private:
		void DoInitialiseGeometries();
		void DoDeleteToDelete();
		void DoUpdateAnimations();
		/**
		 *\~english
		 *\brief		Sorts render nodes by material
		 *\~french
		 *\brief		Trie les noeuds de rendu par matériau.
		 */
		void DoSortRenderNodes();
		/**
		 *\~english
		 *\brief		Binds the given pass.
		 *\param[in]	p_technique		The render technique.
		 *\param[in]	p_pipeline		The render pipeline.
		 *\param[in]	p_pass			The pass.
		 *\param[in]	p_programFlags	The shader program flags (combination of ePROGRAM_FLAG).
		 *\~french
		 *\brief		Active la passe donnée.
		 *\param[in]	p_technique		La technique de rendu.
		 *\param[in]	p_pipeline		Le pipeline de rendu.
		 *\param[in]	p_pass			La passe.
		 *\param[in]	p_programFlags	Les indicateurs du programme shader (combinaison de ePROGRAM_FLAG).
		 */
		void DoBindPass( RenderTechniqueBase & p_technique, Pipeline & p_pipeline, Pass & p_pass, uint32_t p_programFlags );
		/**
		 *\~english
		 *\brief		Applies geometry instance specific transformations.
		 *\param[in]	p_matrixBuffer		The frame variable buffer holding matrices.
		 *\param[in]	p_pipeline			The render pipeline.
		 *\param[in]	p_geometry			The geometry instance.
		 *\param[in]	p_excludedMtxFlags	Combination of MASK_MTXMODE, to be excluded from matrices used in program.
		 *\~french
		 *\brief		Applique les transformations liées à une instance de géométrie.
		 *\param[in]	p_matrixBuffer		Le tampon de variables de programme contenant les matrices.
		 *\param[in]	p_pipeline			Le pipeline de rendu.
		 *\param[in]	p_geometry			L'instance de géométrie.
		 *\param[in]	p_excludedMtxFlags	Combinaison de MASK_MTXMODE, à exclure des matrices utilisées dans le programme.
		 */
		void DoFillMatrixBuffer( Pass & p_pass, Pipeline & p_pipeline, Geometry const & p_geometry, uint64_t p_excludedMtxFlags );
		/**
		 *\~english
		 *\brief		Unbinds the given pass.
		 *\param[in]	p_pass			The pass.
		 *\~french
		 *\brief		Désctive la passe donnée.
		 *\param[in]	p_pass			La passe.
		 */
		void DoUnbindPass( Pass & p_pass );
		/**
		 *\~english
		 *\brief		Renders non instanced submeshes.
		 *\param[in]	p_technique		The render technique.
		 *\param[in]	p_pipeline		The render pipeline.
		 *\param[in]	p_begin			The render nodes begin.
		 *\param[in]	p_end			The render nodes end.
		 *\~french
		 *\brief		Dessine des sous maillages non instanciés.
		 *\param[in]	p_technique		La technique de rendu.
		 *\param[in]	p_pipeline		Le pipeline de rendu.
		 *\param[in]	p_begin			Le début des noeuds de rendu.
		 *\param[in]	p_end			La fin des noeuds de rendu.
		 */
		void DoRenderSubmeshesNonInstanced( RenderTechniqueBase & p_technique, Camera const & p_camera, Pipeline & p_pipeline, SubmeshRenderNodesByMaterialMap const & p_nodes );
		/**
		 *\~english
		 *\brief		Renders instanced submeshes.
		 *\param[in]	p_technique		The render technique.
		 *\param[in]	p_pipeline		The render pipeline.
		 *\param[in]	p_begin			The render nodes begin.
		 *\param[in]	p_end			The render nodes end.
		 *\~french
		 *\brief		Dessine des sous maillages instanciés.
		 *\param[in]	p_technique		La technique de rendu.
		 *\param[in]	p_pipeline		Le pipeline de rendu.
		 *\param[in]	p_begin			Le début des noeuds de rendu.
		 *\param[in]	p_end			La fin des noeuds de rendu.
		 */
		void DoRenderSubmeshesInstanced( RenderTechniqueBase & p_technique, Camera const & p_camera, Pipeline & p_pipeline, SubmeshRenderNodesByMaterialMap const & p_nodes );
		void DoRenderSubmeshesNonInstanced( RenderTechniqueBase & p_technique, Camera const & p_camera, Pipeline & p_pipeline, RenderNodeByDistanceMMap const & p_nodes );
		void DoResortAlpha( SubmeshRenderNodesByMaterialMap p_input, Camera const & p_camera, int p_sign, RenderNodeByDistanceMMap & p_output );
		void DoRenderBillboards( RenderTechniqueBase & p_technique, Pipeline & p_pipeline, BillboardListStrMapIt p_itBegin, BillboardListStrMapIt p_itEnd );

		template< typename MapType >
		void DoMerge( SceneSPtr p_scene, MapType & p_map, MapType & p_myMap )
		{
			Castor::String l_name;

			for ( typename MapType::iterator l_it = p_map.begin(); l_it != p_map.end(); ++l_it )
			{
				if ( l_it->second->GetParent()->GetName() == cuT( "CameraRootNode" ) )
				{
					l_it->second->Detach();
					l_it->second->AttachTo( m_rootCameraNode );
				}
				else if ( l_it->second->GetParent()->GetName() == cuT( "ObjectRootNode" ) )
				{
					l_it->second->Detach();
					l_it->second->AttachTo( m_rootObjectNode );
				}

				l_name = l_it->first;

				while ( p_myMap.find( l_name ) != p_myMap.end() )
				{
					l_name = p_scene->GetName() + cuT( "_" ) + l_name;
				}

				l_it->second->SetName( l_name );
				p_myMap.insert( std::make_pair( l_name, l_it->second ) );
			}

			p_map.clear();
		}

		template< class MapType, class ArrayType >
		void DoRemoveAll( MapType & p_map, ArrayType & p_array )
		{
			auto l_lock = Castor::make_unique_lock( m_mutex );

			for ( auto && l_it : p_map )
			{
				l_it.second->Detach();
				p_array.push_back( l_it.second );
				l_it.second.reset();
			}

			p_map.clear();
		}

		template< class ObjectType, typename KeyType >
		bool DoCheckObject( KeyType const & p_key, std::map< KeyType, std::shared_ptr< ObjectType > > & p_map, Castor::String const & p_strType )
		{
			bool l_return = false;

			if ( !p_key.empty() )
			{
				auto l_lock = Castor::make_unique_lock( m_mutex );
				typename std::map< KeyType, std::shared_ptr< ObjectType > >::const_iterator l_it = p_map.find( p_key );

				if ( l_it == p_map.end() )
				{
					l_return = true;
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

			return l_return;
		}

		template< class ObjectType, typename KeyType >
		bool DoAddObject( std::shared_ptr< ObjectType > p_object, std::map< KeyType, std::shared_ptr< ObjectType > > & p_map, Castor::String const & p_strType )
		{
			bool l_return = false;

			if ( p_object )
			{
				auto l_lock = Castor::make_unique_lock( m_mutex );
				typename std::map< KeyType, std::shared_ptr< ObjectType > >::const_iterator l_it = p_map.find( p_object->GetName() );

				if ( l_it == p_map.end() )
				{
					p_map.insert( std::make_pair( p_object->GetName(), p_object ) );
					l_return = true;
				}
				else
				{
					Castor::String l_strLog = cuT( "Scene::AddObject - Can't add " ) + p_strType + cuT( " [" ) + p_object->GetName() + cuT( "] - A " ) + p_strType + cuT( " with that name already exists" );
					Castor::Logger::LogWarning( l_strLog );
				}
			}
			else
			{
				Castor::Logger::LogWarning( cuT( "Scene::AddObject - The given object is null" ) );
			}

			return l_return;
		}

		template< class ObjectType, typename KeyType >
		std::shared_ptr< ObjectType > DoGetObject( std::map< KeyType, std::shared_ptr< ObjectType > > const & p_map, KeyType const & p_key )const
		{
			auto l_lock = Castor::make_unique_lock( m_mutex );
			typename std::shared_ptr< ObjectType > l_return;
			typename std::map< KeyType, std::shared_ptr< ObjectType > >::const_iterator l_it = p_map.find( p_key );

			if ( l_it != p_map.end() )
			{
				l_return = l_it->second;
			}

			return l_return;
		}

		template< class ObjectType, typename KeyType >
		void DoRemoveObject( std::shared_ptr< ObjectType > p_object, std::map< KeyType, std::shared_ptr< ObjectType > > & p_map, std::vector< std::shared_ptr< ObjectType > > & p_array )
		{
			if ( p_object )
			{
				auto l_lock = Castor::make_unique_lock( m_mutex );
				typename std::map< KeyType, std::shared_ptr< ObjectType > >::const_iterator l_it = p_map.find( p_object->GetName() );

				if ( l_it != p_map.end() )
				{
					p_map.erase( l_it );
				}

				p_array.push_back( p_object );
			}
			else
			{
				Castor::Logger::LogWarning( cuT( "Scene::RemoveObject - The given object is null" ) );
			}
		}

	private:
		//!\~english The scene name	\~french Le nom de la scène
		Castor::String m_name;
		//!\~english The root node	\~french Le noeud père de tous les noeuds de la scène
		SceneNodeSPtr m_rootNode;
		//!\~english The root node used only for cameras (used to ease the use of cameras)	\~french Le noeud père de tous les noeuds de caméra
		SceneNodeSPtr m_rootCameraNode;
		//!\~english The root node for every object other than camera (used to ease the use of cameras)	\~french Le noeud père de tous les noeuds d'objet
		SceneNodeSPtr m_rootObjectNode;
		//!\~english The nodes map	\~french La map des noeuds
		SceneNodePtrStrMap m_nodes;
		//!\~english The nodes to delete array	\~french Le tableau de noeuds à détruire
		SceneNodePtrArray m_nodesToDelete;
		//!\~english Added cameras	\~french Les caméras
		CameraPtrStrMap m_cameras;
		//!\~english The cameras to delete array	\~french Le tableau de caméras à détruire
		CameraPtrArray m_camerasToDelete;
		//!\~english Added lights.	\~french Les lumières.
		LightPtrStrMap m_lights;
		//!\~english The lights sorted byt light type	\~french Les lumières, triées par type de lumière.
		LightsMap m_typeSortedLights;
		//!\~english The lights to delete array	\~french Le tableau de lumières à détruire
		LightPtrArray m_lightsToDelete;
		//!\~english Added primitives	\~french Les objets
		GeometryPtrStrMap m_primitives;
		//!\~english The geometries to delete array	\~french Le tableau d'objets à détruire
		GeometryPtrArray m_primitivesToDelete;
		//!\~english The newly added geometries, it is used to make the vertex buffer of them, then they are removed from the map.	\~french Les géométries nouvellement ajoutées.
		GeometryPtrStrMap m_newlyAddedPrimitives;
		//!\~english The billboards lists	\~french Les listes de billboards
		BillboardListStrMap m_billboards;
		//!\~english The billboards lists to delete	\~french Les listes de billboards à détruire
		BillboardListArray m_billboardsToDelete;
		//!\~english The added animated object groups, it is used to make the vertex buffer of them, then they are removed from the map.	\~french Les groupes d'objets animés.
		AnimatedObjectGroupPtrStrMap m_animatedObjectsGroups;
		//!\~english The scene faces count	\~french Le nombre de faces dans la scène
		uint32_t m_facesCount;
		//!\~english The scene vertices count	\~french Le nombre de vertices dans la scène
		uint32_t m_vertexCount;
		//!\~english Tells if the scene has changed, id est if a geometry has been created or added to it => Vertex buffers need to be generated	\~french Dit si la scène a changé (si des géométries ont besoin d'être initialisées, essentiellement).
		bool m_changed;
		//!\~english Ambient light color	\~french Couleur de la lumière ambiante
		Castor::Colour m_ambientLight;
		//!\~english The mutex, to make the Scene threadsafe	\~french Le mutex protégeant les données de la scène
		mutable std::recursive_mutex m_mutex;
		//!\~english The lights factory	\~french La fabrique de lumières
		LightFactory & m_lightFactory;
		//!\~english The overlays array	\~french Le tableau d'overlays
		OverlayPtrArray m_overlays;
		//!\~english The geometries, sorted by material.	\~french Les géométries, triées par matériau.
		SubmeshRenderNodesByMaterialMap m_renderNodes;
		//!\~english The geometries without alpha blending, sorted by material.	\~french Les géométries sans alpha blending, triées par matériau.
		SubmeshRenderNodesByMaterialMap m_opaqueRenderNodes;
		//!\~english The geometries with alpha blending, sorted by material.	\~french Les géométries avec de l'alpha blend, triées par matériau.
		SubmeshRenderNodesByMaterialMap m_transparentRenderNodes;
		//!\~english The geometries with alpha blending, sorted by distance to the camera	\~french Les géométries avec de l'alpha blend, triées par distance à la caméra
		RenderNodeByDistanceMMap m_distanceSortedTransparentRenderNodes;
		//!\~english The scene background colour	\~french La couleur de fond de la scène
		Castor::Colour m_backgroundColour;
		//!\~english The background image	\~french L'image de fond
		TextureBaseSPtr m_backgroundImage;
		//!\~english The image containing lights data	\~french L'image contenant les données des lumières
		Castor::PxBufferBaseSPtr m_lightsData;
		//!\~english The lights texture	\~french La texture contenant les lumières
		TextureUnitSPtr m_lightsTexture;
	};
}

#endif
