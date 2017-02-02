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
#ifndef ___C3D_ANIMATED_SKELETON_H___
#define ___C3D_ANIMATED_SKELETON_H___

#include "AnimatedObject.hpp"

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
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_name		The object name.
		 *\param[in]	p_skeleton	The skeleton.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_name		Le nom de l'objet.
		 *\param[in]	p_skeleton	Le squelette.
		 */
		C3D_API AnimatedSkeleton( Castor::String const & p_name, Skeleton & p_skeleton );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~AnimatedSkeleton();
		/**
		 *\~english
		 *\brief		Move constructor.
		 *\~french
		 *\brief		Constructeur par déplacement.
		 */
		C3D_API AnimatedSkeleton( AnimatedSkeleton && p_rhs ) = default;
		/**
		 *\~english
		 *\brief		Move assignment operator.
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement.
		 */
		C3D_API AnimatedSkeleton & operator=( AnimatedSkeleton && p_rhs ) = default;
		/**
		 *\~english
		 *\brief		Copy constructor.
		 *\~french
		 *\brief		Constructeur par copie.
		 */
		C3D_API AnimatedSkeleton( AnimatedSkeleton const & p_rhs ) = delete;
		/**
		 *\~english
		 *\brief		Copy assignment operator.
		 *\~french
		 *\brief		Opérateur d'affectation par copie.
		 */
		C3D_API AnimatedSkeleton & operator=( AnimatedSkeleton const & p_rhs ) = delete;
		/**
		 *\~english
		 *\brief		Fills a shader variable with this object's skeleton transforms.
		 *\param[out]	p_variable	Receives the transforms.
		 *\~french
		 *\brief		Remplit une variable de shader avec les transformations du squelette de cet objet.
		 *\param[out]	p_variable	Reçoit les transformations.
		 */
		C3D_API void FillShader( Uniform4x4r & p_variable );
		/**
		 *\copydoc		Castor3D::AnimatedObject::Update
		 */
		C3D_API void Update( std::chrono::milliseconds const & p_tslf )override;
		/**
		 *\copydoc		Castor3D::AnimatedObject::IsPlayingAnimation
		 */
		C3D_API bool IsPlayingAnimation()const override
		{
			return !m_playingAnimations.empty();
		}
		/**
		 *\~english
		 *\brief		Retrieves the skeleton
		 *\return		The skeleton
		 *\~french
		 *\brief		Récupère le squelette
		 *\return		Le squelette
		 */
		inline Skeleton const & GetSkeleton()const
		{
			return m_skeleton;
		}
		/**
		 *\~english
		 *\return		The currently animations for this object.
		 *\~french
		 *\return		Les animations en cours de lecture sur cet objet.
		 */
		inline SkeletonAnimationInstancePtrArray const & GetPlayingAnimations()const
		{
			return m_playingAnimations;
		}

	private:
		/**
		 *\copydoc		Castor3D::AnimatedObject::DoAddAnimation
		 */
		void DoAddAnimation( Castor::String const & p_name )override;
		/**
		 *\copydoc		Castor3D::AnimatedObject::DoAddAnimation
		 */
		void DoStartAnimation( AnimationInstanceSPtr p_animation )override;
		/**
		 *\copydoc		Castor3D::AnimatedObject::DoAddAnimation
		 */
		void DoStopAnimation( AnimationInstanceSPtr p_animation )override;
		/**
		 *\copydoc		Castor3D::AnimatedObject::DoAddAnimation
		 */
		void DoClearAnimations()override;

	protected:
		//!\~english	The skeleton affected by the animations.
		//!\~french		Le squelette affecté par les animations.
		Skeleton & m_skeleton;
		//!\~english	Currently playing animations.
		//!\~french		Les animations en cours de lecture.
		SkeletonAnimationInstancePtrArray m_playingAnimations;
	};
}

#endif

