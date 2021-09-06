/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MovableObject_H___
#define ___C3D_MovableObject_H___

#include "Castor3D/Scene/Animation/AnimationModule.hpp"

#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Animation/Animable.hpp"

#include <CastorUtils/Data/TextWriter.hpp>
#include <CastorUtils/Design/Named.hpp>

namespace castor3d
{
	class MovableObject
		: public Animable
		, public castor::Named
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	name	The name
		 *\param[in]	scene	The parent scene
		 *\param[in]	node	Parent node
		 *\param[in]	type	MovableObject type
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	name	Le nom
		 *\param[in]	scene	La scène parente
		 *\param[in]	node	Noeud parent
		 *\param[in]	type	Le type de MovableObject
		 */
		C3D_API MovableObject( castor::String const & name
			, Scene & scene
			, MovableType type
			, SceneNode & node );
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	name	The name
		 *\param[in]	scene	The parent scene
		 *\param[in]	type	MovableObject type
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	name	Le nom
		 *\param[in]	scene	La scène parente
		 *\param[in]	type	Le type de MovableObject
		 */
		C3D_API MovableObject( castor::String const & name
			, Scene & scene
			, MovableType type );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~MovableObject();
		/**
		 *\~english
		 *\brief		Detaches the movable object from it's parent
		 *\~french
		 *\brief		Détache l'objet de son parent
		 */
		C3D_API void detach();
		/**
		 *\~english
		 *\brief		Attaches the movable object to a node
		 *\~french
		 *\brief		Attache l'object à un noeud
		 */
		C3D_API virtual void attachTo( SceneNode & node );
		/**
		 *\~english
		 *\brief		Retrieves the parent node
		 *\return		The value
		 *\~french
		 *\brief		Récupère le noeud parent
		 *\return		La valeur
		 */
		SceneNode *  getParent()const
		{
			return m_sceneNode;
		}
		/**
		 *\~english
		 *\brief		Retrieves the object type
		 *\return		The value
		 *\~french
		 *\brief		Récupère le type d'objet
		 *\return		La valeur
		 */
		MovableType getType()const
		{
			return m_type;
		}

		Scene * getScene()const
		{
			return m_scene;
		}

	protected:
		Scene * m_scene;
		//!\~english	Movable object type.
		//!\~french		Le type d'objet déplaçable.
		MovableType m_type;
		//!\~english	The parent node name.
		//!\~french		Le nom du noeud parent.
		castor::String m_strNodeName;
		//!\~english	The parent scene node.
		//!\~french		Le noeud parent.
		SceneNode * m_sceneNode;
		//!\~english	The node change notification index.
		//!\~french		L'indice de notifcation des changements du noeud.
		OnSceneNodeChangedConnection m_notifyIndex;
	};

	template< typename CacheT >
	struct MovableMergerT
		: public castor::Named
	{
		MovableMergerT( castor::String const & name )
			: castor::Named{ name }
		{
		}

		void operator()( typename CacheT::ElementObjectCacheT const & source
			, typename CacheT::ElementContT & destination
			, typename CacheT::ElementPtrT element
			, SceneNodeSPtr rootCameraNode
			, SceneNodeSPtr rootObjectNode )const
		{
			using ElementPtrT = typename CacheT::ElementPtrT;

			if ( element->getParent()->getName() == rootCameraNode->getName() )
			{
				element->detach();
				element->attachTo( *rootCameraNode );
			}
			else if ( element->getParent()->getName() == rootObjectNode->getName() )
			{
				element->detach();
				element->attachTo( *rootObjectNode );
			}

			auto name = element->getName();
			auto ires = destination.emplace( name, ElementPtrT{} );

			while ( !ires.second )
			{
				name = getName() + cuT( "_" ) + name;
				ires = destination.emplace( name, ElementPtrT{} );
			}

			ires.first->second = std::move( element );
			ires.first->second->rename( name );
		}
	};

	template< typename CacheT >
	struct MovableAttacherT
	{
		using ElementT = typename CacheT::ElementT;

		void operator()( ElementT & element
			, SceneNode & parent
			, SceneNodeSPtr rootNode
			, SceneNodeSPtr rootCameraNode
			, SceneNodeSPtr rootObjectNode )
		{
			parent.attachObject( element );
		}
	};

	template< typename CacheT >
	struct MovableDetacherT
	{
		using ElementT = typename CacheT::ElementT;

		void operator()( ElementT & element )
		{
			element.detach();
		}
	};
}

#endif
