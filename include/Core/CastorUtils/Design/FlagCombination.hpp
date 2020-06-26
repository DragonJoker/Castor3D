/*
See LICENSE file in root folder
*/
#ifndef ___CU_FlagCombination_H___
#define ___CU_FlagCombination_H___

#include "CastorUtils/Design/DesignModule.hpp"

#include <type_traits>

namespace castor
{
	template< typename FlagType >
	struct FlagIterator
	{
	public:
		//! The basic integer type.
		using BaseType = typename std::underlying_type< FlagType >::type;

	public:
		/**
		* Begin ctor.
		*/
		inline constexpr FlagIterator( BaseType value )
			: m_initialValue{ value }
		{
			doGetNextValue();
		}
		/**
		* End ctor.
		*/
		inline constexpr FlagIterator()
			: m_initialValue{ 0u }
			, m_index{ sizeof( BaseType ) * 8 }
		{
		}

		inline constexpr FlagIterator & operator++()
		{
			doGetNextValue();
			return *this;
		}

		inline constexpr FlagIterator operator++( int )
		{
			FlagIterator result{ *this };
			++( *this );
			return result;
		}

		inline constexpr FlagType operator*()const
		{
			return m_value;
		}

		inline constexpr bool operator==( FlagIterator< FlagType > const & rhs )
		{
			return m_index == rhs.m_index
				&& m_initialValue == rhs.m_initialValue;
		}

		inline constexpr bool operator!=( FlagIterator< FlagType > const & rhs )
		{
			return m_index != rhs.m_index
				|| m_initialValue != rhs.m_initialValue;
		}

	private:
		inline constexpr void doGetNextValue()
		{
			auto v = BaseType{ 1u };

			do
			{
				m_value = FlagType( m_initialValue & ( v << m_index ) );
				++m_index;
			}
			while ( m_value == FlagType( 0 ) && m_index < sizeof( BaseType ) * 8 );
		}

	private:
		BaseType m_initialValue;
		size_t m_index{ 0u };
		FlagType m_value;
	};

	template< typename FlagType >
	class FlagCombination
	{
	public:
		//! The basic integer type.
		using BaseType = typename std::underlying_type< FlagType >::type;
		/**
		*\~english
		*\name
		*	Construction.
		*\~french
		*\name
		*	Construction.
		**/
		/**@{*/
		inline constexpr FlagCombination( FlagType value )noexcept
			: m_value{ BaseType( value ) }
		{
			static_assert( sizeof( FlagType ) == sizeof( BaseType )
				, "Can't combine different size parameters" );
		}

		inline explicit constexpr FlagCombination( BaseType value = BaseType{} )noexcept
			: m_value{ value }
		{
			static_assert( sizeof( FlagType ) == sizeof( BaseType )
				, "Can't combine different size parameters" );
		}
		/**\}*/
		/**
		*\~english
		*\name
		*	Casting.
		*\~french
		*\name
		*	Conversion.
		**/
		/**\{*/
		inline constexpr operator BaseType const &()const noexcept
		{
			return m_value;
		}

		inline constexpr BaseType value()const noexcept
		{
			return m_value;
		}
		/**\}*/
		/**
		*\~english
		*\name
		*	Iteration.
		*\~french
		*\name
		*	Itération.
		**/
		/**\{*/
		inline constexpr FlagIterator< FlagType > begin()const noexcept
		{
			return FlagIterator< FlagType >( m_value );
		}

		inline constexpr FlagIterator< FlagType > end()const noexcept
		{
			return FlagIterator< FlagType >();
		}
		/**\}*/
		/**
		*\~english
		*\name
		*	Binary operators.
		*\~french
		*\name
		*	Opérateurs binaires.
		**/
		/**\{*/
		inline constexpr FlagCombination & operator&=( BaseType rhs )noexcept
		{
			m_value &= rhs;
			return *this;
		}

		inline constexpr FlagCombination & operator|=( BaseType rhs )noexcept
		{
			m_value |= rhs;
			return *this;
		}
		
		inline constexpr FlagCombination & operator^=( BaseType rhs )noexcept
		{
			m_value ^= rhs;
			return *this;
		}

		inline constexpr FlagCombination & operator&=( FlagType rhs )noexcept
		{
			m_value &= BaseType( rhs );
			return *this;
		}

		inline constexpr FlagCombination & operator|=( FlagType rhs )noexcept
		{
			m_value |= BaseType( rhs );
			return *this;
		}
		
		inline constexpr FlagCombination & operator^=( FlagType rhs )noexcept
		{
			m_value ^= BaseType( rhs );
			return *this;
		}

		inline constexpr FlagCombination & operator&=( FlagCombination< FlagType > const & rhs )noexcept
		{
			m_value &= BaseType( rhs );
			return *this;
		}

		inline constexpr FlagCombination & operator|=( FlagCombination< FlagType > const & rhs )noexcept
		{
			m_value |= BaseType( rhs );
			return *this;
		}

		inline constexpr FlagCombination & operator^=( FlagCombination< FlagType > const & rhs )noexcept
		{
			m_value ^= BaseType( rhs );
			return *this;
		}

		inline constexpr FlagCombination & operator~()noexcept
		{
			m_value = ~BaseType( m_value );
			return *this;
		}
		/**\}*/

	private:
		BaseType m_value;
	};
	/**
	*\~english
	*\name
	*	Comparison operators.
	*\~french
	*\name
	*	Opérateurs de comparaison.
	**/
	/**\{*/
	template< typename FlagType >
	inline constexpr bool operator==( FlagCombination< FlagType > const & lhs
		, FlagCombination< FlagType > const & rhs )noexcept
	{
		using Type = typename FlagCombination< FlagType >::BaseType;
		return Type( lhs ) == Type( rhs );
	}

	template< typename FlagType >
	inline constexpr bool operator==( FlagCombination< FlagType > const & lhs
		, FlagType const & rhs )noexcept
	{
		using Type = typename FlagCombination< FlagType >::BaseType;
		return Type( lhs ) == Type( rhs );
	}

	template< typename FlagType >
	inline constexpr bool operator!=( FlagCombination< FlagType > const & lhs
		, FlagCombination< FlagType > const & rhs )noexcept
	{
		using Type = typename FlagCombination< FlagType >::BaseType;
		return Type( lhs ) != Type( rhs );
	}

	template< typename FlagType >
	inline constexpr bool operator!=( FlagCombination< FlagType > const & lhs
		, FlagType const & rhs )noexcept
	{
		using Type = typename FlagCombination< FlagType >::BaseType;
		return Type( lhs ) != Type( rhs );
	}
	/**
	*\~english
	*\name
	*	Binary operators.
	*\~french
	*\name
	*	Opérateurs binaires.
	**/
	/**\{*/
	template< typename FlagType >
	inline constexpr FlagCombination< FlagType > operator&
		( FlagCombination< FlagType > const & lhs
		, FlagType const & rhs )noexcept
	{
		FlagCombination< FlagType > result{ lhs };
		return result &= rhs;
	}

	template< typename FlagType >
	inline constexpr FlagCombination< FlagType > operator|
		( FlagCombination< FlagType > const & lhs
		, FlagType const & rhs )noexcept
	{
		FlagCombination< FlagType > result{ lhs };
		return result |= rhs;
	}
	
	template< typename FlagType >
	inline constexpr FlagCombination< FlagType > operator^
		( FlagCombination< FlagType > const & lhs
		, FlagType const & rhs )noexcept
	{
		FlagCombination< FlagType > result{ lhs };
		return result ^= rhs;
	}

	template< typename FlagType >
	inline constexpr FlagCombination< FlagType > operator&
		( FlagCombination< FlagType > const & lhs
		, typename FlagCombination< FlagType >::BaseType const & rhs )noexcept
	{
		FlagCombination< FlagType > result{ lhs };
		return result &= rhs;
	}

	template< typename FlagType >
	inline constexpr FlagCombination< FlagType > operator|
		( FlagCombination< FlagType > const & lhs
		, typename FlagCombination< FlagType >::BaseType const & rhs )noexcept
	{
		FlagCombination< FlagType > result{ lhs };
		return result |= rhs;
	}

	template< typename FlagType >
	inline constexpr FlagCombination< FlagType > operator^
		( FlagCombination< FlagType > const & lhs
		, typename FlagCombination< FlagType >::BaseType const & rhs )noexcept
	{
		FlagCombination< FlagType > result{ lhs };
		return result ^= rhs;
	}

	template< typename FlagType >
	inline constexpr FlagCombination< FlagType > operator&
		( FlagCombination< FlagType > const & lhs
		, FlagCombination< FlagType > const & rhs )noexcept
	{
		FlagCombination< FlagType > result{ lhs };
		return result &= rhs;
	}

	template< typename FlagType >
	inline constexpr FlagCombination< FlagType > operator|
		( FlagCombination< FlagType > const & lhs
		, FlagCombination< FlagType > const & rhs )noexcept
	{
		FlagCombination< FlagType > result{ lhs };
		return result |= rhs;
	}

	template< typename FlagType >
	inline constexpr FlagCombination< FlagType > operator^
		( FlagCombination< FlagType > const & lhs
		, FlagCombination< FlagType > const & rhs )noexcept
	{
		FlagCombination< FlagType > result{ lhs };
		return result ^= rhs;
	}
	/**\}*/
	/**
	 *\~english
	 *\param[in]	value	The value.
	 *\param[in]	flag	The flag looked for.
	 *\return		\p true if flags contain flag.
	 *\~french
	 *\param[in]	value	La valeur.
	 *\param[in]	flag	L'indicateur recherché.
	 *\return		\p true si flags contient flag.
	 */
	template< typename T, typename U >
	inline constexpr bool checkFlag( T const & value, U const & flag )noexcept
	{
		static_assert( sizeof( T ) == sizeof( U )
			, "Can't check flags for different size parameters" );
		return U( value & T( flag ) ) == flag;
	}
	/**
	 *\~english
	 *\param[in]	value	The value.
	 *\param[in]	flag	The flag looked for.
	 *\return		\p true if flags contain flag.
	 *\~french
	 *\param[in]	value	La valeur.
	 *\param[in]	flag	L'indicateur recherché.
	 *\return		\p true si flags contient flag.
	 */
	template< typename FlagType >
	inline constexpr bool checkFlag( FlagCombination< FlagType > const & value
		, FlagType const & flag )noexcept
	{
		using Type = typename FlagCombination< FlagType >::BaseType;
		return Type( value & flag ) == Type( flag );
	}
	/**
	 *\~english
	 *\param[in]	value	The value.
	 *\param[in]	flag	The flag looked for.
	 *\return		\p true if flags contain flag.
	 *\~french
	 *\param[in]	value	La valeur.
	 *\param[in]	flag	L'indicateur recherché.
	 *\return		\p true si flags contient flag.
	 */
	template< typename FlagType, typename Type >
	inline constexpr bool checkFlag( FlagCombination< FlagType > const & value
		, Type const & flag )noexcept
	{
		static_assert( sizeof( FlagType ) == sizeof( Type )
			, "Can't check flags for different size parameters" );
		return Type( value & flag ) == flag;
	}
	/**
	 *\~english
	 *\brief			adds a flag to the given value.
	 *\param[in,out]	value	The value.
	 *\param[in]		flag	The flag to add.
	 *\return			The value.
	 *\~french
	 *\brief			Ajoute un indicateur à la valeur donnée.
	 *\param[in,out]	value	La valeur.
	 *\param[in]		flag	L'indicateur à ajouter.
	 *\return			La valeur.
	 */
	template< typename FlagType >
	inline constexpr FlagCombination< FlagType > & addFlag
		( FlagCombination< FlagType > & value
		, FlagType const & flag )noexcept
	{
		value |= flag;
		return value;
	}
	/**
	 *\~english
	 *\brief			adds a flag combination to the given value.
	 *\param[in,out]	value	The value.
	 *\param[in]		flags	The flag combination to add.
	 *\return			The value.
	 *\~french
	 *\brief			Ajoute une combinaison d'indicateurs à la valeur donnée.
	 *\param[in,out]	value	La valeur.
	 *\param[in]		flags	La combinaison d'indicateurs à ajouter.
	 *\return			La valeur.
	 */
	template< typename FlagType >
	inline constexpr FlagCombination< FlagType > & addFlags
		( FlagCombination< FlagType > & value
		, FlagCombination< FlagType > const & flags )noexcept
	{
		value |= flags;
		return value;
	}
	/**
	 *\~english
	 *\brief			Removes a flag from the given value.
	 *\param[in,out]	value	The value.
	 *\param[in]		flag	The flag to remove.
	 *\return			The value.
	 *\~french
	 *\brief			Enlève un indicateur de la valeur donnée.
	 *\param[in,out]	value	La valeur.
	 *\param[in]		flag	L'indicateur à enlever.
	 *\return			La valeur.
	 */
	template< typename FlagType >
	inline constexpr FlagCombination< FlagType > & remFlag
		( FlagCombination< FlagType > & value
		, FlagType const & flag )noexcept
	{
		using Type = typename FlagCombination< FlagType >::BaseType;
		value &= FlagType( ~Type( flag ) );
		return value;
	}
	/**
	 *\~english
	 *\brief		adds a flag to the given value.
	 *\param[in]	value	The value.
	 *\param[in]	flag	The flag to add.
	 *\return		The new value.
	 *\~french
	 *\brief		Ajoute un indicateur à la valeur donnée.
	 *\param[in]	value	La valeur.
	 *\param[in]	flag	L'indicateur à ajouter.
	 *\return		La nouvelle valeur.
	 */
	template< typename FlagType >
	inline constexpr FlagCombination< FlagType > addFlag
		( FlagCombination< FlagType > const & value
		, FlagType const & flag )noexcept
	{
		return ( value | flag );
	}
	/**
	 *\~english
	 *\brief		Removes a flag from the given value.
	 *\param[in]	value	The value.
	 *\param[in]	flag	The flag to remove.
	 *\return		The new value.
	 *\~french
	 *\brief		Enlève un indicateur de la valeur donnée.
	 *\param[in]	value	La valeur.
	 *\param[in]	flag	L'indicateur à enlever.
	 *\return		La nouvelle valeur.
	 */
	template< typename FlagType >
	inline constexpr FlagCombination< FlagType > remFlag
		( FlagCombination< FlagType > const & value
		, FlagType const & flag )noexcept
	{
		using Type = typename FlagCombination< FlagType >::BaseType;
		return value & FlagType( ~Type( flag ) );
	}
}

#endif
