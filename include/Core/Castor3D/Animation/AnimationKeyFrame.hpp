/*
See LICENSE file in root folder
*/
#ifndef ___C3D_AnimationKeyFrame_H___
#define ___C3D_AnimationKeyFrame_H___

#include "AnimationModule.hpp"

#include <CastorUtils/Graphics/BoundingBox.hpp>

namespace castor3d
{
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
		explicit AnimationKeyFrame( castor::Milliseconds const & timeIndex = 0_ms )
			: m_timeIndex{ timeIndex }
		{
		}
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API virtual ~AnimationKeyFrame()noexcept = default;
		/**
		 *\~english
		 *\brief		Initialises the keyframe.
		 *\~french
		 *\brief		Initialise la keyframe.
		 */
		C3D_API virtual void initialise()
		{
		}
		/**
		 *\~english
		 *\return		A clone of this object.
		 *\~french
		 *\return		Un clone de cet objet.
		 */
		C3D_API virtual AnimationKeyFrameUPtr clone( Animation & parent )const = 0;
		/**
		 *\~english
		 *\return		The start time index.
		 *\~french
		 *\return		Le temps de départ.
		 */
		castor::Milliseconds const & getTimeIndex()const
		{
			return m_timeIndex;
		}
		/**
		 *\~english
		 *\param[in]	timeIndex	The start time index.
		 *\~french
		 *\param[in]	timeIndex	Le temps de départ.
		 */
		void setTimeIndex( castor::Milliseconds const & timeIndex )
		{
			m_timeIndex = timeIndex;
		}
		/**
		 *\~english
		 *\return		The keyframe's bounding box.
		 *\~french
		 *\return		La bounding box de la keyframe.
		 */
		castor::BoundingBox const & getBoundingBox()const
		{
			return m_boundingBox;
		}

	protected:
		C3D_API void doCloneInto( AnimationKeyFrame & output )const
		{
			output.m_boundingBox = m_boundingBox;
		}

		//!\~english	The start time index.
		//!\~french		L'index de temps de début.
		castor::Milliseconds m_timeIndex;
		//!\~english	The bounding box.
		//!\~french		La bounding box.
		castor::BoundingBox m_boundingBox;
	};
}

#endif

