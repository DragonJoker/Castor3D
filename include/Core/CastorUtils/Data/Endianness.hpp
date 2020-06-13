/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_ENDIANNESS_H___
#define ___CASTOR_ENDIANNESS_H___

#include "CastorUtils/Data/DataModule.hpp"

namespace castor
{
	namespace details
	{
		static union
		{
			uint32_t i;
			char c[4];
		} BigInt = { 0x01020304 };
	}
	/**
	 *\~english
	 *\brief		Detects if the current system is big endian.
	 *\~french
	 *\brief		Détecte si le système courant est big endian.
	 */
	inline bool isBigEndian()noexcept
	{
		return details::BigInt.c[0] == 1;
	}
	/**
	 *\~english
	 *\brief			Convert from little or big endian to the other.
	 *\param[in,out]	value	Value to be converted.
	 *\~french
	 *\brief			Change le boutisme du paramètre
	 *\param[in,out]	value	La valeur à convertir.
	 */
	template< typename T >
	inline T & switchEndianness( T & value )noexcept
	{
		uint8_t * p = reinterpret_cast< uint8_t * >( &value );
		size_t lo, hi;

		for ( lo = 0, hi = sizeof( T ) - 1; hi > lo; lo++, hi-- )
		{
			std::swap( p[lo], p[hi] );
		}

		return value;
	}
	/**
	 *\~english
	 *\brief			Convert from little or big endian to the other.
	 *\param[in,out]	value	Value to be converted.
	 *\~french
	 *\brief			Change le boutisme du paramètre
	 *\param[in,out]	value	La valeur à convertir.
	 */
	template< typename T >
	inline T switchEndianness( T const & value )
	{
		T result{ value };
		switchEndianness( result );
		return result;
	}
	/**
	 *\~english
	 *\brief			Convert the given value to big endian if needed.
	 *\param[in,out]	value	Value to be converted.
	 *\~french
	 *\brief			Convertit la valeur donnée en big endian si nécessaire.
	 *\param[in,out]	value	La valeur à convertir.
	 */
	template< typename T >
	inline T & systemEndianToBigEndian( T & value )noexcept
	{
		if ( !isBigEndian() )
		{
			switchEndianness( value );
		}

		return value;
	}
	/**
	 *\~english
	 *\brief			Convert the given value to big endian if needed.
	 *\param[in,out]	value	Value to be converted.
	 *\~french
	 *\brief			Convertit la valeur donnée en big endian si nécessaire.
	 *\param[in,out]	value	La valeur à convertir.
	 */
	template< typename T >
	inline T systemEndianToBigEndian( T const & value )
	{
		T result{ value };
		switchEndianness( result );
		return result;
	}
	/**
	 *\~english
	 *\brief			Convert the given value to big endian if needed.
	 *\param[in,out]	value	Value to be converted.
	 *\~french
	 *\brief			Convertit la valeur donnée en big endian si nécessaire.
	 *\param[in,out]	value	La valeur à convertir.
	 */
	template< typename T, size_t N >
	inline std::array< T, N > & systemEndianToBigEndian( std::array< T, N > & value )noexcept
	{
		if ( !isBigEndian() )
		{
			for ( auto & element : value )
			{
				switchEndianness( element );
			}
		}

		return value;
	}
	/**
	 *\~english
	 *\brief			Convert the given value to big endian if needed.
	 *\param[in,out]	value	Value to be converted.
	 *\~french
	 *\brief			Convertit la valeur donnée en big endian si nécessaire.
	 *\param[in,out]	value	La valeur à convertir.
	 */
	template< typename T, size_t N >
	inline std::array< T, N > systemEndianToBigEndian( std::array< T, N > const & value )
	{
		std::array< T, N > result{ value };

		if ( !isBigEndian() )
		{
			for ( auto & element : result )
			{
				switchEndianness( element );
			}
		}

		return result;
	}
	/**
	 *\~english
	 *\brief			Convert the given value to big endian if needed.
	 *\param[in,out]	value	Value to be converted.
	 *\~french
	 *\brief			Convertit la valeur donnée en big endian si nécessaire.
	 *\param[in,out]	value	La valeur à convertir.
	 */
	template< typename T >
	inline std::vector< T > & systemEndianToBigEndian( std::vector< T > & value )noexcept
	{
		if ( !isBigEndian() )
		{
			for ( auto & element : value )
			{
				switchEndianness( element );
			}
		}

		return value;
	}
	/**
	 *\~english
	 *\brief			Convert the given value to big endian if needed.
	 *\param[in,out]	value	Value to be converted.
	 *\~french
	 *\brief			Convertit la valeur donnée en big endian si nécessaire.
	 *\param[in,out]	value	La valeur à convertir.
	 */
	template< typename T >
	inline std::vector< T > systemEndianToBigEndian( std::vector< T > const & value )
	{
		std::vector< T > result{ value };

		if ( !isBigEndian() )
		{
			for ( auto & element : result )
			{
				switchEndianness( element );
			}
		}

		return result;
	}
	/**
	 *\~english
	 *\brief			Convert the given big endian value to system endian if needed.
	 *\param[in,out]	value	Value to be converted.
	 *\~french
	 *\brief			Convertit la valeur donnée de big endian à l'endianness du système si nécessaire.
	 *\param[in,out]	value	La valeur à convertir.
	 */
	template< typename T >
	inline T & bigEndianToSystemEndian( T & value )noexcept
	{
		if ( !isBigEndian() )
		{
			switchEndianness( value );
		}

		return value;
	}
	/**
	 *\~english
	 *\brief			Convert the given big endian value to system endian if needed.
	 *\param[in,out]	value	Value to be converted.
	 *\~french
	 *\brief			Convertit la valeur donnée de big endian à l'endianness du système si nécessaire.
	 *\param[in,out]	value	La valeur à convertir.
	 */
	template< typename T >
	inline T bigEndianToSystemEndian( T const & value )
	{
		T result{ value };
		switchEndianness( result );
		return result;
	}
	/**
	 *\~english
	 *\brief			Convert the given value to big endian if needed.
	 *\param[in,out]	value	Value to be converted.
	 *\~french
	 *\brief			Convertit la valeur donnée en big endian si nécessaire.
	 *\param[in,out]	value	La valeur à convertir.
	 */
	template< typename T, size_t N >
	inline std::array< T, N > & bigEndianToSystemEndian( std::array< T, N > & value )noexcept
	{
		if ( !isBigEndian() )
		{
			for ( auto & element : value )
			{
				switchEndianness( element );
			}
		}

		return value;
	}
	/**
	 *\~english
	 *\brief			Convert the given value to big endian if needed.
	 *\param[in,out]	value	Value to be converted.
	 *\~french
	 *\brief			Convertit la valeur donnée en big endian si nécessaire.
	 *\param[in,out]	value	La valeur à convertir.
	 */
	template< typename T, size_t N >
	inline std::array< T, N > bigEndianToSystemEndian( std::array< T, N > const & value )
	{
		std::array< T, N > result{ value };

		if ( !isBigEndian() )
		{
			for ( auto & element : result )
			{
				switchEndianness( element );
			}
		}

		return result;
	}
	/**
	 *\~english
	 *\brief			Convert the given value to big endian if needed.
	 *\param[in,out]	value	Value to be converted.
	 *\~french
	 *\brief			Convertit la valeur donnée en big endian si nécessaire.
	 *\param[in,out]	value	La valeur à convertir.
	 */
	template< typename T >
	inline std::vector< T > & bigEndianToSystemEndian( std::vector< T > & value )noexcept
	{
		if ( !isBigEndian() )
		{
			for ( auto & element : value )
			{
				switchEndianness( element );
			}
		}

		return value;
	}
	/**
	 *\~english
	 *\brief			Convert the given value to big endian if needed.
	 *\param[in,out]	value	Value to be converted.
	 *\~french
	 *\brief			Convertit la valeur donnée en big endian si nécessaire.
	 *\param[in,out]	value	La valeur à convertir.
	 */
	template< typename T >
	inline std::vector< T > bigEndianToSystemEndian( std::vector< T > const & value )
	{
		std::vector< T > result{ value };

		if ( !isBigEndian() )
		{
			for ( auto & element : result )
			{
				switchEndianness( element );
			}
		}

		return result;
	}
}

#endif
