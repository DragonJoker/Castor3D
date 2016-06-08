/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___CASTOR_ENDIANNESS_H___
#define ___CASTOR_ENDIANNESS_H___

#include "CastorUtilsPrerequisites.hpp"

namespace Castor
{
	namespace
	{
		union
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
		return BigInt.c[0] == 1;
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
