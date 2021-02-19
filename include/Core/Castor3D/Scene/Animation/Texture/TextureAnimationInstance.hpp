/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextureAnimationInstance_H___
#define ___C3D_TextureAnimationInstance_H___

#include "TextureAnimationModule.hpp"

#include "Castor3D/Binary/BinaryModule.hpp"
#include "Castor3D/Material/Texture/Animation/TextureAnimationModule.hpp"

#include "Castor3D/Scene/Animation/AnimationInstance.hpp"

namespace castor3d
{
	class TextureAnimationInstance
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
		C3D_API TextureAnimationInstance( TextureAnimationInstance && rhs ) = default;
		C3D_API TextureAnimationInstance & operator=( TextureAnimationInstance && rhs ) = delete;
		C3D_API TextureAnimationInstance( TextureAnimationInstance const & rhs ) = delete;
		C3D_API TextureAnimationInstance & operator=( TextureAnimationInstance const & rhs ) = delete;
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
		C3D_API TextureAnimationInstance( AnimatedTexture & object, TextureAnimation & animation );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~TextureAnimationInstance();
		/**
		 *name Getters.
		**/
		/**@{*/
		TextureAnimation const & getTextureAnimation()const
		{
			return m_textureAnimation;
		}

		AnimatedTexture & getAnimatedMesh()
		{
			return m_animatedTexture;
		}
		/**@}*/

	private:
		void doUpdate()override;

	protected:
		AnimatedTexture & m_animatedTexture;
		TextureAnimation const & m_textureAnimation;
		AnimationKeyFrameArray::iterator m_prev;
		AnimationKeyFrameArray::iterator m_curr;

		friend class BinaryWriter< TextureAnimation >;
		friend class BinaryParser< TextureAnimation >;
	};
}

#endif
