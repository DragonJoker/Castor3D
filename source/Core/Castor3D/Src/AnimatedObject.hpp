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
#ifndef ___C3D_ANIMATED_OBJECT_H___
#define ___C3D_ANIMATED_OBJECT_H___

#include "Castor3DPrerequisites.hpp"

#include <Named.hpp>

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.7.0
	\date		10/12/2013
	\~english
	\brief		Represents the animated objects
	\~french
	\brief		Représente les objets animés
	*/
	class AnimatedObject
		: public Castor::Named
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		C3D_API AnimatedObject();
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_animations	The animations map
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_animations	La map d'animations
		 */
		C3D_API AnimatedObject( AnimationPtrStrMap * p_animations );
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_object		The MovableObject to affect
		 *\param[in]	p_animations	The animations map
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_object		Le MovableObject affecté
		 *\param[in]	p_animations	La map d'animations
		 */
		C3D_API AnimatedObject( MovableObjectSPtr p_object, AnimationPtrStrMap * p_animations );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~AnimatedObject();
		/**
		 *\~english
		 *\brief		Sets the MovableObject
		 *\param[in]	p_object	The MovableObject
		 *\~french
		 *\brief		Définit le MovableObject
		 *\param[in]	p_object	Le MovableObject
		 */
		C3D_API void SetObject( MovableObjectSPtr p_object );
		/**
		 *\~english
		 *\brief		Retrieves the MovableObject
		 *\return		The MovableObject
		 *\~french
		 *\brief		Récupère le MovableObject
		 *\return		Le MovableObject
		 */
		inline MovableObjectSPtr GetMovableObject()const
		{
			return m_object.lock();
		}

	protected:
		//!\~english All animations	\~french Toutes les animations
		AnimationPtrStrMap * m_animations;
		//! The object affected by the animations	\~french	L'objet affecté par les animations
		MovableObjectWPtr m_object;
	};
}

#endif

