/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextureAnimationKeyFrame_H___
#define ___C3D_TextureAnimationKeyFrame_H___

#include "TextureAnimationModule.hpp"
#include "Castor3D/Binary/BinaryModule.hpp"

#include "Castor3D/Animation/AnimationKeyFrame.hpp"
#include "Castor3D/Binary/BinaryParser.hpp"

namespace castor3d
{
	class TextureAnimationKeyFrame
		: public AnimationKeyFrame
		, public castor::OwnedBy< TextureAnimation >
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
			, castor::Milliseconds const & timeIndex );

		void setTile( castor::Point2ui tile )
		{
			m_tile = std::move( tile );
		}

		castor::Point2ui const & getTile()const
		{
			return m_tile;
		}

	private:
		void doSetTimeIndex( castor::Milliseconds const & time )
		{
			m_timeIndex = time;
		}

	private:
		castor::Point2ui m_tile;

		friend class BinaryParser< TextureAnimationKeyFrame >;
	};
}

#endif

