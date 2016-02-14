#include "AnimationObjectBase.hpp"

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

	AnimationObjectBase::BinaryParser::BinaryParser( Path const & p_path )
		: Castor3D::BinaryParser< AnimationObjectBase >( p_path )
	{
	}

	bool AnimationObjectBase::BinaryParser::Fill( AnimationObjectBase const & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_return = true;

		if ( l_return )
		{
			l_return = DoFillChunk( p_obj.m_nodeTransform, eCHUNK_TYPE_MOVING_TRANSFORM, p_chunk );
		}

		for ( auto const & l_kf : p_obj.m_scales.m_keyframes )
		{
			l_return = DoFillChunk( l_kf.GetTimeIndex(), eCHUNK_TYPE_KEYFRAME_TIME, p_chunk );
			l_return = DoFillChunk( l_kf.GetValue(), eCHUNK_TYPE_KEYFRAME_SCALE, p_chunk );
		}

		for ( auto const & l_kf : p_obj.m_translates.m_keyframes )
		{
			l_return = DoFillChunk( l_kf.GetTimeIndex(), eCHUNK_TYPE_KEYFRAME_TIME, p_chunk );
			l_return = DoFillChunk( l_kf.GetValue(), eCHUNK_TYPE_KEYFRAME_TRANSLATE, p_chunk );
		}

		for ( auto const & l_kf : p_obj.m_rotates.m_keyframes )
		{
			l_return = DoFillChunk( l_kf.GetTimeIndex(), eCHUNK_TYPE_KEYFRAME_TIME, p_chunk );
			l_return = DoFillChunk( l_kf.GetValue(), eCHUNK_TYPE_KEYFRAME_ROTATE, p_chunk );
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

	bool AnimationObjectBase::BinaryParser::Parse( AnimationObjectBase & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_return = true;
		Matrix4x4r l_transform;
		Point3r l_point;
		Quaternion l_quat;
		SkeletonAnimationNodeSPtr l_node;
		SkeletonAnimationObjectSPtr l_object;
		SkeletonAnimationBoneSPtr l_bone;
		real l_time;

		switch ( p_chunk.GetChunkType() )
		{
		case eCHUNK_TYPE_MOVING_TRANSFORM:
			l_return = DoParseChunk( p_obj.m_nodeTransform, p_chunk );
			break;

		case eCHUNK_TYPE_KEYFRAME_TIME:
			l_return = DoParseChunk( l_time, p_chunk );
			break;

		case eCHUNK_TYPE_KEYFRAME_SCALE:
			l_return = DoParseChunk( l_point, p_chunk );
			p_obj.AddScaleKeyFrame( l_time, l_point );
			break;

		case eCHUNK_TYPE_KEYFRAME_TRANSLATE:
			l_return = DoParseChunk( l_point, p_chunk );
			p_obj.AddTranslateKeyFrame( l_time, l_point );
			break;

		case eCHUNK_TYPE_KEYFRAME_ROTATE:
			l_return = DoParseChunk( l_quat, p_chunk );
			p_obj.AddRotateKeyFrame( l_time, l_quat );
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

	AnimationObjectBase::AnimationObjectBase( eANIMATION_OBJECT_TYPE p_type )
		: m_length( 0 )
		, m_type( p_type )
	{
		m_scales.SetInterpolationMode( eINTERPOLATOR_MODE_LINEAR );
		m_translates.SetInterpolationMode( eINTERPOLATOR_MODE_LINEAR );
		m_rotates.SetInterpolationMode( eINTERPOLATOR_MODE_LINEAR );
		m_scales.m_default = Point3r{ 1, 1, 1 };
	}

	AnimationObjectBase::AnimationObjectBase( AnimationObjectBase const & p_rhs )
	{
	}

	AnimationObjectBase::~AnimationObjectBase()
	{
	}

	void AnimationObjectBase::AddChild( AnimationObjectBaseSPtr p_object )
	{
		m_children.push_back( p_object );
	}

	void AnimationObjectBase::Update( real p_time, bool p_looped, Matrix4x4r const & p_transformations )
	{
		if ( HasKeyFrames() )
		{
			m_cumulativeTransform = p_transformations * matrix::set_transform( m_cumulativeTransform, m_translates.Compute( p_time ), m_scales.Compute( p_time ), m_rotates.Compute( p_time ) );
		}
		else
		{
			m_cumulativeTransform = p_transformations * m_nodeTransform;
		}

		DoApply();

		for ( auto l_object : m_children )
		{
			l_object->Update( p_time, p_looped, m_cumulativeTransform );
		}
	}

	Point3rKeyFrame & AnimationObjectBase::AddScaleKeyFrame( real p_from, Point3r const & p_value )
	{
		return m_scales.AddKeyFrame( p_from, p_value, m_length );
	}

	Point3rKeyFrame & AnimationObjectBase::AddTranslateKeyFrame( real p_from, Point3r const & p_value )
	{
		return m_translates.AddKeyFrame( p_from, p_value, m_length );
	}

	QuaternionKeyFrame & AnimationObjectBase::AddRotateKeyFrame( real p_from, Quaternion const & p_value )
	{
		return m_rotates.AddKeyFrame( p_from, p_value, m_length );
	}

	void AnimationObjectBase::RemoveScaleKeyFrame( real p_time )
	{
		m_scales.RemoveKeyFrame( p_time );
	}

	void AnimationObjectBase::RemoveTranslateKeyFrame( real p_time )
	{
		m_translates.RemoveKeyFrame( p_time );
	}

	void AnimationObjectBase::RemoveRotateKeyFrame( real p_time )
	{
		m_rotates.RemoveKeyFrame( p_time );
	}

	AnimationObjectBaseSPtr AnimationObjectBase::Clone( Animation & p_animation )
	{
		AnimationObjectBaseSPtr l_return;

		if ( !p_animation.HasObject( GetType(), GetName() ) )
		{
			l_return = DoClone( p_animation );
			l_return->m_length = m_length;
			l_return->m_rotates = m_rotates;
			l_return->m_scales = m_scales;
			l_return->m_translates = m_translates;
			l_return->m_nodeTransform = m_nodeTransform;
			l_return->m_children.clear();

			for ( auto l_object : m_children )
			{
				auto l_clone = l_object->Clone( p_animation );

				if ( l_clone )
				{
					l_return->m_children.push_back( l_clone );
				}
			}
		}

		return l_return;
	}

	//*************************************************************************************************
}
