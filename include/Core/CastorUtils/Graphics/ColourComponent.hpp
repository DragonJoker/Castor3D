/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_COLOUR_COMPONENT_H___
#define ___CASTOR_COLOUR_COMPONENT_H___

#include "CastorUtils/Graphics/GraphicsModule.hpp"

namespace c3d
{
	class ColourComponentValue
	{
	public:
		constexpr ColourComponentValue() = default;
		/**
		 *\~english
		 *\brief		Constructor from HDR component.
		 *\param[in]	rhs		The HDR component value.
		 *\param[in]	gamma	The gamma correction value.
		 *\~french
		 *\brief		Constructeur depuis une componsante HDR.
		 *\param[in]	rhs		La valeur de la composante HDR.
		 *\param[in]	gamma	La valeur de la correction gamma.
		 */
		CU_API explicit ColourComponentValue( HdrColourComponentValue const & rhs
			, float gamma = 2.2f );
		/**
		 *\~english
		 *\brief		Specified constructor
		 *\param[in]	value	The component value
		 *\~french
		 *\brief		Constructeur spécifié
		 *\param[in]	value	La valeur de la composante
		 */
		explicit constexpr ColourComponentValue( float value )
			: m_component{ value }
		{
		}
		/**
		 *\~english
		 *\brief		Assignment operator from float
		 *\param[in]	rhs	The component value, must be between 0.0f and 1.0f
		 *\return		Reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation à partir d'un float
		 *\param[in]	rhs	La valeur de la composante, doit être comprise entre 0.0f et 1.0f
		 *\return		Référence sur cet objet
		 */
		ColourComponentValue & operator=( float rhs )
		{
			m_component = rhs;
			doClamp();
			return *this;
		}
		/**
		 *\~english
		 *\brief		Assignment operator from double
		 *\param[in]	rhs	The component value, must be between 0.0 and 1.0
		 *\return		Reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation à partir d'un double
		 *\param[in]	rhs	La valeur de la composante, doit être comprise entre 0.0 et 1.0
		 *\return		Référence sur cet objet
		 */
		ColourComponentValue & operator=( double rhs )
		{
			m_component = float( rhs );
			doClamp();
			return *this;
		}
		/**
		 *\~english
		 *\brief		Assignment operator from long double
		 *\param[in]	rhs	The component value, must be between 0.0 and 1.0
		 *\return		Reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation à partir d'un long double
		 *\param[in]	rhs	La valeur de la composante, doit être comprise entre 0.0 et 1.0
		 *\return		Référence sur cet objet
		 */
		ColourComponentValue & operator=( long double rhs )
		{
			m_component = float( rhs );
			doClamp();
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
		ColourComponentValue & operator=( int8_t rhs )
		{
			m_component = float( uint8_t( rhs ) ) / 255.0f;
			doClamp();
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
		ColourComponentValue & operator=( uint8_t rhs )
		{
			m_component = float( rhs ) / 255.0f;
			return *this;
		}
		/**
		 *\~english
		 *\brief		Assignment operator from int16_t
		 *\param[in]	rhs	The component value, must be between 0 and 255
		 *\return		Reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation à partir d'un int16_t
		 *\param[in]	rhs	La valeur de la composante, doit être comprise entre 0 et 255
		 *\return		Référence sur cet objet
		 */
		ColourComponentValue & operator=( int16_t rhs )
		{
			m_component = float( rhs ) / 255.0f;
			doClamp();
			return *this;
		}
		/**
		 *\~english
		 *\brief		Assignment operator from uint16_t
		 *\param[in]	rhs	The component value, must be between 0 and 255
		 *\return		Reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation à partir d'un uint16_t
		 *\param[in]	rhs	La valeur de la composante, doit être comprise entre 0 et 255
		 *\return		Référence sur cet objet
		 */
		ColourComponentValue & operator=( uint16_t rhs )
		{
			m_component = float( rhs ) / 255.0f;
			doClamp();
			return *this;
		}
		/**
		 *\~english
		 *\brief		Assignment operator from int32_t
		 *\param[in]	rhs	The component value, must be between 0 and 255
		 *\return		Reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation à partir d'un int32_t
		 *\param[in]	rhs	La valeur de la composante, doit être comprise entre 0 et 255
		 *\return		Référence sur cet objet
		 */
		ColourComponentValue & operator=( int32_t rhs )
		{
			m_component = float( rhs ) / 255.0f;
			doClamp();
			return *this;
		}
		/**
		 *\~english
		 *\brief		Assignment operator from uint32_t
		 *\param[in]	rhs	The component value, must be between 0 and 255
		 *\return		Reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation à partir d'un uint32_t
		 *\param[in]	rhs	La valeur de la composante, doit être comprise entre 0 et 255
		 *\return		Référence sur cet objet
		 */
		ColourComponentValue & operator=( uint32_t rhs )
		{
			m_component = float( rhs ) / 255.0f;
			doClamp();
			return *this;
		}
		/**
		 *\~english
		 *\brief		Assignment operator from int64_t
		 *\param[in]	rhs	The component value, must be between 0 and 255
		 *\return		Reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation à partir d'un int64_t
		 *\param[in]	rhs	La valeur de la composante, doit être comprise entre 0 et 255
		 *\return		Référence sur cet objet
		 */
		ColourComponentValue & operator=( int64_t rhs )
		{
			m_component = float( rhs ) / 255.0f;
			doClamp();
			return *this;
		}
		/**
		 *\~english
		 *\brief		Assignment operator from uint64_t
		 *\param[in]	rhs	The component value, must be between 0 and 255
		 *\return		Reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation à partir d'un uint64_t
		 *\param[in]	rhs	La valeur de la composante, doit être comprise entre 0 et 255
		 *\return		Référence sur cet objet
		 */
		ColourComponentValue & operator=( uint64_t rhs )
		{
			m_component = float( rhs ) / 255.0f;
			doClamp();
			return *this;
		}
		/**
		 *\~english
		 *\brief		int8_t conversion operator
		 *\~french
		 *\brief		Opérateur de conversion vers int8_t
		 */
		int8_t & convertTo( int8_t & v )const
		{
			return v = int8_t( value() * 255.0 );
		}
		/**
		 *\~english
		 *\brief		uint8_t conversion operator
		 *\~french
		 *\brief		Opérateur de conversion vers uint8_t
		 */
		uint8_t & convertTo( uint8_t & v )const
		{
			return v = uint8_t( value() * 255.0 );
		}
		/**
		 *\~english
		 *\brief		int16_t conversion operator
		 *\~french
		 *\brief		Opérateur de conversion vers int16_t
		 */
		int16_t & convertTo( int16_t & v )const
		{
			return v = int16_t( value() * 255.0 );
		}
		/**
		 *\~english
		 *\brief		uint16_t conversion operator
		 *\~french
		 *\brief		Opérateur de conversion vers uint16_t
		 */
		uint16_t & convertTo( uint16_t & v )const
		{
			return v = uint16_t( value() * 255.0 );
		}
		/**
		 *\~english
		 *\brief		int32_t conversion operator
		 *\~french
		 *\brief		Opérateur de conversion vers int32_t
		 */
		int32_t & convertTo( int32_t & v )const
		{
			return v = int32_t( value() * 255.0 );
		}
		/**
		 *\~english
		 *\brief		uint32_t conversion operator
		 *\~french
		 *\brief		Opérateur de conversion vers uint32_t
		 */
		uint32_t & convertTo( uint32_t & v )const
		{
			return v = uint32_t( value() * 255.0 );
		}
		/**
		 *\~english
		 *\brief		int64_t conversion operator
		 *\~french
		 *\brief		Opérateur de conversion vers int64_t
		 */
		int64_t & convertTo( int64_t & v )const
		{
			return v = int64_t( value() * 255.0 );
		}
		/**
		 *\~english
		 *\brief		uint64_t conversion operator
		 *\~french
		 *\brief		Opérateur de conversion vers uint64_t
		 */
		uint64_t & convertTo( uint64_t & v )const
		{
			return v = uint64_t( value() * 255.0 );
		}
		/**
		 *\~english
		 *\brief		float conversion operator
		 *\~french
		 *\brief		Opérateur de conversion vers float
		 */
		float & convertTo( float & v )const
		{
			return v = value();
		}
		/**
		 *\~english
		 *\brief		float conversion operator
		 *\~french
		 *\brief		Opérateur de conversion vers float
		 */
		double & convertTo( double & v )const
		{
			return v = value();
		}
		/**
		 *\~english
		 *\brief		float conversion operator
		 *\~french
		 *\brief		Opérateur de conversion vers float
		 */
		long double & convertTo( long double & v )const
		{
			return v = value();
		}
		/**
		 *\~english
		 *\brief		Addition assignment operator
		 *\remarks		Clamps the result
		 *\param[in]	rhs	The value to add
		 *\return		Result of this + rhs
		 *\~french
		 *\brief		Opérateur d'affectation par addition
		 *\remarks		Clampe le résultat
		 *\param[in]	rhs	La valeur à ajouter
		 *\return		Resultat de this + rhs
		 */
		ColourComponentValue & operator+=( ColourComponentValue const & rhs )
		{
			m_component += rhs.value();
			doClamp();
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
		ColourComponentValue & operator-=( ColourComponentValue const & rhs )
		{
			m_component -= rhs.value();
			doClamp();
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
		ColourComponentValue & operator*=( ColourComponentValue const & rhs )
		{
			m_component *= rhs.value();
			doClamp();
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
		ColourComponentValue & operator/=( ColourComponentValue const & rhs )
		{
			m_component /= rhs.value();
			doClamp();
			return *this;
		}
		/**
		 *\~english
		 *\brief		Addition assignment operator
		 *\param[in]	rhs	The value to add
		 *\return		Result of this + rhs
		 *\~french
		 *\brief		Opérateur d'affectation par addition
		 *\param[in]	rhs	La valeur à ajouter
		 *\return		Resultat de this + rhs
		 */
		template< typename T >
		ColourComponentValue & operator+=( T const & rhs )
		{
			float value = 0;
			ColourComponentValue component( value );
			component = rhs;
			m_component += component.value();
			doClamp();
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
		ColourComponentValue & operator-=( T const & rhs )
		{
			float value = 0;
			ColourComponentValue component( value );
			component = rhs;
			m_component -= component.value();
			doClamp();
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
		ColourComponentValue & operator*=( T const & rhs )
		{
			float value = 0;
			ColourComponentValue component( value );
			component = rhs;
			m_component *= component.value();
			doClamp();
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
		ColourComponentValue & operator/=( T const & rhs )
		{
			float value = 0;
			ColourComponentValue component( value );
			component = rhs;
			m_component /= component.value();
			doClamp();
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
		constexpr operator float()const
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
		constexpr float const & value()const
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
		float & value()
		{
			return m_component;
		}

	private:
		void doClamp()
		{
			if ( value() < 0 )
			{
				m_component = 0;
			}
			else if ( value() > 1 )
			{
				m_component = 1;
			}
		}

	private:
		float m_component{};

		friend bool operator ==( ColourComponentValue const & lhs, ColourComponentValue const & rhs )
		{
			uint8_t uiA;
			uint8_t uiB;
			lhs.convertTo( uiA );
			rhs.convertTo( uiB );
			return uiA == uiB;
		}

		friend float operator+( ColourComponentValue const & lhs, ColourComponentValue const & rhs )
		{
			float fValue;
			lhs.convertTo( fValue );
			ColourComponentValue cpnt( fValue );
			cpnt += rhs;
			return cpnt;
		}

		friend float operator-( ColourComponentValue const & lhs, ColourComponentValue const & rhs )
		{
			float fValue;
			lhs.convertTo( fValue );
			ColourComponentValue cpnt( fValue );
			cpnt -= rhs;
			return cpnt;
		}

		friend float operator*( ColourComponentValue const & lhs, ColourComponentValue const & rhs )
		{
			float fValue;
			lhs.convertTo( fValue );
			ColourComponentValue cpnt( fValue );
			cpnt *= rhs;
			return cpnt;
		}

		friend float operator/( ColourComponentValue const & lhs, ColourComponentValue const & rhs )
		{
			float fValue;
			lhs.convertTo( fValue );
			ColourComponentValue cpnt( fValue );
			cpnt /= rhs;
			return cpnt;
		}

		template< typename T >
		friend float operator+( ColourComponentValue const & lhs, T const & rhs )
		{
			float value;
			lhs.convertTo( value );
			ColourComponentValue cpnt{ value };
			cpnt += rhs;
			return value;
		}

		template< typename T >
		friend float operator-( ColourComponentValue const & lhs, T const & rhs )
		{
			float value;
			lhs.convertTo( value );
			ColourComponentValue cpnt{ value };
			cpnt -= rhs;
			return value;
		}

		template< typename T >
		friend float operator/( ColourComponentValue const & lhs, T const & rhs )
		{
			float value;
			lhs.convertTo( value );
			ColourComponentValue cpnt{ value };
			cpnt /= rhs;
			return value;
		}

		template< typename T >
		friend float operator*( ColourComponentValue const & lhs, T const & rhs )
		{
			float value;
			lhs.convertTo( value );
			ColourComponentValue cpnt{ value };
			cpnt *= rhs;
			return value;
		}
	};
	static_assert( sizeof( ColourComponentValue ) == sizeof( float ) );
}

#endif
