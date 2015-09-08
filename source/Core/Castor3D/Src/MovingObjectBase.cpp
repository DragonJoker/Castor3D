#include "MovingObjectBase.hpp"
#include "KeyFrame.hpp"
#include "Interpolator.hpp"

#include <TransformationMatrix.hpp>
#include <Quaternion.hpp>

using namespace Castor;

namespace Castor3D
{
	MovingObjectBase::MovingObjectBase( eMOVING_OBJECT_TYPE p_eType )
		:	m_rLength( 0.0 )
		,	m_eType( p_eType )
	{
		m_mtxFinalTransformation.set_identity();
	}

	MovingObjectBase::~MovingObjectBase()
	{
	}

	void MovingObjectBase::AddChild( MovingObjectBaseSPtr p_pObject )
	{
		m_arrayChildren.push_back( p_pObject );
	}

	void MovingObjectBase::Update( real p_rTime, bool p_bLooped, Matrix4x4r const & p_mtxTransformations )
	{
		m_mtxTransformations = p_mtxTransformations * DoComputeTransform( p_rTime );
		DoApply();
		std::for_each( m_arrayChildren.begin(), m_arrayChildren.end(), [&]( MovingObjectBaseSPtr p_pObject )
		{
			p_pObject->Update( p_rTime, p_bLooped, m_mtxTransformations );
		} );
	}

	ScaleKeyFrameSPtr MovingObjectBase::AddScaleKeyFrame( real p_rFrom )
	{
		return DoAddKeyFrame< ScaleKeyFrame >( p_rFrom, m_mapScales );
	}

	TranslateKeyFrameSPtr MovingObjectBase::AddTranslateKeyFrame( real p_rFrom )
	{
		return DoAddKeyFrame< TranslateKeyFrame >( p_rFrom, m_mapTranslates );
	}

	RotateKeyFrameSPtr MovingObjectBase::AddRotateKeyFrame( real p_rFrom )
	{
		return DoAddKeyFrame< RotateKeyFrame >( p_rFrom, m_mapRotates );
	}

	void MovingObjectBase::RemoveScaleKeyFrame( real p_rTime )
	{
		DoRemoveKeyFrame< ScaleKeyFrame >( p_rTime, m_mapScales );
	}

	void MovingObjectBase::RemoveTranslateKeyFrame( real p_rTime )
	{
		DoRemoveKeyFrame< TranslateKeyFrame >( p_rTime, m_mapTranslates );
	}

	void MovingObjectBase::RemoveRotateKeyFrame( real p_rTime )
	{
		DoRemoveKeyFrame< RotateKeyFrame >( p_rTime, m_mapRotates );
	}

	MovingObjectBaseSPtr MovingObjectBase::Clone( MovingObjectPtrStrMap & p_map )
	{
		MovingObjectBaseSPtr l_pReturn = DoClone();
		p_map.insert( std::make_pair( l_pReturn->GetName(), l_pReturn ) );
		l_pReturn->m_mtxNodeTransform = m_mtxNodeTransform;
		l_pReturn->m_arrayChildren.clear();
		std::for_each( m_arrayChildren.begin(), m_arrayChildren.end(), [&]( MovingObjectBaseSPtr p_pObject )
		{
			l_pReturn->m_arrayChildren.push_back( p_pObject->Clone( p_map ) );
		} );
		return l_pReturn;
	}

	Matrix4x4r MovingObjectBase::DoComputeTransform( real p_rTime )
	{
		Matrix4x4r l_mtxReturn;

		if ( HasKeyFrames() )
		{
			matrix::set_transform( l_mtxReturn, DoComputeTranslation( p_rTime ), DoComputeScaling( p_rTime ), DoComputeRotation( p_rTime ) );
		}
		else
		{
			l_mtxReturn = m_mtxNodeTransform;
		}

		return l_mtxReturn;
	}

	Point3r MovingObjectBase::DoComputeScaling( real p_rTime )
	{
		return DoCompute< eINTERPOLATOR_MODE_LINEAR >( p_rTime, m_mapScales, Point3r( 1.0, 1.0, 1.0 ) );
	}

	Point3r MovingObjectBase::DoComputeTranslation( real p_rTime )
	{
		return DoCompute< eINTERPOLATOR_MODE_LINEAR >( p_rTime, m_mapTranslates, Point3r() );
	}

	Quaternion MovingObjectBase::DoComputeRotation( real p_rTime )
	{
		return DoCompute< eINTERPOLATOR_MODE_LINEAR >( p_rTime, m_mapRotates, Quaternion() );
	}
}
