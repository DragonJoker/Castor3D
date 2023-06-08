/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_ENDIANNESS_H___
#define ___CASTOR_ENDIANNESS_H___

#include "CastorUtils/Data/DataModule.hpp"

#include <bit>

namespace castor
{
	/**
	 *\~english
	 *\brief		Detects if the current system is big endian.
	 *\~french
	 *\brief		Détecte si le système courant est big endian.
	 */
	inline constexpr bool isBigEndian()noexcept
	{
		return std::endian::native == std::endian::big;
	}
	/**
	 *\~english
	 *\brief		Detects if the current system is little endian.
	 *\~french
	 *\brief		Détecte si le système courant est little endian.
	 */
	inline constexpr bool isLittleEndian()noexcept
	{
		return std::endian::native == std::endian::little;
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
	inline constexpr T & switchEndianness( T & value )noexcept
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
	inline constexpr T switchEndianness( T const & value )
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
	inline constexpr T & systemEndianToBigEndian( T & value )noexcept
	{
		if constexpr ( !isBigEndian() )
		{
			return switchEndianness( value );
		}
		else
		{
			return value;
		}
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
	inline constexpr T systemEndianToBigEndian( T const & value )
	{
		if constexpr ( !isBigEndian() )
		{
			T result{ value };
			switchEndianness( result );
			return result;
		}
		else
		{
			return value;
		}
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
	inline constexpr std::array< T, N > & systemEndianToBigEndian( std::array< T, N > & value )noexcept
	{
		if constexpr ( !isBigEndian() )
		{
			for ( auto & element : value )
			{
				switchEndianness( element );
			}

			return value;
		}
		else
		{
			return value;
		}
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
	inline constexpr std::array< T, N > systemEndianToBigEndian( std::array< T, N > const & value )
	{
		std::array< T, N > result{ value };

		if constexpr ( !isBigEndian() )
		{
			for ( auto & element : result )
			{
				switchEndianness( element );
			}

			return result;
		}
		else
		{
			return result;
		}
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
	inline constexpr std::vector< T > & systemEndianToBigEndian( std::vector< T > & value )noexcept
	{
		if constexpr ( !isBigEndian() )
		{
			for ( auto & element : value )
			{
				switchEndianness( element );
			}

			return value;
		}
		else
		{
			return value;
		}
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
	inline constexpr std::vector< T > systemEndianToBigEndian( std::vector< T > const & value )
	{
		std::vector< T > result{ value };

		if constexpr ( !isBigEndian() )
		{
			for ( auto & element : result )
			{
				switchEndianness( element );
			}

			return result;
		}
		else
		{
			return result;
		}
	}
	/**
	 *\~english
	 *\brief			Convert the given value to little endian if needed.
	 *\param[in,out]	value	Value to be converted.
	 *\~french
	 *\brief			Convertit la valeur donnée en little endian si nécessaire.
	 *\param[in,out]	value	La valeur à convertir.
	 */
	template< typename T >
	inline constexpr T & systemEndianToLittleEndian( T & value )noexcept
	{
		if constexpr ( !isLittleEndian() )
		{
			return switchEndianness( value );
		}
		else
		{
			return value;
		}
	}
	/**
	 *\~english
	 *\brief			Convert the given value to little endian if needed.
	 *\param[in,out]	value	Value to be converted.
	 *\~french
	 *\brief			Convertit la valeur donnée en little endian si nécessaire.
	 *\param[in,out]	value	La valeur à convertir.
	 */
	template< typename T >
	inline constexpr T systemEndianToLittleEndian( T const & value )
	{
		if constexpr ( !isLittleEndian() )
		{
			T result{ value };
			switchEndianness( result );
			return result;
		}
		else
		{
			return value;
		}
	}
	/**
	 *\~english
	 *\brief			Convert the given value to little endian if needed.
	 *\param[in,out]	value	Value to be converted.
	 *\~french
	 *\brief			Convertit la valeur donnée en little endian si nécessaire.
	 *\param[in,out]	value	La valeur à convertir.
	 */
	template< typename T, size_t N >
	inline constexpr std::array< T, N > & systemEndianToLittleEndian( std::array< T, N > & value )noexcept
	{
		if constexpr ( !isLittleEndian() )
		{
			for ( auto & element : value )
			{
				switchEndianness( element );
			}

			return value;
		}
		else
		{
			return value;
		}
	}
	/**
	 *\~english
	 *\brief			Convert the given value to little endian if needed.
	 *\param[in,out]	value	Value to be converted.
	 *\~french
	 *\brief			Convertit la valeur donnée en little endian si nécessaire.
	 *\param[in,out]	value	La valeur à convertir.
	 */
	template< typename T, size_t N >
	inline constexpr std::array< T, N > systemEndianToLittleEndian( std::array< T, N > const & value )
	{
		std::array< T, N > result{ value };

		if constexpr ( !isLittleEndian() )
		{
			for ( auto & element : result )
			{
				switchEndianness( element );
			}

			return result;
		}
		else
		{
			return result;
		}
	}
	/**
	 *\~english
	 *\brief			Convert the given value to little endian if needed.
	 *\param[in,out]	value	Value to be converted.
	 *\~french
	 *\brief			Convertit la valeur donnée en little endian si nécessaire.
	 *\param[in,out]	value	La valeur à convertir.
	 */
	template< typename T >
	inline std::vector< T > & systemEndianToLittleEndian( std::vector< T > & value )noexcept
	{
		if constexpr ( !isLittleEndian() )
		{
			for ( auto & element : value )
			{
				switchEndianness( element );
			}

			return value;
		}
		else
		{
			return value;
		}
	}
	/**
	 *\~english
	 *\brief			Convert the given value to little endian if needed.
	 *\param[in,out]	value	Value to be converted.
	 *\~french
	 *\brief			Convertit la valeur donnée en little endian si nécessaire.
	 *\param[in,out]	value	La valeur à convertir.
	 */
	template< typename T >
	inline std::vector< T > systemEndianToLittleEndian( std::vector< T > const & value )
	{
		std::vector< T > result{ value };

		if constexpr ( !isLittleEndian() )
		{
			for ( auto & element : result )
			{
				switchEndianness( element );
			}

			return result;
		}
		else
		{
			return result;
		}
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
	inline constexpr T & bigEndianToSystemEndian( T & value )noexcept
	{
		if constexpr ( !isBigEndian() )
		{
			return switchEndianness( value );
		}
		else
		{
			return value;
		}
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
	inline constexpr T bigEndianToSystemEndian( T const & value )
	{
		if constexpr ( !isBigEndian() )
		{
			T result{ value };
			switchEndianness( result );
			return result;
		}
		else
		{
			return value;
		}
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
	inline constexpr std::array< T, N > & bigEndianToSystemEndian( std::array< T, N > & value )noexcept
	{
		if constexpr ( !isBigEndian() )
		{
			for ( auto & element : value )
			{
				switchEndianness( element );
			}

			return value;
		}
		else
		{
			return value;
		}
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
	inline constexpr std::array< T, N > bigEndianToSystemEndian( std::array< T, N > const & value )
	{
		std::array< T, N > result{ value };

		if constexpr ( !isBigEndian() )
		{
			for ( auto & element : result )
			{
				switchEndianness( element );
			}

			return result;
		}
		else
		{
			return result;
		}
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
		if constexpr ( !isBigEndian() )
		{
			for ( auto & element : value )
			{
				switchEndianness( element );
			}

			return value;
		}
		else
		{
			return value;
		}
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

		if constexpr ( !isBigEndian() )
		{
			for ( auto & element : result )
			{
				switchEndianness( element );
			}

			return result;
		}
		else
		{
			return result;
		}
	}
	/**
	 *\~english
	 *\brief			Convert the given little endian value to system endian if needed.
	 *\param[in,out]	value	Value to be converted.
	 *\~french
	 *\brief			Convertit la valeur donnée de little endian à l'endianness du système si nécessaire.
	 *\param[in,out]	value	La valeur à convertir.
	 */
	template< typename T >
	inline constexpr T & littleEndianToSystemEndian( T & value )noexcept
	{
		if constexpr ( !isLittleEndian() )
		{
			return switchEndianness( value );
		}
		else
		{
			return value;
		}
	}
	/**
	 *\~english
	 *\brief			Convert the given little endian value to system endian if needed.
	 *\param[in,out]	value	Value to be converted.
	 *\~french
	 *\brief			Convertit la valeur donnée de little endian à l'endianness du système si nécessaire.
	 *\param[in,out]	value	La valeur à convertir.
	 */
	template< typename T >
	inline constexpr T littleEndianToSystemEndian( T const & value )
	{
		if constexpr ( !isLittleEndian() )
		{
			T result{ value };
			switchEndianness( result );
			return result;
		}
		else
		{
			return value;
		}
	}
	/**
	 *\~english
	 *\brief			Convert the given value to little endian if needed.
	 *\param[in,out]	value	Value to be converted.
	 *\~french
	 *\brief			Convertit la valeur donnée en little endian si nécessaire.
	 *\param[in,out]	value	La valeur à convertir.
	 */
	template< typename T, size_t N >
	inline constexpr std::array< T, N > & littleEndianToSystemEndian( std::array< T, N > & value )noexcept
	{
		if constexpr ( !isLittleEndian() )
		{
			for ( auto & element : value )
			{
				switchEndianness( element );
			}

			return value;
		}
		else
		{
			return value;
		}
	}
	/**
	 *\~english
	 *\brief			Convert the given value to little endian if needed.
	 *\param[in,out]	value	Value to be converted.
	 *\~french
	 *\brief			Convertit la valeur donnée en little endian si nécessaire.
	 *\param[in,out]	value	La valeur à convertir.
	 */
	template< typename T, size_t N >
	inline constexpr std::array< T, N > littleEndianToSystemEndian( std::array< T, N > const & value )
	{
		std::array< T, N > result{ value };

		if constexpr ( !isLittleEndian() )
		{
			for ( auto & element : result )
			{
				switchEndianness( element );
			}

			return result;
		}
		else
		{
			return result;
		}
	}
	/**
	 *\~english
	 *\brief			Convert the given value to little endian if needed.
	 *\param[in,out]	value	Value to be converted.
	 *\~french
	 *\brief			Convertit la valeur donnée en little endian si nécessaire.
	 *\param[in,out]	value	La valeur à convertir.
	 */
	template< typename T >
	inline std::vector< T > & littleEndianToSystemEndian( std::vector< T > & value )noexcept
	{
		if constexpr ( !isLittleEndian() )
		{
			for ( auto & element : value )
			{
				switchEndianness( element );
			}

			return value;
		}
		else
		{
			return value;
		}
	}
	/**
	 *\~english
	 *\brief			Convert the given value to little endian if needed.
	 *\param[in,out]	value	Value to be converted.
	 *\~french
	 *\brief			Convertit la valeur donnée en little endian si nécessaire.
	 *\param[in,out]	value	La valeur à convertir.
	 */
	template< typename T >
	inline std::vector< T > littleEndianToSystemEndian( std::vector< T > const & value )
	{
		std::vector< T > result{ value };

		if constexpr ( !isLittleEndian() )
		{
			for ( auto & element : result )
			{
				switchEndianness( element );
			}

			return result;
		}
		else
		{
			return result;
		}
	}
}

#endif
