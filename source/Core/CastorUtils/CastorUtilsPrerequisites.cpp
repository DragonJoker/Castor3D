#include "CastorUtils/CastorUtilsPrerequisites.hpp"

#include "CastorUtils/Config/SmartPtr.hpp"
#include "CastorUtils/Exception/Assertion.hpp"
#include "CastorUtils/Log/Logger.hpp"
#include "CastorUtils/Math/Point.hpp"
#include "CastorUtils/Math/SquareMatrix.hpp"
#include "CastorUtils/Math/Angle.hpp"

namespace castor
{
	Point3f operator*( Matrix4x4f const & lhs, Point3f const & rhs )
	{
		float const * mtx = lhs.constPtr();
		return Point3f
		{
			mtx[0] * rhs[0] + mtx[4] * rhs[1] + mtx[ 8] * rhs[2] + mtx[12],
			mtx[1] * rhs[0] + mtx[5] * rhs[1] + mtx[ 9] * rhs[2] + mtx[13],
			mtx[2] * rhs[0] + mtx[6] * rhs[1] + mtx[10] * rhs[2] + mtx[14]
		};
	}

	Point3f operator*( Point3f const & lhs, Matrix4x4f const & rhs )
	{
		float const * mtx = rhs.constPtr();
		return Point3f
		{
			mtx[0] * lhs[0] + mtx[1] * lhs[1] + mtx[ 2] * lhs[2] + mtx[ 3],
			mtx[4] * lhs[0] + mtx[5] * lhs[1] + mtx[ 6] * lhs[2] + mtx[ 7],
			mtx[8] * lhs[0] + mtx[9] * lhs[1] + mtx[10] * lhs[2] + mtx[11]
		};
	}

	Point4f operator*( Matrix4x4f const & lhs, Point4f const & rhs )
	{
		return Point4f
		{
			point::dot( rhs, lhs.getRow( 0 ) ),
			point::dot( rhs, lhs.getRow( 1 ) ),
			point::dot( rhs, lhs.getRow( 2 ) ),
			point::dot( rhs, lhs.getRow( 3 ) ),
		};
	}

	Point4f operator*( Point4f const & lhs, Matrix4x4f const & rhs )
	{
		return Point4f
		{
			point::dot( lhs, rhs.getColumn( 0 ) ),
			point::dot( lhs, rhs.getColumn( 1 ) ),
			point::dot( lhs, rhs.getColumn( 2 ) ),
			point::dot( lhs, rhs.getColumn( 3 ) ),
		};
	}

	String getName( LengthUnit value )
	{
		switch ( value )
		{
		case LengthUnit::eKilometre:
			return cuT( "km" );
		case LengthUnit::eMetre:
			return cuT( "m" );
		case LengthUnit::eCentimetre:
			return cuT( "cm" );
		case LengthUnit::eMillimetre:
			return cuT( "mm" );
		case LengthUnit::eYard:
			return cuT( "yd" );
		case LengthUnit::eFoot:
			return cuT( "ft" );
		case LengthUnit::eInch:
			return cuT( "in" );
		default:
			CU_Failure( "Unsupported LengthUnit" );
			return cuT( "unknown" );
		}
	}

	void cuLogError( char const * const description )noexcept
	{
		try
		{
			Logger::logError( description );
		}
		catch ( ... )
		{
			// well well well...
		}
	}

	void cuFailure( char const * const description )noexcept
	{
		try
		{
			auto stream = makeStringStream();
			stream << "Assertion failed: " << description << "\n";
			stream << debug::Backtrace{};
			cuLogError( toUtf8( stream.str() ).c_str() );
			assert( false );
		}
		catch ( ... )
		{
			// well well well...
		}
	}
}
