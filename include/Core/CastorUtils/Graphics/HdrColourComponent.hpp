/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_HDR_COLOUR_COMPONENT_H___
#define ___CASTOR_HDR_COLOUR_COMPONENT_H___

#include "CastorUtils/Graphics/GraphicsModule.hpp"

namespace c3d
{
	class HdrColourComponentValue
	{
	public:
		constexpr HdrColourComponentValue()noexcept = default;
		/**
		 *\~english
		 *\brief		Constructor from normalized component.
		 *\param[in]	rhs		The normalized component value.
		 *\param[in]	gamma	The gamma correction value.
		 *\~french
		 *\brief		Constructeur depuis une componsante normalisée.
		 *\param[in]	rhs		La valeur de la composante normalisée.
		 *\param[in]	gamma	La valeur de la correction gamma.
		 */
		CU_API explicit HdrColourComponentValue( ColourComponentValue const & rhs
			, float gamma = 2.2f )noexcept;
		/**
		 *\~english
		 *\brief		Specified constructor
		 *\param[in]	value	The component value
		 *\~french
		 *\brief		Constructeur spécifié
		 *\param[in]	value	La valeur de la composante
		 */
		explicit constexpr HdrColourComponentValue( float value )noexcept
			: m_component{ value }
		{
		}
		/**
		 *\~english
		 *\brief		Assignment operator from float
		 *\param[in]	rhs	The component value
		 *\return		Reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation à partir d'un float
		 *\param[in]	rhs	La valeur de la composante
		 *\return		Référence sur cet objet
		 */
		HdrColourComponentValue & operator=( float rhs )noexcept
		{
			m_component = rhs;
			return *this;
		}
		/**
		 *\~english
		 *\brief		Assignment operator from double
		 *\param[in]	rhs	The component value
		 *\return		Reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation à partir d'un double
		 *\param[in]	rhs	La valeur de la composante
		 *\return		Référence sur cet objet
		 */
		HdrColourComponentValue & operator=( double rhs )noexcept
		{
			m_component = float( rhs );
			return *this;
		}
		/**
		 *\~english
		 *\brief		Assignment operator from long double
		 *\param[in]	rhs	The component value
		 *\return		Reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation à partir d'un long double
		 *\param[in]	rhs	La valeur de la composante
		 *\return		Référence sur cet objet
		 */
		HdrColourComponentValue & operator=( long double rhs )noexcept
		{
			m_component = float( rhs );
			return *this;
		}
		/**
		 *\~english
		 *\brief		Assignment operator from int8_t
		 *\param[in]	rhs	The component value
		 *\return		Reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation à partir d'un int8_t
		 *\param[in]	rhs	La valeur de la composante
		 *\return		Référence sur cet objet
		 */
		HdrColourComponentValue & operator=( int8_t rhs )noexcept
		{
			m_component = float( uint8_t( rhs ) );
			return *this;
		}
		/**
		 *\~english
		 *\brief		Assignment operator from uint8_t
		 *\param[in]	rhs	The component value
		 *\return		Reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation à partir d'un uint8_t
		 *\param[in]	rhs	La valeur de la composante
		 *\return		Référence sur cet objet
		 */
		HdrColourComponentValue & operator=( uint8_t rhs )noexcept
		{
			m_component = float( rhs );
			return *this;
		}
		/**
		 *\~english
		 *\brief		Assignment operator from int16_t
		 *\param[in]	rhs	The component value
		 *\return		Reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation à partir d'un int16_t
		 *\param[in]	rhs	La valeur de la composante
		 *\return		Référence sur cet objet
		 */
		HdrColourComponentValue & operator=( int16_t rhs )noexcept
		{
			m_component = float( rhs );
			return *this;
		}
		/**
		 *\~english
		 *\brief		Assignment operator from uint16_t
		 *\param[in]	rhs	The component value
		 *\return		Reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation à partir d'un uint16_t
		 *\param[in]	rhs	La valeur de la composante
		 *\return		Référence sur cet objet
		 */
		HdrColourComponentValue & operator=( uint16_t rhs )noexcept
		{
			m_component = float( rhs );
			return *this;
		}
		/**
		 *\~english
		 *\brief		Assignment operator from int32_t
		 *\param[in]	rhs	The component value
		 *\return		Reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation à partir d'un int32_t
		 *\param[in]	rhs	La valeur de la composante
		 *\return		Référence sur cet objet
		 */
		HdrColourComponentValue & operator=( int32_t rhs )noexcept
		{
			m_component = float( rhs );
			return *this;
		}
		/**
		 *\~english
		 *\brief		Assignment operator from uint32_t
		 *\param[in]	rhs	The component value
		 *\return		Reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation à partir d'un uint32_t
		 *\param[in]	rhs	La valeur de la composante
		 *\return		Référence sur cet objet
		 */
		HdrColourComponentValue & operator=( uint32_t rhs )noexcept
		{
			m_component = float( rhs );
			return *this;
		}
		/**
		 *\~english
		 *\brief		Assignment operator from int64_t
		 *\param[in]	rhs	The component value
		 *\return		Reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation à partir d'un int64_t
		 *\param[in]	rhs	La valeur de la composante
		 *\return		Référence sur cet objet
		 */
		HdrColourComponentValue & operator=( int64_t rhs )noexcept
		{
			m_component = float( rhs );
			return *this;
		}
		/**
		 *\~english
		 *\brief		Assignment operator from uint64_t
		 *\param[in]	rhs	The component value
		 *\return		Reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation à partir d'un uint64_t
		 *\param[in]	rhs	La valeur de la composante
		 *\return		Référence sur cet objet
		 */
		HdrColourComponentValue & operator=( uint64_t rhs )noexcept
		{
			m_component = float( rhs );
			return *this;
		}
		/**
		 *\~english
		 *\brief		int8_t conversion operator
		 *\~french
		 *\brief		Opérateur de conversion vers int8_t
		 */
		int8_t & convertTo( int8_t & v )const noexcept
		{
			return v = int8_t( value() );
		}
		/**
		 *\~english
		 *\brief		uint8_t conversion operator
		 *\~french
		 *\brief		Opérateur de conversion vers uint8_t
		 */
		uint8_t & convertTo( uint8_t & v )const noexcept
		{
			return v = uint8_t( value() );
		}
		/**
		 *\~english
		 *\brief		int16_t conversion operator
		 *\~french
		 *\brief		Opérateur de conversion vers int16_t
		 */
		int16_t & convertTo( int16_t & v )const noexcept
		{
			return v = int16_t( value() );
		}
		/**
		 *\~english
		 *\brief		uint16_t conversion operator
		 *\~french
		 *\brief		Opérateur de conversion vers uint16_t
		 */
		uint16_t & convertTo( uint16_t & v )const noexcept
		{
			return v = uint16_t( value() );
		}
		/**
		 *\~english
		 *\brief		int32_t conversion operator
		 *\~french
		 *\brief		Opérateur de conversion vers int32_t
		 */
		int32_t & convertTo( int32_t & v )const noexcept
		{
			return v = int32_t( value() );
		}
		/**
		 *\~english
		 *\brief		uint32_t conversion operator
		 *\~french
		 *\brief		Opérateur de conversion vers uint32_t
		 */
		uint32_t & convertTo( uint32_t & v )const noexcept
		{
			return v = uint32_t( value() );
		}
		/**
		 *\~english
		 *\brief		int64_t conversion operator
		 *\~french
		 *\brief		Opérateur de conversion vers int64_t
		 */
		int64_t & convertTo( int64_t & v )const noexcept
		{
			return v = int64_t( value() );
		}
		/**
		 *\~english
		 *\brief		uint64_t conversion operator
		 *\~french
		 *\brief		Opérateur de conversion vers uint64_t
		 */
		uint64_t & convertTo( uint64_t & v )const noexcept
		{
			return v = uint64_t( value() );
		}
		/**
		 *\~english
		 *\brief		float conversion operator
		 *\~french
		 *\brief		Opérateur de conversion vers float
		 */
		float & convertTo( float & v )const noexcept
		{
			return v = value();
		}
		/**
		 *\~english
		 *\brief		float conversion operator
		 *\~french
		 *\brief		Opérateur de conversion vers float
		 */
		double & convertTo( double & v )const noexcept
		{
			return v = value();
		}
		/**
		 *\~english
		 *\brief		float conversion operator
		 *\~french
		 *\brief		Opérateur de conversion vers float
		 */
		long double & convertTo( long double & v )const noexcept
		{
			return v = value();
		}
		/**
		 *\~english
		 *\brief		addition assignment operator
		 *\remarks		Clamps the result
		 *\param[in]	rhs	The value to add
		 *\return		Result of this + rhs
		 *\~french
		 *\brief		Opérateur d'affectation par addition
		 *\remarks		Clampe le résultat
		 *\param[in]	rhs	La valeur à ajouter
		 *\return		Resultat de this + rhs
		 */
		HdrColourComponentValue & operator+=( HdrColourComponentValue const & rhs )noexcept
		{
			m_component += rhs.value();
			return *this;
		}
		/**
		 *\~english
		 *\brief		Substraction assignment operator
		 *\remarks		Clamps the result
		 *\param[in]	rhs	The value to subtract
		 *\return		Result of this - rhs
		 *\~french
		 *\brief		Opérateur d'affectation par soustraction
		 *\remarks		Clampe le résultat
		 *\param[in]	rhs	La valeur à soustraire
		 *\return		Resultat de this - rhs
		 */
		HdrColourComponentValue & operator-=( HdrColourComponentValue const & rhs )noexcept
		{
			m_component -= rhs.value();
			return *this;
		}
		/**
		 *\~english
		 *\brief		Multiplication assignment operator
		 *\remarks		Clamps the result
		 *\param[in]	rhs	The value to multiply
		 *\return		Result of this * rhs
		 *\~french
		 *\brief		Opérateur d'affectation par multiplication
		 *\remarks		Clampe le résultat
		 *\param[in]	rhs	La valeur à multiplier
		 *\return		Resultat de this * rhs
		 */
		HdrColourComponentValue & operator*=( HdrColourComponentValue const & rhs )noexcept
		{
			m_component *= rhs.value();
			return *this;
		}
		/**
		 *\~english
		 *\brief		Division assignment operator
		 *\remarks		Clamps the result
		 *\param[in]	rhs	The value to divide
		 *\return		Result of this / rhs
		 *\~french
		 *\brief		Opérateur d'affectation par division
		 *\remarks		Clampe le résultat
		 *\param[in]	rhs	La valeur à diviser
		 *\return		Resultat de this / rhs
		 */
		HdrColourComponentValue & operator/=( HdrColourComponentValue const & rhs )noexcept
		{
			m_component /= rhs.value();
			return *this;
		}
		/**
		 *\~english
		 *\brief		addition assignment operator
		 *\param[in]	rhs	The value to add
		 *\return		Result of this + rhs
		 *\~french
		 *\brief		Opérateur d'affectation par addition
		 *\param[in]	rhs	La valeur à ajouter
		 *\return		Resultat de this + rhs
		 */
		template< typename T >
		HdrColourComponentValue & operator+=( T const & rhs )noexcept
		{
			m_component = float( m_component + rhs );
			return *this;
		}
		/**
		 *\~english
		 *\brief		Substraction assignment operator
		 *\param[in]	rhs	The value to subtract
		 *\return		Result of this - rhs
		 *\~french
		 *\brief		Opérateur d'affectation par soustraction
		 *\param[in]	rhs	La valeur à soustraire
		 *\return		Resultat de this - rhs
		 */
		template< typename T >
		HdrColourComponentValue & operator-=( T const & rhs )noexcept
		{
			m_component = float( m_component - rhs );
			return *this;
		}
		/**
		 *\~english
		 *\brief		Multiplication assignment operator
		 *\param[in]	rhs	The value to multiply
		 *\return		Result of this * rhs
		 *\~french
		 *\brief		Opérateur d'affectation par multiplication
		 *\param[in]	rhs	La valeur à multiplier
		 *\return		Resultat de this * rhs
		 */
		template< typename T >
		HdrColourComponentValue & operator*=( T const & rhs )noexcept
		{
			m_component = float( m_component * rhs );
			return *this;
		}
		/**
		 *\~english
		 *\brief		Division assignment operator
		 *\param[in]	rhs	The value to divide
		 *\return		Result of this / rhs
		 *\~french
		 *\brief		Opérateur d'affectation par division
		 *\param[in]	rhs	La valeur à diviser
		 *\return		Resultat de this / rhs
		 */
		template< typename T >
		HdrColourComponentValue & operator/=( T const & rhs )noexcept
		{
			m_component = float( m_component / rhs );
			return *this;
		}
		/**
		 *\~english
		 *\brief		Implicit cast operator
		 *\return		The value
		 *\~french
		 *\brief		Opérateur de conversion implicite
		 *\return		La valeur
		 */
		constexpr operator float()const noexcept
		{
			return m_component;
		}
		/**
		 *\~english
		 *\brief		Retrieves the component value
		 *\return		The value
		 *\~french
		 *\brief		Récupère la valeur de la composante
		 *\return		La valeur
		 */
		constexpr float const & value()const noexcept
		{
			return m_component;
		}
		/**
		 *\~english
		 *\brief		Retrieves the component value
		 *\return		The value
		 *\~french
		 *\brief		Récupère la valeur de la composante
		 *\return		La valeur
		 */
		float & value()noexcept
		{
			return m_component;
		}

	private:
		float m_component{};

		friend bool operator==( HdrColourComponentValue const & lhs, HdrColourComponentValue const & rhs )noexcept
		{
			float uiA;
			float uiB;
			lhs.convertTo( uiA );
			rhs.convertTo( uiB );
			return uiA == uiB;
		}

		friend float operator-( HdrColourComponentValue const & lhs, HdrColourComponentValue const & rhs )noexcept
		{
			float fValue;
			lhs.convertTo( fValue );
			HdrColourComponentValue cpnt( fValue );
			cpnt -= rhs;
			return cpnt;
		}

		friend float operator+( HdrColourComponentValue const & lhs, HdrColourComponentValue const & rhs )noexcept
		{
			float fValue;
			lhs.convertTo( fValue );
			HdrColourComponentValue cpnt( fValue );
			cpnt += rhs;
			return cpnt;
		}

		friend float operator*( HdrColourComponentValue const & lhs, HdrColourComponentValue const & rhs )noexcept
		{
			float fValue;
			lhs.convertTo( fValue );
			HdrColourComponentValue cpnt( fValue );
			cpnt *= rhs;
			return cpnt;
		}

		friend float operator/( HdrColourComponentValue const & lhs, HdrColourComponentValue const & rhs )noexcept
		{
			float fValue;
			lhs.convertTo( fValue );
			HdrColourComponentValue cpnt( fValue );
			cpnt /= rhs;
			return cpnt;
		}

		template< typename T >
		friend float operator+( HdrColourComponentValue const & lhs, T const & rhs )noexcept
		{
			float value;
			lhs.convertTo( value );
			HdrColourComponentValue cpnt( value );
			cpnt += rhs;
			return value;
		}

		template< typename T >
		friend float operator-( HdrColourComponentValue const & lhs, T const & rhs )noexcept
		{
			float value;
			lhs.convertTo( value );
			HdrColourComponentValue cpnt( value );
			cpnt -= rhs;
			return value;
		}

		template< typename T >
		friend float operator/( HdrColourComponentValue const & lhs, T const & rhs )noexcept
		{
			float value;
			lhs.convertTo( value );
			HdrColourComponentValue cpnt( value );
			cpnt /= rhs;
			return value;
		}

		template< typename T >
		friend float operator*( HdrColourComponentValue const & lhs, T const & rhs )noexcept
		{
			float value;
			lhs.convertTo( value );
			HdrColourComponentValue cpnt( value );
			cpnt *= rhs;
			return value;
		}
	};
	static_assert( sizeof( HdrColourComponentValue ) == sizeof( float ) );
}

#endif
