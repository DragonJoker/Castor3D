#include "MovingObjectBase.hpp"
#include "KeyFrame.hpp"
#include "Interpolator.hpp"
#include "QuaternionInterpolator.hpp"

#include <TransformationMatrix.hpp>
#include <Quaternion.hpp>

using namespace Castor;

namespace Castor3D
{
	MovingObjectBase::MovingObjectBase( eMOVING_OBJECT_TYPE p_type )
		: m_length( 0.0_r )
		, m_type( p_type )
	{
		m_finalTransformation.set_identity();
	}

	MovingObjectBase::~MovingObjectBase()
	{
	}

	void MovingObjectBase::AddChild( MovingObjectBaseSPtr p_object )
	{
		m_children.push_back( p_object );
	}

	void MovingObjectBase::Update( real p_time, bool p_looped, Matrix4x4r const & p_mtxTransformations )
	{
		m_transformations = p_mtxTransformations * DoComputeTransform( p_time );
		DoApply();

		for ( auto l_object : m_children )
		{
			l_object->Update( p_time, p_looped, m_transformations );
		}
	}

	ScaleKeyFrameSPtr MovingObjectBase::AddScaleKeyFrame( real p_from )
	{
		return DoAddKeyFrame( p_from, m_scales );
	}

	TranslateKeyFrameSPtr MovingObjectBase::AddTranslateKeyFrame( real p_from )
	{
		return DoAddKeyFrame( p_from, m_translates );
	}

	RotateKeyFrameSPtr MovingObjectBase::AddRotateKeyFrame( real p_from )
	{
		return DoAddKeyFrame( p_from, m_rotates );
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

	MovingObjectBaseSPtr MovingObjectBase::Clone( MovingObjectPtrStrMap & p_map )
	{
		MovingObjectBaseSPtr l_return = DoClone();
		p_map.insert( std::make_pair( l_return->GetName(), l_return ) );
		l_return->m_nodeTransform = m_nodeTransform;
		l_return->m_children.clear();

		for ( auto l_object : m_children )
		{
			l_return->m_children.push_back( l_object->Clone( p_map ) );
		}

		return l_return;
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
		return DoCompute< eINTERPOLATOR_MODE_LINEAR >( p_time, m_scales, Point3r( 1.0_r, 1.0_r, 1.0_r ) );
	}

	Point3r MovingObjectBase::DoComputeTranslation( real p_time )
	{
		return DoCompute< eINTERPOLATOR_MODE_LINEAR >( p_time, m_translates, Point3r() );
	}

	Quaternion MovingObjectBase::DoComputeRotation( real p_time )
	{
		return DoCompute< eINTERPOLATOR_MODE_LINEAR >( p_time, m_rotates, Quaternion() );
	}
}
