/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MESH_ANIMATION_INSTANCE_H___
#define ___C3D_MESH_ANIMATION_INSTANCE_H___

#include "Scene/Animation/AnimationInstance.hpp"

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		31/05/2016
	\todo		write and read functions.
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
		 *name Copy / Move.
		 *\~french
		 *name Copie / Déplacement.
		 **/
		/**@{*/
		C3D_API MeshAnimationInstance( MeshAnimationInstance && rhs ) = default;
		C3D_API MeshAnimationInstance & operator=( MeshAnimationInstance && rhs ) = default;
		C3D_API MeshAnimationInstance( MeshAnimationInstance const & rhs ) = delete;
		C3D_API MeshAnimationInstance & operator=( MeshAnimationInstance const & rhs ) = delete;
		/**@}*/
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	object		The parent AnimatedMesh.
		 *\param[in]	animation	The animation.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	object		L'AnimatedMesh parent.
		 *\param[in]	animation	L'animation.
		 */
		C3D_API MeshAnimationInstance( AnimatedMesh & object, MeshAnimation & animation );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~MeshAnimationInstance();
		/**
		*\~english
		*\return		The animation submesh at given ID, nullptr if not found.
		*\~french
		*\return		Le sous-maillage d'animation, à l'ID donné, nullptr si non trouvé.
		*/
		C3D_API MeshAnimationInstanceSubmesh const * getAnimationSubmesh( uint32_t index )const;
		/**
		 *\~english
		 *\return		The animation.
		 *\~french
		 *\return		L'animation.
		 */
		inline MeshAnimation const & getMeshAnimation()const
		{
			return m_meshAnimation;
		}
		/**
		 *\~english
		 *\return		The animated mesh.
		 *\~french
		 *\return		Le maillage animé.
		 */
		inline AnimatedMesh & getAnimatedMesh()
		{
			return m_animatedMesh;
		}

	private:
		/**
		 *\copydoc		castor3d::AnimationInstance::doUpdate
		 */
		void doUpdate()override;

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
		//!\~english	Iterator to the previous keyframe (when playing the animation).
		//!\~french		Itérateur sur la keyframe précédente (quand l'animation est jouée).
		AnimationKeyFrameArray::iterator m_prev;
		//!\~english	Iterator to the current keyframe (when playing the animation).
		//!\~french		Itérateur sur la keyframe courante (quand l'animation est jouée).
		AnimationKeyFrameArray::iterator m_curr;

		friend class BinaryWriter< MeshAnimation >;
		friend class BinaryParser< MeshAnimation >;
	};
}

#endif
