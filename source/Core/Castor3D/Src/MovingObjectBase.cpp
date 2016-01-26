#include "MovingObjectBase.hpp"

#include "Animation.hpp"
#include "KeyFrame.hpp"
#include "Interpolator.hpp"
#include "MovingBone.hpp"
#include "MovingNode.hpp"
#include "MovingObject.hpp"

using namespace Castor;

namespace Castor3D
{
	//*************************************************************************************************

	MovingObjectBase::BinaryParser::BinaryParser( Path const & p_path )
		: Castor3D::BinaryParser< MovingObjectBase >( p_path )
	{
	}

	bool MovingObjectBase::BinaryParser::Fill( MovingObjectBase const & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_return = true;

		if ( l_return )
		{
			l_return = DoFillChunk( p_obj.m_nodeTransform, eCHUNK_TYPE_MOVING_TRANSFORM, p_chunk );
		}

		for ( auto const & l_kf : p_obj.m_scales )
		{
			l_return = DoFillChunk( l_kf.second.GetTimeIndex(), eCHUNK_TYPE_KEYFRAME_TIME, p_chunk );
			l_return = DoFillChunk( l_kf.second.GetValue(), eCHUNK_TYPE_KEYFRAME_SCALE, p_chunk );
		}

		for ( auto const & l_kf : p_obj.m_translates )
		{
			l_return = DoFillChunk( l_kf.second.GetTimeIndex(), eCHUNK_TYPE_KEYFRAME_TIME, p_chunk );
			l_return = DoFillChunk( l_kf.second.GetValue(), eCHUNK_TYPE_KEYFRAME_TRANSLATE, p_chunk );
		}

		for ( auto const & l_kf : p_obj.m_rotates )
		{
			l_return = DoFillChunk( l_kf.second.GetTimeIndex(), eCHUNK_TYPE_KEYFRAME_TIME, p_chunk );
			l_return = DoFillChunk( l_kf.second.GetValue(), eCHUNK_TYPE_KEYFRAME_ROTATE, p_chunk );
		}

		for ( auto const & l_moving : p_obj.m_children )
		{
			switch ( l_moving->GetType() )
			{
			case eMOVING_OBJECT_TYPE_NODE:
				l_return &= MovingNode::BinaryParser( m_path ).Fill( *std::static_pointer_cast< MovingNode >( l_moving ), p_chunk );
				break;

			case eMOVING_OBJECT_TYPE_OBJECT:
				l_return &= MovingObject::BinaryParser( m_path ).Fill( *std::static_pointer_cast< MovingObject >( l_moving ), p_chunk );
				break;

			case eMOVING_OBJECT_TYPE_BONE:
				l_return &= MovingBone::BinaryParser( m_path ).Fill( *std::static_pointer_cast< MovingBone >( l_moving ), p_chunk );
				break;
			}
		}

		return l_return;
	}

	bool MovingObjectBase::BinaryParser::Parse( MovingObjectBase & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_return = true;
		Matrix4x4r l_transform;
		Point3r l_point;
		Quaternion l_quat;
		MovingNodeSPtr l_node;
		MovingObjectSPtr l_object;
		MovingBoneSPtr l_bone;
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
			l_bone = std::make_shared< MovingBone >();
			l_return = MovingBone::BinaryParser( m_path ).Parse( *l_bone, p_chunk );

			if ( l_return )
			{
				p_obj.m_children.push_back( l_bone );
			}

			break;

		case eCHUNK_TYPE_MOVING_NODE:
			l_node = std::make_shared< MovingNode >();
			l_return = MovingNode::BinaryParser( m_path ).Parse( *l_node, p_chunk );

			if ( l_return )
			{
				p_obj.m_children.push_back( l_node );
			}

			break;

		case eCHUNK_TYPE_MOVING_OBJECT:
			l_object = std::make_shared< MovingObject >();
			l_return = MovingObject::BinaryParser( m_path ).Parse( *l_object, p_chunk );

			if ( l_return )
			{
				p_obj.m_children.push_back( l_object );
			}

			break;
		}

		return l_return;
	}

	//*************************************************************************************************

	MovingObjectBase::MovingObjectBase( eMOVING_OBJECT_TYPE p_type )
		: m_length( 0 )
		, m_type( p_type )
	{
	}

	MovingObjectBase::MovingObjectBase( MovingObjectBase const & p_rhs )
	{
	}

	MovingObjectBase::~MovingObjectBase()
	{
	}

	void MovingObjectBase::AddChild( MovingObjectBaseSPtr p_object )
	{
		m_children.push_back( p_object );
	}

	void MovingObjectBase::Update( real p_time, bool p_looped, Matrix4x4r const & p_transformations )
	{
		m_cumulativeTransform = p_transformations * DoComputeTransform( p_time );
		DoApply();

		for ( auto l_object : m_children )
		{
			l_object->Update( p_time, p_looped, m_cumulativeTransform );
		}
	}

	Point3rKeyFrame & MovingObjectBase::AddScaleKeyFrame( real p_from, Point3r const & p_value )
	{
		return DoAddKeyFrame( p_from, m_scales, p_value );
	}

	Point3rKeyFrame & MovingObjectBase::AddTranslateKeyFrame( real p_from, Point3r const & p_value )
	{
		return DoAddKeyFrame( p_from, m_translates, p_value );
	}

	QuaternionKeyFrame & MovingObjectBase::AddRotateKeyFrame( real p_from, Quaternion const & p_value )
	{
		return DoAddKeyFrame( p_from, m_rotates, p_value );
	}

	void MovingObjectBase::RemoveScaleKeyFrame( real p_time )
	{
		DoRemoveKeyFrame( p_time, m_scales );
	}

	void MovingObjectBase::RemoveTranslateKeyFrame( real p_time )
	{
		DoRemoveKeyFrame( p_time, m_translates );
	}

	void MovingObjectBase::RemoveRotateKeyFrame( real p_time )
	{
		DoRemoveKeyFrame( p_time, m_rotates );
	}

	Matrix4x4r MovingObjectBase::DoComputeTransform( real p_time )
	{
		Matrix4x4r l_return;

		if ( HasKeyFrames() )
		{
			matrix::set_transform( l_return, DoComputeTranslation( p_time ), DoComputeScaling( p_time ), DoComputeRotation( p_time ) );
		}
		else
		{
			l_return = m_nodeTransform;
		}

		return l_return;
	}

	Point3r MovingObjectBase::DoComputeScaling( real p_time )
	{
		return DoCompute< eINTERPOLATOR_MODE_LINEAR >( p_time, m_scales, Point3r( 1, 1, 1 ) );
	}

	Point3r MovingObjectBase::DoComputeTranslation( real p_time )
	{
		return DoCompute< eINTERPOLATOR_MODE_LINEAR >( p_time, m_translates, Point3r() );
	}

	Quaternion MovingObjectBase::DoComputeRotation( real p_time )
	{
		return DoCompute< eINTERPOLATOR_MODE_LINEAR >( p_time, m_rotates, Quaternion() );
	}

	MovingObjectBaseSPtr MovingObjectBase::Clone( Animation & p_animation )
	{
		MovingObjectBaseSPtr l_return;

		if ( !p_animation.HasMovingObject( GetType(), GetName() ) )
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
