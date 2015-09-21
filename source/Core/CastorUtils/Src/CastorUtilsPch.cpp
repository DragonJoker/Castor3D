#include "CastorUtilsPch.hpp"

#include "SmartPtr.hpp"
#include "Point.hpp"
#include "SquareMatrix.hpp"

namespace Castor
{
	dummy_dtor g_dummyDtor;

	Point3r operator * ( Matrix4x4r const & p_mtx, Point3r const & p_pt )
	{
		real const * l_pMtx = p_mtx.const_ptr();
		Point3r l_return;
		l_return[0] = l_pMtx[0] * p_pt[0] + l_pMtx[ 1] * p_pt[1] + l_pMtx[ 2] * p_pt[2];// + l_pMtx[ 3];
		l_return[1] = l_pMtx[4] * p_pt[0] + l_pMtx[ 5] * p_pt[1] + l_pMtx[ 6] * p_pt[2];// + l_pMtx[ 7];
		l_return[2] = l_pMtx[8] * p_pt[0] + l_pMtx[ 9] * p_pt[1] + l_pMtx[10] * p_pt[2];// + l_pMtx[11];
		return l_return;
	}

	Point3r operator * ( Point3r const & p_pt, Matrix4x4r const & p_mtx )
	{
		real const * l_pMtx = p_mtx.const_ptr();
		Point3r l_return;
		l_return[0] = l_pMtx[0] * p_pt[0] + l_pMtx[ 4] * p_pt[1] + l_pMtx[ 8] * p_pt[2];
		l_return[1] = l_pMtx[1] * p_pt[0] + l_pMtx[ 5] * p_pt[1] + l_pMtx[ 9] * p_pt[2];
		l_return[2] = l_pMtx[2] * p_pt[0] + l_pMtx[ 6] * p_pt[1] + l_pMtx[10] * p_pt[2];
		return l_return;
	}

	uint32_t GetNext2Pow( uint32_t p_uiDim )
	{
		static uint32_t TwoPows[] =
		{
			1 << 0,
			1 << 1,
			1 << 2,
			1 << 3,
			1 << 4,
			1 << 5,
			1 << 6,
			1 << 7,
			1 << 8,
			1 << 9,
			1 << 10,
			1 << 11,
			1 << 12,
			1 << 13,
			1 << 14,
			1 << 15,
			1 << 16,
			1 << 17,
			1 << 18,
			1 << 19,
			1 << 20,
			1 << 21// should be enough for image dimensions ...
		};
		static uint32_t Size = sizeof( TwoPows ) / sizeof( uint32_t );
		uint32_t l_uiReturn = 0;

		for ( uint32_t i = 0; i < Size && l_uiReturn < p_uiDim; i++ )
		{
			l_uiReturn = TwoPows[i];
		}

		return l_uiReturn;
	}
}

extern "C" void tss_cleanup_implemented() { }
