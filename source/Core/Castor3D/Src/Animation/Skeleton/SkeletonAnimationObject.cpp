#include "SkeletonAnimationObject.hpp"

#include "SkeletonAnimation.hpp"

#include "Animation/Interpolator.hpp"
#include "Animation/KeyFrame.hpp"
#include "SkeletonAnimationBone.hpp"
#include "SkeletonAnimationNode.hpp"

using namespace Castor;

namespace Castor3D
{
	namespace
	{
		using KeyFramed = std::array< double, 11 >;

		void DoConvert( std::vector< KeyFrame > const & p_in, std::vector< KeyFramed > & p_out )
		{
			p_out.resize( p_in.size() );
			auto l_it = p_out.begin();

			for ( auto & l_in : p_in )
			{
				auto & l_out = *l_it;
				size_t l_index{ 0u };
				l_out[l_index++] = l_in.GetRotate()[0];
				l_out[l_index++] = l_in.GetRotate()[1];
				l_out[l_index++] = l_in.GetRotate()[2];
				l_out[l_index++] = l_in.GetRotate()[3];
				l_out[l_index++] = l_in.GetTranslate()[0];
				l_out[l_index++] = l_in.GetTranslate()[1];
				l_out[l_index++] = l_in.GetTranslate()[2];
				l_out[l_index++] = l_in.GetScale()[0];
				l_out[l_index++] = l_in.GetScale()[1];
				l_out[l_index++] = l_in.GetScale()[2];
				l_out[l_index++] = l_in.GetTimeIndex();
				++l_it;
			}
		}

		void DoConvert( std::vector< KeyFramed > const & p_in, std::vector< KeyFrame > & p_out )
		{
			p_out.resize( p_in.size() );
			auto l_it = p_out.begin();

			for ( auto & l_in : p_in )
			{
				size_t l_index{ 0u };
				Quaternion l_rotate{ &l_in[l_index] };
				l_index += 4;
				Point3r l_translate{ l_in[l_index++], l_in[l_index++], l_in[l_index++] };
				Point3r l_scale{ l_in[l_index++], l_in[l_index++], l_in[l_index++] };
				real l_timeIndex{ real( l_in[l_index++] ) };
				( *l_it ) = KeyFrame{ l_timeIndex, l_translate, l_rotate, l_scale };
				++l_it;
			}
		}
	}

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

		if ( !p_obj.m_keyframes.empty() )
		{
			if ( l_return )
			{
				l_return = DoWriteChunk( uint32_t( p_obj.m_keyframes.size() ), eCHUNK_TYPE_KEYFRAME_COUNT, m_chunk );
			}

			if ( l_return )
			{
				std::vector< KeyFramed > l_keyFrames;
				DoConvert( p_obj.m_keyframes, l_keyFrames );
				l_return = DoWriteChunk( l_keyFrames, eCHUNK_TYPE_KEYFRAMES, m_chunk );
			}
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
		std::vector< KeyFramed > l_keyframes;
		SkeletonAnimationNodeSPtr l_node;
		SkeletonAnimationObjectSPtr l_object;
		SkeletonAnimationBoneSPtr l_bone;
		BinaryChunk l_chunk;
		uint32_t l_count{ 0 };

		while ( l_return && DoGetSubChunk( l_chunk ) )
		{
			switch ( l_chunk.GetChunkType() )
			{
			case eCHUNK_TYPE_MOVING_TRANSFORM:
				l_return = DoParseChunk( p_obj.m_nodeTransform, l_chunk );
				break;

			case eCHUNK_TYPE_KEYFRAME_COUNT:
				l_return = DoParseChunk( l_count, l_chunk );

				if ( l_return )
				{
					l_keyframes.resize( l_count );
				}

				break;

			case eCHUNK_TYPE_KEYFRAMES:
				l_return = DoParseChunk( l_keyframes, l_chunk );

				if ( l_return )
				{
					DoConvert( l_keyframes, p_obj.m_keyframes );
				}

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
