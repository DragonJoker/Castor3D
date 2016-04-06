#include "AnimationObject.hpp"

#include "Animation.hpp"
#include "KeyFrame.hpp"
#include "Interpolator.hpp"
#include "SkeletonAnimationBone.hpp"
#include "SkeletonAnimationNode.hpp"
#include "SkeletonAnimationObject.hpp"

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

	AnimationObject::BinaryParser::BinaryParser( Path const & p_path )
		: Castor3D::BinaryParser< AnimationObject >( p_path )
	{
	}

	bool AnimationObject::BinaryParser::Fill( AnimationObject const & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_return = true;

		if ( l_return )
		{
			l_return = DoFillChunk( p_obj.m_nodeTransform, eCHUNK_TYPE_MOVING_TRANSFORM, p_chunk );
		}

		for ( auto const & l_kf : p_obj.m_keyframes )
		{
			l_return &= KeyFrame::BinaryParser( m_path ).Fill( l_kf, p_chunk );
		}

		for ( auto const & l_moving : p_obj.m_children )
		{
			switch ( l_moving->GetType() )
			{
			case eANIMATION_OBJECT_TYPE_NODE:
				l_return &= SkeletonAnimationNode::BinaryParser( m_path ).Fill( *std::static_pointer_cast< SkeletonAnimationNode >( l_moving ), p_chunk );
				break;

			case eANIMATION_OBJECT_TYPE_OBJECT:
				l_return &= SkeletonAnimationObject::BinaryParser( m_path ).Fill( *std::static_pointer_cast< SkeletonAnimationObject >( l_moving ), p_chunk );
				break;

			case eANIMATION_OBJECT_TYPE_BONE:
				l_return &= SkeletonAnimationBone::BinaryParser( m_path ).Fill( *std::static_pointer_cast< SkeletonAnimationBone >( l_moving ), p_chunk );
				break;
			}
		}

		return l_return;
	}

	bool AnimationObject::BinaryParser::Parse( AnimationObject & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_return = true;
		Matrix4x4r l_transform;
		KeyFrame l_keyframe;
		SkeletonAnimationNodeSPtr l_node;
		SkeletonAnimationObjectSPtr l_object;
		SkeletonAnimationBoneSPtr l_bone;

		switch ( p_chunk.GetChunkType() )
		{
		case eCHUNK_TYPE_MOVING_TRANSFORM:
			l_return = DoParseChunk( p_obj.m_nodeTransform, p_chunk );
			break;

		case eCHUNK_TYPE_KEYFRAME:
			l_return = KeyFrame::BinaryParser( m_path ).Parse( l_keyframe, p_chunk );
			p_obj.m_keyframes.push_back( l_keyframe );
			break;

		case eCHUNK_TYPE_MOVING_BONE:
			l_bone = std::make_shared< SkeletonAnimationBone >();
			l_return = SkeletonAnimationBone::BinaryParser( m_path ).Parse( *l_bone, p_chunk );

			if ( l_return )
			{
				p_obj.m_children.push_back( l_bone );
			}

			break;

		case eCHUNK_TYPE_MOVING_NODE:
			l_node = std::make_shared< SkeletonAnimationNode >();
			l_return = SkeletonAnimationNode::BinaryParser( m_path ).Parse( *l_node, p_chunk );

			if ( l_return )
			{
				p_obj.m_children.push_back( l_node );
			}

			break;

		case eCHUNK_TYPE_MOVING_OBJECT:
			l_object = std::make_shared< SkeletonAnimationObject >();
			l_return = SkeletonAnimationObject::BinaryParser( m_path ).Parse( *l_object, p_chunk );

			if ( l_return )
			{
				p_obj.m_children.push_back( l_object );
			}

			break;
		}

		return l_return;
	}

	//*************************************************************************************************

	AnimationObject::AnimationObject( eANIMATION_OBJECT_TYPE p_type )
		: m_length( 0 )
		, m_type( p_type )
		, m_mode( eINTERPOLATOR_MODE_COUNT )
	{
		SetInterpolationMode( eINTERPOLATOR_MODE_LINEAR );
	}

	AnimationObject::AnimationObject( AnimationObject const & p_rhs )
	{
	}

	AnimationObject::~AnimationObject()
	{
	}

	void AnimationObject::AddChild( AnimationObjectSPtr p_object )
	{
		m_children.push_back( p_object );
	}

	void AnimationObject::Update( real p_time,Matrix4x4r const & p_transformations )
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

	KeyFrame & AnimationObject::AddKeyFrame( real p_from, Point3r const & p_translate, Quaternion const & p_rotate, Point3r const & p_scale )
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

	void AnimationObject::RemoveKeyFrame( real p_time )
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

	void AnimationObject::SetInterpolationMode( eINTERPOLATOR_MODE p_mode )
	{
		if ( p_mode != m_mode )
		{
			m_mode = p_mode;

			switch ( m_mode )
			{
			case eINTERPOLATOR_MODE_NONE:
				m_pointInterpolator = std::make_unique < InterpolatorT< Point3r,  eINTERPOLATOR_MODE_NONE > >();
				m_quaternionInterpolator = std::make_unique < InterpolatorT< Quaternion, eINTERPOLATOR_MODE_NONE > >();
				break;

			case eINTERPOLATOR_MODE_LINEAR:
				m_pointInterpolator = std::make_unique < InterpolatorT< Point3r, eINTERPOLATOR_MODE_LINEAR > >();
				m_quaternionInterpolator = std::make_unique < InterpolatorT< Quaternion, eINTERPOLATOR_MODE_LINEAR > >();
				break;

			default:
				FAILURE( "Unsupported interpolator mode" );
				break;
			}
		}
	}

	AnimationObjectSPtr AnimationObject::Clone( Animation & p_animation )
	{
		AnimationObjectSPtr l_return;

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
