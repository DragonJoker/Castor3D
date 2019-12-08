#include "CastorUtils/Math/PlaneEquation.hpp"

namespace castor
{
	PlaneEquation::PlaneEquation()
		: m_d{ 0 }
	{
	}

	PlaneEquation::PlaneEquation( Point3f const & p1
		, Point3f const & p2
		, Point3f const & p3 )
		: m_d{ 0 }
	{
		set( p1, p2, p3 );
	}

	PlaneEquation::PlaneEquation( Point3f const & normal
		, Point3f const & point )
		: m_normal{ normal }
		, m_d{ 0 }
	{
		set( normal, point );
	}

	PlaneEquation::PlaneEquation( Point3f const & normal
		, float d )
		: m_normal{ normal }
		, m_d{ d }
	{
	}

	void PlaneEquation::set( Point3f const & p1
		, Point3f const & p2
		, Point3f const & p3 )
	{
		Point3f v{ p2 - p1 };
		Point3f u{ p3 - p1 };
		m_normal = point::getNormalised( point::cross( u, v ) );
		m_d = -point::dot( p3, m_normal );
	}

	void PlaneEquation::set( Point3f const & normal
		, Point3f const & point )
	{
		m_normal = normal;
		m_d = -point::dot( point, normal );
	}

	void PlaneEquation::set( Point3f const & normal
		, float d )
	{
		m_normal = point::getNormalised( normal );
		m_d = d;
	}

	bool PlaneEquation::isParallel( PlaneEquation const & plane )const
	{
		auto ratioA = m_normal[0] / plane.m_normal[0];
		auto ratioB = m_normal[1] / plane.m_normal[1];
		auto ratioC = m_normal[2] / plane.m_normal[2];
		return ratioA == ratioB
			   && ratioA == ratioC;
	}

	float PlaneEquation::distance( Point3f const & point )const
	{
		return point::dot( m_normal, point ) + m_d;
	}

	Point3f PlaneEquation::project( Point3f const & point )const
	{
		return point - ( m_normal * distance( point ) );
	}

	bool PlaneEquation::intersects( PlaneEquation const & plane
		, Line3D< float > & line )const
	{
		bool result = false;

		if ( !isParallel( plane ) )
		{
			Point3f normal( point::cross( m_normal, plane.m_normal ) );
			float constexpr zero{};
			auto b1 = m_normal[1];
			auto c1 = m_normal[2];
			auto d1 = m_d;
			auto b2 = plane.m_normal[1];
			auto c2 = plane.m_normal[2];
			auto d2 = m_d;
			auto div = ( b1 * c2 ) - ( b2 * c1 );

			if ( b1 != zero && div != zero )
			{
				Point3f point;
				point[2] = ( ( b2 * d1 ) - ( b1 * d2 ) ) / div;
				point[1] = ( ( -c1 * point[2] ) - d1 ) / b1;
				point[0] = 0;
				line = Line3D< float >::fromPointAndSlope( point, normal );
			}

			result = true;
		}

		return result;
	}

	bool PlaneEquation::intersects( PlaneEquation const & plane1
		, PlaneEquation const & plane2
		, Point3f & intersection )const
	{
		bool result = false;

		if ( !isParallel( plane1 )
			&& !isParallel( plane2 )
			&& !plane1.isParallel( plane2 ) )
		{
			auto a1 = m_normal[0], a2 = plane1.m_normal[0], a3 = plane2.m_normal[0];
			auto b1 = m_normal[1], b2 = plane1.m_normal[1], b3 = plane2.m_normal[1];
			auto c1 = m_normal[2], c2 = plane1.m_normal[2], c3 = plane2.m_normal[2];
			auto d = m_d;
			auto d1 = plane1.m_d;
			auto d2 = plane2.m_d;
			float alpha, beta;
			alpha = ( a3 - ( a2 * ( b3 - ( a3 / a1 ) ) / ( b2 - ( a2 / a1 ) ) ) ) / a1;
			beta = ( b3 - ( a3 / a1 ) ) / ( b2 - ( a2 / a1 ) );
			auto comp = ( c1 * alpha ) + ( c2 * beta );

			if ( c3 != comp )
			{
				alpha = ( ( a2 * c1 ) / ( a1 * ( b2 - ( a2 * b1 ) / a1 ) ) ) - ( c2 / ( b2 - ( a2 * b1 ) / a1 ) );
				beta = ( ( a2 * d ) / ( a1 * ( b2 - ( a2 * b1 ) / a1 ) ) ) - ( d1 / ( b2 - ( a2 * b1 ) / a1 ) );
				float x, y, z;
				z = ( ( a3 * ( ( d + ( beta * b1 ) ) / a1 ) ) - d2 ) / ( ( b3 * alpha ) + c3 - ( a3 * ( ( alpha * b1 ) + c1 ) / a1 ) );
				y = ( alpha * z ) + beta;
				x = ( z * ( 0.0f - ( ( alpha * b1 ) + c1 ) ) / a1 ) - ( ( d + ( b1 * beta ) ) / a1 );
				intersection[0] = x;
				intersection[1] = y;
				intersection[2] = z;
				result = true;
			}
		}

		return result;
	}

	bool operator==( PlaneEquation const & lhs
		, PlaneEquation const & rhs )
	{
		bool result = false;

		if ( lhs.isParallel( rhs ) )
		{
			auto ratioA = lhs.m_normal[0] / rhs.m_normal[0];
			auto ratioD = lhs.m_d / rhs.m_d;
			result = ratioA == ratioD;
		}

		return result;
	}

	bool operator!=( PlaneEquation const & lhs
		, PlaneEquation const & rhs )
	{
		return ( !( lhs == rhs ) );
	}
}
