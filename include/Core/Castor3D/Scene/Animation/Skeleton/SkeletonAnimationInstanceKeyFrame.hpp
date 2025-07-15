/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SkeletonAnimationInstanceKeyFrame_H___
#define ___C3D_SkeletonAnimationInstanceKeyFrame_H___

#include "SkeletonAnimationModule.hpp"

#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationKeyFrame.hpp"

namespace c3d
{
	class SkeletonAnimationInstanceKeyFrame
		: public OwnedBy< SkeletonAnimationInstance >
	{
	public:
		using ObjectArray = Vector< Pair< SkeletonAnimationInstanceObject *, Matrix4x4f > >;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	skeletonAnimation	The animation instance.
		 *\param[in]	keyFrame			The animation keyframe.
		 *\param[in]	skeleton			The target animated skeleton.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	skeletonAnimation	L'instance d'animation.
		 *\param[in]	keyFrame			La keyframe d'animation.
		 *\param[in]	skeleton			Le squelette animé cible.
		 */
		C3D_API SkeletonAnimationInstanceKeyFrame( SkeletonAnimationInstance & skeletonAnimation
			, SkeletonAnimationKeyFrame const & keyFrame
			, AnimatedSkeleton & skeleton );
		/**
		 *\~english
		 *\brief		Applies the keyframe.
		 *\~french
		 *\brief		Applique la keyframe.
		 */
		C3D_API void apply();
		/**
		 *\~english
		 *\return		The start time index.
		 *\~french
		 *\return		Le temps de départ.
		 */
		Milliseconds const & getTimeIndex()const
		{
			return m_keyFrame.getTimeIndex();
		}

	private:
		AnimatedSkeleton & m_skeleton;
		SkeletonAnimationKeyFrame const & m_keyFrame;
		ObjectArray m_objects;
		SubmeshBoundingBoxList m_boxes;
	};
	using SkeletonAnimationInstanceKeyFrameArray = Vector< SkeletonAnimationInstanceKeyFrame >;
}

#endif

