#include "CastorUtilsPch.hpp"

#if defined( VLD_AVAILABLE )
#	include <vld.h>
#endif

#include "Config/SmartPtr.hpp"
#include "Math/Point.hpp"
#include "Math/SquareMatrix.hpp"
#include "Math/Angle.hpp"

namespace Castor
{
	template<> const float AngleT< float >::Pi = float( 3.1415926535897932384626433832795028841968 );
	template<> const float AngleT< float >::PiDiv2 = AngleT< float >::Pi / 2;
	template<> const float AngleT< float >::PiMult2 = AngleT< float >::Pi * 2;
	template<> const float AngleT< float >::RadianToDegree = float( 57.295779513082320876798154814105 );
	template<> const float AngleT< float >::RadianToGrad = 200.0f / AngleT< float >::Pi;
	template<> const float AngleT< float >::DegreeToRadian = float( 0.01745329251994329576923690768489 );
	template<> const float AngleT< float >::DegreeToGrad = 200.0f / 180.0f;
	template<> const float AngleT< float >::GradToRadian = AngleT< float >::Pi / 200.0f;
	template<> const float AngleT< float >::GradToDegree = 180.0f / 200.0f;

	template<> const double AngleT< double >::Pi = 3.1415926535897932384626433832795028841968;
	template<> const double AngleT< double >::PiDiv2 = AngleT< double >::Pi / 2;
	template<> const double AngleT< double >::PiMult2 = AngleT< double >::Pi * 2;
	template<> const double AngleT< double >::RadianToDegree = 57.295779513082320876798154814105;
	template<> const double AngleT< double >::RadianToGrad = 200 / AngleT< double >::Pi;
	template<> const double AngleT< double >::DegreeToRadian = 0.01745329251994329576923690768489;
	template<> const double AngleT< double >::DegreeToGrad = 200.0 / 180.0;
	template<> const double AngleT< double >::GradToRadian = AngleT< double >::Pi / 200;
	template<> const double AngleT< double >::GradToDegree = 180.0 / 200.0;

	dummy_dtor g_dummyDtor;

	Point3r operator*( Matrix4x4r const & p_mtx, Point3r const & p_pt )
	{
		real const * l_mtx = p_mtx.const_ptr();
		return Point3r
		{
			l_mtx[0] * p_pt[0] + l_mtx[4] * p_pt[1] + l_mtx[ 8] * p_pt[2] + l_mtx[12],
			l_mtx[1] * p_pt[0] + l_mtx[5] * p_pt[1] + l_mtx[ 9] * p_pt[2] + l_mtx[13],
			l_mtx[2] * p_pt[0] + l_mtx[6] * p_pt[1] + l_mtx[10] * p_pt[2] + l_mtx[14]
		};
	}

	Point3r operator*( Point3r const & p_pt, Matrix4x4r const & p_mtx )
	{
		real const * l_mtx = p_mtx.const_ptr();
		return Point3r
		{
			l_mtx[0] * p_pt[0] + l_mtx[1] * p_pt[1] + l_mtx[ 2] * p_pt[2] + l_mtx[ 3],
			l_mtx[4] * p_pt[0] + l_mtx[5] * p_pt[1] + l_mtx[ 6] * p_pt[2] + l_mtx[ 7],
			l_mtx[8] * p_pt[0] + l_mtx[9] * p_pt[1] + l_mtx[10] * p_pt[2] + l_mtx[11]
		};
	}

	Point4r operator*( Matrix4x4r const & p_mtx, Point4r const & p_pt )
	{
		return Point4r
		{
			point::dot( p_pt, p_mtx.get_row( 0 ) ),
			point::dot( p_pt, p_mtx.get_row( 1 ) ),
			point::dot( p_pt, p_mtx.get_row( 2 ) ),
			point::dot( p_pt, p_mtx.get_row( 3 ) ),
		};
	}

	Point4r operator*( Point4r const & p_pt, Matrix4x4r const & p_mtx )
	{
		return Point4r
		{
			point::dot( p_pt, p_mtx.get_column( 0 ) ),
			point::dot( p_pt, p_mtx.get_column( 1 ) ),
			point::dot( p_pt, p_mtx.get_column( 2 ) ),
			point::dot( p_pt, p_mtx.get_column( 3 ) ),
		};
	}

	uint32_t GetNext2Pow( uint32_t p_value )
	{
		static std::array< uint32_t, 32 > const TwoPows
		{
			1u << 0,
			1u << 1,
			1u << 2,
			1u << 3,
			1u << 4,
			1u << 5,
			1u << 6,
			1u << 7,
			1u << 8,
			1u << 9,
			1u << 10,
			1u << 11,
			1u << 12,
			1u << 13,
			1u << 14,
			1u << 15,
			1u << 16,
			1u << 17,
			1u << 18,
			1u << 19,
			1u << 20,
			1u << 21,
			1u << 22,
			1u << 23,
			1u << 24,
			1u << 25,
			1u << 26,
			1u << 27,
			1u << 28,
			1u << 29,
			1u << 30,
			1u << 31
		};

		uint32_t l_return = 0;

		for ( uint32_t i = 0; i < TwoPows.size() && l_return < p_value; i++ )
		{
			l_return = TwoPows[i];
		}

		return l_return;
	}
}

extern "C" void tss_cleanup_implemented() { }
