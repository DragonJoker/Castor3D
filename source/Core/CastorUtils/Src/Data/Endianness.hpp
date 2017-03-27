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

namespace Castor
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
	inline bool IsBigEndian()noexcept
	{
		return details::BigInt.c[0] == 1;
	}
	/**
	 *\~english
	 *\brief			Convert from little or big endian to the other.
	 *\param[in,out]	p_value	Value to be converted.
	 *\~french
	 *\brief			Change le boutisme du paramètre
	 *\param[in,out]	p_value	La valeur à convertir.
	 */
	template< typename T >
	inline T & SwitchEndianness( T & p_value )noexcept
	{
		uint8_t * p = reinterpret_cast< uint8_t * >( &p_value );
		size_t lo, hi;

		for ( lo = 0, hi = sizeof( T ) - 1; hi > lo; lo++, hi-- )
		{
			std::swap( p[lo], p[hi] );
		}

		return p_value;
	}
	/**
	 *\~english
	 *\brief			Convert from little or big endian to the other.
	 *\param[in,out]	p_value	Value to be converted.
	 *\~french
	 *\brief			Change le boutisme du paramètre
	 *\param[in,out]	p_value	La valeur à convertir.
	 */
	template< typename T >
	inline T SwitchEndianness( T const & p_value )
	{
		T l_value{ p_value };
		SwitchEndianness( l_value );
		return l_value;
	}
	/**
	 *\~english
	 *\brief			Convert the given value to big endian if needed.
	 *\param[in,out]	p_value	Value to be converted.
	 *\~french
	 *\brief			Convertit la valeur donnée en big endian si nécessaire.
	 *\param[in,out]	p_value	La valeur à convertir.
	 */
	template< typename T >
	inline T & SystemEndianToBigEndian( T & p_value )noexcept
	{
		if ( !IsBigEndian() )
		{
			SwitchEndianness( p_value );
		}

		return p_value;
	}
	/**
	 *\~english
	 *\brief			Convert the given value to big endian if needed.
	 *\param[in,out]	p_value	Value to be converted.
	 *\~french
	 *\brief			Convertit la valeur donnée en big endian si nécessaire.
	 *\param[in,out]	p_value	La valeur à convertir.
	 */
	template< typename T >
	inline T SystemEndianToBigEndian( T const & p_value )
	{
		T l_value{ p_value };
		SwitchEndianness( l_value );
		return l_value;
	}
	/**
	 *\~english
	 *\brief			Convert the given value to big endian if needed.
	 *\param[in,out]	p_value	Value to be converted.
	 *\~french
	 *\brief			Convertit la valeur donnée en big endian si nécessaire.
	 *\param[in,out]	p_value	La valeur à convertir.
	 */
	template< typename T, size_t N >
	inline std::array< T, N > & SystemEndianToBigEndian( std::array< T, N > & p_value )noexcept
	{
		if ( !IsBigEndian() )
		{
			for ( auto & l_value : p_value )
			{
				SwitchEndianness( l_value );
			}
		}

		return p_value;
	}
	/**
	 *\~english
	 *\brief			Convert the given value to big endian if needed.
	 *\param[in,out]	p_value	Value to be converted.
	 *\~french
	 *\brief			Convertit la valeur donnée en big endian si nécessaire.
	 *\param[in,out]	p_value	La valeur à convertir.
	 */
	template< typename T, size_t N >
	inline std::array< T, N > SystemEndianToBigEndian( std::array< T, N > const & p_value )
	{
		std::array< T, N > l_value{ p_value };

		if ( !IsBigEndian() )
		{
			for ( auto & l_element : l_value )
			{
				SwitchEndianness( l_element );
			}
		}

		return l_value;
	}
	/**
	 *\~english
	 *\brief			Convert the given value to big endian if needed.
	 *\param[in,out]	p_value	Value to be converted.
	 *\~french
	 *\brief			Convertit la valeur donnée en big endian si nécessaire.
	 *\param[in,out]	p_value	La valeur à convertir.
	 */
	template< typename T >
	inline std::vector< T > & SystemEndianToBigEndian( std::vector< T > & p_value )noexcept
	{
		if ( !IsBigEndian() )
		{
			for ( auto & l_value : p_value )
			{
				SwitchEndianness( l_value );
			}
		}

		return p_value;
	}
	/**
	 *\~english
	 *\brief			Convert the given value to big endian if needed.
	 *\param[in,out]	p_value	Value to be converted.
	 *\~french
	 *\brief			Convertit la valeur donnée en big endian si nécessaire.
	 *\param[in,out]	p_value	La valeur à convertir.
	 */
	template< typename T >
	inline std::vector< T > SystemEndianToBigEndian( std::vector< T > const & p_value )
	{
		std::vector< T > l_value{ p_value };

		if ( !IsBigEndian() )
		{
			for ( auto & l_element : l_value )
			{
				SwitchEndianness( l_element );
			}
		}

		return l_value;
	}
	/**
	 *\~english
	 *\brief			Convert the given big endian value to system endian if needed.
	 *\param[in,out]	p_value	Value to be converted.
	 *\~french
	 *\brief			Convertit la valeur donnée de big endian à l'endianness du système si nécessaire.
	 *\param[in,out]	p_value	La valeur à convertir.
	 */
	template< typename T >
	inline T & BigEndianToSystemEndian( T & p_value )noexcept
	{
		if ( !IsBigEndian() )
		{
			SwitchEndianness( p_value );
		}

		return p_value;
	}
	/**
	 *\~english
	 *\brief			Convert the given big endian value to system endian if needed.
	 *\param[in,out]	p_value	Value to be converted.
	 *\~french
	 *\brief			Convertit la valeur donnée de big endian à l'endianness du système si nécessaire.
	 *\param[in,out]	p_value	La valeur à convertir.
	 */
	template< typename T >
	inline T BigEndianToSystemEndian( T const & p_value )
	{
		T l_value{ p_value };
		SwitchEndianness( p_value );
		return p_value;
	}
	/**
	 *\~english
	 *\brief			Convert the given value to big endian if needed.
	 *\param[in,out]	p_value	Value to be converted.
	 *\~french
	 *\brief			Convertit la valeur donnée en big endian si nécessaire.
	 *\param[in,out]	p_value	La valeur à convertir.
	 */
	template< typename T, size_t N >
	inline std::array< T, N > & BigEndianToSystemEndian( std::array< T, N > & p_value )noexcept
	{
		if ( !IsBigEndian() )
		{
			for ( auto & l_value : p_value )
			{
				SwitchEndianness( l_value );
			}
		}

		return p_value;
	}
	/**
	 *\~english
	 *\brief			Convert the given value to big endian if needed.
	 *\param[in,out]	p_value	Value to be converted.
	 *\~french
	 *\brief			Convertit la valeur donnée en big endian si nécessaire.
	 *\param[in,out]	p_value	La valeur à convertir.
	 */
	template< typename T, size_t N >
	inline std::array< T, N > BigEndianToSystemEndian( std::array< T, N > const & p_value )
	{
		std::array< T, N > l_value{ p_value };

		if ( !IsBigEndian() )
		{
			for ( auto & l_element : l_value )
			{
				SwitchEndianness( l_element );
			}
		}

		return l_value;
	}
	/**
	 *\~english
	 *\brief			Convert the given value to big endian if needed.
	 *\param[in,out]	p_value	Value to be converted.
	 *\~french
	 *\brief			Convertit la valeur donnée en big endian si nécessaire.
	 *\param[in,out]	p_value	La valeur à convertir.
	 */
	template< typename T >
	inline std::vector< T > & BigEndianToSystemEndian( std::vector< T > & p_value )noexcept
	{
		if ( !IsBigEndian() )
		{
			for ( auto & l_value : p_value )
			{
				SwitchEndianness( l_value );
			}
		}

		return p_value;
	}
	/**
	 *\~english
	 *\brief			Convert the given value to big endian if needed.
	 *\param[in,out]	p_value	Value to be converted.
	 *\~french
	 *\brief			Convertit la valeur donnée en big endian si nécessaire.
	 *\param[in,out]	p_value	La valeur à convertir.
	 */
	template< typename T >
	inline std::vector< T > BigEndianToSystemEndian( std::vector< T > const & p_value )
	{
		std::vector< T > l_value{ p_value };

		if ( !IsBigEndian() )
		{
			for ( auto & l_element : l_value )
			{
				SwitchEndianness( l_element );
			}
		}

		return l_value;
	}
}

#endif
