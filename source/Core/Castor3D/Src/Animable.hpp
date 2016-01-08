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
#ifndef ___C3D_ANIMABLE_H___
#define ___C3D_ANIMABLE_H___

#include "Castor3DPrerequisites.hpp"

#include <OwnedBy.hpp>

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		09/02/2010
	\version	0.7.0
	\~english
	\brief		Animable public interface
	\~french
	\brief		interface publique d'animable
	*/
	class Animable
		: public Castor::OwnedBy< Engine >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		C3D_API Animable( Engine & p_engine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~Animable();
		/**
		*\~english
		*\brief		Creates an animation
		*\param[in]	p_name	The animation name
		*\return		The animation
		*\~french
		*\brief		Crée une animation
		*\param[in]	p_name	Le nom de l'animation
		*\return		l'animation
		*/
		C3D_API AnimationSPtr CreateAnimation( Castor::String const & p_name );
		/**
		*\~english
		*\brief		Retrieves an animation
		*\param[in]	p_name	The animation name
		*\return		The animation
		*\~french
		*\brief		Récupère une animation
		*\param[in]	p_name	Le nom de l'animation
		*\return		L'animation
		*/
		C3D_API AnimationSPtr GetAnimation( Castor::String const & p_name );
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the first animation
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur la première animation
		 *\return		La valeur
		 */
		inline AnimationPtrStrMapIt AnimationsBegin()
		{
			return m_mapAnimations.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the first animation
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur la première animation
		 *\return		La valeur
		 */
		inline AnimationPtrStrMapConstIt AnimationsBegin()const
		{
			return m_mapAnimations.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the end of the animations map
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur la fin de la map d'animations
		 *\return		La valeur
		 */
		inline AnimationPtrStrMapIt AnimationsEnd()
		{
			return m_mapAnimations.end();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the end of the animations map
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur la fin de la map d'animations
		 *\return		La valeur
		 */
		inline AnimationPtrStrMapConstIt AnimationsEnd()const
		{
			return m_mapAnimations.end();
		}

	protected:
		//!\~english All animations	\~french Toutes les animations
		AnimationPtrStrMap m_mapAnimations;
	};
}

#endif
