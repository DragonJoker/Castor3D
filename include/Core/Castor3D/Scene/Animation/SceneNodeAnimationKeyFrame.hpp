/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SceneNodeAnimationKeyFrame_H___
#define ___C3D_SceneNodeAnimationKeyFrame_H___

#include "AnimationModule.hpp"
#include "Castor3D/Binary/BinaryModule.hpp"

#include "Castor3D/Animation/AnimationKeyFrame.hpp"
#include "Castor3D/Binary/BinaryParser.hpp"

namespace castor3d
{
	class SceneNodeAnimationKeyFrame
		: public AnimationKeyFrame
		, public castor::OwnedBy< SceneNodeAnimation >
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
		C3D_API SceneNodeAnimationKeyFrame( SceneNodeAnimation & parent
			, castor::Milliseconds const & timeIndex );

		castor::Point3f const & getPosition()const
		{
			return m_position;
		}

		castor::Quaternion const & getRotation()const
		{
			return m_rotation;
		}

		castor::Point3f const & getScale()const
		{
			return m_scale;
		}

		void setTransform( castor::Point3f translate
			, castor::Quaternion rotate
			, castor::Point3f scale )
		{
			m_position = std::move( translate );
			m_rotation = std::move( rotate );
			m_scale = std::move( scale );
		}

		void setPosition( castor::Point3f value )
		{
			m_position = std::move( value );
		}

		void setRotation( castor::Quaternion value )
		{
			m_rotation = std::move( value );
		}

		void setScale( castor::Point3f value )
		{
			m_scale = std::move( value );
		}

	private:
		void doSetTimeIndex( castor::Milliseconds const & time )
		{
			m_timeIndex = time;
		}

	private:
		castor::Point3f m_position;
		castor::Quaternion m_rotation;
		castor::Point3f m_scale;

		friend class BinaryParser< SceneNodeAnimationKeyFrame >;
	};
}

#endif

