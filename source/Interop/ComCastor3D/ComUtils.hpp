/* See LICENSE file in root folder */
#ifndef __C3DCOM_COM_UTILS_H__
#define __C3DCOM_COM_UTILS_H__

#include "ComCastor3D/ComError.hpp"

#include <vector>

namespace CastorCom
{
	/**
	 *\~english
	 *\brief		Frees a COM pointer
	 *\param[in]	val	The pointer
	 *\~french
	 *\brief		Libère un pointeur COM
	 *\param[in]	val	Le pointeur
	 */
	template< typename T > void safeRelease( T *& ptr )
	{
		if ( ptr )
		{
			ptr->Release();
			ptr = nullptr;
		}
	}
	/**
	 *\~english
	 *\brief		Fills a std::string from a BSTR
	 *\param[in]	val	The BSTR
	 *\return		The std::string
	 *\~french
	 *\brief		Remplit un std::string à partir d'un BSTR
	 *\param[in]	val	Le BSTR
	 *\return		Le std::string
	 */
	inline C3DString bstrToC3DString( BSTR val )
	{
		C3DString result = nullptr;

		if ( int length = ::SysStringLen( val );
			int size = ::WideCharToMultiByte( CP_UTF8, 0, val, length, nullptr, 0, nullptr, nullptr ) )
		{
			std::vector< char > out( size, 0 );

			if ( ::WideCharToMultiByte( CP_UTF8, 0, val, length, out.data(), int( out.size() ), nullptr, nullptr ) )
			{
				auto tmp = ( char * )malloc( out.size() + 1U );
				if ( tmp )
					strncpy_s( tmp, out.size() + 1U, out.data(), out.size() );
				result = tmp;
			}
		}

		return result;
	}
	/**
	 *\~english
	 *\brief		Fills a std::string from a BSTR
	 *\param[in]	val	The BSTR
	 *\return		The std::string
	 *\~french
	 *\brief		Remplit un std::string à partir d'un BSTR
	 *\param[in]	val	Le BSTR
	 *\return		Le std::string
	 */
	inline std::string bstrToString( BSTR val )
	{
		std::string result;

		if ( int length = ::SysStringLen( val );
			int size = ::WideCharToMultiByte( CP_UTF8, 0, val, length, nullptr, 0, nullptr, nullptr ) )
		{
			std::vector< char > out( size, 0 );

			if ( ::WideCharToMultiByte( CP_UTF8, 0, val, length, out.data(), int( out.size() ), nullptr, nullptr ) )
			{
				result = std::string( out.begin(), out.end() );
			}
		}

		return result;
	}
	/**
	 *\~english
	 *\brief		Fills a BSTR from a std::string
	 *\param[in]	val	The std::string
	 *\return		The BSTR
	 *\~french
	 *\brief		Remplit un BSTR à partir d'un std::string
	 *\param[in]	val	Le std::string
	 *\return		Le BSTR
	 */
	inline BSTR stringToBstr( std::string const & val )
	{
		BSTR result = nullptr;

		if ( int length = int( val.size() );
			int size = MultiByteToWideChar( CP_UTF8, 0, &val[0], length, nullptr, 0 ) )
		{
			ATL::CComBSTR out( size );
			MultiByteToWideChar( CP_UTF8, 0, &val[0], length, &out[0], size );
			result = out.Detach();
		}

		return result;
	}
	std::u32string stringToU32String( std::string const & val );
	std::string u32StringToString( std::u32string const & val );
	/**
	 *\~english
	 *\brief		Fills a std::string from a BSTR
	 *\param[in]	val	The BSTR
	 *\return		The std::string
	 *\~french
	 *\brief		Remplit un std::string à partir d'un BSTR
	 *\param[in]	val	Le BSTR
	 *\return		Le std::string
	 */
	inline std::u32string bstrToU32String( BSTR val )
	{
		return stringToU32String( bstrToString( val ) );
	}
	/**
	 *\~english
	 *\brief		Fills a BSTR from a std::string
	 *\param[in]	val	The std::string
	 *\return		The BSTR
	 *\~french
	 *\brief		Remplit un BSTR à partir d'un std::string
	 *\param[in]	val	Le std::string
	 *\return		Le BSTR
	 */
	inline BSTR u32StringToBstr( std::u32string const & val )
	{
		return stringToBstr( u32StringToString( val ) );
	}
	/**
	 *\~english
	 *\brief		Retrieves a pointer from a value.
	 *\param[in]	val	The value.
	 *\return		The pointer.
	 *\~french
	 *\brief		Récupère un pointeur depuis une valeur.
	 *\param[in]	val	La valeur.
	 *\return		Le pointeur.
	 */
	template< typename Type >
	Type * getPtr( Type & val )
	{
		return &val;
	}
	/**
	 *\~english
	 *\brief		Retrieves a pointer from a value.
	 *\param[in]	val	The value.
	 *\return		The pointer.
	 *\~french
	 *\brief		Récupère un pointeur depuis une valeur.
	 *\param[in]	val	La valeur.
	 *\return		Le pointeur.
	 */
	template< typename Type >
	Type * getPtr( Type * val )
	{
		return val;
	}

	template< typename FunctorT, ComInternalTypeT InternalT >
	HRESULT destroyInternal( FunctorT func, InternalT *& internal )
	{
		//
		if ( !internal )
			return CComError::dispatchError( E_FAIL, ComTypeIidT< InternalT >, _T( "Destroy" ), _T( "The object must be initialised" ), 0, nullptr );

		//
		auto res = convert( func( internal ) );
		internal = nullptr;
		return res;
	}
}

#endif
