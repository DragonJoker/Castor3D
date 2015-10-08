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
#ifndef ___C3D_SCENE_NODE_H___
#define ___C3D_SCENE_NODE_H___

#include "Castor3DPrerequisites.hpp"
#include "BinaryParser.hpp"

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		The scene node handler class
	\remark		A scene node is a parent for nearly every object in a scene : geometry, camera, ...
	\~french
	\brief		La classe de gestion de noeud de scène
	\remark		Un noeud de scène est un parent pour à peu près tous les objets d'une scène : géométrie, caméra, ...
	*/
	class C3D_API SceneNode
		:	public std::enable_shared_from_this< SceneNode >
	{
	public:
		//!\~english The total number of scene nodes	\~french Le nombre total de noeuds de scène
		static uint64_t Count;
		/*!
		\author Sylvain DOREMUS
		\version 0.6.1.0
		\date 19/10/2011
		\~english
		\brief SceneNode loader
		\remark Loads and saves Castor3D::SceneNode from/into a file
		\~french
		\brief SceneNode loader
		\remark Charge/écrit des Castor3D::SceneNode à partir de/dans un fichier
		*/
		class C3D_API TextLoader
			:	public Castor::Loader< SceneNode, Castor::eFILE_TYPE_TEXT, Castor::TextFile >
			,	public Castor::NonCopyable
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			TextLoader( Castor::File::eENCODING_MODE p_encodingMode = Castor::File::eENCODING_MODE_ASCII );
			/**
			 *\~english
			 * Writes a Castor3D::SceneNode into a text file
			 *\param[in]	p_file	the file to write the scene node in
			 *\param[in]	p_node	the scene node to write
			 *\return \p true if successful
			 *\~french
			 * Ecrit un Castor3D::SceneNode dans un fichier texte
			 *\param[in]	p_file	le fichier dans lequel le Castor3D::SceneNode est écrit
			 *\param[in]	p_node	Le Castor3D::SceneNode à écrire
			 *\return \p true si tout s'est bien passé, \p false sinon
			 */
			virtual bool operator()( SceneNode const & p_node, Castor::TextFile & p_file );
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
			:	public Castor3D::BinaryParser< SceneNode >
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
			virtual bool Fill( SceneNode const & p_obj, BinaryChunk & p_chunk )const;
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
			virtual bool Parse( SceneNode & p_obj, BinaryChunk & p_chunk )const;
		};

	protected:
		typedef std::map< Castor::String, SceneNodeWPtr > SceneNodePtrStrMap;
		typedef std::map< Castor::String, MovableObjectWPtr > MovableObjectPtrStrMap;

	public:
		typedef SceneNodePtrStrMap::iterator node_iterator;
		typedef SceneNodePtrStrMap::const_iterator node_const_iterator;
		typedef MovableObjectPtrStrMap::iterator object_iterator;
		typedef MovableObjectPtrStrMap::const_iterator object_const_iterator;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		SceneNode();
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_scene	The parent scene
		 *\param[in]	p_name		The node's name. If empty the name is "SceneNode<s_nbSceneNodes>"
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_scene	La scène parente
		 *\param[in]	p_name		The node's name. If empty the name is "SceneNode<s_nbSceneNodes>"
		 */
		SceneNode( SceneSPtr p_scene, Castor::String const & p_name = Castor::cuEmptyString );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~SceneNode();
		/**
		 *\~english
		 *\brief		Attaches a MovableObject to the node
		 *\param[in]	p_pObject	The object to attach
		 *\~french
		 *\brief		Attache un MovableObject au noeud
		 *\param[in]	p_pObject	L'objet à attacher
		 */
		void AttachObject( MovableObjectSPtr p_pObject );
		/**
		 *\~english
		 *\brief		Detaches a MovableObject from the node
		 *\param[in]	p_pObject	The object to detach
		 *\~french
		 *\brief		Détache un MovableObject fu noeud
		 *\param[in]	p_pObject	L'objet à détacher
		 */
		void DetachObject( MovableObjectSPtr p_pObject );
		/**
		 *\~english
		 *\brief		Sets the parent node
		 *\param[in]	p_parent	The new parent
		 *\~french
		 *\brief		Définit le noeud parent
		 *\param[in]	p_parent	Le nouveau parent
		 */
		void AttachTo( SceneNodeSPtr p_parent );
		/**
		 *\~english
		 *\brief		Detaches the node from it's parent
		 *\~french
		 *\brief		Détache le noeud de son parent
		 */
		void Detach();
		/**
		 *\~english
		 *\param[in]	p_name	The name of the node
		 *\return		\p true if one of my child has the given name
		 *\~french
		 *\param[in]	p_name	Le nom du noeud
		 *\return		\p true si un des enfants de ce noeud a le nom donné
		 */
		bool HasChild( Castor::String const & p_name );
		/**
		 *\~english
		 *\brief		Add the given node to my childs if it isn't already
		 *\param[in]	p_child	The node to add
		 *\~french
		 *\brief		Ajoute le noeud donné aux enfants de ce noeud, s'il n'y est pas encore
		 *\param[in]	p_child	Le noeud à ajouter
		 */
		void AddChild( SceneNodeSPtr p_child );
		/**
		 *\~english
		 *\brief		Detaches a child from my child's list, if it is one of my childs
		 *\param[in]	p_child	The child to detach
		 *\~french
		 *\brief		Détache un noeud des enfants de ce noeud, s'il en fait partie
		 *\param[in]	p_child	Le noeud à détacher
		 */
		void DetachChild( SceneNodeSPtr p_child );
		/**
		 *\~english
		 *\brief		Detaches a child from my child's list, if it is one of my childs
		 *\param[in]	p_childName	The name of the child to detach
		 *\~french
		 *\brief		Détache un noeud des enfants de ce noeud, s'il en fait partie
		 *\param[in]	p_childName	Le nom du noeud à détacher
		 */
		void DetachChild( Castor::String const & p_childName );
		/**
		 *\~english
		 *\brief		Detaches all my childs
		 *\~french
		 *\brief		Détache tous les enfants de ce noeud
		 */
		void DetachAllChilds();
		/**
		 *\~english
		 *\brief		Rotates around Y axis
		 *\param[in]	p_angle	The rotation angle
		 *\~french
		 *\brief		Tourne le noeud autour de son axe Y
		 *\param[in]	p_angle	L'angle de rotation
		 */
		void Yaw( Castor::Angle const & p_angle );
		/**
		 *\~english
		 *\brief		Rotates around Z axis
		 *\param[in]	p_angle	The rotation angle
		 *\~french
		 *\brief		Tourne le noeud autour de son axe Z
		 *\param[in]	p_angle	L'angle de rotation
		 */
		void Pitch( Castor::Angle const & p_angle );
		/**
		 *\~english
		 *\brief		Rotates around X axis
		 *\param[in]	p_angle	The rotation angle
		 *\~french
		 *\brief		Tourne le noeud autour de son axe X
		 *\param[in]	p_angle	L'angle de rotation
		 */
		void Roll( Castor::Angle const & p_angle );
		/**
		 *\~english
		 *\brief		Rotate the node with the given orientation
		 *\param[in]	p_quat	The orientation to add to current one
		 *\~french
		 *\brief		Tourne le noeud d'une rotation donnée
		 *\param[in]	p_quat	La rotation à appliquer
		 */
		void Rotate( Castor::Quaternion const & p_quat );
		/**
		 *\~english
		 *\brief		Translates the node
		 *\param[in]	p_t	The translation value
		 *\~french
		 *\brief		Translate le noeud
		 *\param[in]	p_t	The La valeur de translation
		 */
		void Translate( Castor::Point3r const & p_t );
		/**
		 *\~english
		 *\brief		Scales the node
		 *\param[in]	p_s	The scale value
		 *\~french
		 *\brief		Change l'échelle du noeud
		 *\param[in]	p_s	La valeur d'échelle
		 */
		void Scale( Castor::Point3r const & p_s );
		/**
		 *\~english
		 *\brief		Sets the orientation
		 *\param[in]	p_orientation	The new orientation
		 *\~french
		 *\brief		Définit l'orientation du noeud
		 *\param[in]	p_orientation	La nouvelle orientation
		 */
		void SetOrientation( Castor::Quaternion const & p_orientation );
		/**
		 *\~english
		 *\brief		Sets the relative position from a Point3r
		 *\param[in]	p_position	The new position
		 *\~french
		 *\brief		Définit la position relative du noeud
		 *\param[in]	p_position	La nouvelle valeur
		 */
		void SetPosition( Castor::Point3r const & p_position );
		/**
		 *\~english
		 *\brief		Sets the relative scale from a Point3r
		 *\param[in]	p_scale	The new scale
		 *\~french
		 *\brief		Définit l'échelle relative du noeud
		 *\param[in]	p_scale	La nouvelle valeur
		 */
		void SetScale( Castor::Point3r const & p_scale );
		/**
		 *\~english
		 *\brief		Creates the vertex buffer of attached geometries
		 *\param[out]	p_nbFaces		Receives the faces count
		 *\param[out]	p_nbVertex		Receives the vertex count
		 *\~french
		 *\brief		Crée les tampons de sommets des géométries attachées
		 *\param[out]	p_nbFaces		Reçoit le nombre de faces
		 *\param[out]	p_nbVertex		Reçoit le nombre de sommets
		 */
		void CreateBuffers( uint32_t & p_nbFaces, uint32_t & p_nbVertex )const;
		/**
		 *\~english
		 *\brief		Gets the nearest geometry held by this node or it's children nodes, which is hit by the ray
		 *\param[in]	p_pRay		The ray
		 *\param[out]	p_fDistance	Receives the distance of the met geometry
		 *\param[out]	p_ppFace	Receives the face of the met geometry
		 *\param[out]	p_ppSubmesh	Receives the submesh of the met geometry
		 *\return		The geometry, NULL if none
		 *\~french
		 *\brief		Récupère la géométrie la plus proche de ce noeud et de ses enfants, qui sera touchée par le rayon
		 *\param[in]	p_pRay		Le rayon
		 *\param[out]	p_fDistance	Reçoit la distance de la géométrie rencontrée
		 *\param[out]	p_ppFace	Reçoit la face dans la géométrie rencontrée
		 *\param[out]	p_ppSubmesh	Reçoit le submesh de la géométrie rencontrée
		 *\return		La géométrie, NULL si aucune
		 */
		GeometrySPtr GetNearestGeometry( Ray * p_pRay, real & p_fDistance, FaceSPtr * p_ppFace, SubmeshSPtr * p_ppSubmesh );
		/**
		 *\~english
		 *\brief		Retrieves the absolute position
		 *\return		The value
		 *\~french
		 *\brief		Récupère la position absolue
		 *\return		La valeur
		 */
		Castor::Point3r GetDerivedPosition();
		/**
		 *\~english
		 *\brief		Retrieves the absolute orientation
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'orientation absolue
		 *\return		La valeur
		 */
		Castor::Quaternion GetDerivedOrientation();
		/**
		 *\~english
		 *\brief		Retrieves the absolute scale
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'échelle absolue
		 *\return		La valeur
		 */
		Castor::Point3r GetDerivedScale();
		/**
		 *\~english
		 *\brief		Retrieves the relative position
		 *\return		The value
		 *\~french
		 *\brief		Récupère la position relative
		 *\return		La valeur
		 */
		inline Castor::Point3r const & GetPosition()const
		{
			return m_ptPosition;
		}
		/**
		 *\~english
		 *\brief		Retrieves the relative orientation
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'orientation relative
		 *\return		La valeur
		 */
		inline Castor::Quaternion const & GetOrientation()const
		{
			return m_qOrientation;
		}
		/**
		 *\~english
		 *\brief		Retrieves the relative scale
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'échelle relative
		 *\return		La valeur
		 */
		inline Castor::Point3r const & GetScale()const
		{
			return m_ptScale;
		}
		/**
		 *\~english
		 *\brief		Retrieves the orientation in axis and angle terms
		 *\param[out]	p_axis	Receives the axis
		 *\param[out]	p_angle	Receives the angle
		 *\~french
		 *\brief		Récupère l'orientation, en termes d'angle et d'axe
		 *\param[out]	p_axis	Reçoit l'axe
		 *\param[out]	p_angle	Reçoit l'angle
		 */
		inline void GetAxisAngle( Castor::Point3r & p_axis, Castor::Angle & p_angle )
		{
			m_qOrientation.ToAxisAngle( p_axis, p_angle );
		}
		/**
		 *\~english
		 *\brief		Retrieves the visibility status
		 *\return		The value
		 *\~french
		 *\brief		Récupère le statut de visibilité
		 *\return		La valeur
		 */
		inline bool IsVisible()const
		{
			return m_bVisible;
		}
		/**
		 *\~english
		 *\brief		Retrieves the displayable status
		 *\return		The value
		 *\~french
		 *\brief		Récupère le statut d'affichabilité
		 *\return		La valeur
		 */
		inline bool IsDisplayable()const
		{
			return m_bDisplayable;
		}
		/**
		 *\~english
		 *\brief		Retrieves the parent node
		 *\return		The value
		 *\~french
		 *\brief		Récupère le noeud parent
		 *\return		La valeur
		 */
		inline SceneNodeSPtr GetParent()const
		{
			return m_pParent.lock();
		}
		/**
		 *\~english
		 *\brief		Retrieves the scene
		 *\return		The value
		 *\~french
		 *\brief		Récupère la scène
		 *\return		La valeur
		 */
		inline SceneSPtr GetScene()const
		{
			return m_pScene.lock();
		}
		/**
		 *\~english
		 *\brief		Retrieves the node name
		 *\return		The value
		 *\~french
		 *\brief		Récupère le nom du noeud
		 *\return		La valeur
		 */
		inline Castor::String GetName()const
		{
			return m_strName;
		}
		/**
		 *\~english
		 *\brief		Retrieves the childs map
		 *\return		The value
		 *\~french
		 *\brief		Récupère la map des enfants
		 *\return		La valeur
		 */
		inline SceneNodePtrStrMap const & GetChilds()const
		{
			return m_mapChilds;
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the first child
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur le premier enfant
		 *\return		La valeur
		 */
		inline node_iterator ChildsBegin()
		{
			return m_mapChilds.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the first child
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur le premier enfant
		 *\return		La valeur
		 */
		inline node_const_iterator ChildsBegin()const
		{
			return m_mapChilds.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to after the last child
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur apèrs le dernier enfant
		 *\return		La valeur
		 */
		inline node_iterator ChildsEnd()
		{
			return m_mapChilds.end();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to after the last child
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur apèrs le dernier enfant
		 *\return		La valeur
		 */
		inline node_const_iterator ChildsEnd()const
		{
			return m_mapChilds.end();
		}
		/**
		 *\~english
		 *\brief		Retrieves the objects map
		 *\return		The value
		 *\~french
		 *\brief		Récupère la map des objets
		 *\return		La valeur
		 */
		inline MovableObjectPtrStrMap const & GetObjects()const
		{
			return m_mapAttachedObjects;
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the first attached object
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur le premier objet attaché
		 *\return		La valeur
		 */
		inline object_iterator ObjectsBegin()
		{
			return m_mapAttachedObjects.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the first attached object
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur le premier objet attaché
		 *\return		La valeur
		 */
		inline object_const_iterator ObjectsBegin()const
		{
			return m_mapAttachedObjects.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to after the last attached object
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur après le dernier objet attaché
		 *\return		La valeur
		 */
		inline object_iterator ObjectsEnd()
		{
			return m_mapAttachedObjects.end();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to after the last attached object
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur après le dernier objet attaché
		 *\return		La valeur
		 */
		inline object_const_iterator ObjectsEnd()const
		{
			return m_mapAttachedObjects.end();
		}
		/**
		 *\~english
		 *\brief		Retrieves the child with given name
		 *\param[in]	p_name	The child name
		 *\return		The value, NULL if not found
		 *\~french
		 *\brief		Récupère l'enfant avec le nom donné
		 *\param[in]	p_name	Le nom de l'enfant
		 *\return		La valeur, NULL si non trouvé
		 */
		inline SceneNodeSPtr GetChild( Castor::String const & p_name )
		{
			return ( m_mapChilds.find( p_name ) != m_mapChilds.end() ? m_mapChilds.find( p_name )->second.lock() : nullptr );
		}
		/**
		 *\~english
		 *\brief		Retrieves the relative transformation matrix
		 *\return		The value
		 *\~french
		 *\brief		Récupère la matrice de transformation relative
		 *\return		La valeur
		 */
		inline Castor::Matrix4x4r const & GetTransformationMatrix()
		{
			DoComputeMatrix();
			return m_mtxMatrix;
		}
		/**
		 *\~english
		 *\brief		Retrieves the absolute transformation matrix
		 *\return		The value
		 *\~french
		 *\brief		Récupère la matrice de transformation absolue
		 *\return		La valeur
		 */
		inline Castor::Matrix4x4r const & GetDerivedTransformationMatrix()
		{
			DoComputeMatrix();
			return m_mtxDerivedMatrix;
		}
		/**
		 *\~english
		 *\brief		Retrieves the transformations matrices modify status
		 *\return		The value
		 *\~french
		 *\brief		Récupère le statut de modification des matrices de transformation
		 *\return		La valeur
		 */
		inline bool IsModified()const
		{
			return m_bMtxChanged || m_bDerivedMtxChanged;
		}
		/**
		 *\~english
		 *\brief		Sets the node visibility status
		 *\param[in]	p_visible	The new value
		 *\~french
		 *\brief		Définit le statut de visibilité du noeud
		 *\param[in]	p_visible	La nouvelle valeur
		 */
		inline void SetVisible( bool p_visible )
		{
			m_bVisible = p_visible;
		}
		/**
		 *\~english
		 *\brief		Sets the node name
		 *\param[in]	p_name	The new value
		 *\~french
		 *\brief		Définit le nom du noeud
		 *\param[in]	p_name	La nouvelle valeur
		 */
		inline void SetName( Castor::String const & p_name )
		{
			m_strName = p_name;
		}

	private:
		void DoComputeMatrix();
		/**
		 *\~english
		 *\brief		Sets this node's children's absolute transformation matrix to be recalculated
		 *\~french
		 *\brief		Fait que la matrice de transfomation des enfants de ce noeud doit être recalculée
		 */
		void DoUpdateChildsDerivedTransform();

	protected:
		//!\~english The node's name	\~french Le nom du noeud
		Castor::String m_strName;
		//!\~english Tells if it is displayable. A node is displayable if his parent is either displayable or the root node	\~french Dit si le noeud est affichable. Il est affichable si son parent est le noeud racine ou s'il est affichable.
		bool m_bDisplayable;
		//!\~english The visible status. If a node is hidden, all objects attached to it are hidden	\~french Le statut de visibilité. Si un noeud est caché, tous les objets qui y sont attachés sont cachés aussi.
		bool m_bVisible;
		//!\~english  The relative orientation of the node	\~french L'orientation du noeud, relative au parent
		Castor::Quaternion m_qOrientation;
		//!\~english  The relative position of the node	\~french La position du noeud, relative au parent
		Castor::Point3r m_ptPosition;
		//!\~english  The relative scale transform value of the node	\~french La mise à l'échelle du noeud, relative au parent
		Castor::Point3r m_ptScale;
		//!\~english  The reative transformation matrix	\~french La matrice de transformation, relative au parent
		Castor::Matrix4x4r m_mtxMatrix;
		//!\~english Tells the relative transformation matrix needs recomputation	\~french Dit si la matrice de transformation relative doit être recalculée
		bool m_bMtxChanged;
		//!\~english  The absolute transformation matrix	\~french la matrice de transformation absolue
		Castor::Matrix4x4r m_mtxDerivedMatrix;
		//!\~english Tells the absolute transformation matrix needs recomputation	\~french Dit si la matrice de transformation absolue doit être recalculée
		bool m_bDerivedMtxChanged;
		//!\~english  This node's parent	\~french Le noeud parent
		SceneNodeWPtr m_pParent;
		//!\~english  This node's childs	\~french Les enfants de ce noeud
		SceneNodePtrStrMap m_mapChilds;
		//!\~english  This node's attached objects	\~french Les objets attachés à ce noeud
		MovableObjectPtrStrMap m_mapAttachedObjects;
		//!\~english The parent scene	\~french La scène parente
		SceneWPtr m_pScene;
	};
}

#endif
