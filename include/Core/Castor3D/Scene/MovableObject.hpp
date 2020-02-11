/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MOVABLE_OBJECT_H___
#define ___C3D_MOVABLE_OBJECT_H___

#include "Castor3D/Scene/SceneNode.hpp"

#include "Castor3D/Scene/Animation/AnimationModule.hpp"
#include "Castor3D/Animation/Animable.hpp"

#include <CastorUtils/Design/Named.hpp>
#include <CastorUtils/Design/OwnedBy.hpp>

namespace castor3d
{
	class MovableObject
		: public Animable
		, public castor::Named
	{
	public:
		/**
		\author		Sylvain DOREMUS
		\date		14/02/2010
		\~english
		\brief		MovableObject loader
		\~english
		\brief		Loader de MovableObject
		*/
		class TextWriter
			: public castor::TextWriter< MovableObject >
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
			 *\brief		Writes a movable object into a text file
			 *\param[in]	object	the movable object to save
			 *\param[in]	file		The file to save the movable object in
			 *\~french
			 *\brief		Ecrit un MovableObject dans un fichier texte
			 *\param[in]	object	Le MovableObject
			 *\param[in]	file		Le fichier
			 */
			C3D_API bool operator()( MovableObject const & object, castor::TextFile & file )override;
		};

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
			, SceneNodeSPtr node );
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
		C3D_API virtual void attachTo( SceneNodeSPtr node );
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
			return m_sceneNode.lock();
		}
		/**
		 *\~english
		 *\brief		Retrieves the object type
		 *\return		The value
		 *\~french
		 *\brief		Récupère le type d'objet
		 *\return		La valeur
		 */
		inline MovableType getType()const
		{
			return m_type;
		}

	protected:
		//!\~english	Movable object type.
		//!\~french		Le type d'objet déplaçable.
		MovableType m_type;
		//!\~english	The parent node name.
		//!\~french		Le nom du noeud parent.
		castor::String m_strNodeName;
		//!\~english	The parent scene node.
		//!\~french		Le noeud parent.
		SceneNodeWPtr m_sceneNode;
		//!\~english	The node change notification index.
		//!\~french		L'indice de notifcation des changements du noeud.
		OnSceneNodeChangedConnection m_notifyIndex;
	};
}

#endif
