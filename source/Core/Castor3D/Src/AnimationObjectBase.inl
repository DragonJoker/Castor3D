#include "Interpolator.hpp"

namespace Castor3D
{
	//*************************************************************************************************

	namespace
	{
		template< typename T >
		void DoFind( real p_time,
					 typename KeyFrameArray< T >::const_iterator const & p_beg,
					 typename KeyFrameArray< T >::const_iterator const & p_end,
					 typename KeyFrameArray< T >::const_iterator & p_prv,
					 typename KeyFrameArray< T >::const_iterator & p_cur )
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

	template< typename T >
	AnimationObjectBase::KeyFrameInterpolation< T > & AnimationObjectBase::KeyFrameInterpolation< T >::operator=( AnimationObjectBase::KeyFrameInterpolation< T > const & p_rhs )
	{
		m_keyframes = p_rhs.m_keyframes;

		if ( m_keyframes.empty() )
		{
			m_prev = m_keyframes.end();
			m_curr = m_keyframes.end();
		}
		else
		{
			m_prev = m_keyframes.begin();
			m_curr = m_prev + 1;
		}

		SetInterpolationMode( p_rhs.m_mode );
		return *this;
	}

	template< typename T >
	void AnimationObjectBase::KeyFrameInterpolation< T >::SetInterpolationMode( eINTERPOLATOR_MODE p_mode )
	{
		if ( p_mode != m_mode )
		{
			m_mode = p_mode;

			switch ( m_mode )
			{
			case eINTERPOLATOR_MODE_NONE:
				m_interpolator = std::make_unique < InterpolatorT< T, eINTERPOLATOR_MODE_NONE > >();
				break;

			case eINTERPOLATOR_MODE_LINEAR:
				m_interpolator = std::make_unique < InterpolatorT< T, eINTERPOLATOR_MODE_LINEAR > >();
				break;

			default:
				FAILURE( "Unsupported interpolator mode" );
				break;
			}
		}
	}

	template< typename T >
	KeyFrame< T > & AnimationObjectBase::KeyFrameInterpolation< T >::AddKeyFrame( real p_from, T const & p_value, real & p_length )
	{
		auto l_it = std::find_if( m_keyframes.begin(), m_keyframes.end(), [&p_from]( KeyFrame< T > & p_keyframe )
		{
			return p_keyframe.GetTimeIndex() >= p_from;
		} );

		if ( l_it == m_keyframes.end() )
		{
			p_length = p_from;
			l_it = m_keyframes.insert( m_keyframes.end(), KeyFrame< T >( p_from, p_value ) );
		}
		else if ( l_it->GetTimeIndex() > p_from )
		{
			if ( l_it != m_keyframes.begin() )
			{
				--l_it;
			}

			l_it = m_keyframes.insert( l_it, KeyFrame< T >( p_from, p_value ) );
		}

		return *l_it;
	}

	template< typename T >
	void AnimationObjectBase::KeyFrameInterpolation< T >::RemoveKeyFrame( real p_time )
	{
		auto l_it = std::find_if( m_keyframes.begin(), m_keyframes.end(), [&p_time]( KeyFrame< T > const & p_keyframe )
		{
			return p_keyframe.GetTimeIndex() == p_time;
		} );

		if ( l_it != m_keyframes.end() )
		{
			m_keyframes.erase( l_it );
		}
	}

	template< class T >
	T AnimationObjectBase::KeyFrameInterpolation< T >::Compute( real p_time )
	{
		T l_return{ m_default };

		if ( m_keyframes.size() == 1 )
		{
			l_return = m_prev->GetValue();
		}
		else
		{
			DoFind< T >( p_time, m_keyframes.begin(), m_keyframes.end() - 1, m_prev, m_curr );
			real l_dt = m_curr->GetTimeIndex() - m_prev->GetTimeIndex();
			real l_factor = ( p_time - m_prev->GetTimeIndex() ) / l_dt;
			l_return = m_interpolator->Interpolate( m_prev->GetValue(), m_curr->GetValue(), l_factor );
		}

		return l_return;
	}

	//*************************************************************************************************
}
