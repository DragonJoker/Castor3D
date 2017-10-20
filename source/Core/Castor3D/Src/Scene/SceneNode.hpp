/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SCENE_NODE_H___
#define ___C3D_SCENE_NODE_H___

#include "Castor3DPrerequisites.hpp"

#include <Design/Named.hpp>
#include <Design/OwnedBy.hpp>
#include <Design/Signal.hpp>

namespace castor3d
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
	class SceneNode
		: public std::enable_shared_from_this< SceneNode >
		, public castor::OwnedBy< Scene >
		, public castor::Named
	{
	public:
		using OnChangedFunction = std::function< void( SceneNode const & ) >;
		using OnChanged = castor::Signal< OnChangedFunction >;

	public:
		//!\~english The total number of scene nodes	\~french Le nombre total de noeuds de scène
		static uint64_t Count;
		/*!
		\author Sylvain DOREMUS
		\version 0.6.1.0
		\date 19/10/2011
		\~english
		\brief SceneNode loader
		\remark Loads and saves castor3d::SceneNode from/into a file
		\~french
		\brief SceneNode loader
		\remark Charge/écrit des castor3d::SceneNode à partir de/dans un fichier
		*/
		class TextWriter
			: public castor::TextWriter< SceneNode >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			C3D_API explicit TextWriter( castor::String const & p_tabs );
			/**
			 *\~english
			 * Writes a castor3d::SceneNode into a text file
			 *\param[in]	p_file	the file to write the scene node in
			 *\param[in]	p_node	the scene node to write
			 *\return \p true if successful
			 *\~french
			 * Ecrit un castor3d::SceneNode dans un fichier texte
			 *\param[in]	p_file	le fichier dans lequel le castor3d::SceneNode est écrit
			 *\param[in]	p_node	Le castor3d::SceneNode à écrire
			 *\return \p true si tout s'est bien passé, \p false sinon
			 */
			C3D_API bool operator()( SceneNode const & p_node, castor::TextFile & p_file )override;
		};

	protected:
		using SceneNodePtrStrMap = std::map< castor::String, SceneNodeWPtr >;
		using MovableObjectArray = std::list< std::reference_wrapper< MovableObject > >;

	public:
		typedef SceneNodePtrStrMap::iterator node_iterator;
		typedef SceneNodePtrStrMap::const_iterator node_const_iterator;
		typedef MovableObjectArray::iterator object_iterator;
		typedef MovableObjectArray::const_iterator object_const_iterator;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_name		The node's name.
		 *\param[in]	p_scene		The parent scene.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_name		Le nom du noeud.
		 *\param[in]	p_scene		La scène parente.
		 */
		C3D_API SceneNode( castor::String const & p_name, Scene & p_scene );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~SceneNode();
		/**
		 *\~english
		 *\brief		Updates the scene node matrices.
		 *\~french
		 *\brief		Met à jour les matrices du noeud.
		 */
		C3D_API void update();
		/**
		 *\~english
		 *\brief		Attaches a MovableObject to the node
		 *\param[in]	p_object	The object to attach
		 *\~french
		 *\brief		Attache un MovableObject au noeud
		 *\param[in]	p_object	L'objet à attacher
		 */
		C3D_API void attachObject( MovableObject & p_object );
		/**
		 *\~english
		 *\brief		Detaches a MovableObject from the node
		 *\param[in]	p_object	The object to detach
		 *\~french
		 *\brief		Détache un MovableObject fu noeud
		 *\param[in]	p_object	L'objet à détacher
		 */
		C3D_API void detachObject( MovableObject & p_object );
		/**
		 *\~english
		 *\brief		sets the parent node
		 *\param[in]	p_parent	The new parent
		 *\~french
		 *\brief		Définit le noeud parent
		 *\param[in]	p_parent	Le nouveau parent
		 */
		C3D_API void attachTo( SceneNodeSPtr p_parent );
		/**
		 *\~english
		 *\brief		Detaches the node from it's parent
		 *\~french
		 *\brief		Détache le noeud de son parent
		 */
		C3D_API void detach();
		/**
		 *\~english
		 *\param[in]	p_name	The name of the node
		 *\return		\p true if one of my child has the given name
		 *\~french
		 *\param[in]	p_name	Le nom du noeud
		 *\return		\p true si un des enfants de ce noeud a le nom donné
		 */
		C3D_API bool hasChild( castor::String const & p_name );
		/**
		 *\~english
		 *\brief		add the given node to my childs if it isn't already
		 *\param[in]	p_child	The node to add
		 *\~french
		 *\brief		Ajoute le noeud donné aux enfants de ce noeud, s'il n'y est pas encore
		 *\param[in]	p_child	Le noeud à ajouter
		 */
		C3D_API void addChild( SceneNodeSPtr p_child );
		/**
		 *\~english
		 *\brief		Detaches a child from my child's list, if it is one of my childs
		 *\param[in]	p_child	The child to detach
		 *\~french
		 *\brief		Détache un noeud des enfants de ce noeud, s'il en fait partie
		 *\param[in]	p_child	Le noeud à détacher
		 */
		C3D_API void detachChild( SceneNodeSPtr p_child );
		/**
		 *\~english
		 *\brief		Detaches a child from my child's list, if it is one of my childs
		 *\param[in]	p_childName	The name of the child to detach
		 *\~french
		 *\brief		Détache un noeud des enfants de ce noeud, s'il en fait partie
		 *\param[in]	p_childName	Le nom du noeud à détacher
		 */
		C3D_API void detachChild( castor::String const & p_childName );
		/**
		 *\~english
		 *\brief		Detaches all my childs
		 *\~french
		 *\brief		Détache tous les enfants de ce noeud
		 */
		C3D_API void detachChildren();
		/**
		 *\~english
		 *\brief		Rotates around Y axis
		 *\param[in]	p_angle	The rotation angle
		 *\~french
		 *\brief		Tourne le noeud autour de son axe Y
		 *\param[in]	p_angle	L'angle de rotation
		 */
		C3D_API void yaw( castor::Angle const & p_angle );
		/**
		 *\~english
		 *\brief		Rotates around Z axis
		 *\param[in]	p_angle	The rotation angle
		 *\~french
		 *\brief		Tourne le noeud autour de son axe Z
		 *\param[in]	p_angle	L'angle de rotation
		 */
		C3D_API void pitch( castor::Angle const & p_angle );
		/**
		 *\~english
		 *\brief		Rotates around X axis
		 *\param[in]	p_angle	The rotation angle
		 *\~french
		 *\brief		Tourne le noeud autour de son axe X
		 *\param[in]	p_angle	L'angle de rotation
		 */
		C3D_API void roll( castor::Angle const & p_angle );
		/**
		 *\~english
		 *\brief		Rotate the node with the given orientation
		 *\param[in]	p_quat	The orientation to add to current one
		 *\~french
		 *\brief		Tourne le noeud d'une rotation donnée
		 *\param[in]	p_quat	La rotation à appliquer
		 */
		C3D_API void rotate( castor::Quaternion const & p_quat );
		/**
		 *\~english
		 *\brief		Translates the node
		 *\param[in]	p_t	The translation value
		 *\~french
		 *\brief		Translate le noeud
		 *\param[in]	p_t	The La valeur de translation
		 */
		C3D_API void translate( castor::Point3r const & p_t );
		/**
		 *\~english
		 *\brief		Scales the node
		 *\param[in]	p_s	The scale value
		 *\~french
		 *\brief		Change l'échelle du noeud
		 *\param[in]	p_s	La valeur d'échelle
		 */
		C3D_API void scale( castor::Point3r const & p_s );
		/**
		 *\~english
		 *\brief		sets the orientation
		 *\param[in]	p_orientation	The new orientation
		 *\~french
		 *\brief		Définit l'orientation du noeud
		 *\param[in]	p_orientation	La nouvelle orientation
		 */
		C3D_API void setOrientation( castor::Quaternion const & p_orientation );
		/**
		 *\~english
		 *\brief		sets the relative position from a Point3r
		 *\param[in]	p_position	The new position
		 *\~french
		 *\brief		Définit la position relative du noeud
		 *\param[in]	p_position	La nouvelle valeur
		 */
		C3D_API void setPosition( castor::Point3r const & p_position );
		/**
		 *\~english
		 *\brief		sets the relative scale from a Point3r
		 *\param[in]	p_scale	The new scale
		 *\~french
		 *\brief		Définit l'échelle relative du noeud
		 *\param[in]	p_scale	La nouvelle valeur
		 */
		C3D_API void setScale( castor::Point3r const & p_scale );
		/**
		 *\~english
		 *\brief		Retrieves the absolute position
		 *\return		The value
		 *\~french
		 *\brief		Récupère la position absolue
		 *\return		La valeur
		 */
		C3D_API castor::Point3r getDerivedPosition()const;
		/**
		 *\~english
		 *\brief		Retrieves the absolute orientation
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'orientation absolue
		 *\return		La valeur
		 */
		C3D_API castor::Quaternion getDerivedOrientation()const;
		/**
		 *\~english
		 *\brief		Retrieves the absolute scale
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'échelle absolue
		 *\return		La valeur
		 */
		C3D_API castor::Point3r getDerivedScale()const;
		/**
		 *\~english
		 *\brief		Retrieves the relative transformation matrix
		 *\return		The value
		 *\~french
		 *\brief		Récupère la matrice de transformation relative
		 *\return		La valeur
		 */
		C3D_API castor::Matrix4x4r const & getTransformationMatrix()const;
		/**
		 *\~english
		 *\brief		Retrieves the absolute transformation matrix
		 *\return		The value
		 *\~french
		 *\brief		Récupère la matrice de transformation absolue
		 *\return		La valeur
		 */
		C3D_API castor::Matrix4x4r const & getDerivedTransformationMatrix()const;
		/**
		 *\~english
		 *\brief		sets the node visibility status
		 *\param[in]	p_visible	The new value
		 *\~french
		 *\brief		Définit le statut de visibilité du noeud
		 *\param[in]	p_visible	La nouvelle valeur
		 */
		C3D_API void setVisible( bool p_visible );
		/**
		 *\~english
		 *\brief		Retrieves the visibility status
		 *\return		The value
		 *\~french
		 *\brief		Récupère le statut de visibilité
		 *\return		La valeur
		 */
		C3D_API bool isVisible()const;
		/**
		 *\~english
		 *\brief		Retrieves the relative position
		 *\return		The value
		 *\~french
		 *\brief		Récupère la position relative
		 *\return		La valeur
		 */
		inline castor::Point3r const & getPosition()const
		{
			return m_position;
		}
		/**
		 *\~english
		 *\brief		Retrieves the relative orientation
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'orientation relative
		 *\return		La valeur
		 */
		inline castor::Quaternion const & getOrientation()const
		{
			return m_orientation;
		}
		/**
		 *\~english
		 *\brief		Retrieves the relative scale
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'échelle relative
		 *\return		La valeur
		 */
		inline castor::Point3r const & getScale()const
		{
			return m_scale;
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
		inline void getAxisAngle( castor::Point3r & p_axis, castor::Angle & p_angle )const
		{
			m_orientation.toAxisAngle( p_axis, p_angle );
		}
		/**
		 *\~english
		 *\brief		Retrieves the displayable status
		 *\return		The value
		 *\~french
		 *\brief		Récupère le statut d'affichabilité
		 *\return		La valeur
		 */
		inline bool isDisplayable()const
		{
			return m_displayable;
		}
		/**
		 *\~english
		 *\brief		Retrieves the parent node
		 *\return		The value
		 *\~french
		 *\brief		Récupère le noeud parent
		 *\return		La valeur
		 */
		inline SceneNodeSPtr getParent()const
		{
			return m_parent.lock();
		}
		/**
		 *\~english
		 *\brief		Retrieves the childs map
		 *\return		The value
		 *\~french
		 *\brief		Récupère la map des enfants
		 *\return		La valeur
		 */
		inline SceneNodePtrStrMap const & getChildren()const
		{
			return m_children;
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the first child
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur le premier enfant
		 *\return		La valeur
		 */
		inline node_iterator childrenBegin()
		{
			return m_children.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the first child
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur le premier enfant
		 *\return		La valeur
		 */
		inline node_const_iterator childrenBegin()const
		{
			return m_children.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to after the last child
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur apèrs le dernier enfant
		 *\return		La valeur
		 */
		inline node_iterator childrenEnd()
		{
			return m_children.end();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to after the last child
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur apèrs le dernier enfant
		 *\return		La valeur
		 */
		inline node_const_iterator childrenEnd()const
		{
			return m_children.end();
		}
		/**
		 *\~english
		 *\brief		Retrieves the objects map
		 *\return		The value
		 *\~french
		 *\brief		Récupère la map des objets
		 *\return		La valeur
		 */
		inline MovableObjectArray const & getObjects()const
		{
			return m_objects;
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the first attached object
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur le premier objet attaché
		 *\return		La valeur
		 */
		inline object_iterator objectsBegin()
		{
			return m_objects.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the first attached object
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur le premier objet attaché
		 *\return		La valeur
		 */
		inline object_const_iterator objectsBegin()const
		{
			return m_objects.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to after the last attached object
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur après le dernier objet attaché
		 *\return		La valeur
		 */
		inline object_iterator objectsEnd()
		{
			return m_objects.end();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to after the last attached object
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur après le dernier objet attaché
		 *\return		La valeur
		 */
		inline object_const_iterator objectsEnd()const
		{
			return m_objects.end();
		}
		/**
		 *\~english
		 *\brief		Retrieves the child with given name
		 *\param[in]	p_name	The child name
		 *\return		The value, nullptr if not found
		 *\~french
		 *\brief		Récupère l'enfant avec le nom donné
		 *\param[in]	p_name	Le nom de l'enfant
		 *\return		La valeur, nullptr si non trouvé
		 */
		inline SceneNodeSPtr getChild( castor::String const & p_name )const
		{
			return ( m_children.find( p_name ) != m_children.end() ? m_children.find( p_name )->second.lock() : nullptr );
		}
		/**
		 *\~english
		 *\brief		Retrieves the transformations matrices modify status
		 *\return		The value
		 *\~french
		 *\brief		Récupère le statut de modification des matrices de transformation
		 *\return		La valeur
		 */
		inline bool isModified()const
		{
			return m_mtxChanged || m_derivedMtxChanged;
		}

	private:
		void doComputeMatrix();
		/**
		 *\~english
		 *\brief		sets this node's children's absolute transformation matrix to be recalculated
		 *\~french
		 *\brief		Fait que la matrice de transfomation des enfants de ce noeud doit être recalculée
		 */
		void doUpdateChildsDerivedTransform();

	public:
		//!\~english	Signal used to notify attached objects that the node has changed.
		//!\~french		Signal utilisé pour notifier aux objets attachés que le noeud a changé.
		OnChanged onChanged;

	protected:
		//!\~english	Tells if it is displayable. A node is displayable if his parent is either displayable or the root node.
		//!\~french		Dit si le noeud est affichable. Il est affichable si son parent est le noeud racine ou s'il est affichable.
		bool m_displayable;
		//!\~english	The visible status. If a node is hidden, all objects attached to it are hidden.
		//!\~french		Le statut de visibilité. Si un noeud est caché, tous les objets qui y sont attachés sont cachés aussi.
		bool m_visible{ true };
		//!\~english  T	he relative orientation of the node.
		//!\~french		L'orientation du noeud, relative au parent.
		castor::Quaternion m_orientation;
		//!\~english	The relative position of the node.
		//!\~french		La position du noeud, relative au parent.
		castor::Point3r m_position{ 0.0_r, 0.0_r, 0.0_r };
		//!\~english	The relative scale transform value of the node.
		//!\~french		La mise à l'échelle du noeud, relative au parent.
		castor::Point3r m_scale{ 1.0_r, 1.0_r, 1.0_r };
		//!\~english	The reative transformation matrix.
		//!\~french		La matrice de transformation, relative au parent.
		castor::Matrix4x4r m_transform{ 1.0_r };
		//!\~english	Tells the relative transformation matrix needs recomputation.
		//!\~french		Dit si la matrice de transformation relative doit être recalculée.
		bool m_mtxChanged{ true };
		//!\~english	The absolute transformation matrix.
		//!\~french		la matrice de transformation absolue.
		castor::Matrix4x4r m_derivedTransform{ 1.0_r };
		//!\~english	Tells the absolute transformation matrix needs recomputation.
		//!\~french		Dit si la matrice de transformation absolue doit être recalculée.
		bool m_derivedMtxChanged{ true };
		//!\~english	This node's parent.
		//!\~french		Le noeud parent.
		SceneNodeWPtr m_parent;
		//!\~english	This node's childs.
		//!\~french		Les enfants de ce noeud.
		SceneNodePtrStrMap m_children;
		//!\~english	This node's attached objects.
		//!\~french		Les objets attachés à ce noeud.
		MovableObjectArray m_objects;
	};
}

#endif
