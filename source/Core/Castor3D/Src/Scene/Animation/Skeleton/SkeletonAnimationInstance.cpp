#include "SkeletonAnimationInstance.hpp"

#include "Engine.hpp"

#include "SkeletonAnimationInstanceBone.hpp"
#include "SkeletonAnimationInstanceNode.hpp"

#include "Animation/Skeleton/SkeletonAnimation.hpp"
#include "Event/Frame/InitialiseEvent.hpp"
#include "Mesh/Skeleton/Bone.hpp"
#include "Scene/Animation/AnimatedSkeleton.hpp"

#include "Scene/Geometry.hpp"

using namespace Castor;

namespace Castor3D
{
	//*************************************************************************************************

	namespace
	{
		Castor::String const & GetMovingTypeName( AnimationObjectType p_type )
		{
			static std::map< AnimationObjectType, String > Names
			{
				{ AnimationObjectType::Node, cuT( "Node_" ) },
				{ AnimationObjectType::Bone, cuT( "Bone_" ) },
			};

			return Names[p_type];
		}
	}

	//*************************************************************************************************

	SkeletonAnimationInstance::SkeletonAnimationInstance( AnimatedSkeleton & p_object, Animation const & p_animation )
		: AnimationInstance{ p_object, p_animation }
	{
	}

	SkeletonAnimationInstance::~SkeletonAnimationInstance()
	{
	}

	SkeletonAnimationObjectSPtr SkeletonAnimation::GetObject( BoneSPtr p_bone )const
	{
		SkeletonAnimationObjectSPtr l_return;
		auto l_it = m_toMove.find( GetMovingTypeName( AnimationObjectType::Bone ) + p_bone->GetName() );

		if ( l_it != m_toMove.end() )
		{
			l_return = l_it->second;
		}

		return l_return;
	}

	void SkeletonAnimationInstance::SetInterpolationMode( InterpolatorType p_mode )
	{
		for ( auto l_moving : m_arrayMoving )
		{
			l_moving->SetInterpolationMode( p_mode );
		}
	}

	void SkeletonAnimationInstance::DoUpdate( real p_tslf )
	{
		for ( auto l_moving : m_arrayMoving )
		{
			l_moving->Update( m_currentTime, Matrix4x4r{ 1 } );
		}
	}

	//*************************************************************************************************
}
