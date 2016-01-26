#include "MovingObjectBase.hpp"

#include "Animation.hpp"
#include "KeyFrame.hpp"
#include "Interpolator.hpp"

using namespace Castor;

namespace Castor3D
{
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
}
