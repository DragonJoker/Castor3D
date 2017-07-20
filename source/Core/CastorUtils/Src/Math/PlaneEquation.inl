#include "Point.hpp"

namespace Castor
{
	template< typename T >
	PlaneEquation< T >::PlaneEquation()
		: m_d{ 0 }
	{
	}

	template< typename T >
	PlaneEquation< T >::PlaneEquation( Point< T, 3 > const & p_p1, Point< T, 3 > const & p_p2, Point< T, 3 > const & p_p3 )
		: m_d{ 0 }
	{
		Set( p_p1, p_p2, p_p3 );
	}

	template< typename T >
	PlaneEquation< T >::PlaneEquation( Point< T, 3 > const & p_normal, Point< T, 3 > const & p_point )
		: m_normal{ p_normal }
		, m_d{ 0 }
	{
		Set( p_normal, p_point );
	}

	template< typename T >
	PlaneEquation< T >::~PlaneEquation()
	{
	}

	template< typename T >
	void PlaneEquation< T >::Set( Point< T, 3 > const & p_p1, Point< T, 3 > const & p_p2, Point< T, 3 > const & p_p3 )
	{
		Point< T, 3 > v( p_p2 - p_p1 );
		Point< T, 3 > w( p_p3 - p_p1 );
		m_normal = Castor::point::get_normalised( w ^ v );
		m_point = ( p_p1 + p_p2 + p_p3 ) / T( 3 );
		m_d = -Castor::point::dot( m_point, m_normal );
	}

	template< typename T >
	void PlaneEquation< T >::Set( Point< T, 3 > const & p_normal, Point< T, 3 > const & p_point )
	{
		m_normal = Castor::point::get_normalised( p_normal );
		m_point = p_point;
		m_d = -Castor::point::dot( m_point, m_normal );
	}

	template< typename T >
	bool PlaneEquation< T >::IsParallel( PlaneEquation< T > const & p_plane )const
	{
		T ratioA = m_normal[0] / p_plane.m_normal[0];
		T ratioB = m_normal[1] / p_plane.m_normal[1];
		T ratioC = m_normal[2] / p_plane.m_normal[2];
		return policy::equals( ratioA, ratioB )
			   && policy::equals( ratioA, ratioC );
	}

	template< typename T >
	T PlaneEquation< T >::Distance( Point< T, 3 > const & p_point )const
	{
		return Castor::point::dot( m_normal, p_point ) + m_d;
	}

	template< typename T >
	Point< T, 3 > PlaneEquation< T >::Project( Point< T, 3 > const & p_point )const
	{
		return p_point - ( m_normal * Distance( p_point ) );
	}

	template< typename T >
	bool PlaneEquation< T >::Intersects( PlaneEquation< T > const & p_plane, Line3D<T> & p_line )const
	{
		bool result = false;

		if ( ! IsParallel( p_plane ) )
		{
			Point< T, 3 > normal( m_normal ^ p_plane.m_normal );
			T constexpr zero{};
			T b1 = m_normal[1];
			T c1 = m_normal[2];
			T d1 = m_d;
			T b2 = p_plane.m_normal[1];
			T c2 = p_plane.m_normal[2];
			T d2 = m_d;
			T div = ( b1 * c2 ) - ( b2 * c1 );

			if ( !policy::equals( b1, zero ) && !policy::equals( div, zero ) )
			{
				Point< T, 3 > point;
				point[2] = ( ( b2 * d1 ) - ( b1 * d2 ) ) / div;
				point[1] = ( ( -c1 * point[2] ) - d1 ) / b1;
				point[0] = 0;
				p_line = Line3D< T >::FromPointAndSlope( point, normal );
			}

			result = true;
		}

		return result;
	}

	template< typename T >
	bool PlaneEquation< T >::Intersects( PlaneEquation< T > const & p_plane1, PlaneEquation< T > const & p_plane2, Point<T, 3> & p_intersection )const
	{
		bool result = false;

		if ( ! IsParallel( p_plane1 ) && ! IsParallel( p_plane2 ) && ! p_plane1.IsParallel( p_plane2 ) )
		{
			T a1 = m_normal[0], a2 = p_plane1.m_normal[0], a3 = p_plane2.m_normal[0];
			T b1 = m_normal[1], b2 = p_plane1.m_normal[1], b3 = p_plane2.m_normal[1];
			T c1 = m_normal[2], c2 = p_plane1.m_normal[2], c3 = p_plane2.m_normal[2];
			T d = m_d;
			T d1 = p_plane1.m_d;
			T d2 = p_plane2.m_d;
			T alpha, beta;
			alpha = ( a3 - ( a2 * ( b3 - ( a3 / a1 ) ) / ( b2 - ( a2 / a1 ) ) ) ) / a1;
			beta = ( b3 - ( a3 / a1 ) ) / ( b2 - ( a2 / a1 ) );
			T comp = ( c1 * alpha ) + ( c2 * beta );

			if ( ! policy::equals( c3, comp ) )
			{
				alpha = ( ( a2 * c1 ) / ( a1 * ( b2 - ( a2 * b1 ) / a1 ) ) ) - ( c2 / ( b2 - ( a2 * b1 ) / a1 ) );
				beta = ( ( a2 * d ) / ( a1 * ( b2 - ( a2 * b1 ) / a1 ) ) ) - ( d1 / ( b2 - ( a2 * b1 ) / a1 ) );
				T x, y, z;
				z = ( ( a3 * ( ( d + ( beta * b1 ) ) / a1 ) ) - d2 ) / ( ( b3 * alpha ) + c3 - ( a3 * ( ( alpha * b1 ) + c1 ) / a1 ) );
				y = ( alpha * z ) + beta;
				x = ( z * ( 0.0f - ( ( alpha * b1 ) + c1 ) ) / a1 ) - ( ( d + ( b1 * beta ) ) / a1 );
				p_intersection[0] = x;
				p_intersection[1] = y;
				p_intersection[2] = z;
				result = true;
			}
		}

		return result;
	}

	template< typename T >
	bool PlaneEquation< T >::LineOn( Line3D< T > const & p_line )const
	{
		return std::abs( p_line[0] * m_normal[0] + p_line[1] * m_normal[1] + p_line[2] * m_normal[2] + m_d ) < std::numeric_limits< T >::epsilon();
	}

	template< typename T >
	bool operator==( PlaneEquation< T > const & p_a, PlaneEquation< T > const & p_b )
	{
		bool result = false;

		if ( p_a.IsParallel( p_b ) )
		{
			T ratioA = p_a.m_normal[0] / p_b.m_normal[0];
			T ratioD = p_a.m_d / p_b.m_d;
			result = PlaneEquation< T >::policy::equals( ratioA, ratioD );
		}

		return result;
	}

	template< typename T >
	bool operator!=( PlaneEquation< T > const & p_a, PlaneEquation< T > const & p_b )
	{
		return ( !( p_a == p_b ) );
	}
}
