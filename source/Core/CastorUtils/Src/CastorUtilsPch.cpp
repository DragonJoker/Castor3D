#include "CastorUtilsPch.hpp"

#if defined( VLD_AVAILABLE )
#	include <vld.h>
#endif

#include "Config/SmartPtr.hpp"
#include "Math/Point.hpp"
#include "Math/SquareMatrix.hpp"

namespace Castor
{
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
