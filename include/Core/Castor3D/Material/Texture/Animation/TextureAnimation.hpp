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
		C3D_API explicit TextureAnimation( Engine & engine
			, castor::String const & name = castor::cuEmptyString );

		C3D_API void setAnimable( TextureUnit & unit );

		C3D_API castor::Point3f getTranslate( castor::Milliseconds const & time )const;
		C3D_API castor::Angle getRotate( castor::Milliseconds const & time )const;
		C3D_API castor::Point3f getScale( castor::Milliseconds const & time )const;

		void addPendingAnimated( AnimatedObject & object )
		{
			m_pending.insert( &object );
		}

		TextureTranslateSpeed const & getTranslateSpeed()const
		{
			return m_translate;
		}

		TextureRotateSpeed const & getRotateSpeed()const
		{
			return m_rotate;
		}

		TextureScaleSpeed const & getScaleSpeed()const
		{
			return m_scale;
		}

		void setTranslateSpeed( TextureTranslateSpeed const & translate )
		{
			m_translate = translate;
		}

		void setRotateSpeed( TextureRotateSpeed const & rotate )
		{
			m_rotate = rotate;
		}

		void setScaleSpeed( TextureScaleSpeed const & scale )
		{
			m_scale = scale;
		}

		void setTransformSpeed( TextureTranslateSpeed const & translate
			, TextureRotateSpeed const & rotate
			, TextureScaleSpeed const & scale )
		{
			setTranslateSpeed( translate );
			setRotateSpeed( rotate );
			setScaleSpeed( scale );
		}

	protected:
		TextureTranslateSpeed m_translate;
		TextureRotateSpeed m_rotate;
		TextureScaleSpeed m_scale;
		std::set< AnimatedObject * > m_pending;

		friend class BinaryWriter< TextureAnimation >;
		friend class BinaryParser< TextureAnimation >;
		friend class TextureAnimationInstance;
	};
}

#endif
