/*
See LICENSE file in root folder
*/
#ifndef ___C3D_AnimatedTexture_H___
#define ___C3D_AnimatedTexture_H___

#include "AnimatedObject.hpp"

#include "Castor3D/Scene/Animation/Texture/TextureAnimationModule.hpp"
#include "Castor3D/Material/Texture/TextureModule.hpp"
#include "Castor3D/Material/Texture/Animation/TextureAnimationModule.hpp"
#include "Castor3D/Material/Pass/PassModule.hpp"

namespace castor3d
{
	class AnimatedTexture
		: public AnimatedObject
	{
	public:
		C3D_API AnimatedTexture( AnimatedTexture && rhs )noexcept = default;
		C3D_API AnimatedTexture & operator=( AnimatedTexture && rhs )noexcept = delete;
		C3D_API AnimatedTexture( AnimatedTexture const & rhs ) = delete;
		C3D_API AnimatedTexture & operator=( AnimatedTexture const & rhs ) = delete;
		C3D_API ~AnimatedTexture()noexcept override = default;
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	sourceInfo	The texture source informations.
		 *\param[in]	config		The texture configuration.
		 *\param[in]	pass		The pass using the texture.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	sourceInfo	Les informations de source de la texture.
		 *\param[in]	config		La configuration de texture
		 *\param[in]	pass		La passe utilisant la texture.
		 */
		C3D_API AnimatedTexture( TextureSourceInfo const & sourceInfo
			, TextureConfiguration const & config
			, Pass & pass );
		/**
		 *\copydoc		castor3d::AnimatedObject::update
		 */
		C3D_API void update( castor::Milliseconds const & elapsed )override;
		C3D_API void fillBuffer( TextureAnimationData * buffer )const;
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
		bool hasTextureUnit()const noexcept
		{
			return m_texture != nullptr;
		}

		TextureUnit & getTextureUnit()const noexcept
		{
			CU_Require( hasTextureUnit() );
			return *m_texture;
		}

		Pass & getPass()const noexcept
		{
			return m_pass;
		}

		TextureAnimationInstance & getPlayingAnimation()const noexcept
		{
			CU_Require( m_playingAnimation );
			return *m_playingAnimation;
		}
		/**@}*/
		/**
		 *name Setters.
		**/
		/**@{*/
		void setTextureUnit( TextureUnit & texture )noexcept
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

