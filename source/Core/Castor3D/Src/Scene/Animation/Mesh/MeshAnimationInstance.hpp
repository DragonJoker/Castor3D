/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MESH_ANIMATION_INSTANCE_H___
#define ___C3D_MESH_ANIMATION_INSTANCE_H___

#include "Scene/Animation/AnimationInstance.hpp"
#include "Scene/Animation/Mesh/MeshAnimationInstanceSubmesh.hpp"

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		31/05/2016
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
		/**
		 *\~english
		 *\return		The current keyframe current ratio.
		 *\~french
		 *\return		Le ratio actuel dans la keyframe courante.
		 */
		inline float getRatio()const
		{
			return m_ratio;
		}

	private:
		/**
		 *\copydoc		castor3d::AnimationInstance::doUpdate
		 */
		void doUpdate()override;

	protected:
		AnimatedMesh & m_animatedMesh;
		MeshAnimation const & m_meshAnimation;
		MeshAnimationInstanceSubmeshMap m_submeshes;
		AnimationKeyFrameArray::iterator m_prev;
		AnimationKeyFrameArray::iterator m_curr;
		float m_ratio{ 0.0f };

		friend class BinaryWriter< MeshAnimation >;
		friend class BinaryParser< MeshAnimation >;
	};
}

#endif
