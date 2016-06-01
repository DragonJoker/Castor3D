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

	namespace
	{
		void DoFind( real p_time,
					 typename KeyFrameArray::const_iterator const & p_beg,
					 typename KeyFrameArray::const_iterator const & p_end,
					 typename KeyFrameArray::const_iterator & p_prv,
					 typename KeyFrameArray::const_iterator & p_cur )
		{
			if ( p_beg == p_end )
			{
				p_prv = p_cur = p_beg;
			}
			else
			{
				while ( p_prv != p_beg && p_prv->GetTimeIndex() >= p_time )
				{
					// Time has gone too fast backward.
					--p_prv;
					--p_cur;
				}

				while ( p_cur != p_end && p_cur->GetTimeIndex() < p_time )
				{
					// Time has gone too fast forward.
					++p_prv;
					++p_cur;
				}

				ENSURE( p_prv != p_cur );
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

		if ( l_return )
		{
			l_return = DoWriteChunk( p_obj.GetInterpolationMode(), eCHUNK_TYPE_ANIM_INTERPOLATOR, m_chunk );
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
		InterpolatorType l_mode;
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

			case eCHUNK_TYPE_ANIM_INTERPOLATOR:
				l_return = DoParseChunk( l_mode, l_chunk );

				if ( l_return )
				{
					p_obj.SetInterpolationMode( l_mode );
				}

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
		SetInterpolationMode( InterpolatorType::Linear );
	}

	SkeletonAnimationObject::~SkeletonAnimationObject()
	{
	}

	void SkeletonAnimationObject::AddChild( SkeletonAnimationObjectSPtr p_object )
	{
		p_object->m_parent = shared_from_this();
		m_children.push_back( p_object );
	}

	void SkeletonAnimationObject::Update( real p_time, Matrix4x4r const & p_transformations )
	{
		if ( HasKeyFrames() )
		{
			Point3r l_translate{};
			Quaternion l_rotate{};
			Point3r l_scale{ 1.0_r, 1.0_r , 1.0_r };

			if ( m_keyframes.size() == 1 )
			{
				l_translate = m_prev->GetTranslate();
				l_rotate = m_prev->GetRotate();
				l_scale = m_prev->GetScale();
			}
			else
			{
				DoFind( p_time, m_keyframes.begin(), m_keyframes.end() - 1, m_prev, m_curr );
				real l_dt = m_curr->GetTimeIndex() - m_prev->GetTimeIndex();
				real l_factor = ( p_time - m_prev->GetTimeIndex() ) / l_dt;
				l_translate = m_pointInterpolator->Interpolate( m_prev->GetTranslate(), m_curr->GetTranslate(), l_factor );
				l_rotate = m_quaternionInterpolator->Interpolate( m_prev->GetRotate(), m_curr->GetRotate(), l_factor );
				l_scale = m_pointInterpolator->Interpolate( m_prev->GetScale(), m_curr->GetScale(), l_factor );
			}

			m_cumulativeTransform = p_transformations * matrix::set_transform( m_cumulativeTransform, l_translate, l_scale, l_rotate );
		}
		else
		{
			m_cumulativeTransform = p_transformations * m_nodeTransform;
		}

		DoApply();

		for ( auto l_object : m_children )
		{
			l_object->Update( p_time, m_cumulativeTransform );
		}
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

	void SkeletonAnimationObject::SetInterpolationMode( InterpolatorType p_mode )
	{
		if ( p_mode != m_mode )
		{
			m_mode = p_mode;

			switch ( m_mode )
			{
			case InterpolatorType::None:
				m_pointInterpolator = std::make_unique < InterpolatorT< Point3r,  InterpolatorType::None > >();
				m_quaternionInterpolator = std::make_unique < InterpolatorT< Quaternion, InterpolatorType::None > >();
				break;

			case InterpolatorType::Linear:
				m_pointInterpolator = std::make_unique < InterpolatorT< Point3r, InterpolatorType::Linear > >();
				m_quaternionInterpolator = std::make_unique < InterpolatorT< Quaternion, InterpolatorType::Linear > >();
				break;

			default:
				FAILURE( "Unsupported interpolator mode" );
				break;
			}
		}
	}

	SkeletonAnimationObjectSPtr SkeletonAnimationObject::Clone( SkeletonAnimation & p_animation )
	{
		SkeletonAnimationObjectSPtr l_return;

		if ( !p_animation.HasObject( GetType(), GetName() ) )
		{
			l_return = DoClone( p_animation );
			l_return->m_length = m_length;
			l_return->m_keyframes = m_keyframes;
			l_return->m_nodeTransform = m_nodeTransform;
			l_return->m_children.clear();

			if ( l_return->m_keyframes.empty() )
			{
				l_return->m_prev = l_return->m_keyframes.end();
				l_return->m_curr = l_return->m_keyframes.end();
			}
			else
			{
				l_return->m_prev = l_return->m_keyframes.begin();
				l_return->m_curr = l_return->m_prev + 1;
			}

			for ( auto l_object : m_children )
			{
				auto l_clone = l_object->Clone( p_animation );

				if ( l_clone )
				{
					l_return->m_children.push_back( l_clone );
				}
			}

			l_return->SetInterpolationMode( m_mode );
		}

		return l_return;
	}

	//*************************************************************************************************
}
