/*
See LICENSE file in root folder
*/
#ifndef ___C3D_AnimatedSceneNode_H___
#define ___C3D_AnimatedSceneNode_H___

#include "AnimatedObject.hpp"

#include "Castor3D/Scene/Animation/SceneNode/SceneNodeAnimationModule.hpp"

namespace castor3d
{
	class AnimatedSceneNode
		: public AnimatedObject
	{
	public:
		/**
		 *\~english
		 *name Copy / Move.
		 *\~french
		 *name Copie / Déplacement.
		 **/
		/**@{*/
		C3D_API AnimatedSceneNode( AnimatedSceneNode && rhs ) = default;
		C3D_API AnimatedSceneNode & operator=( AnimatedSceneNode && rhs ) = delete;
		C3D_API AnimatedSceneNode( AnimatedSceneNode const & rhs ) = delete;
		C3D_API AnimatedSceneNode & operator=( AnimatedSceneNode const & rhs ) = delete;
		/**@}*/
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	name	The object name.
		 *\param[in]	node	The scene node.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	name	Le nom de l'objet.
		 *\param[in]	node	Le noeud de scène.
		 */
		C3D_API AnimatedSceneNode( castor::String const & name
			, SceneNode & node );
		/**
		 *\copydoc		castor3d::AnimatedObject::update
		 */
		C3D_API void update( castor::Milliseconds const & elapsed )override;
		/**
		 *\copydoc		castor3d::AnimatedObject::isPlayingAnimation
		 */
		C3D_API bool isPlayingAnimation()const override
		{
			return m_playingAnimation != nullptr;
		}
		/**
		 *name Getters.
		**/
		/**@{*/
		SceneNode & getSceneNode()const
		{
			return m_node;
		}

		SceneNodeAnimationInstance & getPlayingAnimation()const
		{
			CU_Require( m_playingAnimation );
			return *m_playingAnimation;
		}
		/**@}*/

	private:
		void doAddAnimation( castor::String const & name )override;
		void doStartAnimation( AnimationInstance & animation )override;
		void doStopAnimation( AnimationInstance & animation )override;
		void doClearAnimations()override;

	private:
		SceneNode & m_node;
		SceneNodeAnimationInstanceRPtr m_playingAnimation{ nullptr };
	};
}

#endif

