namespace Castor3D
{
	template< class KeyFrameType >
	std::shared_ptr< KeyFrameType > MovingObjectBase::DoAddKeyFrame( real p_from, std::map< real, std::shared_ptr< KeyFrameType > > & p_map )
	{
		std::shared_ptr< KeyFrameType > l_return;
		auto l_it = p_map.find( p_from );

		if ( l_it != p_map.end() )
		{
			l_return = l_it->second;
		}

		if ( !l_return )
		{
			if ( p_from > m_length )
			{
				m_length = p_from;
			}

			l_return = std::make_shared< KeyFrameType >();
			l_return->SetTimeIndex( p_from );
			p_map.insert( std::make_pair( p_from, l_return ) );
		}

		return l_return;
	}

	template< class KeyFrameType >
	void MovingObjectBase::DoRemoveKeyFrame( real p_time, std::map< real, std::shared_ptr< KeyFrameType > > & p_map )
	{
		std::shared_ptr< KeyFrameType > l_pKeyFrame;
		auto l_it = p_map.find( p_time );

		if ( l_it != p_map.end() )
		{
			l_pKeyFrame = l_it->second;
			p_map.erase( l_it );
		}

		l_pKeyFrame.reset();
	}

	template< eINTERPOLATOR_MODE Mode, class ValueType, class KeyFrameType >
	ValueType MovingObjectBase::DoCompute( real p_time, std::map< real, std::shared_ptr< KeyFrameType > > const & p_map, ValueType const & p_default )
	{
		ValueType l_return( p_default );
		auto l_itCur = std::find_if( p_map.begin(), p_map.end(), [&p_time]( std::pair< real, std::shared_ptr< KeyFrameType > > const & p_pair )
		{
			return p_pair.second->GetTimeIndex() >= p_time;
		} );

		if ( l_itCur != p_map.end() )
		{
			auto l_itPrv = l_itCur;

			if ( l_itPrv == p_map.begin() )
			{
				l_return = l_itCur->second->GetValue();
			}
			else
			{
				--l_itPrv;
				real l_dt = l_itCur->first - l_itPrv->first;
				real l_factor = ( p_time - l_itPrv->first ) / l_dt;
				l_return = Interpolator< ValueType, Mode >( l_itPrv->second->GetValue(), l_itCur->second->GetValue() ).Interpolate( l_factor );
			}
		}

		return l_return;
	}
}
