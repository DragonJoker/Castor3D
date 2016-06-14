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
#ifndef ___C3D_MESH_ANIMATION_INSTANCE_H___
#define ___C3D_MESH_ANIMATION_INSTANCE_H___

#include "Scene/Animation/AnimationInstance.hpp"

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		31/05/2016
	\todo		Write and Read functions.
	\~english
	\brief		Mesh animation instance.
	\~french
	\brief		Instance d'animation de maillage.
	*/
	class MeshAnimationInstance
		: public AnimationInstance
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_object	The parent AnimatedMesh.
		 *\param[in]	p_animation	The animation.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_object	L'AnimatedMesh parent.
		 *\param[in]	p_animation	L'animation.
		 */
		C3D_API MeshAnimationInstance( AnimatedMesh & p_object, MeshAnimation const & p_animation );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~MeshAnimationInstance();
		/**
		 *\~english
		 *\return		The animation.
		 *\~french
		 *\return		L'animation.
		 */
		inline MeshAnimation const & GetMeshAnimation()const
		{
			return m_meshAnimation;
		}

	private:
		/**
		 *\~copydoc		Castor3D::AnimationInstance::DoUpdate
		 */
		void DoUpdate()override;

	protected:
		//!\~english	The animated mesh.
		//!\~french		Le maillage animé.
		AnimatedMesh & m_animatedMesh;
		//!\~english	The mesh animation.
		//!\~french		L'animation de maillage.
		MeshAnimation const & m_meshAnimation;
		//!\~english	The animated submeshes.
		//!\~french		Les sous-maillages animés.
		MeshAnimationInstanceSubmeshArray m_submeshes;

		friend class BinaryWriter< MeshAnimation >;
		friend class BinaryParser< MeshAnimation >;
	};
}

#endif
