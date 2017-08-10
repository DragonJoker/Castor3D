/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___CASTOR_ENDIANNESS_H___
#define ___CASTOR_ENDIANNESS_H___

#include "CastorUtilsPrerequisites.hpp"

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
