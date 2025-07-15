/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextureAnimationKeyFrame_H___
#define ___C3D_TextureAnimationKeyFrame_H___

#include "TextureAnimationModule.hpp"
#include "Castor3D/Binary/BinaryModule.hpp"

#include "Castor3D/Animation/AnimationKeyFrame.hpp"
#include "Castor3D/Binary/BinaryParser.hpp"

namespace c3d
{
	class TextureAnimationKeyFrame
		: public AnimationKeyFrame
		, public OwnedBy< TextureAnimation >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	parent		The parent animation.
		 *\param[in]	timeIndex	When the key frame starts.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	parent		L'animation parente.
		 *\param[in]	timeIndex	Quand la key frame commence.
		 */
		C3D_API TextureAnimationKeyFrame( TextureAnimation & parent
			, Milliseconds const & timeIndex );
		/**
		 *\~english
		 *\return		A clone of this object.
		 *\~french
		 *\return		Un clone de cet objet.
		 */
		C3D_API AnimationKeyFrameUPtr clone( Animation & parent )const override;

		void setTile( Point2ui tile )noexcept
		{
			m_tile = c3d::move( tile );
		}

		Point2ui const & getTile()const noexcept
		{
			return m_tile;
		}

	private:
		void doSetTimeIndex( Milliseconds const & time )noexcept
		{
			m_timeIndex = time;
		}

	private:
		Point2ui m_tile;

		friend class BinaryParser< TextureAnimationKeyFrame >;
	};
}

#endif

