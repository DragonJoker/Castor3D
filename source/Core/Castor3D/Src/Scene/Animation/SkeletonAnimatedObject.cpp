#include "SkeletonAnimatedObject.hpp"

#include "Animation/Interpolator.hpp"
#include "Animation/KeyFrame.hpp"
#include "Animation/Skeleton/SkeletonAnimationObject.hpp"

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

	SkeletonAnimatedObject::SkeletonAnimatedObject( AnimatedSkeleton & p_animatedSkeleton, SkeletonAnimationObjectSPtr p_animationObject )
		: OwnedBy< AnimatedSkeleton >{ p_animatedSkeleton }
		, m_animationObject{ p_animationObject }
	{
		SetInterpolationMode( InterpolatorType::Linear );
	}

	SkeletonAnimatedObject::~SkeletonAnimatedObject()
	{
	}

	void SkeletonAnimatedObject::AddChild( SkeletonAnimatedObjectSPtr p_object )
	{
		m_children.push_back( p_object );
	}

	void SkeletonAnimatedObject::Update( real p_time, Matrix4x4r const & p_transformations )
	{
		if ( HasKeyFrames() )
		{
			Point3r l_translate{};
			Quaternion l_rotate{};
			Point3r l_scale{ 1.0_r, 1.0_r , 1.0_r };

			if ( m_animationObject->GetKeyFrames().size() == 1 )
			{
				l_translate = m_prev->GetTranslate();
				l_rotate = m_prev->GetRotate();
				l_scale = m_prev->GetScale();
			}
			else
			{
				DoFind( p_time, m_animationObject->GetKeyFrames().begin(), m_animationObject->GetKeyFrames().end() - 1, m_prev, m_curr );
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
			m_cumulativeTransform = p_transformations * m_animationObject->GetNodeTransform();
		}

		DoApply();

		for ( auto l_object : m_children )
		{
			l_object->Update( p_time, m_cumulativeTransform );
		}
	}

	void SkeletonAnimatedObject::SetInterpolationMode( InterpolatorType p_mode )
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

	//*************************************************************************************************
}
