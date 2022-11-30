/* See LICENSE file in root folder */
#ifndef ___C3DCOM_PREREQUISITES_H___
#define ___C3DCOM_PREREQUISITES_H___

#if defined( __linux__ )
#	define COMC3D_API
#elif defined( _WIN32 )
#	if defined( ComCastor3D_EXPORTS )
#		define COMC3D_API __declspec( dllexport )
#	else
#		define COMC3D_API __declspec( dllimport )
#	endif
#endif

#include <Castor3D/Castor3DPrerequisites.hpp>

// Include all the standard header *after* all the configuration
// settings have been made
#define _ATL_DLL

#ifndef STRICT
#	define STRICT
#endif

#ifndef _WIN32_WINNT
#	define _WIN32_WINNT 0x0501
#endif

#define _ATL_APARTMENT_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit
#define ATL_NO_ASSERT_ON_DESTROY_NONEXISTENT_WINDOW

#pragma warning( disable: 4191 )
#pragma warning( disable: 4365 )
#pragma warning( disable: 4371 )
#pragma warning( disable: 5204 )
#include <atlbase.h>
#include <atlcom.h>
#include <atlwin.h>
#include <atltypes.h>
#include <atlctl.h>
#include <atlhost.h>
#include <initguid.h> // Realizes CLSID definitions
#include <atlsafe.h>

#ifndef TEXT
#	define TEXT( x ) _T( x )
#endif

namespace CastorCom
{
	class ComPosition;
	using tstring = std::basic_string< TCHAR >;

	static tstring const ERROR_UNINITIALISED_INSTANCE = _T( "The instance must be initialised" );
	static tstring const ERROR_UNSUPPORTED_C3D_TYPE = _T( "The type is unsupported" );

	template< typename T, typename FuncT >
	HRESULT retrieveValue( T * pVal, FuncT p_func )
	{
		HRESULT hr = E_POINTER;

		if ( *pVal )
		{
			*pVal = p_func();
			hr = S_OK;
		}

		return hr;
	}

	template< typename ITypeT >
	struct ComITypeTraitsT
	{
		static constexpr bool hasIType = false;
		using Type = void;
	};

	template< typename ITypeT >
	struct ComITypeTraitsT< ITypeT * >
		: ComITypeTraitsT< ITypeT >
	{
	};

	template< typename ITypeT >
	struct ComITypeTraitsT< ITypeT const * >
		: ComITypeTraitsT< ITypeT >
	{
	};

	template< typename ITypeT >
	struct ComITypeTraitsT< ITypeT & >
		: ComITypeTraitsT< ITypeT >
	{
	};

	template< typename ITypeT >
	struct ComITypeTraitsT< ITypeT const & >
		: ComITypeTraitsT< ITypeT >
	{
	};

	template< typename ITypeT >
	struct ComITypeTraitsT< ITypeT const >
		: ComITypeTraitsT< ITypeT >
	{
	};

	template< typename ITypeT >
	static inline const bool isComITypeV = ComITypeTraitsT< ITypeT >::hasIType;
	template< typename ITypeT >
	concept ComITypeT = isComITypeV< ITypeT >;

	template< ComITypeT ITypeT >
	using ComITypeTypeT = typename ComITypeTraitsT< ITypeT >::Type;

	template< typename TypeT >
	struct ComTypeTraitsT
	{
		static constexpr bool hasIType = false;
		static constexpr bool hasType = false;
		static constexpr bool hasInternalType = false;
		using IType = void;
		using CType = void;
		static inline const CLSID clsid{};
		static inline const CLSID iid{};
		static inline const UINT rid{};
		using Internal = TypeT;
		using InternalMbr = Internal;
		using GetInternal = Internal;
		using SetInternal = Internal;
	};

	template< typename TypeT >
	struct ComTypeTraitsT< TypeT * >
		: ComTypeTraitsT< TypeT >
	{
	};

	template< typename TypeT >
	struct ComTypeTraitsT< TypeT const * >
		: ComTypeTraitsT< TypeT >
	{
	};

	template< typename TypeT >
	struct ComTypeTraitsT< TypeT & >
		: ComTypeTraitsT< TypeT >
	{
	};

	template< typename TypeT >
	struct ComTypeTraitsT< TypeT const & >
		: ComTypeTraitsT< TypeT >
	{
	};

	template< typename TypeT >
	struct ComTypeTraitsT< TypeT const >
		: ComTypeTraitsT< TypeT >
	{
	};

	template< typename TypeT, typename KeyT >
	struct ComTypeTraitsT< castor::ResourceWPtrT< TypeT, KeyT > >
		: ComTypeTraitsT< TypeT >
	{
	};

	template< typename TypeT >
	struct ComTypeTraitsT< castor::UniquePtr< TypeT > >
		: ComTypeTraitsT< TypeT >
	{
	};

	template< typename TypeT >
	struct ComTypeTraitsT< std::shared_ptr< TypeT > >
		: ComTypeTraitsT< TypeT >
	{
	};

	template< typename TypeT >
	struct ComTypeTraitsT< std::unique_ptr< TypeT > >
		: ComTypeTraitsT< TypeT >
	{
	};

	template< typename TypeT >
	static inline const bool hasComTypeV = ComTypeTraitsT< TypeT >::hasType;
	template< typename TypeT >
	static inline const bool hasComInternalTypeV = ComTypeTraitsT< TypeT >::hasInternalType;

	template< typename TypeT >
	concept ComTypeT = hasComTypeV< TypeT >;
	template< typename TypeT >
	concept ComInternalTypeT = hasComInternalTypeV< TypeT >;

	template< ComTypeT TypeT >
	using ComTypeITypeT = typename ComTypeTraitsT< TypeT >::IType;
	template< ComTypeT TypeT >
	using ComTypeCTypeT = typename ComTypeTraitsT< TypeT >::CType;

	template< ComTypeT TypeT >
	static inline const CLSID & ComTypeClsidT = ComTypeTraitsT< TypeT >::clsid;
	template< ComTypeT TypeT >
	static inline const CLSID & ComTypeIidT = ComTypeTraitsT< TypeT >::iid;
	template< ComTypeT TypeT >
	static inline const UINT & ComTypeRidT = ComTypeTraitsT< TypeT >::rid;

	template< typename TypeT >
	using ComTypeInternalT = typename ComTypeTraitsT< TypeT >::Internal;
	template< typename TypeT >
	using ComTypeGetInternalT = typename ComTypeTraitsT< TypeT >::GetInternal;

	template< ComInternalTypeT TypeT >
	using ComTypeInternalMbrT = typename ComTypeTraitsT< TypeT >::InternalMbr;
	template< ComInternalTypeT TypeT >
	using ComTypeSetInternalT = typename ComTypeTraitsT< TypeT >::SetInternal;

	template< ComITypeT ITypeT >
	using ComITypeCTypeT = ComTypeCTypeT< ComITypeTypeT< ITypeT > >;
	template< ComITypeT ITypeT >
	using ComITypeGetDstTypeT = ComTypeITypeT< ComITypeTypeT< ITypeT > >;
}

#include "ComCastor3D/ComCastorUtils.hpp"
#include "ComCastor3D/Castor3D_h.h"
#include "ComCastor3D/Win32/resource.h"

#endif
