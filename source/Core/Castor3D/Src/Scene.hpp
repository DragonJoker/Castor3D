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
		inline TextureSPtr GetBackgroundImage()const
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

		//@}

	private:
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
		 *\param[in]	p_technique			The render technique.
		 *\param[in]	p_pipeline			The render pipeline.
		 *\param[in]	p_pass				The pass.
		 *\param[in]	p_geometry			The geometry instance.
		 *\param[in]	p_programFlags		The shader program flags (combination of ePROGRAM_FLAG).
		 *\param[in]	p_excludedMtxFlags	Combination of MASK_MTXMODE, to be excluded from matrices used in program.
		 *\~french
		 *\brief		Active la passe donnée.
		 *\param[in]	p_technique			La technique de rendu.
		 *\param[in]	p_pipeline			Le pipeline de rendu.
		 *\param[in]	p_pass				La passe.
		 *\param[in]	p_geometry			L'instance de géométrie.
		 *\param[in]	p_programFlags		Les indicateurs du programme shader (combinaison de ePROGRAM_FLAG).
		 *\param[in]	p_excludedMtxFlags	Combinaison de MASK_MTXMODE, à exclure des matrices utilisées dans le programme.
		 */
		void DoBindPass( RenderTechniqueBase & p_technique, Pipeline & p_pipeline, Pass & p_pass, Geometry const & p_geometry, uint32_t p_programFlags, uint64_t p_excludedMtxFlags );
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

	private:
		//!\~english The root node	\~french Le noeud père de tous les noeuds de la scène
		SceneNodeSPtr m_rootNode;
		//!\~english The root node used only for cameras (used to ease the use of cameras)	\~french Le noeud père de tous les noeuds de caméra
		SceneNodeSPtr m_rootCameraNode;
		//!\~english The root node for every object other than camera (used to ease the use of cameras)	\~french Le noeud père de tous les noeuds d'objet
		SceneNodeSPtr m_rootObjectNode;
		//!\~english The scene nodes manager.	\~french Le gestionnaire de noeuds de scène.
		DECLARE_MANAGER_MEMBER( sceneNode, SceneNode );
		//!\~english The camera manager.	\~french Le gestionnaire de caméras.
		DECLARE_MANAGER_MEMBER( camera, Camera );
		//!\~english The lights manager.	\~french Le gestionnaire de lumières.
		DECLARE_MANAGER_MEMBER( light, Light );
		//!\~english The geometies manager.	\~french Le gestionnaire de géométries.
		DECLARE_MANAGER_MEMBER( geometry, Geometry );
		//!\~english The billboards manager.	\~french Le gestionnaire de billboards.
		DECLARE_MANAGER_MEMBER( billboard, Billboard );
		//!\~english The animated objects groups manager.	\~french Le gestionnaire de groupes d'objets animés.
		DECLARE_MANAGER_MEMBER( animatedObjectGroup, AnimatedObjectGroup );
		//!\~english Tells if the scene has changed, id est if a geometry has been created or added to it => Vertex buffers need to be generated	\~french Dit si la scène a changé (si des géométries ont besoin d'être initialisées, essentiellement).
		bool m_changed;
		//!\~english Ambient light color	\~french Couleur de la lumière ambiante
		Castor::Colour m_ambientLight;
		//!\~english The mutex, to make the Scene threadsafe	\~french Le mutex protégeant les données de la scène
		mutable std::recursive_mutex m_mutex;
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
		TextureSPtr m_backgroundImage;
	};
}

#undef DECLARE_MANAGER_MEMBER

#endif
