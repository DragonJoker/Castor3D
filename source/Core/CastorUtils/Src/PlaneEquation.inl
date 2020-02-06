﻿namespace Castor
{
	template< typename T >
	PlaneEquation< T >::PlaneEquation()
	{
	}

	template< typename T >
	PlaneEquation< T >::PlaneEquation( Point< T, 3 > const & p_p1, Point< T, 3 > const & p_p2, Point< T, 3 > const & p_p3 )
	{
		Set( p_p1, p_p2, p_p3 );
	}

	template< typename T >
	PlaneEquation< T >::PlaneEquation( Point< T, 3 > const & p_ptNormal, Point< T, 3 > const & p_ptPoint )
		:	m_normal( p_ptNormal	)
	{
		Set( p_ptNormal, p_ptPoint );
	}

	template< typename T >
	PlaneEquation< T >::~PlaneEquation()
	{
	}

	template< typename T >
	void PlaneEquation< T >::Set( Point< T, 3 > const & p_p1, Point< T, 3 > const & p_p2, Point< T, 3 > const & p_p3 )
	{
		Point< T, 3 > l_v( p_p2 - p_p1 );
		Point< T, 3 > l_w( p_p3 - p_p1 );
		m_normal = Castor::point::get_normalised( l_w ^ l_v );
		m_d = -Castor::point::dot( m_normal, p_p1 );
	}

	template< typename T >
	void PlaneEquation< T >::Set( Point< T, 3 > const & p_ptNormal, Point< T, 3 > const & p_ptPoint )
	{
		m_normal = Castor::point::get_normalised( p_ptNormal );
		m_d = -Castor::point::dot( m_normal, p_ptPoint );
	}

	template< typename T >
	bool PlaneEquation< T >::IsParallel( PlaneEquation< T > const & p_plane )const
	{
		T l_ratioA = m_normal[0] / p_plane.m_normal[0];
		T l_ratioB = m_normal[1] / p_plane.m_normal[1];
		T l_ratioC = m_normal[2] / p_plane.m_normal[2];
		return ( policy::equals( l_ratioA, l_ratioB ) && policy::equals( l_ratioA, l_ratioC ) );
	}

	template< typename T >
	bool PlaneEquation< T >::operator ==( PlaneEquation< T > const & p_plane )const
	{
		bool l_bReturn = false;

		if ( IsParallel( p_plane ) )
		{
			T l_ratioA = m_normal[0] / p_plane.m_normal[0];
			T l_ratioD = m_d / p_plane.m_d;
			l_bReturn = policy::equals( l_ratioA, l_ratioD );
		}

		return l_bReturn;
	}

	template< typename T >
	bool PlaneEquation< T >::operator !=( PlaneEquation< T > const & p_plane )const
	{
		return ( !( *this == p_plane ) );
	}

	template< typename T >
	T PlaneEquation< T >::Distance( Point< T, 3 > const & p_point )const
	{
		return Castor::point::dot( m_normal, p_point ) + m_d;
	}

	template< typename T >
	Point< T, 3 > PlaneEquation< T >::Project( Point< T, 3 > const & p_point )const
	{
		return p_point - Distance( p_point ) * m_normal;
	}

	template< typename T >
	bool PlaneEquation< T >::Intersects( PlaneEquation< T > const & p_plane, Line3D<T> & p_line )const
	{
		bool l_bReturn = false;

		if ( ! IsParallel( p_plane ) )
		{
			Point< T, 3 > l_normal( m_normal ^ p_plane.m_normal );
			T b1 = m_normal[1];
			T c1 = m_normal[2];
			T d1 = m_d;
			T b2 = p_plane.m_normal[1];
			T c2 = p_plane.m_normal[2];
			T d2 = p_plane.m_d;
			T div = ( b1 * c2 ) - ( b2 * c1 );

			if ( !policy::equals( b1, T() ) && !policy::equals( div, T() ) )
			{
				Point< T, 3 > l_point;
				l_point[2] = ( ( b2 * d1 ) - ( b1 * d2 ) ) / div;
				l_point[1] = ( ( -c1 * l_point[2] ) - d1 ) / b1;
				l_point[0] = 0;
				p_line = Line3D< T >::FromPointAndSlope( l_point, l_normal );
			}

			l_bReturn = true;
		}

		return l_bReturn;
	}

	template< typename T >
	bool PlaneEquation< T >::Intersects( PlaneEquation< T > const & p_plane1, PlaneEquation< T > const & p_plane2, Point<T, 3> & p_intersection )const
	{
		bool l_bReturn = false;

		if ( ! IsParallel( p_plane1 ) && ! IsParallel( p_plane2 ) && ! p_plane1.IsParallel( p_plane2 ) )
		{
			T a1 = m_normal[0], a2 = p_plane1.m_normal[0], a3 = p_plane2.m_normal[0];
			T b1 = m_normal[1], b2 = p_plane1.m_normal[1], b3 = p_plane2.m_normal[1];
			T c1 = m_normal[2], c2 = p_plane1.m_normal[2], c3 = p_plane2.m_normal[2];
			T alpha, beta;
			alpha = ( a3 - ( a2 * ( b3 - ( a3 / a1 ) ) / ( b2 - ( a2 / a1 ) ) ) ) / a1;
			beta = ( b3 - ( a3 / a1 ) ) / ( b2 - ( a2 / a1 ) );
			T l_c3 = ( c1 * alpha ) + ( c2 * beta );

			if ( ! policy::equals( c3, l_c3 ) )
			{
				alpha = ( ( a2 * c1 ) / ( a1 * ( b2 - ( a2 * b1 ) / a1 ) ) ) - ( c2 / ( b2 - ( a2 * b1 ) / a1 ) );
				beta = ( ( a2 * m_d ) / ( a1 * ( b2 - ( a2 * b1 ) / a1 ) ) ) - ( p_plane1.m_d / ( b2 - ( a2 * b1 ) / a1 ) );
				T x, y, z;
				z = ( ( a3 * ( ( m_d + ( beta * b1 ) ) / a1 ) ) - p_plane2.m_d ) / ( ( b3 * alpha ) + c3 - ( a3 * ( ( alpha * b1 ) + c1 ) / a1 ) );
				y = ( alpha * z ) + beta;
				x = ( z * ( 0.0f - ( ( alpha * b1 ) + c1 ) ) / a1 ) - ( ( m_d + ( b1 * beta ) ) / a1 );
				p_intersection[0] = x;
				p_intersection[1] = y;
				p_intersection[2] = z;
				l_bReturn = true;
			}
		}

		return l_bReturn;
	}

	template< typename T >
	bool PlaneEquation< T >::LineOn( Line3D< T > const & p_line )const
	{
		return std::abs( p_line[0] * m_normal[0] + p_line[1] * m_normal[1] + p_line[2] * m_normal[2] + m_d ) < std::numeric_limits< T >::epsilon();
	}
}
