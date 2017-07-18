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
#ifndef ___CU_FlagCombination_H___
#define ___CU_FlagCombination_H___

#include "CastorUtilsPrerequisites.hpp"

namespace Castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		23/11/2016
	\~english
	\brief		Templated class that retrieves the integral base type from a size.
	\~french
	\brief		Classe template qui récupère le type entier de base depuis une taille.
	*/
	template< size_t Size > struct BaseTypeFromSize;
	/*!
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
	/*!
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
	/*!
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
	/*!
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
	/*!
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
		 *\param[in]	p_value	The value.
		 *\~french
		 *\brief		Constructeur depuis le type d'indicateur.
		 *\param[in]	p_value	La valeur.
		 */
		inline FlagCombination( FlagType p_value )noexcept
			: m_value{ BaseType( p_value ) }
		{
			static_assert( sizeof( FlagType ) == sizeof( BaseType ), "Can't combine different size parameters" );
		}
		/**
		 *\~english
		 *\brief		Constructor from combined flags.
		 *\param[in]	p_value	The value.
		 *\~french
		 *\brief		Constructeur depuis des indicateurs combinés.
		 *\param[in]	p_value	La valeur.
		 */
		inline FlagCombination( BaseType p_value = BaseType{} )noexcept
			: m_value{ p_value }
		{
			static_assert( sizeof( FlagType ) == sizeof( BaseType ), "Can't combine different size parameters" );
		}
		/**
		 *\~english
		 *\brief		Binary AND assignment operator.
		 *\param[in]	p_rhs	The value.
		 *\return		A reference to this object.
		 *\~french
		 *\brief		Opérateur d'affectation par ET binaire.
		 *\param[in]	p_rhs	La valeur.
		 *\return		Une référence sur cet objet.
		 */
		inline FlagCombination & operator&=( BaseType p_rhs )noexcept
		{
			m_value &= p_rhs;
			return *this;
		}
		/**
		 *\~english
		 *\brief		Binary OR assignment operator.
		 *\param[in]	p_rhs	The value.
		 *\return		A reference to this object.
		 *\~french
		 *\brief		Opérateur d'affectation par OU binaire.
		 *\param[in]	p_rhs	La valeur.
		 *\return		Une référence sur cet objet.
		 */
		inline FlagCombination & operator|=( BaseType p_rhs )noexcept
		{
			m_value |= p_rhs;
			return *this;
		}
		/**
		 *\~english
		 *\brief		Binary AND assignment operator.
		 *\param[in]	p_rhs	The value.
		 *\return		A reference to this object.
		 *\~french
		 *\brief		Opérateur d'affectation par ET binaire.
		 *\param[in]	p_rhs	La valeur.
		 *\return		Une référence sur cet objet.
		 */
		inline FlagCombination & operator&=( FlagType p_rhs )noexcept
		{
			m_value &= BaseType( p_rhs );
			return *this;
		}
		/**
		 *\~english
		 *\brief		Binary OR assignment operator.
		 *\param[in]	p_rhs	The value.
		 *\return		A reference to this object.
		 *\~french
		 *\brief		Opérateur d'affectation par OU binaire.
		 *\param[in]	p_rhs	La valeur.
		 *\return		Une référence sur cet objet.
		 */
		inline FlagCombination & operator|=( FlagType p_rhs )noexcept
		{
			m_value |= BaseType( p_rhs );
			return *this;
		}
		/**
		 *\~english
		 *\brief		Binary AND assignment operator.
		 *\param[in]	p_rhs	The value.
		 *\return		A reference to this object.
		 *\~french
		 *\brief		Opérateur d'affectation par ET binaire.
		 *\param[in]	p_rhs	La valeur.
		 *\return		Une référence sur cet objet.
		 */
		inline FlagCombination & operator&=( FlagCombination< FlagType, BaseType > const & p_rhs )noexcept
		{
			m_value &= BaseType( p_rhs );
			return *this;
		}
		/**
		 *\~english
		 *\brief		Binary OR assignment operator.
		 *\param[in]	p_rhs	The value.
		 *\return		A reference to this object.
		 *\~french
		 *\brief		Opérateur d'affectation par OU binaire.
		 *\param[in]	p_rhs	La valeur.
		 *\return		Une référence sur cet objet.
		 */
		inline FlagCombination & operator|=( FlagCombination< FlagType, BaseType > const & p_rhs )noexcept
		{
			m_value |= BaseType( p_rhs );
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
	 *\param[in]	p_lhs, p_rhs	The operands.
	 *\~french
	 *\brief		Opérateur d'égalité.
	 *\param[in]	p_lhs, p_rhs	Les opérandes.
	 */
	template< typename FlagType, typename Type >
	inline bool operator==( FlagCombination< FlagType, Type > const & p_lhs, FlagCombination< FlagType, Type > const & p_rhs )noexcept
	{
		return Type( p_lhs ) == Type( p_rhs );
	}
	/**
	 *\~english
	 *\brief		Equality operator.
	 *\param[in]	p_lhs, p_rhs	The operands.
	 *\~french
	 *\brief		Opérateur d'égalité.
	 *\param[in]	p_lhs, p_rhs	Les opérandes.
	 */
	template< typename FlagType, typename Type >
	inline bool operator==( FlagCombination< FlagType, Type > const & p_lhs, FlagType const & p_rhs )noexcept
	{
		return Type( p_lhs ) == Type( p_rhs );
	}
	/**
	 *\~english
	 *\brief		Difference operator.
	 *\param[in]	p_lhs, p_rhs	The operands.
	 *\~french
	 *\brief		Opérateur de différence.
	 *\param[in]	p_lhs, p_rhs	Les opérandes.
	 */
	template< typename FlagType, typename Type >
	inline bool operator!=( FlagCombination< FlagType, Type > const & p_lhs, FlagCombination< FlagType, Type > const & p_rhs )noexcept
	{
		return Type( p_lhs ) != Type( p_rhs );
	}
	/**
	 *\~english
	 *\brief		Difference operator.
	 *\param[in]	p_lhs, p_rhs	The operands.
	 *\~french
	 *\brief		Opérateur de différence.
	 *\param[in]	p_lhs, p_rhs	Les opérandes.
	 */
	template< typename FlagType, typename Type >
	inline bool operator!=( FlagCombination< FlagType, Type > const & p_lhs, FlagType const & p_rhs )noexcept
	{
		return Type( p_lhs ) != Type( p_rhs );
	}
	/**
	 *\~english
	 *\brief		Binary AND operator.
	 *\param[in]	p_lhs, p_rhs	The operands.
	 *\return		The resulting flags combination.
	 *\~french
	 *\brief		Opérateur ET binaire.
	 *\param[in]	p_lhs, p_rhs	Les opérandes.
	 *\return		La combinaison de flags résultante.
	 */
	template< typename FlagType, typename Type >
	inline FlagCombination< FlagType, Type > operator&( FlagCombination< FlagType, Type > const & p_lhs, FlagType const & p_rhs )noexcept
	{
		static_assert( sizeof( FlagType ) == sizeof( Type ), "Can't apply binary AND on different size parameters" );
		FlagCombination< FlagType, Type > result{ p_lhs };
		return result &= p_rhs;
	}
	/**
	 *\~english
	 *\brief		Binary OR operator.
	 *\param[in]	p_lhs, p_rhs	The operands.
	 *\return		The resulting flags combination.
	 *\~french
	 *\brief		Opérateur OU binaire.
	 *\param[in]	p_lhs, p_rhs	Les opérandes.
	 *\return		La combinaison de flags résultante.
	 */
	template< typename FlagType, typename Type >
	inline FlagCombination< FlagType, Type > operator|( FlagCombination< FlagType, Type > const & p_lhs, FlagType const & p_rhs )noexcept
	{
		static_assert( sizeof( FlagType ) == sizeof( Type ), "Can't apply binary OR on different size parameters" );
		FlagCombination< FlagType, Type > result{ p_lhs };
		return result |= p_rhs;
	}
	/**
	 *\~english
	 *\param[in]	p_value	The value.
	 *\param[in]	p_flag	The flag looked for.
	 *\return		\p true if p_flags contain p_flag.
	 *\~french
	 *\param[in]	p_value	La valeur.
	 *\param[in]	p_flag	L'indicateur recherché.
	 *\return		\p true si p_flags contient p_flag.
	 */
	template< typename T, typename U >
	inline bool CheckFlag( T const & p_value, U const & p_flag )noexcept
	{
		static_assert( sizeof( T ) == sizeof( U ), "Can't check flags for different size parameters" );
		return U( p_value & T( p_flag ) ) == p_flag;;
	}
	/**
	 *\~english
	 *\param[in]	p_value	The value.
	 *\param[in]	p_flag	The flag looked for.
	 *\return		\p true if p_flags contain p_flag.
	 *\~french
	 *\param[in]	p_value	La valeur.
	 *\param[in]	p_flag	L'indicateur recherché.
	 *\return		\p true si p_flags contient p_flag.
	 */
	template< typename FlagType, typename Type >
	inline bool CheckFlag( FlagCombination< FlagType, Type > const & p_value, FlagType const & p_flag )noexcept
	{
		static_assert( sizeof( FlagType ) == sizeof( Type ), "Can't check flags for different size parameters" );
		return Type( p_value & p_flag ) == Type( p_flag );
	}
	/**
	 *\~english
	 *\param[in]	p_value	The value.
	 *\param[in]	p_flag	The flag looked for.
	 *\return		\p true if p_flags contain p_flag.
	 *\~french
	 *\param[in]	p_value	La valeur.
	 *\param[in]	p_flag	L'indicateur recherché.
	 *\return		\p true si p_flags contient p_flag.
	 */
	template< typename FlagType, typename Type >
	inline bool CheckFlag( FlagCombination< FlagType, Type > const & p_value, Type const & p_flag )noexcept
	{
		static_assert( sizeof( FlagType ) == sizeof( Type ), "Can't check flags for different size parameters" );
		return Type( p_value & p_flag ) == p_flag;
	}
	/**
	 *\~english
	 *\brief			Adds a flag to the given value.
	 *\param[in,out]	p_value	The value.
	 *\param[in]		p_flag	The flag to add.
	 *\return			The value.
	 *\~french
	 *\brief			Ajoute un indicateur à la valeur donnée.
	 *\param[in,out]	p_value	La valeur.
	 *\param[in]		p_flag	L'indicateur à ajouter.
	 *\return			La valeur.
	 */
	template< typename FlagType, typename Type >
	inline FlagCombination< FlagType, Type > & AddFlag( FlagCombination< FlagType, Type > & p_value, FlagType const & p_flag )noexcept
	{
		static_assert( sizeof( FlagType ) == sizeof( Type ), "Can't add flags for different size parameters" );
		p_value |= p_flag;
		return p_value;
	}
	/**
	 *\~english
	 *\brief			Adds a flag combination to the given value.
	 *\param[in,out]	p_value	The value.
	 *\param[in]		p_flags	The flag combination to add.
	 *\return			The value.
	 *\~french
	 *\brief			Ajoute une combinaison d'indicateurs à la valeur donnée.
	 *\param[in,out]	p_value	La valeur.
	 *\param[in]		p_flags	La combinaison d'indicateurs à ajouter.
	 *\return			La valeur.
	 */
	template< typename FlagType, typename Type >
	inline FlagCombination< FlagType, Type > & AddFlags( FlagCombination< FlagType, Type > & p_value, FlagCombination< FlagType, Type > const & p_flags )noexcept
	{
		static_assert( sizeof( FlagType ) == sizeof( Type ), "Can't add flags for different size parameters" );
		p_value |= p_flags;
		return p_value;
	}
	/**
	 *\~english
	 *\brief			Removes a flag from the given value.
	 *\param[in,out]	p_value	The value.
	 *\param[in]		p_flag	The flag to remove.
	 *\return			The value.
	 *\~french
	 *\brief			Enlève un indicateur de la valeur donnée.
	 *\param[in,out]	p_value	La valeur.
	 *\param[in]		p_flag	L'indicateur à enlever.
	 *\return			La valeur.
	 */
	template< typename FlagType, typename Type >
	inline FlagCombination< FlagType, Type > & RemFlag( FlagCombination< FlagType, Type > & p_value, FlagType const & p_flag )noexcept
	{
		static_assert( sizeof( FlagType ) == sizeof( Type ), "Can't remove flags for different size parameters" );
		p_value &= ~Type( p_flag );
		return p_value;
	}
	/**
	 *\~english
	 *\brief		Adds a flag to the given value.
	 *\param[in]	p_value	The value.
	 *\param[in]	p_flag	The flag to add.
	 *\return		The new value.
	 *\~french
	 *\brief		Ajoute un indicateur à la valeur donnée.
	 *\param[in]	p_value	La valeur.
	 *\param[in]	p_flag	L'indicateur à ajouter.
	 *\return		La nouvelle valeur.
	 */
	template< typename FlagType, typename Type >
	inline FlagCombination< FlagType, Type > AddFlag( FlagCombination< FlagType, Type > const & p_value, FlagType const & p_flag )noexcept
	{
		static_assert( sizeof( FlagType ) == sizeof( Type ), "Can't add flags for different size parameters" );
		return ( p_value | p_flag );
	}
	/**
	 *\~english
	 *\brief		Removes a flag from the given value.
	 *\param[in]	p_value	The value.
	 *\param[in]	p_flag	The flag to remove.
	 *\return		The new value.
	 *\~french
	 *\brief		Enlève un indicateur de la valeur donnée.
	 *\param[in]	p_value	La valeur.
	 *\param[in]	p_flag	L'indicateur à enlever.
	 *\return		La nouvelle valeur.
	 */
	template< typename FlagType, typename Type >
	inline FlagCombination< FlagType, Type > RemFlag( FlagCombination< FlagType, Type > const & p_value, FlagType const & p_flag )noexcept
	{
		static_assert( sizeof( FlagType ) == sizeof( Type ), "Can't remove flags for different size parameters" );
		return p_value & ~Type( p_flag );
	}
}

#endif
