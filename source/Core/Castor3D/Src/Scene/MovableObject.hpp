/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MOVABLE_OBJECT_H___
#define ___C3D_MOVABLE_OBJECT_H___

#include "SceneNode.hpp"

#include "Animation/Animable.hpp"

#include <Design/Named.hpp>
#include <Design/OwnedBy.hpp>

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Movable object class
	\~french
	\brief		Classe d'objet déplaçable
	*/
	class MovableObject
		: public Animable
		, public castor::Named
	{
	public:
		/*!
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
			C3D_API explicit TextWriter( castor::String const & p_tabs );
			/**
			 *\~english
			 *\brief		Writes a movable object into a text file
			 *\param[in]	p_object	the movable object to save
			 *\param[in]	p_file		The file to save the movable object in
			 *\~french
			 *\brief		Ecrit un MovableObject dans un fichier texte
			 *\param[in]	p_object	Le MovableObject
			 *\param[in]	p_file		Le fichier
			 */
			C3D_API bool operator()( MovableObject const & p_object, castor::TextFile & p_file )override;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_name	The name
		 *\param[in]	p_scene	The parent scene
		 *\param[in]	p_sn	Parent node
		 *\param[in]	p_type	MovableObject type
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_name	Le nom
		 *\param[in]	p_scene	La scène parente
		 *\param[in]	p_sn	Noeud parent
		 *\param[in]	p_type	Le type de MovableObject
		 */
		C3D_API MovableObject( castor::String const & p_name, Scene & p_scene, MovableType p_type, SceneNodeSPtr p_sn );
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
		C3D_API virtual void attachTo( SceneNodeSPtr p_node );
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
		SceneNode::OnChanged::connection m_notifyIndex;
	};
}

#endif
