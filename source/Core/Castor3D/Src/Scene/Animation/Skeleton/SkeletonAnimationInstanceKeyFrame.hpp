/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SkeletonAnimationInstanceKeyFrame_H___
#define ___C3D_SkeletonAnimationInstanceKeyFrame_H___

#include "Animation/Skeleton/SkeletonAnimationKeyFrame.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		The class which manages key frames
	\remark		Key frames are the frames where the animation must be at a precise state
	\~french
	\brief		Classe qui gère une key frame
	\remark		Les key frames sont les frames auxquelles une animation est dans un état précis
	*/
	class SkeletonAnimationInstanceKeyFrame
		: public castor::OwnedBy< SkeletonAnimationInstance >
	{
	public:
		using ObjectArray = std::vector< std::pair< SkeletonAnimationInstanceObject *, castor::Matrix4x4r > >;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	skeletonAnimation	The animation instance.
		 *\param[in]	keyFrame			The animation keyframe.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	skeletonAnimation	L'instance d'animation.
		 *\param[in]	keyFrame			La keyframe d'animation.
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
		inline castor::Milliseconds const & getTimeIndex()const
		{
			return m_keyFrame.getTimeIndex();
		}

	private:
		AnimatedSkeleton & m_skeleton;
		SkeletonAnimationKeyFrame const & m_keyFrame;
		ObjectArray m_objects;
		SubmeshBoundingBoxList m_boxes;
	};
	using SkeletonAnimationInstanceKeyFrameArray = std::vector< SkeletonAnimationInstanceKeyFrame >;
}

#endif

