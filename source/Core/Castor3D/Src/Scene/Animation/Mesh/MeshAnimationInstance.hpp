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
		C3D_API MeshAnimationInstance( AnimatedMesh & p_object, MeshAnimation & p_animation );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~MeshAnimationInstance();
		/**
		 *\~english
		 *\brief		Move constructor.
		 *\~french
		 *\brief		Constructeur par déplacement.
		 */
		C3D_API MeshAnimationInstance( MeshAnimationInstance && p_rhs ) = default;
		/**
		 *\~english
		 *\brief		Move assignment operator.
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement.
		 */
		C3D_API MeshAnimationInstance & operator=( MeshAnimationInstance && p_rhs ) = default;
		/**
		 *\~english
		 *\brief		Copy constructor.
		 *\~french
		 *\brief		Constructeur par copie.
		 */
		C3D_API MeshAnimationInstance( MeshAnimationInstance const & p_rhs ) = delete;
		/**
		 *\~english
		 *\brief		Copy assignment operator.
		 *\~french
		 *\brief		Opérateur d'affectation par copie.
		 */
		C3D_API MeshAnimationInstance & operator=( MeshAnimationInstance const & p_rhs ) = delete;
		/**
		*\~english
		*\return		The animation submesh at given ID, nullptr if not found.
		*\~french
		*\return		Le sous-maillage d'animation, à l'ID donné, nullptr si non trouvé.
		*/
		C3D_API MeshAnimationInstanceSubmesh const * GetAnimationSubmesh( uint32_t p_index )const;
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
		 *\copydoc		Castor3D::AnimationInstance::DoUpdate
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
		MeshAnimationInstanceSubmeshMap m_submeshes;

		friend class BinaryWriter< MeshAnimation >;
		friend class BinaryParser< MeshAnimation >;
	};
}

#endif
