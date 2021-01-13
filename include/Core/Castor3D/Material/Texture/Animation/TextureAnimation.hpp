/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextureAnimation_H___
#define ___C3D_TextureAnimation_H___

#include "TextureAnimationModule.hpp"

#include "Castor3D/Binary/BinaryModule.hpp"
#include "Castor3D/Scene/Animation/AnimationModule.hpp"
#include "Castor3D/Material/Texture/TextureModule.hpp"

#include "Castor3D/Animation/Animation.hpp"

#include <CastorUtils/Math/Speed.hpp>

namespace castor3d
{
	class TextureAnimation
		: public AnimationT< Engine >
	{
	public:
		/**
		 *\~english
		 *name Copy / Move.
		 *\~french
		 *name Copie / Déplacement.
		 **/
		/**@{*/
		C3D_API TextureAnimation( TextureAnimation && rhs ) = default;
		C3D_API TextureAnimation & operator=( TextureAnimation && rhs ) = delete;
		C3D_API TextureAnimation( TextureAnimation const & rhs ) = delete;
		C3D_API TextureAnimation & operator=( TextureAnimation const & rhs ) = delete;
		/**@}*/
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	texture	The parent texture.
		 *\param[in]	name	The name of the animation.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	texture	La texture parente.
		 *\param[in]	name	Le nom de l'animation.
		 */
		C3D_API explicit TextureAnimation( TextureUnit & texture
			, castor::String const & name = castor::cuEmptyString );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~TextureAnimation();

		C3D_API castor::Point3f getTranslate( castor::Milliseconds const & time )const;
		C3D_API castor::Angle getRotate( castor::Milliseconds const & time )const;

		TextureTranslateSpeed const & getTranslateSpeed()
		{
			return m_translate;
		}

		TextureRotateSpeed const & getRotateSpeed()
		{
			return m_rotate;
		}

		void setTranslateSpeed( TextureTranslateSpeed const & translate )
		{
			m_translate = translate;
		}

		void setRotateSpeed( TextureRotateSpeed const & rotate )
		{
			m_rotate = rotate;
		}

		void setTransformSpeed( TextureTranslateSpeed const & translate
			, TextureRotateSpeed const & rotate )
		{
			setTranslateSpeed( translate );
			setRotateSpeed( rotate );
		}

	protected:
		TextureTranslateSpeed m_translate;
		TextureRotateSpeed m_rotate;

		friend class BinaryWriter< TextureAnimation >;
		friend class BinaryParser< TextureAnimation >;
		friend class TextureAnimationInstance;
	};
}

#endif
