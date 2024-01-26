/*
See LICENSE file in root folder
*/
#ifndef ___CU_FlagCombination_H___
#define ___CU_FlagCombination_H___

#include "CastorUtils/Design/DesignModule.hpp"

#include <iterator>
#include <type_traits>

namespace castor
{
	template< typename FlagTypeT, typename IteratorTraitsT >
	struct FlagIterator
	{
	public:
		//! The basic integer type.
		using BaseType = typename std::underlying_type< FlagTypeT >::type;

	public:
		constexpr FlagIterator( FlagIterator && value )noexcept
			: m_initialValue{ value.m_initialValue }
			, m_index{ value.m_index }
			, m_value{ value.m_value }
		{
		}

		constexpr FlagIterator( FlagIterator const & value )
			: m_initialValue{ value.m_initialValue }
			, m_index{ value.m_index }
			, m_value{ value.m_value }
		{
		}

		constexpr FlagIterator & operator=( FlagIterator && value )noexcept
		{
			m_initialValue = value.m_initialValue;
			m_index = value.m_index;
			m_value = value.m_value;
		}

		constexpr FlagIterator & operator=( FlagIterator const & value )
		{
			m_initialValue = value.m_initialValue;
			m_index = value.m_index;
			m_value = value.m_value;
		}
		/**
		* Begin ctor.
		*/
		explicit constexpr FlagIterator( BaseType contValue )
			: m_initialValue{ contValue }
		{
			doGetNextValue();
		}
		/**
		* End ctor.
		*/
		constexpr FlagIterator( BaseType contValue
			, FlagTypeT iterValue )
			: m_initialValue{ contValue }
			, m_index{ sizeof( BaseType ) * 8 }
			, m_value{ iterValue }
		{
		}

		constexpr FlagIterator & operator++()
		{
			doGetNextValue();
			return *this;
		}

		constexpr FlagIterator operator++( int )
		{
			FlagIterator result{ *this };
			++( *this );
			return result;
		}

		constexpr FlagTypeT operator*()const
		{
			return m_value;
		}

	private:
		constexpr void doGetNextValue()
		{
			auto v = BaseType{ 1u };

			do
			{
				m_value = FlagTypeT( m_initialValue & ( v << m_index ) );
				++m_index;
			}
			while ( m_value == FlagTypeT( 0 ) && m_index < sizeof( BaseType ) * 8 );
		}

	private:
		BaseType m_initialValue;
		size_t m_index{ 0u };
		FlagTypeT m_value;

		template< typename FlagType, typename IteratorTraits >
		friend constexpr bool operator==( FlagIterator< FlagType, IteratorTraits > const & lhs
			, FlagIterator< FlagType, IteratorTraits > const & rhs );
	};

	template< typename FlagTypeT, typename IteratorTraitsT >
	constexpr bool operator==( FlagIterator< FlagTypeT, IteratorTraitsT > const & lhs
		, FlagIterator< FlagTypeT, IteratorTraitsT > const & rhs )
	{
		return lhs.m_index == rhs.m_index
			&& lhs.m_initialValue == rhs.m_initialValue;
	}

	template< typename FlagTypeT, typename IteratorTraitsT >
	constexpr bool operator!=( FlagIterator< FlagTypeT, IteratorTraitsT > const & lhs
		, FlagIterator< FlagTypeT, IteratorTraitsT > const & rhs )
	{
		return !( lhs == rhs );
	}

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
		constexpr FlagCombination( FlagType value )noexcept
			: m_value{ BaseType( value ) }
		{
			static_assert( sizeof( FlagType ) == sizeof( BaseType )
				, "Can't combine different size parameters" );
		}

		explicit constexpr FlagCombination( BaseType value = BaseType{} )noexcept
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
		constexpr operator BaseType const &()const noexcept
		{
			return m_value;
		}

		constexpr BaseType value()const noexcept
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
		constexpr FlagIterator< FlagType > begin()const noexcept
		{
			return FlagIterator< FlagType >( m_value );
		}

		constexpr FlagIterator< FlagType > end()const noexcept
		{
			return FlagIterator< FlagType >( m_value, FlagType{} );
		}

		constexpr size_t size()const noexcept
		{
			size_t result = 0;

			for ( auto it = begin(); it != end(); ++it )
			{
				++result;
			}

			return result;
		}

		constexpr bool empty()const noexcept
		{
			return value() == 0u;
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
		constexpr FlagCombination & operator&=( BaseType rhs )noexcept
		{
			m_value &= rhs;
			return *this;
		}

		constexpr FlagCombination & operator|=( BaseType rhs )noexcept
		{
			m_value |= rhs;
			return *this;
		}
		
		constexpr FlagCombination & operator^=( BaseType rhs )noexcept
		{
			m_value ^= rhs;
			return *this;
		}

		constexpr FlagCombination & operator&=( FlagType rhs )noexcept
		{
			m_value &= BaseType( rhs );
			return *this;
		}

		constexpr FlagCombination & operator|=( FlagType rhs )noexcept
		{
			m_value |= BaseType( rhs );
			return *this;
		}
		
		constexpr FlagCombination & operator^=( FlagType rhs )noexcept
		{
			m_value ^= BaseType( rhs );
			return *this;
		}

		constexpr FlagCombination & operator&=( FlagCombination< FlagType > const & rhs )noexcept
		{
			m_value &= BaseType( rhs );
			return *this;
		}

		constexpr FlagCombination & operator|=( FlagCombination< FlagType > const & rhs )noexcept
		{
			m_value |= BaseType( rhs );
			return *this;
		}

		constexpr FlagCombination & operator^=( FlagCombination< FlagType > const & rhs )noexcept
		{
			m_value ^= BaseType( rhs );
			return *this;
		}

		constexpr FlagCombination & operator~()noexcept
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
	constexpr bool operator==( FlagCombination< FlagType > const & lhs
		, FlagCombination< FlagType > const & rhs )noexcept
	{
		using Type = typename FlagCombination< FlagType >::BaseType;
		return Type( lhs ) == Type( rhs );
	}

	template< typename FlagType >
	constexpr bool operator==( FlagCombination< FlagType > const & lhs
		, FlagType const & rhs )noexcept
	{
		using Type = typename FlagCombination< FlagType >::BaseType;
		return Type( lhs ) == Type( rhs );
	}

	template< typename FlagType >
	constexpr bool operator!=( FlagCombination< FlagType > const & lhs
		, FlagCombination< FlagType > const & rhs )noexcept
	{
		using Type = typename FlagCombination< FlagType >::BaseType;
		return Type( lhs ) != Type( rhs );
	}

	template< typename FlagType >
	constexpr bool operator!=( FlagCombination< FlagType > const & lhs
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
	constexpr FlagCombination< FlagType > operator&
		( FlagCombination< FlagType > const & lhs
		, FlagType const & rhs )noexcept
	{
		FlagCombination< FlagType > result{ lhs };
		return result &= rhs;
	}

	template< typename FlagType >
	constexpr FlagCombination< FlagType > operator|
		( FlagCombination< FlagType > const & lhs
		, FlagType const & rhs )noexcept
	{
		FlagCombination< FlagType > result{ lhs };
		return result |= rhs;
	}
	
	template< typename FlagType >
	constexpr FlagCombination< FlagType > operator^
		( FlagCombination< FlagType > const & lhs
		, FlagType const & rhs )noexcept
	{
		FlagCombination< FlagType > result{ lhs };
		return result ^= rhs;
	}

	template< typename FlagType >
	constexpr FlagCombination< FlagType > operator&
		( FlagCombination< FlagType > const & lhs
		, typename FlagCombination< FlagType >::BaseType const & rhs )noexcept
	{
		FlagCombination< FlagType > result{ lhs };
		return result &= rhs;
	}

	template< typename FlagType >
	constexpr FlagCombination< FlagType > operator|
		( FlagCombination< FlagType > const & lhs
		, typename FlagCombination< FlagType >::BaseType const & rhs )noexcept
	{
		FlagCombination< FlagType > result{ lhs };
		return result |= rhs;
	}

	template< typename FlagType >
	constexpr FlagCombination< FlagType > operator^
		( FlagCombination< FlagType > const & lhs
		, typename FlagCombination< FlagType >::BaseType const & rhs )noexcept
	{
		FlagCombination< FlagType > result{ lhs };
		return result ^= rhs;
	}

	template< typename FlagType >
	constexpr FlagCombination< FlagType > operator&
		( FlagCombination< FlagType > const & lhs
		, FlagCombination< FlagType > const & rhs )noexcept
	{
		FlagCombination< FlagType > result{ lhs };
		return result &= rhs;
	}

	template< typename FlagType >
	constexpr FlagCombination< FlagType > operator|
		( FlagCombination< FlagType > const & lhs
		, FlagCombination< FlagType > const & rhs )noexcept
	{
		FlagCombination< FlagType > result{ lhs };
		return result |= rhs;
	}

	template< typename FlagType >
	constexpr FlagCombination< FlagType > operator^
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
	 *\param[in]	rhs		The flags looked for.
	 *\return		\p true if flags contain any of rhs.
	 *\~french
	 *\param[in]	value	La valeur.
	 *\param[in]	rhs		Les indicateurs recherchés.
	 *\return		\p true si flags contient n'importe lequel de rhs.
	 */
	template< typename T, typename U >
	constexpr bool hasAll( T const & value, U const & rhs )noexcept
	{
		static_assert( sizeof( T ) == sizeof( U )
			, "Can't check flags for different size parameters" );
		return T( value & T( rhs ) ) == T( rhs );
	}
	/**
	 *\~english
	 *\param[in]	value	The value.
	 *\param[in]	rhs		The flags looked for.
	 *\return		\p true if flags contain any of rhs.
	 *\~french
	 *\param[in]	value	La valeur.
	 *\param[in]	rhs		Les indicateurs recherchés.
	 *\return		\p true si flags contient n'importe lequel de rhs.
	 */
	template< typename FlagType >
	constexpr bool hasAll( FlagCombination< FlagType > const & value
		, FlagType const & rhs )noexcept
	{
		return ( value & rhs ) == rhs;
	}
	/**
	 *\~english
	 *\param[in]	value	The value.
	 *\param[in]	rhs		The flags looked for.
	 *\return		\p true if flags contain any of rhs.
	 *\~french
	 *\param[in]	value	La valeur.
	 *\param[in]	rhs		Les indicateurs recherchés.
	 *\return		\p true si flags contient n'importe lequel de rhs.
	 */
	template< typename FlagType >
	constexpr bool hasAll( FlagType const & value
		, FlagCombination< FlagType > const & rhs )noexcept
	{
		return ( rhs & value ) == rhs;
	}
	/**
	 *\~english
	 *\param[in]	value	The value.
	 *\param[in]	rhs		The flags looked for.
	 *\return		\p true if flags contain any of rhs.
	 *\~french
	 *\param[in]	value	La valeur.
	 *\param[in]	rhs		Les indicateurs recherchés.
	 *\return		\p true si flags contient n'importe lequel de rhs.
	 */
	template< typename FlagType >
	constexpr bool hasAll( typename FlagCombination< FlagType >::BaseType const & value
		, FlagCombination< FlagType > const & rhs )noexcept
	{
		return hasAll( FlagType{ value }, rhs );
	}
	/**
	 *\~english
	 *\param[in]	value	The value.
	 *\param[in]	rhs		The flags looked for.
	 *\return		\p true if flags contain any of rhs.
	 *\~french
	 *\param[in]	value	La valeur.
	 *\param[in]	rhs		Les indicateurs recherchés.
	 *\return		\p true si flags contient n'importe lequel de rhs.
	 */
	template< typename FlagType >
	constexpr bool hasAll( FlagCombination< FlagType > const & value
		, typename FlagCombination< FlagType >::BaseType const & rhs )noexcept
	{
		return hasAll( value, FlagType{ rhs } );
	}
	/**
	 *\~english
	 *\param[in]	value	The value.
	 *\param[in]	rhs		The flags looked for.
	 *\return		\p true if flags contain any of rhs.
	 *\~french
	 *\param[in]	value	La valeur.
	 *\param[in]	rhs		Les indicateurs recherchés.
	 *\return		\p true si flags contient n'importe lequel de rhs.
	 */
	template< typename FlagType >
	constexpr bool hasAll( FlagCombination< FlagType > const & value
		, FlagCombination< FlagType > const & rhs )noexcept
	{
		return ( value & rhs ) == rhs;
	}
	/**
	 *\~english
	 *\param[in]	value	The value.
	 *\param[in]	rhs		The flags looked for.
	 *\return		\p true if flags contain any of rhs.
	 *\~french
	 *\param[in]	value	La valeur.
	 *\param[in]	rhs		Les indicateurs recherchés.
	 *\return		\p true si flags contient n'importe lequel de rhs.
	 */
	template< typename T, typename U >
	constexpr bool hasAny( T const & value, U const & rhs )noexcept
	{
		static_assert( sizeof( T ) == sizeof( U )
			, "Can't check flags for different size parameters" );
		return T( value & T( rhs ) ) != T{};
	}
	/**
	 *\~english
	 *\param[in]	value	The value.
	 *\param[in]	rhs		The flags looked for.
	 *\return		\p true if flags contain any of rhs.
	 *\~french
	 *\param[in]	value	La valeur.
	 *\param[in]	rhs		Les indicateurs recherchés.
	 *\return		\p true si flags contient n'importe lequel de rhs.
	 */
	template< typename FlagType >
	constexpr bool hasAny( FlagCombination< FlagType > const & value
		, FlagType const & rhs )noexcept
	{
		return ( value & rhs ) != FlagType{};
	}
	/**
	 *\~english
	 *\param[in]	value	The value.
	 *\param[in]	rhs		The flags looked for.
	 *\return		\p true if flags contain any of rhs.
	 *\~french
	 *\param[in]	value	La valeur.
	 *\param[in]	rhs		Les indicateurs recherchés.
	 *\return		\p true si flags contient n'importe lequel de rhs.
	 */
	template< typename FlagType, typename Type >
	constexpr bool hasAny( FlagCombination< FlagType > const & value
		, Type const & rhs )noexcept
	{
		static_assert( sizeof( FlagType ) == sizeof( Type )
			, "Can't check flags for different size parameters" );
		return ( value & rhs ) != FlagType{};
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
	template< typename T, typename U >
	constexpr bool checkFlag( T const & value, U const & flag )noexcept
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
	constexpr bool checkFlag( FlagCombination< FlagType > const & value
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
	constexpr bool checkFlag( FlagCombination< FlagType > const & value
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
	constexpr FlagCombination< FlagType > & addFlag
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
	constexpr FlagCombination< FlagType > & addFlags
		( FlagCombination< FlagType > & value
		, FlagCombination< FlagType > const & flags )noexcept
	{
		value |= flags;
		return value;
	}
	/**
	 *\~english
	 *\brief			Removes a flag combination from the given value.
	 *\param[in,out]	value	The value.
	 *\param[in]		flags	The flags to remove.
	 *\return			The value.
	 *\~french
	 *\brief			Enlève une combinaison d'indicateurs de la valeur donnée.
	 *\param[in,out]	value	La valeur.
	 *\param[in]		flags	Les indicateurs à enlever.
	 *\return			La valeur.
	 */
	template< typename FlagType >
	constexpr FlagCombination< FlagType > & remFlags
		( FlagCombination< FlagType > & value
		, FlagCombination< FlagType > const & flags )noexcept
	{
		using Type = typename FlagCombination< FlagType >::BaseType;
		value &= Type( ~Type( flags ) );
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
	constexpr FlagCombination< FlagType > & remFlag
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
	constexpr FlagCombination< FlagType > addFlag
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
	constexpr FlagCombination< FlagType > remFlag
		( FlagCombination< FlagType > const & value
		, FlagType const & flag )noexcept
	{
		using Type = typename FlagCombination< FlagType >::BaseType;
		return value & FlagType( ~Type( flag ) );
	}
}

#endif
