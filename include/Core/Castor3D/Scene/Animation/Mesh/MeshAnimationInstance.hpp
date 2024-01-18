/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MESH_ANIMATION_INSTANCE_H___
#define ___C3D_MESH_ANIMATION_INSTANCE_H___

#include "Castor3D/Binary/BinaryModule.hpp"

#include "Castor3D/Scene/Animation/AnimationInstance.hpp"
#include "Castor3D/Scene/Animation/Mesh/MeshAnimationInstanceSubmesh.hpp"

namespace castor3d
{
	class MeshAnimationInstance
		: public AnimationInstance
	{
	public:
		C3D_API MeshAnimationInstance( MeshAnimationInstance && rhs ) = default;
		C3D_API MeshAnimationInstance & operator=( MeshAnimationInstance && rhs ) = delete;
		C3D_API MeshAnimationInstance( MeshAnimationInstance const & rhs ) = delete;
		C3D_API MeshAnimationInstance & operator=( MeshAnimationInstance const & rhs ) = delete;
		C3D_API ~MeshAnimationInstance()noexcept override = default;
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
		*\return		The animation submesh at given ID, nullptr if not found.
		*\~french
		*\return		Le sous-maillage d'animation, à l'ID donné, nullptr si non trouvé.
		*/
		C3D_API MeshAnimationInstanceSubmesh const * getAnimationSubmesh( uint32_t index )const;
		/**
		 *\~english
		 *\brief		Moves the objects back to initial state.
		 *\~french
		 *\brief		Remet les objets à l'état initial.
		 */
		C3D_API void clear();
		/**
		 *\~english
		 *\return		The animation.
		 *\~french
		 *\return		L'animation.
		 */
		MeshAnimation const & getMeshAnimation()const
		{
			return m_meshAnimation;
		}
		/**
		 *\~english
		 *\return		The animated mesh.
		 *\~french
		 *\return		Le maillage animé.
		 */
		AnimatedMesh & getAnimatedMesh()
		{
			return m_animatedMesh;
		}

	private:
		void doUpdate()override;

	protected:
		AnimatedMesh & m_animatedMesh;
		MeshAnimation const & m_meshAnimation;
		MeshAnimationInstanceSubmeshMap m_submeshes;
		AnimationKeyFrameArray::iterator m_prev;
		AnimationKeyFrameArray::iterator m_curr;
		bool m_stopping{ false };

		friend class BinaryWriter< MeshAnimation >;
		friend class BinaryParser< MeshAnimation >;
	};
}

#endif
