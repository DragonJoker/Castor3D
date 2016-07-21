/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

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
#ifndef ___C3D_MOVABLE_OBJECT_H___
#define ___C3D_MOVABLE_OBJECT_H___

#include "SceneNode.hpp"

#include "Animation/Animable.hpp"

#include <Design/OwnedBy.hpp>

namespace Castor3D
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
		: public std::enable_shared_from_this< MovableObject >
		, public Animable
		, public Castor::Named
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
			: public Castor::TextWriter< MovableObject >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			C3D_API explicit TextWriter( Castor::String const & p_tabs );
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
			C3D_API bool operator()( MovableObject const & p_object, Castor::TextFile & p_file )override;
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
		C3D_API MovableObject( Castor::String const & p_name, Scene & p_scene, eMOVABLE_TYPE p_type, SceneNodeSPtr p_sn );
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
		C3D_API void Detach();
		/**
		 *\~english
		 *\brief		Attaches the movable object to a node
		 *\~french
		 *\brief		Attache l'object à un noeud
		 */
		C3D_API virtual void AttachTo( SceneNodeSPtr p_node );
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
			return m_pSceneNode.lock();
		}
		/**
		 *\~english
		 *\brief		Retrieves the object type
		 *\return		The value
		 *\~french
		 *\brief		Récupère le type d'objet
		 *\return		La valeur
		 */
		inline eMOVABLE_TYPE GetType()const
		{
			return m_type;
		}

	protected:
		//!\~english Movable object type	\~french Le type d'objet déplaçable
		eMOVABLE_TYPE m_type;
		//!\~english The parent node name	\~french Le nom du noeud parent
		Castor::String m_strNodeName;
		//!\~english The parent scene node	\~french Le noeud parent
		SceneNodeWPtr m_pSceneNode;
		//!\~english The node change notification index.	\~french L'indice de notifcation des changements du noeud.
		uint32_t m_notifyIndex = 0;
	};
}

#endif
