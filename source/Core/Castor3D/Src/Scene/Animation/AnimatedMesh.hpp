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
#ifndef ___C3D_ANIMATED_MESH_H___
#define ___C3D_ANIMATED_MESH_H___

#include "AnimatedObject.hpp"

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		14/06/2016
	\~english
	\brief		Represents the animated submeshes
	\~french
	\brief		Représente les sous-maillages animés
	*/
	class AnimatedMesh
		: public AnimatedObject
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_name	The object name.
		 *\param[in]	p_mesh	The mesh.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_name	Le nom de l'objet.
		 *\param[in]	p_mesh	Le maillage.
		 */
		C3D_API AnimatedMesh( Castor::String const & p_name, Mesh & p_mesh );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~AnimatedMesh();
		/**
		 *\~english
		 *\brief		Move constructor.
		 *\~french
		 *\brief		Constructeur par déplacement.
		 */
		C3D_API AnimatedMesh( AnimatedMesh && p_rhs ) = default;
		/**
		 *\~english
		 *\brief		Move assignment operator.
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement.
		 */
		C3D_API AnimatedMesh & operator=( AnimatedMesh && p_rhs ) = default;
		/**
		 *\~english
		 *\brief		Copy constructor.
		 *\~french
		 *\brief		Constructeur par copie.
		 */
		C3D_API AnimatedMesh( AnimatedMesh const & p_rhs ) = delete;
		/**
		 *\~english
		 *\brief		Copy assignment operator.
		 *\~french
		 *\brief		Opérateur d'affectation par copie.
		 */
		C3D_API AnimatedMesh & operator=( AnimatedMesh const & p_rhs ) = delete;
		/**
		 *\copydoc		Castor3D::AnimatedObject::Update
		 */
		C3D_API void Update( std::chrono::milliseconds const & p_tslf )override;
		/**
		 *\copydoc		Castor3D::AnimatedObject::IsPlayingAnimation
		 */
		C3D_API bool IsPlayingAnimation()const override
		{
			return m_playingAnimation != nullptr;
		}
		/**
		 *\~english
		 *\brief		Retrieves the skeleton
		 *\return		The skeleton
		 *\~french
		 *\brief		Récupère le squelette
		 *\return		Le squelette
		 */
		inline Mesh const & GetMesh()const
		{
			return m_mesh;
		}
		/**
		 *\~english
		 *\brief		Retrieves the skeleton
		 *\return		The skeleton
		 *\~french
		 *\brief		Récupère le squelette
		 *\return		Le squelette
		 */
		inline Mesh & GetMesh()
		{
			return m_mesh;
		}
		/**
		 *\~english
		 *\return		The currently playing animation for this object.
		 *\~french
		 *\return		L'animation en cours de lecture sur cet objet.
		 */
		inline MeshAnimationInstance const & GetPlayingAnimation()const
		{
			return *m_playingAnimation;
		}

	private:
		/**
		 *\copydoc		Castor3D::AnimatedObject::DoAddAnimation
		 */
		void DoAddAnimation( Castor::String const & p_name )override;
		/**
		 *\copydoc		Castor3D::AnimatedObject::DoStartAnimation
		 */
		void DoStartAnimation( AnimationInstance & p_animation )override;
		/**
		 *\copydoc		Castor3D::AnimatedObject::DoStopAnimation
		 */
		void DoStopAnimation( AnimationInstance & p_animation )override;
		/**
		 *\copydoc		Castor3D::AnimatedObject::DoClearAnimations
		 */
		void DoClearAnimations()override;

	protected:
		//!\~english	The submesh affected by the animations.
		//!\~french		Le sous-maillage affecté par les animations.
		Mesh & m_mesh;
		//!\~english	Currently playing animation.
		//!\~french		L'animation en cours de lecture.
		MeshAnimationInstanceRPtr m_playingAnimation;
	};
}

#endif

