#include "SkeletonAnimationObject.hpp"

#include "SkeletonAnimation.hpp"

#include "Animation/Interpolator.hpp"
#include "Animation/KeyFrame.hpp"
#include "SkeletonAnimationBone.hpp"
#include "SkeletonAnimationNode.hpp"

using namespace Castor;

namespace Castor3D
{
	//*************************************************************************************************

	bool BinaryWriter< SkeletonAnimationObject >::DoWrite( SkeletonAnimationObject const & p_obj )
	{
		bool l_return = true;

		if ( l_return )
		{
			l_return = DoWriteChunk( p_obj.m_nodeTransform, eCHUNK_TYPE_MOVING_TRANSFORM, m_chunk );
		}

		if ( l_return )
		{
			l_return = DoWriteChunk( p_obj.m_length, eCHUNK_TYPE_ANIM_LENGTH, m_chunk );
		}

		for ( auto const & l_kf : p_obj.m_keyframes )
		{
			l_return &= BinaryWriter< KeyFrame >{}.Write( l_kf, m_chunk );
		}

		for ( auto const & l_moving : p_obj.m_children )
		{
			switch ( l_moving->GetType() )
			{
			case AnimationObjectType::Node:
				l_return &= BinaryWriter< SkeletonAnimationNode >{}.Write( *std::static_pointer_cast< SkeletonAnimationNode >( l_moving ), m_chunk );
				break;

			case AnimationObjectType::Bone:
				l_return &= BinaryWriter< SkeletonAnimationBone >{}.Write( *std::static_pointer_cast< SkeletonAnimationBone >( l_moving ), m_chunk );
				break;
			}
		}

		return l_return;
	}

	//*************************************************************************************************

	bool BinaryParser< SkeletonAnimationObject >::DoParse( SkeletonAnimationObject & p_obj )
	{
		bool l_return = true;
		Matrix4x4r l_transform;
		KeyFrame l_keyframe;
		SkeletonAnimationNodeSPtr l_node;
		SkeletonAnimationObjectSPtr l_object;
		SkeletonAnimationBoneSPtr l_bone;
		BinaryChunk l_chunk;

		while ( l_return && DoGetSubChunk( l_chunk ) )
		{
			switch ( l_chunk.GetChunkType() )
			{
			case eCHUNK_TYPE_MOVING_TRANSFORM:
				l_return = DoParseChunk( p_obj.m_nodeTransform, l_chunk );
				break;

			case eCHUNK_TYPE_KEYFRAME:
				l_return = BinaryParser< KeyFrame >{}.Parse( l_keyframe, l_chunk );
				p_obj.m_keyframes.push_back( l_keyframe );
				break;

			case eCHUNK_TYPE_ANIM_LENGTH:
				l_return = DoParseChunk( p_obj.m_length, l_chunk );
				break;

			case eCHUNK_TYPE_SKELETON_ANIMATION_BONE:
				l_bone = std::make_shared< SkeletonAnimationBone >( *p_obj.GetOwner() );
				l_return = BinaryParser< SkeletonAnimationBone >{}.Parse( *l_bone, l_chunk );

				if ( l_return )
				{
					p_obj.AddChild( l_bone );
					p_obj.GetOwner()->AddObject( l_bone, p_obj.shared_from_this() );
				}

				break;

			case eCHUNK_TYPE_SKELETON_ANIMATION_NODE:
				l_node = std::make_shared< SkeletonAnimationNode >( *p_obj.GetOwner() );
				l_return = BinaryParser< SkeletonAnimationNode >{}.Parse( *l_node, l_chunk );

				if ( l_return )
				{
					p_obj.AddChild( l_node );
					p_obj.GetOwner()->AddObject( l_node, p_obj.shared_from_this() );
				}

				break;
			}
		}

		return l_return;
	}

	//*************************************************************************************************

	SkeletonAnimationObject::SkeletonAnimationObject( SkeletonAnimation & p_animation, AnimationObjectType p_type )
		: OwnedBy< SkeletonAnimation >{ p_animation }
		, m_type{ p_type }
	{
	}

	SkeletonAnimationObject::~SkeletonAnimationObject()
	{
	}

	void SkeletonAnimationObject::AddChild( SkeletonAnimationObjectSPtr p_object )
	{
		p_object->m_parent = shared_from_this();
		m_children.push_back( p_object );
	}

	KeyFrame & SkeletonAnimationObject::AddKeyFrame( real p_from, Point3r const & p_translate, Quaternion const & p_rotate, Point3r const & p_scale )
	{
		auto l_it = std::find_if( m_keyframes.begin(), m_keyframes.end(), [&p_from]( KeyFrame & p_keyframe )
		{
			return p_keyframe.GetTimeIndex() >= p_from;
		} );

		if ( l_it == m_keyframes.end() )
		{
			m_length = p_from;
			l_it = m_keyframes.insert( m_keyframes.end(), KeyFrame{ p_from, p_translate, p_rotate, p_scale } );
		}
		else if ( l_it->GetTimeIndex() > p_from )
		{
			if ( l_it != m_keyframes.begin() )
			{
				--l_it;
			}

			l_it = m_keyframes.insert( l_it, KeyFrame{ p_from, p_translate, p_rotate, p_scale } );
		}

		return *l_it;
	}

	void SkeletonAnimationObject::RemoveKeyFrame( real p_time )
	{
		auto l_it = std::find_if( m_keyframes.begin(), m_keyframes.end(), [&p_time]( KeyFrame const & p_keyframe )
		{
			return p_keyframe.GetTimeIndex() == p_time;
		} );

		if ( l_it != m_keyframes.end() )
		{
			m_keyframes.erase( l_it );
		}
	}

	//*************************************************************************************************
}
