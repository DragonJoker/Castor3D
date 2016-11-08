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
#ifndef ___CASTOR_HDR_COLOUR_COMPONENT_H___
#define ___CASTOR_HDR_COLOUR_COMPONENT_H___

#include "CastorUtilsPrerequisites.hpp"

namespace Castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		07/10=1/2016
	\~english
	\brief		Defines a HDR colour component (R, G, B or A) to be used in Castor::HdrColour.
	\remark		Holds conversion operators to be converted either into float or uint8_t, with corresponding operations
	\~french
	\brief		Représente une composante de couleur HDR (R, V, B ou A) pour l'utilisation dans Castor::HdrColour
	\remark		Définit les opérateurs de conversion en float ou uint8_t, avec les opérations correspondantes
	*/
	class HdrColourComponent
	{
	private:
		friend class std::array< HdrColourComponent, 4 >;
		/**
		 *\~english
		 *\brief		Copy Constructor
		 *\param[in]	p_object	The object to copy
		 *\~french
		 *\brief		Constructeur par recopie
		 *\param[in]	p_object	L'objet à copier
		 */
		HdrColourComponent()
			: m_pfComponent( nullptr )
		{
		}

	public:
		/**
		 *\~english
		 *\brief		Specified constructor
		 *\param[in]	p_value	The component value
		 *\~french
		 *\brief		Constructeur spécifié
		 *\param[in]	p_value	La valeur de la composante
		 */
		HdrColourComponent( float * p_value )
			: m_pfComponent( p_value )
		{
		}
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	p_rhs	The component
		 *\return		Reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	p_rhs	La composante
		 *\return		Référence sur cet objet
		 */
		HdrColourComponent & operator=( HdrColourComponent const & p_rhs )
		{
			*m_pfComponent = *p_rhs.m_pfComponent;
			return *this;
		}
		/**
		 *\~english
		 *\brief		Assignment operator from float
		 *\param[in]	p_rhs	The component value, must be between 0.0f and 1.0f
		 *\return		Reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation à partir d'un float
		 *\param[in]	p_rhs	La valeur de la composante, doit être comprise entre 0.0f et 1.0f
		 *\return		Référence sur cet objet
		 */
		HdrColourComponent & operator=( float p_rhs )
		{
			*m_pfComponent = p_rhs;
			return *this;
		}
		/**
		 *\~english
		 *\brief		Assignment operator from double
		 *\param[in]	p_rhs	The component value, must be between 0.0 and 1.0
		 *\return		Reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation à partir d'un double
		 *\param[in]	p_rhs	La valeur de la composante, doit être comprise entre 0.0 et 1.0
		 *\return		Référence sur cet objet
		 */
		HdrColourComponent & operator=( double p_rhs )
		{
			*m_pfComponent = float( p_rhs );
			return *this;
		}
		/**
		 *\~english
		 *\brief		Assignment operator from long double
		 *\param[in]	p_rhs	The component value, must be between 0.0 and 1.0
		 *\return		Reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation à partir d'un long double
		 *\param[in]	p_rhs	La valeur de la composante, doit être comprise entre 0.0 et 1.0
		 *\return		Référence sur cet objet
		 */
		HdrColourComponent & operator=( long double p_rhs )
		{
			*m_pfComponent = float( p_rhs );
			return *this;
		}
		/**
		 *\~english
		 *\brief		Assignment operator from int8_t
		 *\param[in]	p_rhs	The component value
		 *\return		Reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation à partir d'un int8_t
		 *\param[in]	p_rhs	La valeur de la composante
		 *\return		Référence sur cet objet
		 */
		HdrColourComponent & operator=( int8_t p_rhs )
		{
			*m_pfComponent = float( uint8_t( p_rhs ) );
			return *this;
		}
		/**
		 *\~english
		 *\brief		Assignment operator from uint8_t
		 *\param[in]	p_rhs	The component value
		 *\return		Reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation à partir d'un uint8_t
		 *\param[in]	p_rhs	La valeur de la composante
		 *\return		Référence sur cet objet
		 */
		HdrColourComponent & operator=( uint8_t p_rhs )
		{
			*m_pfComponent = float( p_rhs );
			return *this;
		}
		/**
		 *\~english
		 *\brief		Assignment operator from int16_t
		 *\param[in]	p_rhs	The component value, must be between 0 and 255
		 *\return		Reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation à partir d'un int16_t
		 *\param[in]	p_rhs	La valeur de la composante, doit être comprise entre 0 et 255
		 *\return		Référence sur cet objet
		 */
		HdrColourComponent & operator=( int16_t p_rhs )
		{
			*m_pfComponent = float( p_rhs );
			return *this;
		}
		/**
		 *\~english
		 *\brief		Assignment operator from uint16_t
		 *\param[in]	p_rhs	The component value, must be between 0 and 255
		 *\return		Reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation à partir d'un uint16_t
		 *\param[in]	p_rhs	La valeur de la composante, doit être comprise entre 0 et 255
		 *\return		Référence sur cet objet
		 */
		HdrColourComponent & operator=( uint16_t p_rhs )
		{
			*m_pfComponent = float( p_rhs );
			return *this;
		}
		/**
		 *\~english
		 *\brief		Assignment operator from int32_t
		 *\param[in]	p_rhs	The component value, must be between 0 and 255
		 *\return		Reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation à partir d'un int32_t
		 *\param[in]	p_rhs	La valeur de la composante, doit être comprise entre 0 et 255
		 *\return		Référence sur cet objet
		 */
		HdrColourComponent & operator=( int32_t p_rhs )
		{
			*m_pfComponent = float( p_rhs );
			return *this;
		}
		/**
		 *\~english
		 *\brief		Assignment operator from uint32_t
		 *\param[in]	p_rhs	The component value, must be between 0 and 255
		 *\return		Reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation à partir d'un uint32_t
		 *\param[in]	p_rhs	La valeur de la composante, doit être comprise entre 0 et 255
		 *\return		Référence sur cet objet
		 */
		HdrColourComponent & operator=( uint32_t p_rhs )
		{
			*m_pfComponent = float( p_rhs );
			return *this;
		}
		/**
		 *\~english
		 *\brief		Assignment operator from int64_t
		 *\param[in]	p_rhs	The component value, must be between 0 and 255
		 *\return		Reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation à partir d'un int64_t
		 *\param[in]	p_rhs	La valeur de la composante, doit être comprise entre 0 et 255
		 *\return		Référence sur cet objet
		 */
		HdrColourComponent & operator=( int64_t p_rhs )
		{
			*m_pfComponent = float( p_rhs );
			return *this;
		}
		/**
		 *\~english
		 *\brief		Assignment operator from uint64_t
		 *\param[in]	p_rhs	The component value, must be between 0 and 255
		 *\return		Reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation à partir d'un uint64_t
		 *\param[in]	p_rhs	La valeur de la composante, doit être comprise entre 0 et 255
		 *\return		Référence sur cet objet
		 */
		HdrColourComponent & operator=( uint64_t p_rhs )
		{
			*m_pfComponent = float( p_rhs );
			return *this;
		}
		/**
		 *\~english
		 *\brief		int8_t conversion operator
		 *\~french
		 *\brief		Opérateur de conversion vers int8_t
		 */
		inline int8_t & convert_to( int8_t & p_value )const
		{
			return p_value = int8_t( value() );
		}
		/**
		 *\~english
		 *\brief		uint8_t conversion operator
		 *\~french
		 *\brief		Opérateur de conversion vers uint8_t
		 */
		inline uint8_t & convert_to( uint8_t & p_value )const
		{
			return p_value = uint8_t( value() );
		}
		/**
		 *\~english
		 *\brief		int16_t conversion operator
		 *\~french
		 *\brief		Opérateur de conversion vers int16_t
		 */
		inline int16_t & convert_to( int16_t & p_value )const
		{
			return p_value = int16_t( value() );
		}
		/**
		 *\~english
		 *\brief		uint16_t conversion operator
		 *\~french
		 *\brief		Opérateur de conversion vers uint16_t
		 */
		inline uint16_t & convert_to( uint16_t & p_value )const
		{
			return p_value = uint16_t( value() );
		}
		/**
		 *\~english
		 *\brief		int32_t conversion operator
		 *\~french
		 *\brief		Opérateur de conversion vers int32_t
		 */
		inline int32_t & convert_to( int32_t & p_value )const
		{
			return p_value = int32_t( value() );
		}
		/**
		 *\~english
		 *\brief		uint32_t conversion operator
		 *\~french
		 *\brief		Opérateur de conversion vers uint32_t
		 */
		inline uint32_t & convert_to( uint32_t & p_value )const
		{
			return p_value = uint32_t( value() );
		}
		/**
		 *\~english
		 *\brief		int64_t conversion operator
		 *\~french
		 *\brief		Opérateur de conversion vers int64_t
		 */
		inline int64_t & convert_to( int64_t & p_value )const
		{
			return p_value = int64_t( value() );
		}
		/**
		 *\~english
		 *\brief		uint64_t conversion operator
		 *\~french
		 *\brief		Opérateur de conversion vers uint64_t
		 */
		inline uint64_t & convert_to( uint64_t & p_value )const
		{
			return p_value = uint64_t( value() );
		}
		/**
		 *\~english
		 *\brief		float conversion operator
		 *\~french
		 *\brief		Opérateur de conversion vers float
		 */
		inline float & convert_to( float & p_value )const
		{
			return p_value = value();
		}
		/**
		 *\~english
		 *\brief		float conversion operator
		 *\~french
		 *\brief		Opérateur de conversion vers float
		 */
		inline double & convert_to( double & p_value )const
		{
			return p_value = value();
		}
		/**
		 *\~english
		 *\brief		float conversion operator
		 *\~french
		 *\brief		Opérateur de conversion vers float
		 */
		inline long double & convert_to( long double & p_value )const
		{
			return p_value = value();
		}
		/**
		 *\~english
		 *\brief		Sets the component value
		 *\remarks		Clamps the result
		 *\param[in]	p_value	The value
		 *\~french
		 *\brief		Définit la valeur de la composante
		 *\remarks		Clampe le résultat
		 *\param[in]	p_value	La valeur
		 */
		void link( float * p_value )
		{
			m_pfComponent = p_value;
		}
		/**
		 *\~english
		 *\brief		Addition assignment operator
		 *\remarks		Clamps the result
		 *\param[in]	p_rhs	The value to add
		 *\return		Result of this + p_rhs
		 *\~french
		 *\brief		Opérateur d'affectation par addition
		 *\remarks		Clampe le résultat
		 *\param[in]	p_rhs	La valeur à ajouter
		 *\return		Resultat de this + p_rhs
		 */
		HdrColourComponent & operator+=( HdrColourComponent const & p_rhs )
		{
			*m_pfComponent += p_rhs.value();
			return *this;
		}
		/**
		 *\~english
		 *\brief		Substraction assignment operator
		 *\remarks		Clamps the result
		 *\param[in]	p_rhs	The value to substract
		 *\return		Result of this - p_rhs
		 *\~french
		 *\brief		Opérateur d'affectation par soustraction
		 *\remarks		Clampe le résultat
		 *\param[in]	p_rhs	La valeur à soustraire
		 *\return		Resultat de this - p_rhs
		 */
		HdrColourComponent & operator-=( HdrColourComponent const & p_rhs )
		{
			*m_pfComponent -= p_rhs.value();
			return *this;
		}
		/**
		 *\~english
		 *\brief		Multiplication assignment operator
		 *\remarks		Clamps the result
		 *\param[in]	p_rhs	The value to multiply
		 *\return		Result of this * p_rhs
		 *\~french
		 *\brief		Opérateur d'affectation par multiplication
		 *\remarks		Clampe le résultat
		 *\param[in]	p_rhs	La valeur à multiplier
		 *\return		Resultat de this * p_rhs
		 */
		HdrColourComponent & operator*=( HdrColourComponent const & p_rhs )
		{
			*m_pfComponent *= p_rhs.value();
			return *this;
		}
		/**
		 *\~english
		 *\brief		Division assignment operator
		 *\remarks		Clamps the result
		 *\param[in]	p_rhs	The value to divide
		 *\return		Result of this / p_rhs
		 *\~french
		 *\brief		Opérateur d'affectation par division
		 *\remarks		Clampe le résultat
		 *\param[in]	p_rhs	La valeur à diviser
		 *\return		Resultat de this / p_rhs
		 */
		HdrColourComponent & operator/=( HdrColourComponent const & p_rhs )
		{
			*m_pfComponent /= p_rhs.value();
			return *this;
		}
		/**
		 *\~english
		 *\brief		Addition assignment operator
		 *\param[in]	p_rhs	The value to add
		 *\return		Result of this + p_rhs
		 *\~french
		 *\brief		Opérateur d'affectation par addition
		 *\param[in]	p_rhs	La valeur à ajouter
		 *\return		Resultat de this + p_rhs
		 */
		template< typename T >
		HdrColourComponent & operator+=( T const & p_rhs )
		{
			*m_pfComponent += p_rhs;
			return *this;
		}
		/**
		 *\~english
		 *\brief		Substraction assignment operator
		 *\param[in]	p_rhs	The value to substract
		 *\return		Result of this - p_rhs
		 *\~french
		 *\brief		Opérateur d'affectation par soustraction
		 *\param[in]	p_rhs	La valeur à soustraire
		 *\return		Resultat de this - p_rhs
		 */
		template< typename T >
		HdrColourComponent & operator-=( T const & p_rhs )
		{
			*m_pfComponent -= p_rhs;
			return *this;
		}
		/**
		 *\~english
		 *\brief		Multiplication assignment operator
		 *\param[in]	p_rhs	The value to multiply
		 *\return		Result of this * p_rhs
		 *\~french
		 *\brief		Opérateur d'affectation par multiplication
		 *\param[in]	p_rhs	La valeur à multiplier
		 *\return		Resultat de this * p_rhs
		 */
		template< typename T >
		HdrColourComponent & operator*=( T const & p_rhs )
		{
			*m_pfComponent *= p_rhs;
			return *this;
		}
		/**
		 *\~english
		 *\brief		Division assignment operator
		 *\param[in]	p_rhs	The value to divide
		 *\return		Result of this / p_rhs
		 *\~french
		 *\brief		Opérateur d'affectation par division
		 *\param[in]	p_rhs	La valeur à diviser
		 *\return		Resultat de this / p_rhs
		 */
		template< typename T >
		HdrColourComponent & operator/=( T const & p_rhs )
		{
			*m_pfComponent /= p_rhs;
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
			return *m_pfComponent;
		}
		/**
		 *\~english
		 *\brief		Retrieves the component value
		 *\return		The value
		 *\~french
		 *\brief		Récupère la valeur de la composante
		 *\return		La valeur
		 */
		inline float const & value()const
		{
			return *m_pfComponent;
		}
		/**
		 *\~english
		 *\brief		Retrieves the component value
		 *\return		The value
		 *\~french
		 *\brief		Récupère la valeur de la composante
		 *\return		La valeur
		 */
		inline float & value()
		{
			return *m_pfComponent;
		}

	private:
		float * m_pfComponent;
	};
	/**
	 *\~english
	 *\brief		Equality operator
	 *\param[in]	p_lhs, p_rhs	The components to compare
	 *\~french
	 *\brief		Opérateur d'égalité
	 *\param[in]	p_lhs, p_rhs	Les composantes à comparer
	 */
	CU_API bool operator==( HdrColourComponent const & p_lhs, HdrColourComponent const & p_rhs );
	/**
	 *\~english
	 *\brief		Inequality operator
	 *\param[in]	p_lhs, p_rhs	The components to compare
	 *\~french
	 *\brief		Opérateur de différence
	 *\param[in]	p_lhs, p_rhs	Les composantes à comparer
	 */
	CU_API bool operator!=( HdrColourComponent const & p_lhs, HdrColourComponent const & p_rhs );
	/**
	 *\~english
	 *\brief		Addition assignment operator
	 *\param[in]	p_lhs, p_rhs	The components to add
	 *\return		Result of p_lhs + p_rhs
	 *\~french
	 *\brief		Opérateur d'affectation par addition
	 *\param[in]	p_lhs, p_rhs	Les composantes à ajouter
	 *\return		Resultat de p_lhs + p_rhs
	 */
	template< typename T >
	float operator+( HdrColourComponent const & p_lhs, T const & p_rhs )
	{
		float l_fValue;
		p_lhs.convert_to( l_fValue );
		HdrColourComponent l_cpnt( &l_fValue );
		l_cpnt += p_rhs;
		return l_fValue;
	}
	/**
	 *\~english
	 *\brief		Substraction assignment operator
	 *\param[in]	p_lhs, p_rhs	The components to subtract
	 *\return		Result of p_lhs - p_rhs
	 *\~french
	 *\brief		Opérateur d'affectation par soustraction
	 *\param[in]	p_lhs, p_rhs	Les composantes à soustraire
	 *\return		Resultat de p_lhs - p_rhs
	 */
	template< typename T >
	float operator-( HdrColourComponent const & p_lhs, T const & p_rhs )
	{
		float l_fValue;
		p_lhs.convert_to( l_fValue );
		HdrColourComponent l_cpnt( &l_fValue );
		l_cpnt -= p_rhs;
		return l_fValue;
	}
	/**
	 *\~english
	 *\brief		Division assignment operator
	 *\param[in]	p_lhs, p_rhs	The components to divide
	 *\return		Result of p_lhs / p_rhs
	 *\~french
	 *\brief		Opérateur d'affectation par division
	 *\param[in]	p_lhs, p_rhs	Les composantes à diviser
	 *\return		Resultat de p_lhs / p_rhs
	 */
	template< typename T >
	float operator/( HdrColourComponent const & p_lhs, T const & p_rhs )
	{
		float l_fValue;
		p_lhs.convert_to( l_fValue );
		HdrColourComponent l_cpnt( &l_fValue );
		l_cpnt /= p_rhs;
		return l_fValue;
	}
	/**
	 *\~english
	 *\brief		Multiplication assignment operator
	 *\param[in]	p_lhs, p_rhs	The components to multiply
	 *\return		Result of p_lhs * p_rhs
	 *\~french
	 *\brief		Opérateur d'affectation par multiplication
	 *\param[in]	p_lhs, p_rhs	Les composantes à multiplier
	 *\return		Resultat de p_lhs * p_rhs
	 */
	template< typename T >
	float operator*( HdrColourComponent const & p_lhs, T const & p_rhs )
	{
		float l_fValue;
		p_lhs.convert_to( l_fValue );
		HdrColourComponent l_cpnt( &l_fValue );
		l_cpnt *= p_rhs;
		return l_fValue;
	}
	/**
	 *\~english
	 *\brief		Addition operator
	 *\param[in]	p_lhs, p_rhs	The components to add
	 *\return		Result of p_lhs + p_rhs
	 *\~french
	 *\brief		Opérateur d'addition
	 *\param[in]	p_lhs, p_rhs	Les composantes à ajouter
	 *\return		Resultat de p_lhs + p_rhs
	 */
	CU_API float operator+( HdrColourComponent const & p_lhs, HdrColourComponent const & p_rhs );
	/**
	 *\~english
	 *\brief		Subtraction operator
	 *\param[in]	p_lhs, p_rhs	The components to subtract
	 *\return		Result of p_lhs - p_rhs
	 *\~french
	 *\brief		Opérateur de soustraction
	 *\param[in]	p_lhs, p_rhs	Les composantes à soustraire
	 *\return		Resultat de p_lhs - p_rhs
	 */
	CU_API float operator-( HdrColourComponent const & p_lhs, HdrColourComponent const & p_rhs );
	/**
	 *\~english
	 *\brief		Multiplication operator
	 *\param[in]	p_lhs, p_rhs	The components to multiply
	 *\return		Result of p_lhs * p_rhs
	 *\~french
	 *\brief		Opérateur de multiplication
	 *\param[in]	p_lhs, p_rhs	Les composantes à multiplier
	 *\return		Resultat de p_lhs * p_rhs
	 */
	CU_API float operator*( HdrColourComponent const & p_lhs, HdrColourComponent const & p_rhs );
	/**
	 *\~english
	 *\brief		Division operator
	 *\param[in]	p_lhs, p_rhs	The components to divide
	 *\return		Result of p_lhs / p_rhs
	 *\~french
	 *\brief		Opérateur de division
	 *\param[in]	p_lhs, p_rhs	Les composantes à diviser
	 *\return		Resultat de p_lhs / p_rhs
	 */
	CU_API float operator/( HdrColourComponent const & p_lhs, HdrColourComponent const & p_rhs );
}

#endif
