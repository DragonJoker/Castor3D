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
		 *\copydoc		Castor3D::AnimatedObject::Update
		 */
		C3D_API void Update( real p_tslf )override;
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
		//!\~english	The submesh affected by the animations.
		//!\~french		Le sous-maillage affecté par les animations.
		Mesh & m_mesh;
		//!\~english	Currently playing animation.
		//!\~french		L'animation en cours de lecture.
		MeshAnimationInstanceSPtr m_playingAnimation;
	};
}

#endif

