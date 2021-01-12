/*
See LICENSE file in root folder
*/
#ifndef ___C3D_AnimatedTexture_H___
#define ___C3D_AnimatedTexture_H___

#include "AnimatedObject.hpp"

#include "Castor3D/Scene/Animation/Texture/TextureAnimationModule.hpp"
#include "Castor3D/Material/Texture/TextureModule.hpp"

namespace castor3d
{
	class AnimatedTexture
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
		C3D_API AnimatedTexture( AnimatedTexture && rhs ) = default;
		C3D_API AnimatedTexture & operator=( AnimatedTexture && rhs ) = delete;
		C3D_API AnimatedTexture( AnimatedTexture const & rhs ) = delete;
		C3D_API AnimatedTexture & operator=( AnimatedTexture const & rhs ) = delete;
		/**@}*/
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	texture		The texture.
		 *\param[in]	pass		The pass using the texture.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	texture		La texture.
		 *\param[in]	pass		La passe utilisant la texture.
		 */
		C3D_API AnimatedTexture( TextureUnit & texture
			, Pass & pass );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~AnimatedTexture();
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
		TextureUnit const & getTexture()const
		{
			return m_texture;
		}

		TextureUnit & getTexture()
		{
			return m_texture;
		}

		Pass const & getPass()const
		{
			return m_pass;
		}

		Pass & getPass()
		{
			return m_pass;
		}

		TextureAnimationInstance const & getPlayingAnimation()const
		{
			return *m_playingAnimation;
		}
		/**@}*/

	private:
		/**
		 *\copydoc		castor3d::AnimatedObject::doAddAnimation
		 */
		void doAddAnimation( castor::String const & name )override;
		/**
		 *\copydoc		castor3d::AnimatedObject::doStartAnimation
		 */
		void doStartAnimation( AnimationInstance & animation )override;
		/**
		 *\copydoc		castor3d::AnimatedObject::doStopAnimation
		 */
		void doStopAnimation( AnimationInstance & animation )override;
		/**
		 *\copydoc		castor3d::AnimatedObject::doClearAnimations
		 */
		void doClearAnimations()override;

	private:
		TextureUnit & m_texture;
		Pass & m_pass;
		TextureAnimationInstanceRPtr m_playingAnimation{ nullptr };
	};
}

#endif

