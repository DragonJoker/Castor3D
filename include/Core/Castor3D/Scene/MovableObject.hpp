/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MovableObject_H___
#define ___C3D_MovableObject_H___

#include "Castor3D/Scene/Animation/AnimationModule.hpp"

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
}

#endif
