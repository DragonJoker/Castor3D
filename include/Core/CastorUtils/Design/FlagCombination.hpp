/*
See LICENSE file in root folder
*/
#ifndef ___CU_FlagCombination_H___
#define ___CU_FlagCombination_H___

#include "CastorUtils/CastorUtilsPrerequisites.hpp"

namespace castor
{
	/**
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		23/11/2016
	\~english
	\brief		Templated class that retrieves the integral base type from a size.
	\~french
	\brief		Classe template qui récupère le type entier de base depuis une taille.
	*/
	template< size_t Size > struct BaseTypeFromSize;
	/**
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		23/11/2016
	\~english
	\brief		Templated class that retrieves the integral base type from a size.
	\remarks	Specialisation for 1u.
	\~french
	\brief		Classe template qui récupère le type entier de base depuis une taille.
	\remarks	Spécialisation pour 1u.
	*/
	template<> struct BaseTypeFromSize< 1u >
	{
		using Type = uint8_t;
	};
	/**
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		23/11/2016
	\~english
	\brief		Templated class that retrieves the integral base type from a size.
	\remarks	Specialisation for 2u.
	\~french
	\brief		Classe template qui récupère le type entier de base depuis une taille.
	\remarks	Spécialisation pour 2u.
	*/
	template<> struct BaseTypeFromSize< 2u >
	{
		using Type = uint16_t;
	};
	/**
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		23/11/2016
	\~english
	\brief		Templated class that retrieves the integral base type from a size.
	\remarks	Specialisation for 4u.
	\~french
	\brief		Classe template qui récupère le type entier de base depuis une taille.
	\remarks	Spécialisation pour 4u.
	*/
	template<> struct BaseTypeFromSize< 4u >
	{
		using Type = uint32_t;
	};
	/**
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		23/11/2016
	\~english
	\brief		Templated class that retrieves the integral base type from a size.
	\remarks	Specialisation for 8u.
	\~french
	\brief		Classe template qui récupère le type entier de base depuis une taille.
	\remarks	Spécialisation pour 8u.
	*/
	template<> struct BaseTypeFromSize< 8u >
	{
		using Type = uint64_t;
	};
	/**
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		23/11/2016
	\~english
	\brief		Templated class that provides implicit conversion from scoped enum to integral base type.
	\~french
	\brief		Classe template qui fournit une conversion implicite depuis un scoped enum vers un type entier de base.
	*/
	template< typename FlagType, typename BaseType >
	class FlagCombination
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor from a flag type.
		 *\param[in]	value	The value.
		 *\~french
		 *\brief		Constructeur depuis le type d'indicateur.
		 *\param[in]	value	La valeur.
		 */
		inline FlagCombination( FlagType value )noexcept
			: m_value{ BaseType( value ) }
		{
			static_assert( sizeof( FlagType ) == sizeof( BaseType ), "Can't combine different size parameters" );
		}
		/**
		 *\~english
		 *\brief		Constructor from combined flags.
		 *\param[in]	value	The value.
		 *\~french
		 *\brief		Constructeur depuis des indicateurs combinés.
		 *\param[in]	value	La valeur.
		 */
		inline FlagCombination( BaseType value = BaseType{} )noexcept
			: m_value{ value }
		{
			static_assert( sizeof( FlagType ) == sizeof( BaseType ), "Can't combine different size parameters" );
		}
		/**
		 *\~english
		 *\brief		Binary AND assignment operator.
		 *\param[in]	rhs	The value.
		 *\return		A reference to this object.
		 *\~french
		 *\brief		Opérateur d'affectation par ET binaire.
		 *\param[in]	rhs	La valeur.
		 *\return		Une référence sur cet objet.
		 */
		inline FlagCombination & operator&=( BaseType rhs )noexcept
		{
			m_value &= rhs;
			return *this;
		}
		/**
		 *\~english
		 *\brief		Binary OR assignment operator.
		 *\param[in]	rhs	The value.
		 *\return		A reference to this object.
		 *\~french
		 *\brief		Opérateur d'affectation par OU binaire.
		 *\param[in]	rhs	La valeur.
		 *\return		Une référence sur cet objet.
		 */
		inline FlagCombination & operator|=( BaseType rhs )noexcept
		{
			m_value |= rhs;
			return *this;
		}
		/**
		 *\~english
		 *\brief		Binary AND assignment operator.
		 *\param[in]	rhs	The value.
		 *\return		A reference to this object.
		 *\~french
		 *\brief		Opérateur d'affectation par ET binaire.
		 *\param[in]	rhs	La valeur.
		 *\return		Une référence sur cet objet.
		 */
		inline FlagCombination & operator&=( FlagType rhs )noexcept
		{
			m_value &= BaseType( rhs );
			return *this;
		}
		/**
		 *\~english
		 *\brief		Binary OR assignment operator.
		 *\param[in]	rhs	The value.
		 *\return		A reference to this object.
		 *\~french
		 *\brief		Opérateur d'affectation par OU binaire.
		 *\param[in]	rhs	La valeur.
		 *\return		Une référence sur cet objet.
		 */
		inline FlagCombination & operator|=( FlagType rhs )noexcept
		{
			m_value |= BaseType( rhs );
			return *this;
		}
		/**
		 *\~english
		 *\brief		Binary AND assignment operator.
		 *\param[in]	rhs	The value.
		 *\return		A reference to this object.
		 *\~french
		 *\brief		Opérateur d'affectation par ET binaire.
		 *\param[in]	rhs	La valeur.
		 *\return		Une référence sur cet objet.
		 */
		inline FlagCombination & operator&=( FlagCombination< FlagType, BaseType > const & rhs )noexcept
		{
			m_value &= BaseType( rhs );
			return *this;
		}
		/**
		 *\~english
		 *\brief		Binary OR assignment operator.
		 *\param[in]	rhs	The value.
		 *\return		A reference to this object.
		 *\~french
		 *\brief		Opérateur d'affectation par OU binaire.
		 *\param[in]	rhs	La valeur.
		 *\return		Une référence sur cet objet.
		 */
		inline FlagCombination & operator|=( FlagCombination< FlagType, BaseType > const & rhs )noexcept
		{
			m_value |= BaseType( rhs );
			return *this;
		}
		/**
		 *\~english
		 *\brief		Implicit cast to integral base type.
		 *\~french
		 *\brief		Conversion implicite vers le type entier de base.
		 */
		inline operator BaseType()const noexcept
		{
			return m_value;
		}

	private:
		BaseType m_value;
	};
	/**
	 *\~english
	 *\brief		Equality operator.
	 *\param[in]	lhs, rhs	The operands.
	 *\~french
	 *\brief		Opérateur d'égalité.
	 *\param[in]	lhs, rhs	Les opérandes.
	 */
	template< typename FlagType, typename Type >
	inline bool operator==( FlagCombination< FlagType, Type > const & lhs
		, FlagCombination< FlagType, Type > const & rhs )noexcept
	{
		return Type( lhs ) == Type( rhs );
	}
	/**
	 *\~english
	 *\brief		Equality operator.
	 *\param[in]	lhs, rhs	The operands.
	 *\~french
	 *\brief		Opérateur d'égalité.
	 *\param[in]	lhs, rhs	Les opérandes.
	 */
	template< typename FlagType, typename Type >
	inline bool operator==( FlagCombination< FlagType, Type > const & lhs
		, FlagType const & rhs )noexcept
	{
		return Type( lhs ) == Type( rhs );
	}
	/**
	 *\~english
	 *\brief		Difference operator.
	 *\param[in]	lhs, rhs	The operands.
	 *\~french
	 *\brief		Opérateur de différence.
	 *\param[in]	lhs, rhs	Les opérandes.
	 */
	template< typename FlagType, typename Type >
	inline bool operator!=( FlagCombination< FlagType, Type > const & lhs
		, FlagCombination< FlagType, Type > const & rhs )noexcept
	{
		return Type( lhs ) != Type( rhs );
	}
	/**
	 *\~english
	 *\brief		Difference operator.
	 *\param[in]	lhs, rhs	The operands.
	 *\~french
	 *\brief		Opérateur de différence.
	 *\param[in]	lhs, rhs	Les opérandes.
	 */
	template< typename FlagType, typename Type >
	inline bool operator!=( FlagCombination< FlagType, Type > const & lhs
		, FlagType const & rhs )noexcept
	{
		return Type( lhs ) != Type( rhs );
	}
	/**
	 *\~english
	 *\brief		Binary AND operator.
	 *\param[in]	lhs, rhs	The operands.
	 *\return		The resulting flags combination.
	 *\~french
	 *\brief		Opérateur ET binaire.
	 *\param[in]	lhs, rhs	Les opérandes.
	 *\return		La combinaison de flags résultante.
	 */
	template< typename FlagType, typename Type >
	inline FlagCombination< FlagType, Type > operator&( FlagCombination< FlagType, Type > const & lhs
		, FlagType const & rhs )noexcept
	{
		static_assert( sizeof( FlagType ) == sizeof( Type ), "Can't apply binary AND on different size parameters" );
		FlagCombination< FlagType, Type > result{ lhs };
		return result &= rhs;
	}
	/**
	 *\~english
	 *\brief		Binary OR operator.
	 *\param[in]	lhs, rhs	The operands.
	 *\return		The resulting flags combination.
	 *\~french
	 *\brief		Opérateur OU binaire.
	 *\param[in]	lhs, rhs	Les opérandes.
	 *\return		La combinaison de flags résultante.
	 */
	template< typename FlagType, typename Type >
	inline FlagCombination< FlagType, Type > operator|( FlagCombination< FlagType, Type > const & lhs
		, FlagType const & rhs )noexcept
	{
		static_assert( sizeof( FlagType ) == sizeof( Type ), "Can't apply binary OR on different size parameters" );
		FlagCombination< FlagType, Type > result{ lhs };
		return result |= rhs;
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
	inline bool checkFlag( T const & value, U const & flag )noexcept
	{
		static_assert( sizeof( T ) == sizeof( U ), "Can't check flags for different size parameters" );
		return U( value & T( flag ) ) == flag;;
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
	inline bool checkFlag( FlagCombination< FlagType, Type > const & value
		, FlagType const & flag )noexcept
	{
		static_assert( sizeof( FlagType ) == sizeof( Type ), "Can't check flags for different size parameters" );
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
	inline bool checkFlag( FlagCombination< FlagType, Type > const & value
		, Type const & flag )noexcept
	{
		static_assert( sizeof( FlagType ) == sizeof( Type ), "Can't check flags for different size parameters" );
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
	template< typename FlagType, typename Type >
	inline FlagCombination< FlagType, Type > & addFlag( FlagCombination< FlagType, Type > & value
		, FlagType const & flag )noexcept
	{
		static_assert( sizeof( FlagType ) == sizeof( Type ), "Can't add flags for different size parameters" );
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
	template< typename FlagType, typename Type >
	inline FlagCombination< FlagType, Type > & addFlags( FlagCombination< FlagType, Type > & value
		, FlagCombination< FlagType, Type > const & flags )noexcept
	{
		static_assert( sizeof( FlagType ) == sizeof( Type ), "Can't add flags for different size parameters" );
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
	template< typename FlagType, typename Type >
	inline FlagCombination< FlagType, Type > & remFlag( FlagCombination< FlagType, Type > & value
		, FlagType const & flag )noexcept
	{
		static_assert( sizeof( FlagType ) == sizeof( Type ), "Can't remove flags for different size parameters" );
		value &= ~Type( flag );
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
	template< typename FlagType, typename Type >
	inline FlagCombination< FlagType, Type > addFlag( FlagCombination< FlagType, Type > const & value
		, FlagType const & flag )noexcept
	{
		static_assert( sizeof( FlagType ) == sizeof( Type ), "Can't add flags for different size parameters" );
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
	template< typename FlagType, typename Type >
	inline FlagCombination< FlagType, Type > remFlag( FlagCombination< FlagType, Type > const & value
		, FlagType const & flag )noexcept
	{
		static_assert( sizeof( FlagType ) == sizeof( Type ), "Can't remove flags for different size parameters" );
		return value & ~Type( flag );
	}
}

#endif
