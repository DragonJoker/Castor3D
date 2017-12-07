/*
See LICENSE file in root folder
*/
#ifndef ___C3D_AnimationKeyFrame_H___
#define ___C3D_AnimationKeyFrame_H___

#include "Castor3DPrerequisites.hpp"

#include <Graphics/BoundingBox.hpp>

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Key frames are the frames where the animation must be at a precise state.
	\~french
	\brief		Les key frames sont les frames auxquelles une animation est dans un état précis.
	*/
	class AnimationKeyFrame
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	timeIndex	When the key frame starts.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	timeIndex	Quand la key frame commence.
		 */
		AnimationKeyFrame( castor::Milliseconds const & timeIndex = 0_ms );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~AnimationKeyFrame() = default;
		/**
		 *\~english
		 *\return		The start time index.
		 *\~french
		 *\return		Le temps de départ.
		 */
		inline castor::Milliseconds const & getTimeIndex()const
		{
			return m_timeIndex;
		}

	protected:
		//!\~english	The start time index.
		//!\~french		L'index de temps de début.
		castor::Milliseconds m_timeIndex;
		//!\~english	The bounding box.
		//!\~french		La bounding box.
		castor::BoundingBox m_boundingBox;
	};
}

#endif

