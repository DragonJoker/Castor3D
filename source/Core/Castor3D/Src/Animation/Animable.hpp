/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___C3D_ANIMABLE_H___
#define ___C3D_ANIMABLE_H___

#include "Binary/BinaryParser.hpp"
#include "Binary/BinaryWriter.hpp"

#include <Design/OwnedBy.hpp>

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
		/**
		 *\~english
		 *\brief		Move constructor.
		 *\~french
		 *\brief		Constructeur par déplacement.
		 */
		C3D_API Animable( Animable && p_rhs ) = default;
		/**
		 *\~english
		 *\brief		Move assignment operator.
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement.
		 */
		C3D_API Animable & operator=( Animable && p_rhs ) = default;
		/**
		 *\~english
		 *\brief		Copy constructor.
		 *\~french
		 *\brief		Constructeur par copie.
		 */
		C3D_API Animable( Animable const & p_rhs ) = delete;
		/**
		 *\~english
		 *\brief		Copy assignment operator.
		 *\~french
		 *\brief		Opérateur d'affectation par copie.
		 */
		C3D_API Animable & operator=( Animable const & p_rhs ) = delete;

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
		C3D_API bool HasAnimation( Castor::String const & p_name )const;
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
		C3D_API Animation const & GetAnimation( Castor::String const & p_name )const;
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
		C3D_API Animation & GetAnimation( Castor::String const & p_name );
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

	protected:
		/**
		 *\~english
		 *\brief		Adds an animation.
		 *\param[in]	p_animation	The animation.
		 *\~french
		 *\brief		Ajoute une animation.
		 *\param[in]	p_animation	L'animation.
		 */
		void DoAddAnimation( AnimationSPtr && p_animation );
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
		template< typename Type >
		Type & DoGetAnimation( Castor::String const & p_name )
		{
			return static_cast< Type & >( GetAnimation( p_name ) );
		}

	protected:
		//!\~english	All animations.
		//!\~french		Toutes les animations.
		AnimationPtrStrMap m_animations;
	};
}

#endif
