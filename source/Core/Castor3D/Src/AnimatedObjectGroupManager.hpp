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
#ifndef ___C3D_ANIMATED_OBJECT_GROUP_MANAGER_H___
#define ___C3D_ANIMATED_OBJECT_GROUP_MANAGER_H___

#include "ObjectManager.hpp"

#include "AnimatedObjectGroup.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		04/02/2016
	\version	0.8.0
	\~english
	\brief		Helper structure to get an object type name.
	\~french
	\brief		Structure permettant de récupérer le nom du type d'un objet.
	*/
	template<> struct ManagedObjectNamer< AnimatedObjectGroup >
	{
		C3D_API static const Castor::String Name;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		29/01/2016
	\version	0.8.0
	\~english
	\brief		AnimatedObjectGroup manager.
	\~french
	\brief		Gestionnaire de AnimatedObjectGroup.
	*/
	class AnimatedObjectGroupManager
		: public ObjectManager< Castor::String, AnimatedObjectGroup >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_owner				The owner.
		 *\param[in]	p_rootNode			The root node.
		 *\param[in]	p_rootCameraNode	The cameras root node.
		 *\param[in]	p_rootObjectNode	The objects root node.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_owner				Le propriétaire.
		 *\param[in]	p_rootNode			Le noeud racine.
		 *\param[in]	p_rootCameraNode	Le noeud racine des caméras.
		 *\param[in]	p_rootObjectNode	Le noeud racine des objets.
		 */
		C3D_API AnimatedObjectGroupManager( Scene & p_owner, SceneNodeSPtr p_rootNode, SceneNodeSPtr p_rootCameraNode, SceneNodeSPtr p_rootObjectNode );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~AnimatedObjectGroupManager();
		/**
		 *\~english
		 *\brief		Updates currently playing animations in animated objects.
		 *\~french
		 *\brief		Met à jour les animations en cours pour les objets animés.
		 */
		C3D_API void Update();
		/**
		 *\~english
		 *\brief		Creates an object from a name.
		 *\param[in]	p_name		The object name.
		 *\param[in]	p_params	The other constructor parameters.
		 *\return		The created object.
		 *\~french
		 *\brief		Crée un objet à partir d'un nom.
		 *\param[in]	p_name		Le nom d'objet.
		 *\param[in]	p_params	Les autres paramètres de construction.
		 *\return		L'objet créé.
		 */
		template< typename ... Parameters >
		inline std::shared_ptr< AnimatedObjectGroup > Create( Castor::String const & p_name, Parameters && ... p_params )
		{
			auto l_lock = Castor::make_unique_lock( m_elements );
			std::shared_ptr< AnimatedObjectGroup > l_return;

			if ( !m_elements.has( p_name ) )
			{
				l_return = std::make_shared< AnimatedObjectGroup >( p_name, *this->GetScene(), std::forward< Parameters >( p_params )... );
				m_elements.insert( p_name, l_return );
				Castor::Logger::LogInfo( Castor::StringStream() << INFO_MANAGER_CREATED_OBJECT << this->GetObjectTypeName() << cuT( ": " ) << p_name );
				GetScene()->SetChanged();
			}
			else
			{
				l_return = m_elements.find( p_name );
				Castor::Logger::LogWarning( Castor::StringStream() << WARNING_MANAGER_DUPLICATE_OBJECT << this->GetObjectTypeName() << cuT( ": " ) << p_name );
			}

			return l_return;
		}
	};
}

#endif
