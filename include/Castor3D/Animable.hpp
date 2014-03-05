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
#ifndef ___C3D_Animable___
#define ___C3D_Animable___

#include "Prerequisites.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

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
	class C3D_API Animable
	{
	protected:
		//!\~english	All animations	\~french	Toutes les animations
		AnimationPtrStrMap m_mapAnimations;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		Animable();
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~Animable();
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the first animation
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur la première animation
		 *\return		La valeur
		 */
		inline AnimationPtrStrMapIt AnimationsBegin() { return m_mapAnimations.begin(); }
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the first animation
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur la première animation
		 *\return		La valeur
		 */
		inline AnimationPtrStrMapConstIt AnimationsBegin()const { return m_mapAnimations.begin(); }
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the end of the animations map
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur la fin de la map d'animations
		 *\return		La valeur
		 */
		inline AnimationPtrStrMapIt AnimationsEnd() { return m_mapAnimations.end(); }
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the end of the animations map
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur la fin de la map d'animations
		 *\return		La valeur
		 */
		inline AnimationPtrStrMapConstIt AnimationsEnd()const { return m_mapAnimations.end(); }
		/**
		 *\~english
		 *\brief		Creates an animation
		 *\param[in]	p_strName	The animation name
		 *\return		The animation
		 *\~french
		 *\brief		Crée une animation
		 *\param[in]	p_strName	Le nom de l'animation
		 *\return		l'animation
		 */
		AnimationSPtr CreateAnimation( Castor::String const & p_strName );
		/**
		 *\~english
		 *\brief		Retrieves an animation
		 *\param[in]	p_strName	The animation name
		 *\return		The animation
		 *\~french
		 *\brief		Récupère une animation
		 *\param[in]	p_strName	Le nom de l'animation
		 *\return		L'animation
		 */
		AnimationSPtr GetAnimation( Castor::String const & p_strName );
	};
}

#pragma warning( pop )

#endif
