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
		C3D_API TextureAnimation( TextureAnimation && rhs )noexcept = default;
		C3D_API TextureAnimation & operator=( TextureAnimation && rhs )noexcept = delete;
		C3D_API TextureAnimation( TextureAnimation const & rhs ) = delete;
		C3D_API TextureAnimation & operator=( TextureAnimation const & rhs ) = delete;
		C3D_API ~TextureAnimation()noexcept override = default;
		/**@}*/
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\param[in]	name	The name of the animation.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 *\param[in]	name	Le nom de l'animation.
		 */
		C3D_API explicit TextureAnimation( Engine & engine
			, castor::String const & name = castor::cuEmptyString );

		C3D_API void setAnimable( TextureUnit & unit );
		C3D_API void initialiseTiles( TextureUnit const & unit );

		C3D_API castor::Point3f getTranslate( castor::Milliseconds const & time )const;
		C3D_API castor::Angle getRotate( castor::Milliseconds const & time )const;
		C3D_API castor::Point3f getScale( castor::Milliseconds const & time )const;
		C3D_API bool isTransformAnimated()const;

		void addPendingAnimated( AnimatedObject & object )noexcept
		{
			m_pending.insert( &object );
		}

		TextureTranslateSpeed const & getTranslateSpeed()const noexcept
		{
			return m_translate;
		}

		TextureRotateSpeed const & getRotateSpeed()const noexcept
		{
			return m_rotate;
		}

		TextureScaleSpeed const & getScaleSpeed()const noexcept
		{
			return m_scale;
		}

		void setTranslateSpeed( TextureTranslateSpeed translate )noexcept
		{
			m_translate = castor::move( translate );
		}

		void setRotateSpeed( TextureRotateSpeed rotate )noexcept
		{
			m_rotate = castor::move( rotate );
		}

		void setScaleSpeed( TextureScaleSpeed scale )noexcept
		{
			m_scale = castor::move( scale );
		}

		void enableTileAnim()noexcept
		{
			m_tileAnim = true;
		}

		bool isTileAnimated()const noexcept
		{
			return m_tileAnim;
		}

		void setTransformSpeed( TextureTranslateSpeed translate
			, TextureRotateSpeed rotate
			, TextureScaleSpeed scale )noexcept
		{
			setTranslateSpeed( castor::move( translate ) );
			setRotateSpeed( castor::move( rotate ) );
			setScaleSpeed( castor::move( scale ) );
		}

	protected:
		TextureTranslateSpeed m_translate{};
		TextureRotateSpeed m_rotate{};
		TextureScaleSpeed m_scale{};
		bool m_tileAnim{};
		castor::Set< AnimatedObject * > m_pending;

		friend class BinaryWriter< TextureAnimation >;
		friend class BinaryParser< TextureAnimation >;
		friend class TextureAnimationInstance;
	};
}

#endif
