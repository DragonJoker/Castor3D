/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Castor3DModule_H___
#define ___C3D_Castor3DModule_H___

#undef RGB
#include <CastorUtils/CastorUtilsPrerequisites.hpp>

#include <ashespp/AshesPPPrerequisites.hpp>

#include <cstdint>

#if !defined( CU_PlatformWindows )
#	define C3D_API
#else
#	ifdef MemoryBarrier
#		undef MemoryBarrier
#	endif
#	if defined( Castor3D_EXPORTS )
#		define C3D_API __declspec( dllexport )
#	else
#		define C3D_API __declspec( dllimport )
#	endif
#endif

namespace castor3d
{
	static uint32_t constexpr InvalidIndex = ~( 0u );

	using castor::UInt32StrMap;
	using castor::UInt64StrMap;
	using castor::UInt32Array;

	class Engine;

	CU_DeclareSmartPtr( Engine );

	C3D_API castor::Matrix4x4f convert( std::array< float, 16 > const & value );
	C3D_API VkClearColorValue convert( castor::RgbaColour const & value );
	C3D_API castor::RgbaColour convert( VkClearColorValue const & value );
}

#endif
