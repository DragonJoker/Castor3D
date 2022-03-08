/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SceneNode_H___
#define ___C3D_SceneNode_H___

#include "Castor3D/Scene/Animation/AnimationModule.hpp"

#include "Castor3D/Animation/Animable.hpp"

#include <CastorUtils/Data/TextWriter.hpp>
#include <CastorUtils/Design/Named.hpp>
#include <CastorUtils/Math/Quaternion.hpp>
#include <CastorUtils/Math/SquareMatrix.hpp>
#include <CastorUtils/Multithreading/SpinMutex.hpp>

namespace castor3d
{
	class SceneNode
		: public std::enable_shared_from_this< SceneNode >
		, public Animable
		, public castor::Named
	{
	public:
		//!\~english	The total number of scene nodes.
		//!\~french		Le nombre total de noeuds de scène.
		static uint64_t Count;
		using SceneNodeMap = std::map< castor::String, SceneNodeWPtr >;
		using MovableArray = std::list< std::reference_wrapper< MovableObject > >;

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
		C3D_API SceneNode( castor::String const & name
			, Scene & scene );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~SceneNode()override;
		/**
		 *\~english
		 *\brief		Updates the scene node matrices.
		 *\~french
		 *\brief		Met à jour les matrices du noeud.
		 */
		C3D_API void update();
		/**
		 *\name Attached objects management.
		**/
		/**@{*/
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
		/**@}*/
		/**
		 *\name Children management.
		**/
		/**@{*/
		/**
		 *\~english
		 *\brief		Sets the parent node
		 *\param[in]	parent	The new parent
		 *\~french
		 *\brief		Définit le noeud parent
		 *\param[in]	parent	Le nouveau parent
		 */
		C3D_API void attachTo( SceneNode & parent );
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
		/**@}*/
		/**
		 *\name Local transformations.
		**/
		/**@{*/
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
		 *\brief		Creates an animation
		 *\param[in]	name	The animation name
		 *\return		The animation
		 *\~french
		 *\brief		Crée une animation
		 *\param[in]	name	Le nom de l'animation
		 *\return		l'animation
		 */
		C3D_API SceneNodeAnimation & createAnimation( castor::String const & name );
		/**
		 *\~english
		 *\brief		Removes an animation
		 *\param[in]	name	The animation name
		 *\~french
		 *\brief		Retire une animation
		 *\param[in]	name	Le nom de l'animation
		 */
		C3D_API void removeAnimation( castor::String const & name );
		/**@}*/
		/**
		 *\name Absolute value getters.
		**/
		/**@{*/
		C3D_API castor::Point3f getDerivedPosition()const;
		C3D_API castor::Quaternion getDerivedOrientation()const;
		C3D_API castor::Point3f getDerivedScale()const;
		C3D_API castor::Matrix4x4f const & getTransformationMatrix()const;
		C3D_API castor::Matrix4x4f const & getDerivedTransformationMatrix()const;
		/**
		 *\name Local value getters.
		**/
		/**@{*/
		castor::Point3f const & getPosition()const
		{
			return m_position;
		}

		castor::Quaternion const & getOrientation()const
		{
			return m_orientation;
		}

		castor::Point3f const & getScale()const
		{
			return m_scale;
		}

		void getAxisAngle( castor::Point3f & axis, castor::Angle & angle )const
		{
			m_orientation.toAxisAngle( axis, angle );
		}
		/**@}*/
		/**
		 *\name Other getters.
		**/
		/**@{*/
		C3D_API bool isVisible()const;

		bool isDisplayable()const
		{
			return m_displayable;
		}

		SceneNode * getParent()const
		{
			return m_parent;
		}

		Scene * getScene()const
		{
			return &m_scene;
		}
		/**@}*/
		/**
		 *\name Other getters.
		**/
		/**@{*/
		C3D_API void setOrientation( castor::Quaternion const & orientation );
		C3D_API void setPosition( castor::Point3f const & position );
		C3D_API void setScale( castor::Point3f const & scale );
		C3D_API void setVisible( bool visible );
		C3D_API SceneNodeMap const & getChildren()const;
		C3D_API SceneNodeSPtr getChild( castor::String const & name )const;
		C3D_API MovableArray const & getObjects()const;

		bool isModified()const
		{
			return m_mtxChanged || m_derivedMtxChanged;
		}

		uint64_t getId()const
		{
			return m_id;
		}
		/**@}*/

	private:
		void doComputeMatrix();
		void doUpdateChildsDerivedTransform();

	public:
		//!\~english	Signal used to notify that the node has changed.
		//!\~french		Signal utilisé pour notifier que le noeud a changé.
		OnSceneNodeChanged onChanged;

	private:
		static uint64_t CurrentId;
		Scene & m_scene;
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
		SceneNode * m_parent{ nullptr };
		mutable castor::SpinMutex m_childrenLock;
		mutable castor::SpinMutex m_objectsLock;
		SceneNodeMap m_children;
		MovableArray m_objects;
	};

	template< typename CacheT >
	struct SceneNodeAttacherT
	{
		using ElementT = typename CacheT::ElementT;

		void operator()( ElementT & element
			, SceneNode & parent
			, SceneNodeSPtr rootNode
			, SceneNodeSPtr rootCameraNode
			, SceneNodeSPtr rootObjectNode )
		{
			element.attachTo( parent );
		}
	};

	template< typename CacheT >
	struct SceneNodeDetacherT
	{
		using ElementT = typename CacheT::ElementT;

		void operator()( ElementT & element )
		{
			element.detach();
		}
	};
}

#endif
