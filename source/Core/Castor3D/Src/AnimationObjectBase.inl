#include "Interpolator.hpp"

namespace Castor3D
{
	//*************************************************************************************************

	template< typename T >
	AnimationObjectBase::KeyFrameInterpolation< T > & AnimationObjectBase::KeyFrameInterpolation< T >::operator=( AnimationObjectBase::KeyFrameInterpolation< T > const & p_rhs )
	{
		m_keyframes = p_rhs.m_keyframes;
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

	//*************************************************************************************************

	template< class KeyFrameType, typename T >
	KeyFrameType & AnimationObjectBase::DoAddKeyFrame( real p_from, std::map< real, KeyFrameType > & p_map, T const & p_value )
	{
		auto l_it = p_map.find( p_from );

		if ( l_it == p_map.end() )
		{
			if ( p_from > m_length )
			{
				m_length = p_from;
			}

			l_it = p_map.insert( std::make_pair( p_from, KeyFrameType( p_from, p_value ) ) ).first;
		}

		return l_it->second;
	}

	template< class KeyFrameType >
	void AnimationObjectBase::DoRemoveKeyFrame( real p_time, std::map< real, KeyFrameType > & p_map )
	{
		auto l_it = p_map.find( p_time );

		if ( l_it != p_map.end() )
		{
			p_map.erase( l_it );
		}
	}

	template< class T >
	T AnimationObjectBase::DoCompute( real p_time, KeyFrameInterpolation< T > const & p_keyframes, T const & p_default )
	{
		T l_return{ p_default };
		auto l_itCur = std::find_if( p_keyframes.m_keyframes.begin(), p_keyframes.m_keyframes.end(), [&p_time]( KeyFrameRealMap< T >::value_type const & p_pair )
		{
			return p_pair.second.GetTimeIndex() >= p_time;
		} );

		if ( l_itCur != p_keyframes.m_keyframes.end() )
		{
			auto l_itPrv = l_itCur;

			if ( l_itPrv == p_keyframes.m_keyframes.begin() )
			{
				l_return = l_itCur->second.GetValue();
			}
			else
			{
				--l_itPrv;
				real l_dt = l_itCur->first - l_itPrv->first;
				real l_factor = ( p_time - l_itPrv->first ) / l_dt;
				l_return = p_keyframes.m_interpolator->Interpolate( l_itPrv->second.GetValue(), l_itCur->second.GetValue(), l_factor );
			}
		}

		return l_return;
	}

	//*************************************************************************************************
}
