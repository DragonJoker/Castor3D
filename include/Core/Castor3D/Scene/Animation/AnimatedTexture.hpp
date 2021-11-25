/*
See LICENSE file in root folder
*/
#ifndef ___C3D_AnimatedTexture_H___
#define ___C3D_AnimatedTexture_H___

#include "AnimatedObject.hpp"

#include "Castor3D/Scene/Animation/Texture/TextureAnimationModule.hpp"
#include "Castor3D/Material/Texture/TextureModule.hpp"
#include "Castor3D/Material/Pass/PassModule.hpp"

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
		C3D_API AnimatedTexture( TextureSourceInfo const & sourceInfo
			, TextureConfiguration const & config
			, Pass & pass );
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
			CU_Require( m_texture );
			return *m_texture;
		}

		TextureUnit & getTexture()
		{
			CU_Require( m_texture );
			return *m_texture;
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
		/**
		 *name Setters.
		**/
		/**@{*/
		void setTexture( TextureUnit & texture )
		{
			CU_Require( !m_texture );
			m_texture = &texture;
		}
		/**@}*/

	private:
		void doAddAnimation( castor::String const & name )override;
		void doStartAnimation( AnimationInstance & animation )override;
		void doStopAnimation( AnimationInstance & animation )override;
		void doClearAnimations()override;

	private:
		TextureUnit * m_texture{};
		Pass & m_pass;
		TextureAnimationInstanceRPtr m_playingAnimation{ nullptr };
	};
}

#endif

