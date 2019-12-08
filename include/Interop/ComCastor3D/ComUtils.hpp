/* See LICENSE file in root folder */
#ifndef __C3DCOM_COM_UTILS_H__
#define __C3DCOM_COM_UTILS_H__

#include "ComCastor3D/ComError.hpp"

#include <atlsafe.h>

namespace CastorCom
{
	static tstring const ERROR_WRONG_VARIANT_TYPE = _T( "Wrong type given" );
	static tstring const ERROR_EXPECTED_STRING = _T( "Expected at least a BSTR" );
	static tstring const ERROR_EXPECTED_DISPATCH = _T( "Expected at least a IDispatch" );
	static tstring const ERROR_EXPECTED_VARIANT_ARRAY = _T( "Expected at least a SafeArray of Variant" );
	/**
	 *\~english
	 *\brief		Frees a COM pointer
	 *\param[in]	val	The pointer
	 *\~french
	 *\brief		Lib�re un pointeur COM
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
	 *\brief		Fills a castor::String from a BSTR
	 *\param[in]	val	The BSTR
	 *\return		The castor::String
	 *\~french
	 *\brief		Remplit un castor::String � partir d'un BSTR
	 *\param[in]	val	Le BSTR
	 *\return		Le castor::String
	 */
	inline castor::String fromBstr( BSTR val )
	{
		castor::String l_return;
		int l_length = ::SysStringLen( val );
		int l_size = ::WideCharToMultiByte( CP_UTF8, 0, val, l_length, nullptr, 0, nullptr, nullptr );

		if ( l_size )
		{
			std::vector< char > l_out( l_size, 0 );

			if ( ::WideCharToMultiByte( CP_UTF8, 0, val, l_length, l_out.data(), int( l_out.size() ), nullptr, nullptr ) )
			{
				l_return = castor::string::stringCast< xchar >( std::string( l_out.begin(), l_out.end() ) );
			}
		}

		return l_return;
	}
	/**
	 *\~english
	 *\brief		Fills a BSTR from a castor::String
	 *\param[in]	val	The castor::String
	 *\return		The BSTR
	 *\~french
	 *\brief		Remplit un BSTR � partir d'un castor::String
	 *\param[in]	val	Le castor::String
	 *\return		Le BSTR
	 */
	inline BSTR toBstr( castor::String const & val )
	{
		BSTR l_return = nullptr;
		std::string l_in = castor::string::stringCast< char >( val );
		int l_length = int( l_in.size() );
		int l_size = MultiByteToWideChar( CP_UTF8, 0, &l_in[0], l_length, nullptr, 0 );

		if ( l_size )
		{
			ATL::CComBSTR l_out( l_size );
			MultiByteToWideChar( CP_UTF8, 0, &l_in[0], l_length, &l_out[0], l_size );
			l_return = l_out.Detach();
		}

		return l_return;
	}
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		10/09/2014
	\~english
	\brief		Functor used to fill a VARIANT from a castor::String
	\remarks	Use it if you need a SAFEARRAY of BSTR
	\~french
	\brief		Foncteur utilis� pour remplir un VARIANT � partir d'un castor::String
	\remarks	A utiliser si vous avez besoin d'un SAFEARRAY de BSTR
	*/
	struct BStrValueSetter
	{
		/**
		 *\~english
		 *\brief		Fills a VARIANT from a castor::String
		 *\remarks		The VARIANT type will be VT_BSTR
		 *\param[in]	value	The castor::String
		 *\param[out]	variant	The VARIANT
		 *\return		S_OK
		 *\~french
		 *\brief		Remplit un VARIANT � partir d'un castor::String
		 *\remarks		Le type du VARIANT sera VT_BSTR
		 *\param[in]	value	Le castor::String
		 *\param[out]	variant	Le VARIANT
		 *\return		S_OK
		 */
		HRESULT operator()( const castor::String & value
			, VARIANT & variant )
		{
			variant.vt = VT_BSTR;
			variant.bstrVal = toBstr( value );
			return S_OK;
		}
	};

	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		10/09/2014
	\~english
	\brief		Functor used to fill a castor::String from a VARIANT
	\remarks	Use it if you need a SAFEARRAY of BSTR
	\~french
	\brief		Foncteur utilis� pour remplir un castor::String � partir d'un VARIANT
	\remarks	A utiliser si vous avez besoin d'un SAFEARRAY de BSTR
	*/
	struct StrValueSetter
	{
		/**
		 *\~english
		 *\brief		Fills a castor::String from a VARIANT
		 *\remarks		The VARIANT type must contain VT_BSTR
		 *\param[in]	variant	The VARIANT
		 *\param[out]	value	The castor::String
		 *\return		S_OK if successful
		 *\~french
		 *\brief		Remplit un castor::String � partir d'un VARIANT
		 *\remarks		Le type du VARIANT doit contenir VT_BSTR
		 *\param[in]	variant	Le VARIANT
		 *\param[out]	value	Le castor::String
		 *\return		S_OK si �a amarch�
		 */
		HRESULT operator()( const VARIANT & variant
			, castor::String & value )
		{
			HRESULT hr = ( ( variant.vt | VT_BSTR ) == VT_BSTR ) ? S_OK : E_FAIL;

			if ( hr == S_OK )
			{
				value = fromBstr( variant.bstrVal );
			}
			else
			{
				hr = CComError::dispatchError( E_FAIL	// This represents the error
					, LIBID_Castor3D					// This is the GUID of PixelComponents throwing error
					, ERROR_WRONG_VARIANT_TYPE.c_str()	// This is generally displayed as the title
					, ERROR_EXPECTED_STRING.c_str()		// This is the description
					, 0									// This is the context in the help file
					, nullptr );
			}

			return hr;
		}
	};

	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		10/09/2014
	\~english
	\brief		Structure used to fill a VARIANT
	\arg		ComType		The COM data type
	\arg		CastorType	The Castor data type
	\~french
	\brief		Structure utilis�e pour remplir un VARIANT
	\arg		ComType		Le type COM
	\arg		CastorType	Le type Castor
	*/
	template< typename ComType, typename CastorType >
	struct ValueGetter
	{
		/**
		 *\~english
		 *\brief		Fills a VARIANT with the given value
		 *\param[out]	variant		The VARIANT
		 *\param[in]	value		The value
		 *\param[in]	comsetter	The function used to fill the ComType value from the CastorType value
		 *\~french
		 *\brief		Remplit un VARIANT � partir de la valeur donn�e
		 *\param[out]	variant		Le VARIANT
		 *\param[in]	value		La valeur
		 *\param[in]	comSetter	La fonction utilis�e pour remplir la valeur ComType � partir de la valeur CastorType
		 */
		HRESULT operator()( VARIANT & variant
			, CastorType * value
			, HRESULT( * comsetter )( CastorType *, ComType * ) )
		{
			ComType * pValue = static_cast< ComType * >( ComType::CreateInstance() );
			HRESULT hr = comsetter( value, pValue );

			if ( hr == S_OK )
			{
				VARIANT var;
				var.vt = VT_DISPATCH;
				var.pdispVal = pValue;
				variant = var;
			}

			return hr;
		}
	};

	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		10/09/2014
	\~english
	\brief		Structure used to fill a value from a VARIANT
	\arg		ComType		The COM data type
	\arg		CastorType	The Castor data type
	\~french
	\brief		Structure utilis�e pour remplir une valeur � partir d'un VARIANT
	\arg		ComType		Le type COM
	\arg		CastorType	Le type Castor
	*/
	template< typename ComType, typename CastorType >
	struct ValuePutter
	{
		/**
		 *\~english
		 *\brief		Fills a value with the given VARIANT
		 *\param[in]	variant		The VARIANT
		 *\param[out]	value		The value
		 *\param[in]	comPutter	The function used to fill the CastorType value from the ComType value
		 *\~french
		 *\brief		Remplit une valeur avec le VARIANT donn�
		 *\param[in]	variant		Le VARIANT
		 *\param[out]	value		La valeur
		 *\param[in]	comsetter	La fonction utilis�e pour remplir la valeur CastorType � partir de la valeur ComType
		 */
		HRESULT operator()( const VARIANT & variant
			, CastorType * value
			, HRESULT( * comPutter )( ComType *, CastorType * ) )
		{
			HRESULT hr = ( ( variant.vt & VT_DISPATCH ) == VT_DISPATCH ) ? S_OK : E_FAIL;

			if ( hr == S_OK )
			{
				ComType * pValue = static_cast< ComType * >( variant.pdispVal );
				hr = comPutter( pValue, value );
			}
			else
			{
				hr = CComError::dispatchError( E_FAIL	// This represents the error
					, LIBID_Castor3D					// This is the GUID of PixelComponents throwing error
					, ERROR_WRONG_VARIANT_TYPE.c_str()	// This is generally displayed as the title
					, ERROR_EXPECTED_DISPATCH.c_str()	// This is the description
					, 0									// This is the context in the help file
					, nullptr );
			}

			return hr;
		}
	};

	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		10/09/2014
	\~english
	\brief		Structure used to fill a SAFEARRAY
	\arg		CastorType	The Castor data type
	\~french
	\brief		Structure utilis�e pour remplir un SAFEARRAY
	\arg		CastorType	Le type Castor
	*/
	template< typename CastorType >
	struct SafeArrayGetter
	{
		/**
		 *\~english
		 *\brief		Fills a SAFEARRAY with the values contained in a vector
		 *\arg			setterFunc		The function or functor used to fill the safe array. Must be of the form "HRESULT func( const CastorType &, VARIANT & )"
		 *\param[in]	vals			The vector containing values
		 *\param[out]	pVal			Receives the SAFEARRAY
		 *\param[in]	comValuesetter	The function used to fill a VARIANT value from an CastorType value
		 *\~french
		 *\brief		Remplit un SAFEARRAY avec les valeurs contenues dans un vector
		 *\arg			setterFunc		La fonction ou foncteur utilis� pour remplir le SAFEARRAY. doit �tre de la forme "HRESULT func( const CastorType &, VARIANT & )"
		 *\param[in]	vals			Le vector contenant les valeurs
		 *\param[out]	pVal			Re�oit le SAFEARRAY
		 *\param[in]	comValuesetter	La fonction utilis�e pour remplir un VARIANT � partir d'une valeur CastorType
		 */
		template< typename SetterFunc >
		HRESULT operator()( std::vector< CastorType > const & vals
			, VARIANT * pVal
			, SetterFunc comValueSetter )
		{
			///@remarks We create the SAFEARRAY
			CComSafeArray< VARIANT > array;
			HRESULT hr = array.create( ULONG( vals.size() ) );

			///@remarks And we fill it
			if ( hr == S_OK )
			{
				for ( int i = 0; i < int( vals.size() ) && hr == S_OK; ++i )
				{
					VARIANT value;
					::VariantInit( &value );
					hr = comValuesetter( vals[i], value );

					if ( hr == S_OK )
					{
						array[i] = value;
					}
				}

				if ( hr == S_OK )
				{
					CComVariant vArray( array.detach() );
					hr = vArray.detach( pVal );
				}
			}

			return hr;
		}
	};

	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		10/09/2014
	\~english
	\brief		Structure used to retrieve a value from a SAFEARRAY
	\arg		CallerType	The caller class type
	\arg		CastorType	The Castor data type
	\~french
	\brief		Structure utilis�e pour r�cup�rer des valeurs � partir d'un SAFEARRAY
	\arg		CallerType	Le type de l'appelant
	\arg		CastorType	Le type Castor
	*/
	template< typename CastorType, typename CallerType >
	struct SafeArrayPutter
	{
		/**
		 *\~english
		 *\brief		Fills a vector with the values contained in a SAFEARRAY
		 *\arg			PutterFunc	The function or functor used to fill the vector. Must be of the form "HRESULT func( const VARIANT &, CastorType & )"
		 *\arg			setterFunc	The function to set the value in the Castor object. Must be of the form "void func( const std::vector< CastorType > & )"
		 *\param[in]	val			The SAFEARRAY containing values
		 *\param[in]	valuePutter	The function used to fill an CastorType value from a VARIANT value
		 *\param[in]	caller		The caller object (since this functor is usually called from a member function of that class)
		 *\param[in]	valuesetter	The member function used to set the array of CastorType values
		 *\~french
		 *\brief		Remplit un vector avec les valeurs contenues dans un SAFEARRAY
		 *\arg			PutterFunc	La fonction ou foncteur utilis�e pour remplir le vector. doit �tre de la forme "HRESULT func( const VARIANT &, CastorType & )"
		 *\arg			setterFunc	La fonction utilis�e pour d�finir la valeur de l'objet Castor. doit �tre de la forme "void func( const std::vector< CastorType > & )"
		 *\param[in]	val			Le SAFEARRAY contenant les valeurs
		 *\param[in]	valuePutter	La fonction utilis�e pour remplir une valeur CastorType � partir d'un VARIANT
		 *\param[in]	caller		L'appelant (car ce foncteur est g�n�ralement appel� depuis une fonction membre de cette classe)
		 *\param[in]	valuesetter	La fonction membre utilis�e pour d�finir le vector de valeurs CastorType
		 */
		template< typename PutterFunc, typename SetterFunc >
		HRESULT operator()( VARIANT val
			, PutterFunc valuePutter
			, CallerType * caller
			, SetterFunc valueSetter )
		{
			uint32_t type = VT_ARRAY | VT_VARIANT;
			HRESULT hr = ( ( val.vt & type ) == type ) ? S_OK : E_FAIL;

			if ( hr == S_OK )
			{
				ATL::CComSafeArray< VARIANT > array;

				if ( ( val.vt & VT_BYREF ) == VT_BYREF )
				{
					array = *val.pparray;
				}
				else
				{
					array = *val.parray;
				}

				const ULONG nCount = array.getCount();
				std::vector< CastorType > vals( nCount, CastorType() );

				for ( ULONG i = 0; i < nCount && hr == S_OK; ++i )
				{
					hr = valuePutter( array[int( i )], vals[i] );
				}

				if ( hr == S_OK )
				{
					valuesetter( vals );
				}
			}
			else
			{
				hr = CComError::dispatchError( E_FAIL		// This represents the error
					, LIBID_Castor3D						// This is the GUID of PixelComponents throwing error
					, ERROR_WRONG_VARIANT_TYPE.c_str()		// This is generally displayed as the title
					, ERROR_EXPECTED_VARIANT_ARRAY.c_str()	// This is the description
					, 0										// This is the context in the help file
					, nullptr );
			}

			return hr;
		}
		/**
		 *\~english
		 *\brief		Fills a vector with the values contained in a SAFEARRAY
		 *\arg			PutterFunc	The function or functor used to fill the safe array. Must be of the form "HRESULT func( const VARIANT &, CastorType & )"
		 *\param[in]	val			The SAFEARRAY containing values
		 *\param[in]	valuePutter	The function used to fill an CastorType value from a VARIANT value
		 *\param[in]	caller		The caller class (since this functor is usually called from a member function of that class)
		 *\param[in]	valuesetter	The member function used to set the array of CastorType values
		 *\~french
		 *\brief		Remplit un vector avec les valeurs contenues dans un SAFEARRAY
		 *\arg			PutterFunc	La fonction ou foncteur utilis�e pour remplir le vector. doit �tre de la forme "HRESULT func( const VARIANT &, CastorType & )"
		 *\param[in]	val			Le SAFEARRAY contenant les valeurs
		 *\param[in]	valuePutter	La fonction utilis�e pour remplir une valeur CastorType � partir d'un VARIANT
		 *\param[in]	caller		L'appelant (car ce foncteur est g�n�ralement appel� depuis une fonction membre de cette classe)
		 *\param[in]	valuesetter	La fonction membre utilis�e pour d�finir le vector de valeurs CastorType
		 */
		template< typename PutterFunc >
		HRESULT operator()( VARIANT val
			, PutterFunc valuePutter
			, CallerType * caller
			, void ( __thiscall CallerType::* valuesetter )( const std::vector< CastorType > & ) )
		{
			uint32_t type = VT_ARRAY | VT_VARIANT;
			HRESULT hr = ( ( val.vt & type ) == type ) ? S_OK : E_FAIL;

			if ( hr == S_OK )
			{
				ATL::CComSafeArray< VARIANT > array;

				if ( ( val.vt & VT_BYREF ) == VT_BYREF )
				{
					array = *val.pparray;
				}
				else
				{
					array = *val.parray;
				}

				const ULONG nCount = array.getCount();
				std::vector< CastorType > vals( nCount, CastorType() );

				for ( ULONG i = 0; i < nCount && hr == S_OK; ++i )
				{
					hr = valuePutter( array[int( i )], vals[i] );
				}

				if ( hr == S_OK )
				{
					( caller->*valuesetter )( vals );
				}
			}
			else
			{
				hr = CComError::dispatchError( E_FAIL		// This represents the error
					, LIBID_Castor3D						// This is the GUID of PixelComponents throwing error
					, ERROR_WRONG_VARIANT_TYPE.c_str()		// This is generally displayed as the title
					, ERROR_EXPECTED_VARIANT_ARRAY.c_str()	// This is the description
					, 0										// This is the context in the help file
					, nullptr );
			}

			return hr;
		}
	};
}

#endif
