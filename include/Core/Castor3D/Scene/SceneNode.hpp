/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SCENE_NODE_H___
#define ___C3D_SCENE_NODE_H___

#include "SceneModule.hpp"

#include <CastorUtils/Design/Named.hpp>
#include <CastorUtils/Design/OwnedBy.hpp>
#include <CastorUtils/Design/Signal.hpp>

namespace castor3d
{
	class SceneNode
		: public std::enable_shared_from_this< SceneNode >
		, public castor::OwnedBy< Scene >
		, public castor::Named
	{
	public:
		//!\~english	The total number of scene nodes.
		//!\~french		Le nombre total de noeuds de scène.
		static uint64_t Count;
		/**
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
			C3D_API explicit TextWriter( castor::String const & tabs );
			/**
			 *\~english
			 * Writes a castor3d::SceneNode into a text file
			 *\param[in]	file	the file to write the scene node in
			 *\param[in]	node	the scene node to write
			 *\return \p true if successful
			 *\~french
			 * Ecrit un castor3d::SceneNode dans un fichier texte
			 *\param[in]	file	le fichier dans lequel le castor3d::SceneNode est écrit
			 *\param[in]	node	Le castor3d::SceneNode à écrire
			 *\return \p true si tout s'est bien passé, \p false sinon
			 */
			C3D_API bool operator()( SceneNode const & node, castor::TextFile & file )override;
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
		 *\param[in]	name		The node's name.
		 *\param[in]	scene		The parent scene.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	name		Le nom du noeud.
		 *\param[in]	scene		La scène parente.
		 */
		C3D_API SceneNode( castor::String const & name, Scene & scene );
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
		 *\param[in]	object	The object to attach
		 *\~french
		 *\brief		Attache un MovableObject au noeud
		 *\param[in]	object	L'objet à attacher
		 */
		C3D_API void attachObject( MovableObject & object );
		/**
		 *\~english
		 *\brief		Detaches a MovableObject from the node
		 *\param[in]	object	The object to detach
		 *\~french
		 *\brief		Détache un MovableObject fu noeud
		 *\param[in]	object	L'objet à détacher
		 */
		C3D_API void detachObject( MovableObject & object );
		/**
		 *\~english
		 *\brief		Sets the parent node
		 *\param[in]	parent	The new parent
		 *\~french
		 *\brief		Définit le noeud parent
		 *\param[in]	parent	Le nouveau parent
		 */
		C3D_API void attachTo( SceneNodeSPtr parent );
		/**
		 *\~english
		 *\brief		Detaches the node from it's parent
		 *\~french
		 *\brief		Détache le noeud de son parent
		 */
		C3D_API void detach();
		/**
		 *\~english
		 *\param[in]	name	The name of the node
		 *\return		\p true if one of my child has the given name
		 *\~french
		 *\param[in]	name	Le nom du noeud
		 *\return		\p true si un des enfants de ce noeud a le nom donné
		 */
		C3D_API bool hasChild( castor::String const & name );
		/**
		 *\~english
		 *\brief		add the given node to my childs if it isn't already
		 *\param[in]	child	The node to add
		 *\~french
		 *\brief		Ajoute le noeud donné aux enfants de ce noeud, s'il n'y est pas encore
		 *\param[in]	child	Le noeud à ajouter
		 */
		C3D_API void addChild( SceneNodeSPtr child );
		/**
		 *\~english
		 *\brief		Detaches a child from my child's list, if it is one of my childs
		 *\param[in]	child	The child to detach
		 *\~french
		 *\brief		Détache un noeud des enfants de ce noeud, s'il en fait partie
		 *\param[in]	child	Le noeud à détacher
		 */
		C3D_API void detachChild( SceneNodeSPtr child );
		/**
		 *\~english
		 *\brief		Detaches a child from my child's list, if it is one of my childs
		 *\param[in]	childName	The name of the child to detach
		 *\~french
		 *\brief		Détache un noeud des enfants de ce noeud, s'il en fait partie
		 *\param[in]	childName	Le nom du noeud à détacher
		 */
		C3D_API void detachChild( castor::String const & childName );
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
		 *\param[in]	angle	The rotation angle
		 *\~french
		 *\brief		Tourne le noeud autour de son axe Y
		 *\param[in]	angle	L'angle de rotation
		 */
		C3D_API void yaw( castor::Angle const & angle );
		/**
		 *\~english
		 *\brief		Rotates around Z axis
		 *\param[in]	angle	The rotation angle
		 *\~french
		 *\brief		Tourne le noeud autour de son axe Z
		 *\param[in]	angle	L'angle de rotation
		 */
		C3D_API void pitch( castor::Angle const & angle );
		/**
		 *\~english
		 *\brief		Rotates around X axis
		 *\param[in]	angle	The rotation angle
		 *\~french
		 *\brief		Tourne le noeud autour de son axe X
		 *\param[in]	angle	L'angle de rotation
		 */
		C3D_API void roll( castor::Angle const & angle );
		/**
		 *\~english
		 *\brief		Rotate the node with the given orientation
		 *\param[in]	quat	The orientation to add to current one
		 *\~french
		 *\brief		Tourne le noeud d'une rotation donnée
		 *\param[in]	quat	La rotation à appliquer
		 */
		C3D_API void rotate( castor::Quaternion const & quat );
		/**
		 *\~english
		 *\brief		Translates the node
		 *\param[in]	t	The translation value
		 *\~french
		 *\brief		Translate le noeud
		 *\param[in]	t	The La valeur de translation
		 */
		C3D_API void translate( castor::Point3f const & t );
		/**
		 *\~english
		 *\brief		Scales the node
		 *\param[in]	s	The scale value
		 *\~french
		 *\brief		Change l'échelle du noeud
		 *\param[in]	s	La valeur d'échelle
		 */
		C3D_API void scale( castor::Point3f const & s );
		/**
		 *\~english
		 *\brief		Sets the orientation
		 *\param[in]	orientation	The new orientation
		 *\~french
		 *\brief		Définit l'orientation du noeud
		 *\param[in]	orientation	La nouvelle orientation
		 */
		C3D_API void setOrientation( castor::Quaternion const & orientation );
		/**
		 *\~english
		 *\brief		Sets the relative position from a Point3r
		 *\param[in]	position	The new position
		 *\~french
		 *\brief		Définit la position relative du noeud
		 *\param[in]	position	La nouvelle valeur
		 */
		C3D_API void setPosition( castor::Point3f const & position );
		/**
		 *\~english
		 *\brief		Sets the relative scale from a Point3r
		 *\param[in]	scale	The new scale
		 *\~french
		 *\brief		Définit l'échelle relative du noeud
		 *\param[in]	scale	La nouvelle valeur
		 */
		C3D_API void setScale( castor::Point3f const & scale );
		/**
		 *\~english
		 *\brief		Retrieves the absolute position
		 *\return		The value
		 *\~french
		 *\brief		Récupère la position absolue
		 *\return		La valeur
		 */
		C3D_API castor::Point3f getDerivedPosition()const;
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
		C3D_API castor::Point3f getDerivedScale()const;
		/**
		 *\~english
		 *\brief		Retrieves the relative transformation matrix
		 *\return		The value
		 *\~french
		 *\brief		Récupère la matrice de transformation relative
		 *\return		La valeur
		 */
		C3D_API castor::Matrix4x4f const & getTransformationMatrix()const;
		/**
		 *\~english
		 *\brief		Retrieves the absolute transformation matrix
		 *\return		The value
		 *\~french
		 *\brief		Récupère la matrice de transformation absolue
		 *\return		La valeur
		 */
		C3D_API castor::Matrix4x4f const & getDerivedTransformationMatrix()const;
		/**
		 *\~english
		 *\brief		Sets the node visibility status
		 *\param[in]	visible	The new value
		 *\~french
		 *\brief		Définit le statut de visibilité du noeud
		 *\param[in]	visible	La nouvelle valeur
		 */
		C3D_API void setVisible( bool visible );
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
		inline castor::Point3f const & getPosition()const
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
		inline castor::Point3f const & getScale()const
		{
			return m_scale;
		}
		/**
		 *\~english
		 *\brief		Retrieves the orientation in axis and angle terms
		 *\param[out]	axis	Receives the axis
		 *\param[out]	angle	Receives the angle
		 *\~french
		 *\brief		Récupère l'orientation, en termes d'angle et d'axe
		 *\param[out]	axis	Reçoit l'axe
		 *\param[out]	angle	Reçoit l'angle
		 */
		inline void getAxisAngle( castor::Point3f & axis, castor::Angle & angle )const
		{
			m_orientation.toAxisAngle( axis, angle );
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
		 *\param[in]	name	The child name
		 *\return		The value, nullptr if not found
		 *\~french
		 *\brief		Récupère l'enfant avec le nom donné
		 *\param[in]	name	Le nom de l'enfant
		 *\return		La valeur, nullptr si non trouvé
		 */
		inline SceneNodeSPtr getChild( castor::String const & name )const
		{
			return ( m_children.find( name ) != m_children.end() ? m_children.find( name )->second.lock() : nullptr );
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
		/**
		 *\~english
		 *\return		The scene node ID.
		 *\~french
		 *\return		L'ID du noeud.
		 */
		inline uint64_t getId()const
		{
			return m_id;
		}

	private:
		void doComputeMatrix();
		/**
		 *\~english
		 *\brief		Sets this node's children's absolute transformation matrix to be recalculated
		 *\~french
		 *\brief		Fait que la matrice de transfomation des enfants de ce noeud doit être recalculée
		 */
		void doUpdateChildsDerivedTransform();

	public:
		//!\~english	Signal used to notify attached objects that the node has changed.
		//!\~french		Signal utilisé pour notifier aux objets attachés que le noeud a changé.
		OnSceneNodeChanged onChanged;

	private:
		static uint64_t CurrentId;
		uint64_t m_id;
		bool m_displayable;
		bool m_visible{ true };
		castor::Quaternion m_orientation;
		castor::Point3f m_position{ 0.0f, 0.0f, 0.0f };
		castor::Point3f m_scale{ 1.0, 1.0f, 1.0f };
		castor::Matrix4x4f m_transform{ 1.0f };
		bool m_mtxChanged{ true };
		castor::Matrix4x4f m_derivedTransform{ 1.0f };
		bool m_derivedMtxChanged{ true };
		SceneNodeWPtr m_parent;
		SceneNodePtrStrMap m_children;
		MovableObjectArray m_objects;
	};
}

#endif
