/*
See LICENSE file in root folder
*/
#ifndef ___CU_Length_H___
#define ___CU_Length_H___

#include "CastorUtils/Math/MathModule.hpp"

#include "CastorUtils/Exception/Assertion.hpp"

namespace c3d
{
	namespace details
	{
		template< LengthUnit FromT >
		struct LengthUnitConvertFactors;

		template<>
		struct LengthUnitConvertFactors< LengthUnit::eKilometre >
		{
			static constexpr Array< double, size_t( LengthUnit::eCount ) > factors{ double( 1.0 )
				, double( 1000.0 )
				, double( 100000.0 )
				, double( 1000000.0 )
				, double( 1.0 / 0.0009144 )
				, double( 1.0 / 0.0003048 )
				, double( 1.0 / 0.0000254 ) };
		};

		template<>
		struct LengthUnitConvertFactors< LengthUnit::eMetre >
		{
			static constexpr Array< double, size_t( LengthUnit::eCount ) > factors{ double( 1.0 / 1000.0 )
				, double( 1.0 )
				, double( 100.0 )
				, double( 1000.0 )
				, double( 1.0 / 0.9144 )
				, double( 1.0 / 0.3048 )
				, double( 1.0 / 0.0254 ) };
		};

		template<>
		struct LengthUnitConvertFactors< LengthUnit::eCentimetre >
		{
			static constexpr Array< double, size_t( LengthUnit::eCount ) > factors{ double( 1.0 / 100000.0 )
				, double( 1.0 / 100.0 )
				, double( 1.0 )
				, double( 10.0 )
				, double( 1.0 / 91.44 )
				, double( 1.0 / 30.48 )
				, double( 1.0 / 2.54 ) };
		};

		template<>
		struct LengthUnitConvertFactors< LengthUnit::eMillimetre >
		{
			static constexpr Array< double, size_t( LengthUnit::eCount ) > factors{ double( 1.0 / 1000000.0 )
				, double( 1.0 / 1000.0 )
				, double( 1.0 / 10.0 )
				, double( 1.0 )
				, double( 1.0 / 914.4 )
				, double( 1.0 / 304.8 )
				, double( 1.0 / 25.4 ) };
		};

		template<>
		struct LengthUnitConvertFactors< LengthUnit::eYard >
		{
			static constexpr Array< double, size_t( LengthUnit::eCount ) > factors{ double( 0.0009144 )
				, double( 0.9144 )
				, double( 91.44 )
				, double( 914.4 )
				, double( 1.0 )
				, double( 3.0 )
				, double( 36.0 ) };
		};

		template<>
		struct LengthUnitConvertFactors< LengthUnit::eFoot >
		{
			static constexpr Array< double, size_t( LengthUnit::eCount ) > factors{ double( 0.0003048 )
				, double( 0.3048 )
				, double( 30.48 )
				, double( 304.8 )
				, double( 1.0 / 3.0 )
				, double( 1.0 )
				, double( 12.0 ) };
		};

		template<>
		struct LengthUnitConvertFactors< LengthUnit::eInch >
		{
			static constexpr Array< double, size_t( LengthUnit::eCount ) > factors{ double( 0.0000254 )
				, double( 0.0254 )
				, double( 2.54 )
				, double( 25.4 )
				, double( 1.0 / 36.0 )
				, double( 1.0 / 12.0 )
				, double( 1.0 ) };
		};

		template< LengthUnit FromT, typename TypeT >
		static constexpr TypeT convertTo( TypeT const & value
			, LengthUnit to )noexcept
		{
			using ConvertFactors = LengthUnitConvertFactors< FromT >;

			switch ( to )
			{
			case LengthUnit::eKilometre:
			case LengthUnit::eMetre:
			case LengthUnit::eCentimetre:
			case LengthUnit::eMillimetre:
			case LengthUnit::eYard:
			case LengthUnit::eFoot:
			case LengthUnit::eInch:
				return TypeT( pointCast< double >( value ) * ConvertFactors::factors[size_t( to )] );
			default:
				CU_Failure( "Unsupported length unit for conversion" );
				return value;
			}
		}
	}

	template< typename TypeT >
	class LengthT
	{
	private:
		/**
		 *\~english
		 *\brief		Specified Constructor
		 *\remarks		Private so specified construction only available through named ctors.
		 *\param[in]	value	The length value.
		 *\param[in]	unit	The length unit.
		 *\~french
		 *\brief		Constructeur spécifié
		 *\remarks		Privé afin que la construction spécifiée ne soit accessible qu'à partir des constructeurs nommés.
		 *\param[in]	value	La valeur de la longueur.
		 *\param[in]	unit	L'unité de la longueur.
		 */
		template< typename TypeU >
		constexpr LengthT( TypeU value, LengthUnit unit )noexcept
			: m_value( TypeT( value ) )
			, m_unit{ unit }
		{
		}

	public:
		/**
		 *\~english
		 *\brief		Named constructor, expressed in the specified unit.
		 *\param[in]	value	The value.
		 *\param[in]	unit	The length unit.
		 *\return		The length.
		 *\~french
		 *\brief		Constructeur nommé, à partir d'une valeur exprimée dans l'unité donnée.
		 *\param[in]	value	La valeur.
		 *\param[in]	unit	L'unité de la longueur.
		 *\return		La longueur.
		 */
		template< typename TypeU >
		static constexpr LengthT< TypeT > fromUnit( TypeU value
			, LengthUnit unit )noexcept
		{
			return LengthT< TypeT >( value, unit );
		}
		/**
		 *\~english
		 *\brief		Named constructor, from kilometres.
		 *\param[in]	value	The value.
		 *\return		The length.
		 *\~french
		 *\brief		Constructeur nommé, à partir d'une valeur en kilomètres.
		 *\param[in]	value	La valeur.
		 *\return		La longueur.
		 */
		template< typename TypeU >
		static constexpr LengthT< TypeT > fromKilometres( TypeU value )noexcept
		{
			return LengthT< TypeT >( value, LengthUnit::eKilometre );
		}
		/**
		 *\~english
		 *\brief		Named constructor, from metres.
		 *\param[in]	value	The value.
		 *\return		The length.
		 *\~french
		 *\brief		Constructeur nommé, à partir d'une valeur en mètres.
		 *\param[in]	value	La valeur.
		 *\return		La longueur.
		 */
		template< typename TypeU >
		static constexpr LengthT< TypeT > fromMetres( TypeU value )noexcept
		{
			return LengthT< TypeT >( value, LengthUnit::eMetre );
		}
		/**
		 *\~english
		 *\brief		Named constructor, from centimetres.
		 *\param[in]	value	The value.
		 *\return		The length.
		 *\~french
		 *\brief		Constructeur nommé, à partir d'une valeur en centimètres.
		 *\param[in]	value	La valeur.
		 *\return		La longueur.
		 */
		template< typename TypeU >
		static constexpr LengthT< TypeT > fromCentimetres( TypeU value )noexcept
		{
			return LengthT< TypeT >( value, LengthUnit::eCentimetre );
		}
		/**
		 *\~english
		 *\brief		Named constructor, from millimetres.
		 *\param[in]	value	The value.
		 *\return		The length.
		 *\~french
		 *\brief		Constructeur nommé, à partir d'une valeur en millimètres.
		 *\param[in]	value	La valeur.
		 *\return		La longueur.
		 */
		template< typename TypeU >
		static constexpr LengthT< TypeT > fromMillimetres( TypeU value )noexcept
		{
			return LengthT< TypeT >( value, LengthUnit::eMillimetre );
		}
		/**
		 *\~english
		 *\brief		Named constructor, from yards.
		 *\param[in]	value	The value.
		 *\return		The length.
		 *\~french
		 *\brief		Constructeur nommé, à partir d'une valeur en yards.
		 *\param[in]	value	La valeur.
		 *\return		La longueur.
		 */
		template< typename TypeU >
		static constexpr LengthT< TypeT > fromYards( TypeU value )noexcept
		{
			return LengthT< TypeT >( value, LengthUnit::eYard );
		}
		/**
		 *\~english
		 *\brief		Named constructor, from feet.
		 *\param[in]	value	The value.
		 *\return		The length.
		 *\~french
		 *\brief		Constructeur nommé, à partir d'une valeur en pieds.
		 *\param[in]	value	La valeur.
		 *\return		La longueur.
		 */
		template< typename TypeU >
		static constexpr LengthT< TypeT > fromFeet( TypeU value )noexcept
		{
			return LengthT< TypeT >( value, LengthUnit::eFoot );
		}
		/**
		 *\~english
		 *\brief		Named constructor, from inches.
		 *\param[in]	value	The value.
		 *\return		The length.
		 *\~french
		 *\brief		Constructeur nommé, à partir d'une valeur en pouces.
		 *\param[in]	value	La valeur.
		 *\return		La longueur.
		 */
		template< typename TypeU >
		static constexpr LengthT< TypeT > fromInches( TypeU value )noexcept
		{
			return LengthT< TypeT >( value, LengthUnit::eInch );
		}
		/**
		 *\~english
		 *\brief		Default constructor
		 *\~french
		 *\brief		Constructeur par défaut
		 */
		explicit constexpr LengthT( LengthUnit unit = LengthUnit::eMetre )noexcept
			: LengthT< TypeT >{ TypeT{}, unit }
		{
		}
		/**
		 *\~english
		 *\return		The value converted to given unit.
		 *\~french
		 *\return		La valeur convertie dans l'unité donnée.
		 */
		TypeT unit( LengthUnit u )const noexcept;
		/**
		 *\~english
		 *\return		The value converted to kilometres.
		 *\~french
		 *\return		La valeur convertie en kilomètres.
		 */
		TypeT kilometres()const noexcept;
		/**
		 *\~english
		 *\return		The value converted to metres.
		 *\~french
		 *\return		La valeur convertie en mètres.
		 */
		TypeT metres()const noexcept;
		/**
		 *\~english
		 *\return		The value converted to centimetres.
		 *\~french
		 *\return		La valeur convertie en centimètres.
		 */
		TypeT centimetres()const noexcept;
		/**
		 *\~english
		 *\return		The value converted to millimetres.
		 *\~french
		 *\return		La valeur convertie en millimètres.
		 */
		TypeT millimetres()const noexcept;
		/**
		 *\~english
		 *\return		The value converted to yards.
		 *\~french
		 *\return		La valeur convertie en yards.
		 */
		TypeT yards()const noexcept;
		/**
		 *\~english
		 *\return		The value converted to feet.
		 *\~french
		 *\return		La valeur convertie en pieds.
		 */
		TypeT feet()const noexcept;
		/**
		 *\~english
		 *\return		The value converted to inches.
		 *\~french
		 *\return		La valeur convertie en pouces.
		 */
		TypeT inches()const noexcept;
		/**
		 *\~english
		 *\param[in]	value	Length expressed in kilometres.
		 *\~french
		 *\param[in]	value	La longueur exprimée en kilomètres.
		 */
		void kilometres( double value )noexcept;
		/**
		 *\~english
		 *\param[in]	value	Length expressed in metres.
		 *\~french
		 *\param[in]	value	La longueur exprimée en mètres.
		 */
		void metres( double value )noexcept;
		/**
		 *\~english
		 *\param[in]	value	Length expressed in centimetres.
		 *\~french
		 *\param[in]	value	La longueur exprimée en centimètres.
		 */
		void centimetres( double value )noexcept;
		/**
		 *\~english
		 *\param[in]	value	Length expressed in millimetres.
		 *\~french
		 *\param[in]	value	La longueur exprimée en millimètres.
		 */
		void millimetres( double value )noexcept;
		/**
		 *\~english
		 *\param[in]	value	Length expressed in yards.
		 *\~french
		 *\param[in]	value	La longueur exprimée en yards.
		 */
		void yards( double value )noexcept;
		/**
		 *\~english
		 *\param[in]	value	Length expressed in feet.
		 *\~french
		 *\param[in]	value	La longueur exprimée en pieds.
		 */
		void feet( double value )noexcept;
		/**
		 *\~english
		 *\param[in]	value	Length expressed in inches.
		 *\~french
		 *\param[in]	value	La longueur exprimée en pouces.
		 */
		void inches( double value )noexcept;
		/**
		 *\~english
		 *\name Arithmetic operators.
		 *\~french
		 *\name Opérateurs arithmétiques.
		**/
		/**@{*/
		LengthT< TypeT > & operator=( TypeT const & rhs )noexcept;
		LengthT< TypeT > operator-()const noexcept;
		LengthT< TypeT > & operator+=( LengthT< TypeT > const & rhs )noexcept;
		LengthT< TypeT > & operator-=( LengthT< TypeT > const & rhs )noexcept;
		LengthT< TypeT > & operator+=( TypeT const & rhs )noexcept;
		LengthT< TypeT > & operator-=( TypeT const & rhs )noexcept;
		LengthT< TypeT > & operator*=( double rhs )noexcept;
		LengthT< TypeT > & operator/=( double rhs )noexcept;
		/**@}*/
		/**
		 *\~english
		*\return		The length value, in its own unit.
		 *\~french
		*\return		La valeur de la longueur, dans son unité.
		*/
		explicit operator TypeT const &()const noexcept
		{
			return m_value;
		}

		TypeT const & value()const noexcept
		{
			return m_value;
		}

		TypeT & operator*()noexcept
		{
			return m_value;
		}

		LengthUnit lengthUnit()const noexcept
		{
			return m_unit;
		}

	private:
		TypeT m_value;
		LengthUnit m_unit;

	private:
		static constexpr TypeT convert( TypeT const & value
			, LengthUnit from
			, LengthUnit to )noexcept
		{
			switch ( from )
			{
			case LengthUnit::eKilometre:
				return details::convertTo< LengthUnit::eKilometre >( value, to );
			case LengthUnit::eMetre:
				return details::convertTo< LengthUnit::eMetre >( value, to );
			case LengthUnit::eCentimetre:
				return details::convertTo< LengthUnit::eCentimetre >( value, to );
			case LengthUnit::eMillimetre:
				return details::convertTo< LengthUnit::eMillimetre >( value, to );
			case LengthUnit::eYard:
				return details::convertTo< LengthUnit::eYard >( value, to );
			case LengthUnit::eFoot:
				return details::convertTo< LengthUnit::eFoot >( value, to );
			case LengthUnit::eInch:
				return details::convertTo< LengthUnit::eInch >( value, to );
			default:
				CU_Failure( "Unsupported length unit for conversion" );
				return value;
			}
		}
		/**
		 *\~english
		 *\name Logic operators.
		 *\~french
		 *\name Opérateurs logiques.
		**/
		/**@{*/
		friend bool operator==( LengthT const & lhs, LengthT const & rhs )noexcept
		{
			return std::abs( lhs.m_value - convert( rhs.m_value, rhs.m_unit, lhs.m_unit ) ) < std::numeric_limits< TypeT >::epsilon();
		}

		friend bool operator!=( LengthT const & lhs, LengthT const & rhs )noexcept
		{
			return !( lhs == rhs );
		}

		friend bool operator<( LengthT const & lhs, LengthT const & rhs )noexcept
		{
			return lhs.m_value < convert( rhs.m_value, rhs.m_unit, lhs.m_unit );
		}

		friend bool operator>=( LengthT const & lhs, LengthT const & rhs )noexcept
		{
			return !( lhs < rhs );
		}

		friend bool operator>( LengthT const & lhs, LengthT const & rhs )noexcept
		{
			return lhs.m_value > convert( rhs.m_value, rhs.m_unit, lhs.m_unit );
		}

		friend bool operator<=( LengthT const & lhs, LengthT const & rhs )noexcept
		{
			return !( lhs > rhs );
		}
		/**@}*/
		/**
		 *\~english
		 *\name Arithmetic operators.
		 *\~french
		 *\name Opérateurs arithmétiques.
		**/
		/**@{*/
		friend LengthT operator+( LengthT const & lhs, LengthT const & rhs )noexcept
		{
			LengthT result{ lhs };
			result += rhs;
			return result;
		}

		friend LengthT operator-( LengthT const & lhs, LengthT const & rhs )noexcept
		{
			LengthT result{ lhs };
			result -= rhs;
			return result;
		}

		friend LengthT operator+( TypeT const & lhs, LengthT const & rhs )noexcept
		{
			return LengthT::fromUnit( lhs, rhs.lengthUnit() ) + rhs;
		}

		friend LengthT operator-( TypeT const & lhs, LengthT const & rhs )noexcept
		{
			return LengthT::fromUnit( lhs, rhs.lengthUnit() ) - rhs;
		}

		friend LengthT operator+( LengthT const & lhs, TypeT const & rhs )noexcept
		{
			LengthT result{ lhs };
			result += rhs;
			return result;
		}

		friend LengthT operator-( LengthT const & lhs, TypeT const & rhs )noexcept
		{
			LengthT result{ lhs };
			result -= rhs;
			return result;
		}

		friend LengthT operator*( LengthT const & lhs, double rhs )noexcept
		{
			LengthT result{ lhs };
			result *= rhs;
			return result;
		}

		friend LengthT operator/( LengthT const & lhs, double rhs )noexcept
		{
			LengthT result{ lhs };
			result /= rhs;
			return result;
		}
		/**@}*/
	};

	template< typename TypeT >
	static constexpr TypeT convert( TypeT const & value
		, LengthUnit from
		, LengthUnit to )noexcept;
}

inline c3d::Length operator ""_km( long double value )
{
	return c3d::Length::fromKilometres( value );
}

inline c3d::Length operator ""_m( long double value )
{
	return c3d::Length::fromMetres( value );
}

inline c3d::Length operator ""_cm( long double value )
{
	return c3d::Length::fromCentimetres( value );
}

inline c3d::Length operator ""_mm( long double value )
{
	return c3d::Length::fromMillimetres( value );
}

inline c3d::Length operator ""_yd( long double value )
{
	return c3d::Length::fromYards( value );
}

inline c3d::Length operator ""_ft( long double value )
{
	return c3d::Length::fromFeet( value );
}

inline c3d::Length operator ""_in( long double value )
{
	return c3d::Length::fromInches( value );
}

#include "Length.inl"

#endif
