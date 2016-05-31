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
#ifndef ___C3D_ANIMATED_SKELETON_H___
#define ___C3D_ANIMATED_SKELETON_H___

#include "Animation/AnimatedObject.hpp"

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
	class AnimatedSkeleton
		: public AnimatedObject
	{
	public:
		/*!
		\author		Sylvain DOREMUS
		\date		28/05/2016
		\~english
		\brief		AnimatedObject loader.
		\~english
		\brief		Loader de AnimatedObject.
		*/
		class TextWriter
			: public Castor::TextWriter< AnimatedSkeleton >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor.
			 *\~french
			 *\brief		Constructeur.
			 */
			C3D_API TextWriter( Castor::String const & p_tabs );
			/**
			 *\~english
			 *\brief		Writes a AnimatedObject into a text file.
			 *\param[in]	p_object	the write to save.
			 *\param[in]	p_file		the file to write the AnimatedObject in.
			 *\~french
			 *\brief		Ecrit un AnimatedObject dans un fichier texte.
			 *\param[in]	p_object	L'AnimatedObject.
			 *\param[in]	p_file		Le fichier.
			 */
			C3D_API virtual bool operator()( AnimatedSkeleton const & p_object, Castor::TextFile & p_file );
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_name	The object name
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_name	Le nom de l'objet
		 */
		C3D_API AnimatedSkeleton( Castor::String const & p_name );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~AnimatedSkeleton();
		/**
		 *\~english
		 *\brief		Defines the skeleton
		 *\param[in]	p_skeleton	The skeleton
		 *\~french
		 *\brief		Définit le squelette
		 *\param[in]	p_skeleton	Le squelette
		 */
		C3D_API void SetSkeleton( SkeletonSPtr p_skeleton );
		/**
		 *\~english
		 *\brief		Retrieves the skeleton
		 *\return		The skeleton
		 *\~french
		 *\brief		Récupère le squelette
		 *\return		Le squelette
		 */
		inline SkeletonSPtr GetSkeleton()const
		{
			return m_skeleton.lock();
		}

	private:
		/**
		 *\copydoc		Castor3D::AnimatedObject::DoFillShader
		 */
		void DoFillShader( Matrix4x4rFrameVariable & p_variable )override;
		/**
		 *\copydoc		Castor3D::AnimatedObject::DoCopyAnimations
		 */
		void DoCopyAnimations( AnimableSPtr p_object )override;

	protected:
		//! The skeleton affected by the animations.	\~french Le squelette affecté par les animations.
		SkeletonWPtr m_skeleton;
	};
}

#endif

