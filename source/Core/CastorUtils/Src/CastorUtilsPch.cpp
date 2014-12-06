#include "CastorUtilsPch.hpp"

#if defined( __BORLANDC__)
#	pragma message( "Compiler : Borland C++")
#elif defined( __clang__)
#	pragma message( "Compiler : Clang")
#elif defined( __COMO__)
#	pragma message( "Compiler : Comeau C++")
#elif defined( __DECC)
#	pragma message( "Compiler : Compaq C/C++")
#elif defined( __CYGWIN32__)
#	pragma message( "Compiler : Cygwin")
#elif defined( __DCC__)
#	pragma message( "Compiler : Diab C/C++")
#elif defined( __DMC__)
#	pragma message( "Compiler : Digital Mars")
#elif defined( __SYSC_)
#	pragma message( "Compiler : Dignus Systems/C and Systems/C++")
#elif defined( __DJGPP__)
#	pragma message( "Compiler : DJGPP")
#elif defined( __PATHCC__)
#	pragma message( "Compiler : EKOPath")
#elif defined( __EDG__)
#	pragma message( "Compiler : EDG C++ Front End")
#elif defined( __GNUC__)
#	pragma message( "Compiler : GNU C/C++")
#elif defined( __ghs__)
#	pragma message( "Compiler : Green Hill C/C++")
#elif defined( __HP_aCC)
#	pragma message( "Compiler : HP aCC")
#elif defined( __IAR_SYSTEMS_ICC__)
#	pragma message( "Compiler : IAR C/C++")
#elif defined( __IBMCPP__)
#	pragma message( "Compiler : IBM XL C/C++")
#elif defined( __IAR_SYSTEMS_ICC__)
#	pragma message( "Compiler : IBM z/OS")
#elif defined( __INTEL_COMPILER) || defined( __ICC) || defined( __ECC) || defined( __ICL)
#	pragma message( "Compiler : Intel C/C++")
#elif defined( __KCC)
#	pragma message( "Compiler : KAI C++")
#elif defined( __CA__) || defined( __KEIL__)
#	pragma message( "Compiler : Keil CARM")
#elif defined( __C166__)
#	pragma message( "Compiler : Keil C166")
#elif defined( __C51__) || defined( __CX51__)
#	pragma message( "Compiler : Keil C51")
#elif defined( __LCC__)
#	pragma message( "Compiler : LCC")
#elif defined( __llvm__)
#	pragma message( "Compiler : LLVM")
#elif defined( __HIGHC__)
#	pragma message( "Compiler : MetaWare High C/C++")
#elif defined( __MWERKS__)
#	pragma message( "Compiler : Metrowerks CodeWarrior")
#elif defined( __MINGW32__)
#	pragma message( "Compiler : MinGW32")
#elif defined( __sgi) || defined( sgi)
#	pragma message( "Compiler : MIPSpro")
#elif defined( __MRC__)
#	pragma message( "Compiler : MPW C++")
#elif defined( _MSC_VER)
#	pragma message( "Compiler : Microsoft Visual C++")
#elif defined( _MRI)
#	pragma message( "Compiler : Microtec C/C++")
#elif defined( _PACC_VER)
#	pragma message( "Compiler : Palm C/C++")
#elif defined( __PGI)
#	pragma message( "Compiler : Portland Group C/C++")
#elif defined( __RENESAS__) || defined( __HITACHI__)
#	pragma message( "Compiler : Renesas C/C++")
#elif defined( _SCO_DS)
#	pragma message( "Compiler : SCO OpenServer")
#elif defined( __SUNPRO_C) || defined( __SUNPRO_CC)
#	pragma message( "Compiler : Sun Studio")
#elif defined( __TenDRA__)
#	pragma message( "Compiler : TenDRA C/C++")
#elif defined( _UCC)
#	pragma message( "Compiler : Ultimate C/C++")
#elif defined( __WATCOMC__)
#	pragma message( "Compiler : Watcom C++")
#else
#	pragma error( "Compiler : Unknown")
#endif

#if defined( _WIN64)
#	pragma message( "Target OS : Microsoft Windows 64bits")
#elif defined( _WIN32)
#	pragma message( "Target OS : Microsoft Windows 32bits")
#elif defined( __linux__) || defined( __linux) || defined( linux) || defined( __gnu_linux__)
#	pragma message( "Target OS : Linux")
#elif defined( __unix__) || defined( __unix) || defined( unix)
#	pragma message( "Target OS : Unix")
#elif defined( __APPLE__) || defined( __MACH__)
#	pragma message( "Target OS : Mac OS X")
#else
#	pragma message( "Target OS : Unknown")
#endif

#if defined( __i386__) || defined( __i386) || defined( i386)
#	pragma message( "Target architecture : IA-32")
#elif defined( __ia64__) || defined( __ia64) || defined( ia64)
#	pragma message( "Target architecture : Itanium")
#elif defined( __x86_64__) || defined( __x86_64)
#	pragma message( "Target architecture : Intel EM64T")
#endif

#if CASTOR_HAS_VARIADIC_TEMPLATES
#   pragma message( "variadic templates available")
#else
#   pragma message( "variadic templates not available")
#endif

#if CASTOR_HAS_NULLPTR
#   pragma message( "null_ptr available")
#else
#   pragma message( "null_ptr not available")
#endif

#if CASTOR_HAS_STDSMARTPTR
#   pragma message( "shared_ptr available from std")
#else
#   if CASTOR_HAS_TR1SMARTPTR
#       pragma message( "shared_ptr available from tr1")
#   else
#       pragma message( "shared_ptr available from boost")
#   endif
#endif

#if CASTOR_HAS_STDARRAY
#   pragma message( "array available from std")
#else
#   if CASTOR_HAS_TR1ARRAY
#       pragma message( "array available from tr1")
#   else
#       pragma message( "array available from boost")
#   endif
#endif

#if CASTOR_HAS_STDFUNCTIONAL
#   pragma message( "function available from std")
#else
#   if CASTOR_HAS_TR1FUNCTIONAL
#       pragma message( "function available from tr1")
#   else
#       pragma message( "function available from boost")
#   endif
#endif

#if CASTOR_HAS_DEFAULTED_FUNC
#   pragma message( "default function attributes available")
#else
#   pragma message( "default function attributes not available")
#endif

#if CASTOR_HAS_DELETED_FUNC
#   pragma message( "delete function attributes available")
#else
#   pragma message( "delete function attributes not available")
#endif

#include "SmartPtr.hpp"
#include "Point.hpp"
#include "SquareMatrix.hpp"

namespace Castor
{
	dummy_dtor g_dummyDtor;

	Point3r operator * ( Matrix4x4r const & p_mtx, Point3r const & p_pt )
	{
		real const * l_pMtx = p_mtx.const_ptr();
		Point3r l_ptReturn;
		l_ptReturn[0] = l_pMtx[0] * p_pt[0] + l_pMtx[ 1] * p_pt[1] + l_pMtx[ 2] * p_pt[2];// + l_pMtx[ 3];
		l_ptReturn[1] = l_pMtx[4] * p_pt[0] + l_pMtx[ 5] * p_pt[1] + l_pMtx[ 6] * p_pt[2];// + l_pMtx[ 7];
		l_ptReturn[2] = l_pMtx[8] * p_pt[0] + l_pMtx[ 9] * p_pt[1] + l_pMtx[10] * p_pt[2];// + l_pMtx[11];
		return l_ptReturn;
	}

	Point3r operator * ( Point3r const & p_pt, Matrix4x4r const & p_mtx )
	{
		real const * l_pMtx = p_mtx.const_ptr();
		Point3r l_ptReturn;
		l_ptReturn[0] = l_pMtx[0] * p_pt[0] + l_pMtx[ 4] * p_pt[1] + l_pMtx[ 8] * p_pt[2];
		l_ptReturn[1] = l_pMtx[1] * p_pt[0] + l_pMtx[ 5] * p_pt[1] + l_pMtx[ 9] * p_pt[2];
		l_ptReturn[2] = l_pMtx[2] * p_pt[0] + l_pMtx[ 6] * p_pt[1] + l_pMtx[10] * p_pt[2];
		return l_ptReturn;
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
