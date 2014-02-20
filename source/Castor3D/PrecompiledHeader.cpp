#include "Castor3D/PrecompiledHeader.hpp"

#pragma warning( disable:4311 )
#pragma warning( disable:4312 )

#if defined( VLD_AVAILABLE )
#	include <vld.h>
#elif defined( _WIN32 ) && !defined( NDEBUG )
#	define _CRTDBG_MAP_ALLOC
#	include <cstdlib>
#	include <crtdbg.h>
#endif

Castor::Point3r Castor3D::operator * ( Castor::Matrix4x4r const & p_mtx, Castor::Point3r const & p_pt )
{
	Castor::real const * l_pMtx = p_mtx.const_ptr();
	Castor::Point3r l_ptReturn;
	l_ptReturn[0] = l_pMtx[0] * p_pt[0] + l_pMtx[ 1] * p_pt[1] + l_pMtx[ 2] * p_pt[2];// + l_pMtx[ 3];
	l_ptReturn[1] = l_pMtx[4] * p_pt[0] + l_pMtx[ 5] * p_pt[1] + l_pMtx[ 6] * p_pt[2];// + l_pMtx[ 7];
	l_ptReturn[2] = l_pMtx[8] * p_pt[0] + l_pMtx[ 9] * p_pt[1] + l_pMtx[10] * p_pt[2];// + l_pMtx[11];
	return l_ptReturn;
}

Castor::Point3r Castor3D::operator * ( Castor::Point3r const & p_pt, Castor::Matrix4x4r const & p_mtx )
{
	Castor::real const * l_pMtx = p_mtx.const_ptr();
	Castor::Point3r l_ptReturn;
	l_ptReturn[0] = l_pMtx[0] * p_pt[0] + l_pMtx[ 4] * p_pt[1] + l_pMtx[ 8] * p_pt[2];
	l_ptReturn[1] = l_pMtx[1] * p_pt[0] + l_pMtx[ 5] * p_pt[1] + l_pMtx[ 9] * p_pt[2];
	l_ptReturn[2] = l_pMtx[2] * p_pt[0] + l_pMtx[ 6] * p_pt[1] + l_pMtx[10] * p_pt[2];
	return l_ptReturn;
}

uint32_t Castor3D::GetNext2Pow( uint32_t p_uiDim )
{
	static uint32_t TwoPows[] = { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072, 262144, 524288, 1048576 };// should be enough for image dimensions ...
	static uint32_t Size = sizeof( TwoPows ) / sizeof( uint32_t );
	uint32_t l_uiReturn = 0;

	for( uint32_t i = 0; i < Size && l_uiReturn < p_uiDim; i++ )
	{
		l_uiReturn = TwoPows[i];
	}

	return l_uiReturn;
}
