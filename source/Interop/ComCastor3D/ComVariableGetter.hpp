/* See LICENSE file in root folder */
#ifndef ___C3DCOM_COM_VARIABLE_GETTER_H___
#define ___C3DCOM_COM_VARIABLE_GETTER_H___

#include "ComCastor3D/ComParameterCast.hpp"

namespace CastorCom
{
	template< typename InternalPtrT, typename DstTypeT, typename SrcTypeT, typename FunctorT, ComInternalTypeT InternalT >
	HRESULT getValue( LPCTSTR name, FunctorT func, SrcTypeT * pRet, InternalT const & internal )
	{
		//
		if ( !pRet )
			return E_POINTER;
		if constexpr ( std::is_same_v< InternalPtrT, InternalT > )
			if ( !internal )
				return CComError::dispatchError( E_POINTER, ComTypeIidT< InternalT >, name, _T( "The object must be initialised" ), 0, nullptr );

		//
		DstTypeT res{};
		auto hr = convert( func( getPtr( internal ), &res ) );
		*pRet = details::parameterCast< SrcTypeT >( res );
		return hr;
	}

	template< typename InternalPtrT, typename DstTypeT, typename FunctorT, ComInternalTypeT InternalT >
	HRESULT getValue( LPCTSTR name, FunctorT func, BSTR * pRet, InternalT const & internal )
	{
		static_assert( std::is_same_v< DstTypeT, C3DString > );
		//
		if ( !pRet )
			return E_POINTER;
		if constexpr ( std::is_same_v< InternalPtrT, InternalT > )
			if ( !internal )
				return CComError::dispatchError( E_POINTER, ComTypeIidT< InternalT >, name, _T( "The object must be initialised" ), 0, nullptr );

		//
		C3DString res{};
		auto hr = convert( func( getPtr( internal ), &res ) );
		*pRet = stringToBstr( res );
		free( ( void * )res );
		return hr;
	}

	template< typename InternalPtrT, ComITypeT ITypeT, typename FunctorT, ComInternalTypeT InternalT >
	HRESULT getValueSimplePtr( LPCTSTR name, FunctorT func, ITypeT ** pRet, InternalT const & internal )
	{
		//
		if constexpr ( std::is_same_v< InternalPtrT, InternalT > )
			if ( !internal )
				return CComError::dispatchError( E_FAIL, ComTypeIidT< InternalT >, name, _T( "The object must be initialised" ), 0, nullptr );
		if ( !pRet )
			return E_POINTER;

		//
		using CType = ComITypeCTypeT< ITypeT >;
		if ( CType::CreateInstance( pRet ) != S_OK )
			return E_FAIL;

		//
		return convert( func( getPtr( internal )
			, getPtr( static_cast< CType * >( *pRet )->getInternal() ) ) );
	}

	template< typename InternalPtrT, ComITypeT ITypeT, typename FunctorT, ComInternalTypeT InternalT >
	HRESULT getValueManagedPtr( LPCTSTR name, FunctorT func, ITypeT ** pRet, InternalT const & internal )
	{
		//
		if constexpr ( std::is_same_v< InternalPtrT, InternalT > )
			if ( !internal )
				return CComError::dispatchError( E_FAIL, ComTypeIidT< InternalT >, name, _T( "The object must be initialised" ), 0, nullptr );
		if ( !pRet )
			return E_POINTER;

		//
		using CType = ComITypeCTypeT< ITypeT >;
		if ( CType::CreateInstance( pRet ) != S_OK )
			return E_FAIL;

		//
		return convert( func( getPtr( internal )
			, &static_cast< CType * >( *pRet )->getInternal() ) );
	}
}

#endif
