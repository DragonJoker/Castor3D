/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SceneNodeAnimationInstance_H___
#define ___C3D_SceneNodeAnimationInstance_H___

#include "SceneNodeAnimationModule.hpp"
#include "Castor3D/Scene/Animation/AnimationModule.hpp"
#include "Castor3D/Binary/BinaryModule.hpp"
#include "Castor3D/Animation/Interpolator.hpp"

#include "Castor3D/Scene/Animation/AnimationInstance.hpp"

namespace castor3d
{
	class SceneNodeAnimationInstance
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
		C3D_API SceneNodeAnimationInstance( SceneNodeAnimationInstance && rhs ) = default;
		C3D_API SceneNodeAnimationInstance & operator=( SceneNodeAnimationInstance && rhs ) = delete;
		C3D_API SceneNodeAnimationInstance( SceneNodeAnimationInstance const & rhs ) = delete;
		C3D_API SceneNodeAnimationInstance & operator=( SceneNodeAnimationInstance const & rhs ) = delete;
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
		C3D_API SceneNodeAnimationInstance( AnimatedSceneNode & object
			, SceneNodeAnimation & animation );
		/**
		 *name Getters.
		**/
		/**@{*/
		SceneNodeAnimation const & getSceneNodeAnimation()const
		{
			return m_sceneNodeAnimation;
		}

		AnimatedSceneNode & getAnimatedSceneNode()
		{
			return m_animatedSceneNode;
		}

		castor::Point3f const & getInitialPosition()const
		{
			return m_initialTranslate;
		}

		castor::Quaternion const & getInitialOrientation()const
		{
			return m_initialRotate;
		}

		castor::Point3f const & getInitialScale()const
		{
			return m_initialScale;
		}
		/**@}*/

	private:
		void doUpdate()override;

	protected:
		AnimatedSceneNode & m_animatedSceneNode;
		SceneNodeAnimation & m_sceneNodeAnimation;
		AnimationKeyFrameArray::iterator m_prev;
		AnimationKeyFrameArray::iterator m_curr;
		InterpolatorT< castor::Point3f, InterpolatorType::eLinear > m_vecInterpolator;
		InterpolatorT< castor::Quaternion, InterpolatorType::eLinear > m_quatInterpolator;
		castor::Point3f m_initialTranslate;
		castor::Quaternion m_initialRotate;
		castor::Point3f m_initialScale;

		friend class BinaryWriter< SceneNodeAnimation >;
		friend class BinaryParser< SceneNodeAnimation >;
	};
}

#endif
