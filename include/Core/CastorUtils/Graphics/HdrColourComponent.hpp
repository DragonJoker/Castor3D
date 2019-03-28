/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_HDR_COLOUR_COMPONENT_H___
#define ___CASTOR_HDR_COLOUR_COMPONENT_H___

#include "CastorUtils/CastorUtilsPrerequisites.hpp"

namespace castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		07/10=1/2016
	\~english
	\brief		Defines a HDR colour PixelComponents (R, G, B or A) to be used in castor::HdrRgbColour or castor::HdrRgbaColour.
	\remark		Holds conversion operators to be converted either into float or uint8_t, with corresponding operations
	\~french
	\brief		Représente une composante de couleur HDR (R, V, B ou A) pour l'utilisation dans castor::HdrRgbColour ou castor::HdrRgbaColour.
	\remark		Définit les opérateurs de conversion en float ou uint8_t, avec les opérations correspondantes.
	*/
	class HdrColourComponent
	{
	private:
		friend class std::array< HdrColourComponent, 3u >;
		friend class std::array< HdrColourComponent, 4u >;
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		HdrColourComponent()
			: m_component( nullptr )
		{
		}

	public:
		/**
		 *\~english
		 *\brief		Specified constructor
		 *\param[in]	value	The PixelComponents value
		 *\~french
		 *\brief		Constructeur spécifié
		 *\param[in]	value	La valeur de la composante
		 */
		explicit HdrColourComponent( float * value )
			: m_component( value )
		{
		}
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	rhs	The PixelComponents
		 *\return		Reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	rhs	La composante
		 *\return		Référence sur cet objet
		 */
		HdrColourComponent & operator=( HdrColourComponent const & rhs )
		{
			*m_component = *rhs.m_component;
			return *this;
		}
		/**
		 *\~english
		 *\brief		Assignment operator from float
		 *\param[in]	rhs	The PixelComponents value, must be between 0.0f and 1.0f
		 *\return		Reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation à partir d'un float
		 *\param[in]	rhs	La valeur de la composante, doit être comprise entre 0.0f et 1.0f
		 *\return		Référence sur cet objet
		 */
		HdrColourComponent & operator=( float rhs )
		{
			*m_component = rhs;
			return *this;
		}
		/**
		 *\~english
		 *\brief		Assignment operator from double
		 *\param[in]	rhs	The PixelComponents value, must be between 0.0 and 1.0
		 *\return		Reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation à partir d'un double
		 *\param[in]	rhs	La valeur de la composante, doit être comprise entre 0.0 et 1.0
		 *\return		Référence sur cet objet
		 */
		HdrColourComponent & operator=( double rhs )
		{
			*m_component = float( rhs );
			return *this;
		}
		/**
		 *\~english
		 *\brief		Assignment operator from long double
		 *\param[in]	rhs	The PixelComponents value, must be between 0.0 and 1.0
		 *\return		Reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation à partir d'un long double
		 *\param[in]	rhs	La valeur de la composante, doit être comprise entre 0.0 et 1.0
		 *\return		Référence sur cet objet
		 */
		HdrColourComponent & operator=( long double rhs )
		{
			*m_component = float( rhs );
			return *this;
		}
		/**
		 *\~english
		 *\brief		Assignment operator from int8_t
		 *\param[in]	rhs	The PixelComponents value
		 *\return		Reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation à partir d'un int8_t
		 *\param[in]	rhs	La valeur de la composante
		 *\return		Référence sur cet objet
		 */
		HdrColourComponent & operator=( int8_t rhs )
		{
			*m_component = float( uint8_t( rhs ) );
			return *this;
		}
		/**
		 *\~english
		 *\brief		Assignment operator from uint8_t
		 *\param[in]	rhs	The PixelComponents value
		 *\return		Reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation à partir d'un uint8_t
		 *\param[in]	rhs	La valeur de la composante
		 *\return		Référence sur cet objet
		 */
		HdrColourComponent & operator=( uint8_t rhs )
		{
			*m_component = float( rhs );
			return *this;
		}
		/**
		 *\~english
		 *\brief		Assignment operator from int16_t
		 *\param[in]	rhs	The PixelComponents value, must be between 0 and 255
		 *\return		Reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation à partir d'un int16_t
		 *\param[in]	rhs	La valeur de la composante, doit être comprise entre 0 et 255
		 *\return		Référence sur cet objet
		 */
		HdrColourComponent & operator=( int16_t rhs )
		{
			*m_component = float( rhs );
			return *this;
		}
		/**
		 *\~english
		 *\brief		Assignment operator from uint16_t
		 *\param[in]	rhs	The PixelComponents value, must be between 0 and 255
		 *\return		Reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation à partir d'un uint16_t
		 *\param[in]	rhs	La valeur de la composante, doit être comprise entre 0 et 255
		 *\return		Référence sur cet objet
		 */
		HdrColourComponent & operator=( uint16_t rhs )
		{
			*m_component = float( rhs );
			return *this;
		}
		/**
		 *\~english
		 *\brief		Assignment operator from int32_t
		 *\param[in]	rhs	The PixelComponents value, must be between 0 and 255
		 *\return		Reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation à partir d'un int32_t
		 *\param[in]	rhs	La valeur de la composante, doit être comprise entre 0 et 255
		 *\return		Référence sur cet objet
		 */
		HdrColourComponent & operator=( int32_t rhs )
		{
			*m_component = float( rhs );
			return *this;
		}
		/**
		 *\~english
		 *\brief		Assignment operator from uint32_t
		 *\param[in]	rhs	The PixelComponents value, must be between 0 and 255
		 *\return		Reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation à partir d'un uint32_t
		 *\param[in]	rhs	La valeur de la composante, doit être comprise entre 0 et 255
		 *\return		Référence sur cet objet
		 */
		HdrColourComponent & operator=( uint32_t rhs )
		{
			*m_component = float( rhs );
			return *this;
		}
		/**
		 *\~english
		 *\brief		Assignment operator from int64_t
		 *\param[in]	rhs	The PixelComponents value, must be between 0 and 255
		 *\return		Reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation à partir d'un int64_t
		 *\param[in]	rhs	La valeur de la composante, doit être comprise entre 0 et 255
		 *\return		Référence sur cet objet
		 */
		HdrColourComponent & operator=( int64_t rhs )
		{
			*m_component = float( rhs );
			return *this;
		}
		/**
		 *\~english
		 *\brief		Assignment operator from uint64_t
		 *\param[in]	rhs	The PixelComponents value, must be between 0 and 255
		 *\return		Reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation à partir d'un uint64_t
		 *\param[in]	rhs	La valeur de la composante, doit être comprise entre 0 et 255
		 *\return		Référence sur cet objet
		 */
		HdrColourComponent & operator=( uint64_t rhs )
		{
			*m_component = float( rhs );
			return *this;
		}
		/**
		 *\~english
		 *\brief		int8_t conversion operator
		 *\~french
		 *\brief		Opérateur de conversion vers int8_t
		 */
		inline int8_t & convertTo( int8_t & v )const
		{
			return v = int8_t( value() );
		}
		/**
		 *\~english
		 *\brief		uint8_t conversion operator
		 *\~french
		 *\brief		Opérateur de conversion vers uint8_t
		 */
		inline uint8_t & convertTo( uint8_t & v )const
		{
			return v = uint8_t( value() );
		}
		/**
		 *\~english
		 *\brief		int16_t conversion operator
		 *\~french
		 *\brief		Opérateur de conversion vers int16_t
		 */
		inline int16_t & convertTo( int16_t & v )const
		{
			return v = int16_t( value() );
		}
		/**
		 *\~english
		 *\brief		uint16_t conversion operator
		 *\~french
		 *\brief		Opérateur de conversion vers uint16_t
		 */
		inline uint16_t & convertTo( uint16_t & v )const
		{
			return v = uint16_t( value() );
		}
		/**
		 *\~english
		 *\brief		int32_t conversion operator
		 *\~french
		 *\brief		Opérateur de conversion vers int32_t
		 */
		inline int32_t & convertTo( int32_t & v )const
		{
			return v = int32_t( value() );
		}
		/**
		 *\~english
		 *\brief		uint32_t conversion operator
		 *\~french
		 *\brief		Opérateur de conversion vers uint32_t
		 */
		inline uint32_t & convertTo( uint32_t & v )const
		{
			return v = uint32_t( value() );
		}
		/**
		 *\~english
		 *\brief		int64_t conversion operator
		 *\~french
		 *\brief		Opérateur de conversion vers int64_t
		 */
		inline int64_t & convertTo( int64_t & v )const
		{
			return v = int64_t( value() );
		}
		/**
		 *\~english
		 *\brief		uint64_t conversion operator
		 *\~french
		 *\brief		Opérateur de conversion vers uint64_t
		 */
		inline uint64_t & convertTo( uint64_t & v )const
		{
			return v = uint64_t( value() );
		}
		/**
		 *\~english
		 *\brief		float conversion operator
		 *\~french
		 *\brief		Opérateur de conversion vers float
		 */
		inline float & convertTo( float & v )const
		{
			return v = value();
		}
		/**
		 *\~english
		 *\brief		float conversion operator
		 *\~french
		 *\brief		Opérateur de conversion vers float
		 */
		inline double & convertTo( double & v )const
		{
			return v = value();
		}
		/**
		 *\~english
		 *\brief		float conversion operator
		 *\~french
		 *\brief		Opérateur de conversion vers float
		 */
		inline long double & convertTo( long double & v )const
		{
			return v = value();
		}
		/**
		 *\~english
		 *\brief		sets the PixelComponents value
		 *\remarks		Clamps the result
		 *\param[in]	value	The value
		 *\~french
		 *\brief		Définit la valeur de la composante
		 *\remarks		Clampe le résultat
		 *\param[in]	value	La valeur
		 */
		void link( float * value )
		{
			m_component = value;
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
		HdrColourComponent & operator+=( HdrColourComponent const & rhs )
		{
			*m_component += rhs.value();
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
		HdrColourComponent & operator-=( HdrColourComponent const & rhs )
		{
			*m_component -= rhs.value();
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
		HdrColourComponent & operator*=( HdrColourComponent const & rhs )
		{
			*m_component *= rhs.value();
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
		HdrColourComponent & operator/=( HdrColourComponent const & rhs )
		{
			*m_component /= rhs.value();
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
		HdrColourComponent & operator+=( T const & rhs )
		{
			*m_component = float( *m_component + rhs );
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
		HdrColourComponent & operator-=( T const & rhs )
		{
			*m_component = float( *m_component - rhs );
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
		HdrColourComponent & operator*=( T const & rhs )
		{
			*m_component = float( *m_component * rhs );
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
		HdrColourComponent & operator/=( T const & rhs )
		{
			*m_component = float( *m_component / rhs );
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
		inline operator float()const
		{
			return *m_component;
		}
		/**
		 *\~english
		 *\brief		Retrieves the PixelComponents value
		 *\return		The value
		 *\~french
		 *\brief		Récupère la valeur de la composante
		 *\return		La valeur
		 */
		inline float const & value()const
		{
			return *m_component;
		}
		/**
		 *\~english
		 *\brief		Retrieves the PixelComponents value
		 *\return		The value
		 *\~french
		 *\brief		Récupère la valeur de la composante
		 *\return		La valeur
		 */
		inline float & value()
		{
			return *m_component;
		}

	private:
		float * m_component;
	};
	/**
	 *\~english
	 *\brief		Equality operator
	 *\param[in]	lhs, rhs	The components to compare
	 *\~french
	 *\brief		Opérateur d'égalité
	 *\param[in]	lhs, rhs	Les composantes à comparer
	 */
	CU_API bool operator==( HdrColourComponent const & lhs, HdrColourComponent const & rhs );
	/**
	 *\~english
	 *\brief		Inequality operator
	 *\param[in]	lhs, rhs	The components to compare
	 *\~french
	 *\brief		Opérateur de différence
	 *\param[in]	lhs, rhs	Les composantes à comparer
	 */
	CU_API bool operator!=( HdrColourComponent const & lhs, HdrColourComponent const & rhs );
	/**
	 *\~english
	 *\brief		addition assignment operator
	 *\param[in]	lhs, rhs	The components to add
	 *\return		Result of lhs + rhs
	 *\~french
	 *\brief		Opérateur d'affectation par addition
	 *\param[in]	lhs, rhs	Les composantes à ajouter
	 *\return		Resultat de lhs + rhs
	 */
	template< typename T >
	float operator+( HdrColourComponent const & lhs, T const & rhs )
	{
		float value;
		lhs.convertTo( value );
		HdrColourComponent cpnt( &value );
		cpnt += rhs;
		return value;
	}
	/**
	 *\~english
	 *\brief		Substraction assignment operator
	 *\param[in]	lhs, rhs	The components to subtract
	 *\return		Result of lhs - rhs
	 *\~french
	 *\brief		Opérateur d'affectation par soustraction
	 *\param[in]	lhs, rhs	Les composantes à soustraire
	 *\return		Resultat de lhs - rhs
	 */
	template< typename T >
	float operator-( HdrColourComponent const & lhs, T const & rhs )
	{
		float value;
		lhs.convertTo( value );
		HdrColourComponent cpnt( &value );
		cpnt -= rhs;
		return value;
	}
	/**
	 *\~english
	 *\brief		Division assignment operator
	 *\param[in]	lhs, rhs	The components to divide
	 *\return		Result of lhs / rhs
	 *\~french
	 *\brief		Opérateur d'affectation par division
	 *\param[in]	lhs, rhs	Les composantes à diviser
	 *\return		Resultat de lhs / rhs
	 */
	template< typename T >
	float operator/( HdrColourComponent const & lhs, T const & rhs )
	{
		float value;
		lhs.convertTo( value );
		HdrColourComponent cpnt( &value );
		cpnt /= rhs;
		return value;
	}
	/**
	 *\~english
	 *\brief		Multiplication assignment operator
	 *\param[in]	lhs, rhs	The components to multiply
	 *\return		Result of lhs * rhs
	 *\~french
	 *\brief		Opérateur d'affectation par multiplication
	 *\param[in]	lhs, rhs	Les composantes à multiplier
	 *\return		Resultat de lhs * rhs
	 */
	template< typename T >
	float operator*( HdrColourComponent const & lhs, T const & rhs )
	{
		float value;
		lhs.convertTo( value );
		HdrColourComponent cpnt( &value );
		cpnt *= rhs;
		return value;
	}
	/**
	 *\~english
	 *\brief		addition operator
	 *\param[in]	lhs, rhs	The components to add
	 *\return		Result of lhs + rhs
	 *\~french
	 *\brief		Opérateur d'addition
	 *\param[in]	lhs, rhs	Les composantes à ajouter
	 *\return		Resultat de lhs + rhs
	 */
	CU_API float operator+( HdrColourComponent const & lhs, HdrColourComponent const & rhs );
	/**
	 *\~english
	 *\brief		Subtraction operator
	 *\param[in]	lhs, rhs	The components to subtract
	 *\return		Result of lhs - rhs
	 *\~french
	 *\brief		Opérateur de soustraction
	 *\param[in]	lhs, rhs	Les composantes à soustraire
	 *\return		Resultat de lhs - rhs
	 */
	CU_API float operator-( HdrColourComponent const & lhs, HdrColourComponent const & rhs );
	/**
	 *\~english
	 *\brief		Multiplication operator
	 *\param[in]	lhs, rhs	The components to multiply
	 *\return		Result of lhs * rhs
	 *\~french
	 *\brief		Opérateur de multiplication
	 *\param[in]	lhs, rhs	Les composantes à multiplier
	 *\return		Resultat de lhs * rhs
	 */
	CU_API float operator*( HdrColourComponent const & lhs, HdrColourComponent const & rhs );
	/**
	 *\~english
	 *\brief		Division operator
	 *\param[in]	lhs, rhs	The components to divide
	 *\return		Result of lhs / rhs
	 *\~french
	 *\brief		Opérateur de division
	 *\param[in]	lhs, rhs	Les composantes à diviser
	 *\return		Resultat de lhs / rhs
	 */
	CU_API float operator/( HdrColourComponent const & lhs, HdrColourComponent const & rhs );
}

#endif
