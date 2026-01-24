#include "Castor3D/Scene/Animation/Skeleton/SkeletonAnimationInstanceKeyFrame.hpp"

#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationKeyFrame.hpp"
#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Scene/Animation/AnimatedSkeleton.hpp"
#include "Castor3D/Scene/Animation/Skeleton/SkeletonAnimationInstance.hpp"
#include "Castor3D/Scene/Animation/Skeleton/SkeletonAnimationInstanceObject.hpp"

namespace c3d
{
	SkeletonAnimationInstanceKeyFrame::SkeletonAnimationInstanceKeyFrame( SkeletonAnimationInstance & skeletonAnimation
		, SkeletonAnimationKeyFrame const & keyFrame
		, AnimatedSkeleton & skeleton )
		: OwnedBy< SkeletonAnimationInstance >{ skeletonAnimation }
		, m_skeleton{ skeleton }
		, m_keyFrame{ keyFrame }
	{
		for ( auto const & object : skeletonAnimation )
		{
			auto it = keyFrame.find( object->getObject() );
			m_objects.emplace_back( object.get()
				, ( it != m_keyFrame.end()
					? it->cumulative
					: Matrix4x4f{ Matrix4x4f::getIdentity() } ) );
		}

		m_boxes = m_keyFrame.computeBoundingBoxes( m_skeleton.getMesh(), m_skeleton.getSkeleton() );
	}

	void SkeletonAnimationInstanceKeyFrame::apply()const
	{
		for ( auto const & [object, transform] : m_objects )
		{
			object->update( transform );
		}

		m_skeleton.getGeometry().updateContainers( m_boxes );
	}
}
