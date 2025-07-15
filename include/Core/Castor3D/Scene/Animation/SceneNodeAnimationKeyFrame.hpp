/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SceneNodeAnimationKeyFrame_H___
#define ___C3D_SceneNodeAnimationKeyFrame_H___

#include "AnimationModule.hpp"
#include "Castor3D/Binary/BinaryModule.hpp"

#include "Castor3D/Animation/AnimationKeyFrame.hpp"
#include "Castor3D/Binary/BinaryParser.hpp"

namespace c3d
{
	class SceneNodeAnimationKeyFrame
		: public AnimationKeyFrame
		, public OwnedBy< SceneNodeAnimation >
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
			, Milliseconds const & timeIndex );
		/**
		 *\~english
		 *\return		A clone of this object.
		 *\~french
		 *\return		Un clone de cet objet.
		 */
		C3D_API AnimationKeyFrameUPtr clone( Animation & parent )const override;

		Point3f const & getPosition()const
		{
			return m_position;
		}

		Quaternion const & getRotation()const
		{
			return m_rotation;
		}

		Point3f const & getScale()const
		{
			return m_scale;
		}

		void setTransform( Point3f translate
			, Quaternion rotate
			, Point3f scale )
		{
			m_position = c3d::move( translate );
			m_rotation = c3d::move( rotate );
			m_scale = c3d::move( scale );
		}

		void setPosition( Point3f value )
		{
			m_position = c3d::move( value );
		}

		void setRotation( Quaternion value )
		{
			m_rotation = c3d::move( value );
		}

		void setScale( Point3f value )
		{
			m_scale = c3d::move( value );
		}

	private:
		void doSetTimeIndex( Milliseconds const & time )
		{
			m_timeIndex = time;
		}

	private:
		Point3f m_position;
		Quaternion m_rotation;
		Point3f m_scale;

		friend class BinaryParser< SceneNodeAnimationKeyFrame >;
	};
}

#endif

