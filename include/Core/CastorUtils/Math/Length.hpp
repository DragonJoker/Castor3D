/*
See LICENSE file in root folder
*/
#ifndef ___CU_Length_H___
#define ___CU_Length_H___

#include "CastorUtils/Math/MathModule.hpp"

#include "CastorUtils/Exception/Assertion.hpp"

namespace castor
{
	enum class LengthUnit
	{
		eKilometre,
		eMetre,
		eCentimetre,
		eMillimetre,
		eYard,
		eFoot,
		eInch,
		CU_ScopedEnumBounds( eKilometre )
	};

	inline String getName( LengthUnit value )
	{
		switch ( value )
		{
		case LengthUnit::eKilometre:
			return "km";
		case LengthUnit::eMetre:
			return "m";
		case LengthUnit::eCentimetre:
			return "cm";
		case LengthUnit::eMillimetre:
			return "mm";
		case LengthUnit::eYard:
			return "yd";
		case LengthUnit::eFoot:
			return "ft";
		case LengthUnit::eInch:
			return "in";
		default:
			CU_Failure( "Unsupported LengthUnit" );
			return "Unknown";
		}
	}

	template< typename TypeT >
	class LengthT
	{
	private:
		/**
		 *\~english
		 *\brief		Specified Constructor
		 *\remarks		Private so specified construction only available through named ctors
		 *\~french
		 *\brief		Constructeur spécifié
		 *\remarks		Privé afin que la construction spécifiée ne soit accessible qu'à partir des constructeurs nommés
		 */
		template< typename TypeU >
		LengthT( TypeU value, LengthUnit unit )noexcept
			: m_value( TypeT( value ) )
			, m_unit{ unit }
		{
		}

	public:
		/**
		 *\~english
		 *\brief		Named constructor, expressed in the specified unit.
		 *\param[in]	value	The value.
		 *\return		The length.
		 *\~french
		 *\brief		Constructeur nommé, à partir d'une valeur exprimée dans l'unité donnée.
		 *\param[in]	value	La valeur.
		 *\return		La longueur.
		 */
		template< typename TypeU >
		static LengthT< TypeT > fromUnit( TypeU value
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
		static LengthT< TypeT > fromKilometres( TypeU value )noexcept
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
		static LengthT< TypeT > fromMetres( TypeU value )noexcept
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
		static LengthT< TypeT > fromCentimetres( TypeU value )noexcept
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
		static LengthT< TypeT > fromMillimetres( TypeU value )noexcept
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
		static LengthT< TypeT > fromYards( TypeU value )noexcept
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
		static LengthT< TypeT > fromFeet( TypeU value )noexcept
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
		static LengthT< TypeT > fromInches( TypeU value )noexcept
		{
			return LengthT< TypeT >( value, LengthUnit::eInch );
		}
		/**
		 *\~english
		 *\brief		Default constructor
		 *\~french
		 *\brief		Constructeur par défaut
		 */
		inline explicit LengthT( LengthUnit unit = LengthUnit::eMetre )noexcept;
		/**
		 *\~english
		 *\return		The value converted to given unit.
		 *\~french
		 *\return		La valeur convertie dans l'unité donnée.
		 */
		inline TypeT unit( LengthUnit u )const noexcept;
		/**
		 *\~english
		 *\return		The value converted to kilometres.
		 *\~french
		 *\return		La valeur convertie en kilomètres.
		 */
		inline TypeT kilometres()const noexcept;
		/**
		 *\~english
		 *\return		The value converted to metres.
		 *\~french
		 *\return		La valeur convertie en mètres.
		 */
		inline TypeT metres()const noexcept;
		/**
		 *\~english
		 *\return		The value converted to centimetres.
		 *\~french
		 *\return		La valeur convertie en centimètres.
		 */
		inline TypeT centimetres()const noexcept;
		/**
		 *\~english
		 *\return		The value converted to millimetres.
		 *\~french
		 *\return		La valeur convertie en millimètres.
		 */
		inline TypeT millimetres()const noexcept;
		/**
		 *\~english
		 *\return		The value converted to yards.
		 *\~french
		 *\return		La valeur convertie en yards.
		 */
		inline TypeT yards()const noexcept;
		/**
		 *\~english
		 *\return		The value converted to feet.
		 *\~french
		 *\return		La valeur convertie en pieds.
		 */
		inline TypeT feet()const noexcept;
		/**
		 *\~english
		 *\return		The value converted to inches.
		 *\~french
		 *\return		La valeur convertie en pouces.
		 */
		inline TypeT inches()const noexcept;
		/**
		 *\~english
		 *\param[in]	value	Length expressed in kilometres.
		 *\~french
		 *\param[in]	value	La longueur exprimée en kilomètres.
		 */
		inline void kilometres( double value )noexcept;
		/**
		 *\~english
		 *\param[in]	value	Length expressed in metres.
		 *\~french
		 *\param[in]	value	La longueur exprimée en mètres.
		 */
		inline void metres( double value )noexcept;
		/**
		 *\~english
		 *\param[in]	value	Length expressed in centimetres.
		 *\~french
		 *\param[in]	value	La longueur exprimée en centimètres.
		 */
		inline void centimetres( double value )noexcept;
		/**
		 *\~english
		 *\param[in]	value	Length expressed in millimetres.
		 *\~french
		 *\param[in]	value	La longueur exprimée en millimètres.
		 */
		inline void millimetres( double value )noexcept;
		/**
		 *\~english
		 *\param[in]	value	Length expressed in yards.
		 *\~french
		 *\param[in]	value	La longueur exprimée en yards.
		 */
		inline void yards( double value )noexcept;
		/**
		 *\~english
		 *\param[in]	value	Length expressed in feet.
		 *\~french
		 *\param[in]	value	La longueur exprimée en pieds.
		 */
		inline void feet( double value )noexcept;
		/**
		 *\~english
		 *\param[in]	value	Length expressed in inches.
		 *\~french
		 *\param[in]	value	La longueur exprimée en pouces.
		 */
		inline void inches( double value )noexcept;
		/**
		 *\~english
		 *\name Arithmetic operators.
		 *\~french
		 *\name Opérateurs arithmétiques.
		**/
		/**@{*/
		inline LengthT< TypeT > & operator=( TypeT const & rhs )noexcept;
		inline LengthT< TypeT > & operator-()noexcept;
		inline LengthT< TypeT > & operator+=( LengthT< TypeT > const & rhs )noexcept;
		inline LengthT< TypeT > & operator-=( LengthT< TypeT > const & rhs )noexcept;
		inline LengthT< TypeT > & operator+=( TypeT const & rhs )noexcept;
		inline LengthT< TypeT > & operator-=( TypeT const & rhs )noexcept;
		inline LengthT< TypeT > & operator*=( double rhs )noexcept;
		inline LengthT< TypeT > & operator/=( double rhs )noexcept;
		/**@}*/
		/**
		 *\~english
		*\return		The length value, in its own unit.
		 *\~french
		*\return		La valeur de la longueur, dans son unité.
		*/
		explicit inline operator TypeT const &()const noexcept
		{
			return m_value;
		}

		inline TypeT const & value()const noexcept
		{
			return m_value;
		}

		inline TypeT & operator*()noexcept
		{
			return m_value;
		}

		inline LengthUnit lengthUnit()const noexcept
		{
			return m_unit;
		}

	private:
		TypeT m_value;
		LengthUnit m_unit;

		template< typename TypeU >
		friend bool operator==( LengthT< TypeU > const & lhs, LengthT< TypeU > const & rhs )noexcept;
		template< typename TypeU >
		friend bool operator<( LengthT< TypeU > const & lhs, LengthT< TypeU > const & rhs )noexcept;
		template< typename TypeU >
		friend bool operator>( LengthT< TypeU > const & lhs, LengthT< TypeU > const & rhs )noexcept;
	};

	template< typename TypeT >
	static inline TypeT convert( TypeT const & value
		, LengthUnit from
		, LengthUnit to );
	/**
	 *\~english
	 *\name Logic operators.
	 *\~french
	 *\name Opérateurs logiques.
	**/
	/**@{*/
	template< typename TypeT >
	inline bool operator==( LengthT< TypeT > const & lhs, LengthT< TypeT > const & rhs )noexcept;
	template< typename TypeT >
	inline bool operator!=( LengthT< TypeT > const & lhs, LengthT< TypeT > const & rhs )noexcept;
	template< typename TypeT >
	inline bool operator<( LengthT< TypeT > const & lhs, LengthT< TypeT > const & rhs )noexcept;
	template< typename TypeT >
	inline bool operator>=( LengthT< TypeT > const & lhs, LengthT< TypeT > const & rhs )noexcept;
	template< typename TypeT >
	inline bool operator>( LengthT< TypeT > const & lhs, LengthT< TypeT > const & rhs )noexcept;
	template< typename TypeT >
	inline bool operator<=( LengthT< TypeT > const & lhs, LengthT< TypeT > const & rhs )noexcept;
	/**@}*/
	/**
	 *\~english
	 *\name Arithmetic operators.
	 *\~french
	 *\name Opérateurs arithmétiques.
	**/
	/**@{*/
	template< typename TypeT >
	inline LengthT< TypeT > operator+( LengthT< TypeT > const & lhs, LengthT< TypeT > const & rhs )noexcept;
	template< typename TypeT >
	inline LengthT< TypeT > operator-( LengthT< TypeT > const & lhs, LengthT< TypeT > const & rhs )noexcept;
	template< typename TypeT >
	inline LengthT< TypeT > operator+( TypeT const & lhs, LengthT< TypeT > const & rhs )noexcept;
	template< typename TypeT >
	inline LengthT< TypeT > operator-( TypeT const & lhs, LengthT< TypeT > const & rhs )noexcept;
	template< typename TypeT >
	inline LengthT< TypeT > operator+( LengthT< TypeT > const & lhs, TypeT const & rhs )noexcept;
	template< typename TypeT >
	inline LengthT< TypeT > operator-( LengthT< TypeT > const & lhs, TypeT const & rhs )noexcept;
	template< typename TypeT >
	inline LengthT< TypeT > operator*( LengthT< TypeT > const & lhs, double rhs )noexcept;
	template< typename TypeT >
	inline LengthT< TypeT > operator/( LengthT< TypeT > const & lhs, double rhs )noexcept;
	/**@}*/
}

inline castor::Length operator "" _km( long double value )
{
	return castor::Length::fromKilometres( value );
}

inline castor::Length operator "" _m( long double value )
{
	return castor::Length::fromMetres( value );
}

inline castor::Length operator "" _cm( long double value )
{
	return castor::Length::fromCentimetres( value );
}

inline castor::Length operator "" _mm( long double value )
{
	return castor::Length::fromMillimetres( value );
}

inline castor::Length operator "" _yd( long double value )
{
	return castor::Length::fromYards( value );
}

inline castor::Length operator "" _ft( long double value )
{
	return castor::Length::fromFeet( value );
}

inline castor::Length operator "" _in( long double value )
{
	return castor::Length::fromInches( value );
}

#include "Length.inl"

#endif
