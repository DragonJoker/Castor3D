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
#ifndef ___C3D_ANIMABLE_H___
#define ___C3D_ANIMABLE_H___

#include "Binary/BinaryParser.hpp"
#include "Binary/BinaryWriter.hpp"

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
		: public Castor::OwnedBy< Scene >
	{
	protected:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_scene	The scene.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_scene	La scène.
		 */
		explicit Animable( Scene & p_scene );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~Animable();

	public:
		/**
		 *\~english
		 *\brief		Empties the animations map.
		 *\~french
		 *\brief		Vid ela map d'animations.
		 */
		C3D_API void CleanupAnimations();
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
		 *\return		The animations.
		 *\~french
		 *\return		Les animations.
		 */
		inline AnimationPtrStrMap const & GetAnimations()const
		{
			return m_animations;
		}
		/**
		 *\~english
		 *\brief		Retrieves the animated skeleton instance
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'instance animée du squelette
		 *\return		La valeur
		 */
		inline AnimatedObjectSPtr const & GetAnimatedObject()const
		{
			return m_animatedObject;
		}
		/**
		 *\~english
		 *\brief		Sets the animated skeleton instance
		 *\param[in]	p_object	The new value
		 *\~french
		 *\brief		Définit l'instance animée du squelette
		 *\param[in]	p_object	La nouvelle valeur
		 */
		inline void SetAnimatedObject( AnimatedObjectSPtr const & p_object )
		{
			m_animatedObject = p_object;
		}

	protected:
		/**
		 *\~english
		 *\brief		Adds an animation.
		 *\param[in]	p_animation	The animation.
		 *\~french
		 *\brief		Ajoute une animation.
		 *\param[in]	p_animation	L'animation.
		 */
		void DoAddAnimation( AnimationSPtr p_animation );

	protected:
		//!\~english	All animations.
		//!\~french		Toutes les animations.
		AnimationPtrStrMap m_animations;
		//!\~english	The animated object instance, if any.
		//!\~french		L'instance d'objet animé, s'il y en a un.
		AnimatedObjectSPtr m_animatedObject;
	};
}

#endif
