#include "SkeletonAnimationInstanceKeyFrame.hpp"

#include "Animation/Skeleton/SkeletonAnimationKeyFrame.hpp"
#include "Scene/Animation/Skeleton/SkeletonAnimationInstance.hpp"
#include "Scene/Animation/Skeleton/SkeletonAnimationInstanceObject.hpp"

using namespace castor;

namespace castor3d
{
	namespace
	{
		void recAdd( SkeletonAnimationInstanceObjectSPtr object
			, SkeletonAnimationKeyFrame const & keyFrame
			, SkeletonAnimationInstanceKeyFrame::ObjectArray & objects )
		{

			for ( auto & child : object->getChildren() )
			{
				recAdd( child, keyFrame, objects );
			}
		}
	}

	SkeletonAnimationInstanceKeyFrame::SkeletonAnimationInstanceKeyFrame( SkeletonAnimationInstance & skeletonAnimation
		, SkeletonAnimationKeyFrame const & keyFrame )
		: OwnedBy< SkeletonAnimationInstance >{ skeletonAnimation }
		, m_keyFrame{ keyFrame }
	{
		for ( auto & object : skeletonAnimation )
		{
			m_objects.emplace_back( object.get(), keyFrame.find( object->getObject() )->second );
		}
	}

	void SkeletonAnimationInstanceKeyFrame::apply()
	{
		for ( auto object : m_objects )
		{
			object.first->update( object.second );
		}
	}
}
