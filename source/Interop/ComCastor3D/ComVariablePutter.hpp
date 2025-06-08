/* See LICENSE file in root folder */
#ifndef ___C3DCOM_COM_VARIABLE_PUTTER_H___
#define ___C3DCOM_COM_VARIABLE_PUTTER_H___

#include "ComCastor3D/ComParameterCast.hpp"

namespace CastorCom
{
	template< typename InternalPtrT, typename DstTypeT, typename SrcTypeT, typename FunctorT, ComInternalTypeT InternalT >
	HRESULT putValue( LPCTSTR name, FunctorT func, SrcTypeT const & val, InternalT & internal )
	{
		//
		if constexpr ( std::is_same_v< InternalPtrT, InternalT > )
			if ( !internal )
				return CComError::dispatchError( E_POINTER, ComTypeIidT< InternalT >, name, _T( "The object must be initialised" ), 0, nullptr );

		//
		return convert( func( getPtr( internal ), details::parameterCast< DstTypeT >( val ) ) );
	}

	template< typename InternalPtrT, typename DstTypeT, typename FunctorT, ComInternalTypeT InternalT >
	HRESULT putValue( LPCTSTR name, FunctorT func, BSTR const & val, InternalT & internal )
	{
		static_assert( std::is_same_v< DstTypeT, C3DString > );
		//
		if constexpr ( std::is_same_v< InternalPtrT, InternalT > )
			if ( !internal )
				return CComError::dispatchError( E_POINTER, ComTypeIidT< InternalT >, name, _T( "The object must be initialised" ), 0, nullptr );

		//
		std::string res = bstrToString( val );
		return convert( func( getPtr( internal ), res.c_str() ) );
	}

	template< typename InternalPtrT, ComITypeT ITypeT, typename FunctorT, ComInternalTypeT InternalT >
	HRESULT putValueSimplePtr( LPCTSTR name, FunctorT func, ITypeT * val, InternalT & internal )
	{
		//
		if constexpr ( std::is_same_v< InternalPtrT, InternalT > )
			if ( !internal )
				return CComError::dispatchError( E_POINTER, ComTypeIidT< InternalT >, name, _T( "The object must be initialised" ), 0, nullptr );
		if ( !val )
			return E_POINTER;

		//
		using CType = ComITypeCTypeT< ITypeT >;
		return convert( func( getPtr( internal ), getPtr( static_cast< CType * >( val )->getInternal() ) ) );
	}

	template< typename InternalPtrT, ComITypeT ITypeT, typename FunctorT, ComInternalTypeT InternalT >
	HRESULT putValueManagedPtr( LPCTSTR name, FunctorT func, ITypeT * val, InternalT & internal )
	{
		//
		if constexpr ( std::is_same_v< InternalPtrT, InternalT > )
			if ( !internal )
				return CComError::dispatchError( E_POINTER, ComTypeIidT< InternalT >, name, _T( "The object must be initialised" ), 0, nullptr );
		if ( !val )
			return E_POINTER;

		//
		using CType = ComITypeCTypeT< ITypeT >;
		return convert( func( getPtr( internal ), getPtr( static_cast< CType * >( val )->getInternal() ) ) );
	}
}

#endif
