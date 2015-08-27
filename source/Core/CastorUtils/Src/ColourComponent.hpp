/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___CASTOR_COLOUR_COMPONENT_H___
#define ___CASTOR_COLOUR_COMPONENT_H___

#include "CastorUtilsPrerequisites.hpp"

namespace Castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		19/10/2011
	\todo		Remove accessors, create traits
	\~english
	\brief		Defines a colour component (R, G, B or A) to be used in Castor::Colour.
	\remark		Holds conversion operators to be converted either into float or uint8_t, with corresponding operations
				<br />A colour component value is a floating number between 0.0 and 1.0
	\~french
	\brief		Représente une composante de couleur (R, V, B ou A) pour l'utilisation dans Castor::Colour
	\remark		Définit les opérateurs de conversion en float ou uint8_t, avec les opérations correspondantes
				<br />La valeur d'une composante de couleur est un nombre flottant compris entre 0.0 et 1.0
	*/
	class ColourComponent
	{
	private:
		friend class std::array< ColourComponent, 4 >;
		/**
		 *\~english
		 *\brief		Copy Constructor
		 *\param[in]	p_object	The object to copy
		 *\~french
		 *\brief		Constructeur par recopie
		 *\param[in]	p_object	L'objet à copier
		 */
		ColourComponent()	: m_pfComponent( NULL ) {}

	public:
		/**
		 *\~english
		 *\brief		Specified constructor
		 *\param[in]	p_fComponent	The component value
		 *\~french
		 *\brief		Constructeur spécifié
		 *\param[in]	p_fComponent	La valeur de la composante
		 */
		ColourComponent( float * p_pfComponent )	: m_pfComponent( p_pfComponent	) {}
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	p_component	The component
		 *\return		Reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	p_component	La composante
		 *\return		Référence sur cet objet
		 */
		ColourComponent & operator =( ColourComponent const & p_component )
		{
			*m_pfComponent = *p_component.m_pfComponent;
			return *this;
		}
		/**
		 *\~english
		 *\brief		Assignment operator from float
		 *\param[in]	p_fComponent	The component value, must be between 0.0f and 1.0f
		 *\return		Reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation à partir d'un float
		 *\param[in]	p_fComponent	La valeur de la composante, doit être comprise entre 0.0f et 1.0f
		 *\return		Référence sur cet objet
		 */
		ColourComponent & operator =( float p_fComponent )
		{
			*m_pfComponent = p_fComponent;
			DoClamp();
			return *this;
		}
		/**
		 *\~english
		 *\brief		Assignment operator from double
		 *\param[in]	p_dComponent	The component value, must be between 0.0 and 1.0
		 *\return		Reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation à partir d'un double
		 *\param[in]	p_dComponent	La valeur de la composante, doit être comprise entre 0.0 et 1.0
		 *\return		Référence sur cet objet
		 */
		ColourComponent & operator =( double p_dComponent )
		{
			*m_pfComponent = float( p_dComponent );
			DoClamp();
			return *this;
		}
		/**
		 *\~english
		 *\brief		Assignment operator from long double
		 *\param[in]	p_dComponent	The component value, must be between 0.0 and 1.0
		 *\return		Reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation à partir d'un long double
		 *\param[in]	p_dComponent	La valeur de la composante, doit être comprise entre 0.0 et 1.0
		 *\return		Référence sur cet objet
		 */
		ColourComponent & operator =( long double p_ldComponent )
		{
			*m_pfComponent = float( p_ldComponent );
			DoClamp();
			return *this;
		}
		/**
		 *\~english
		 *\brief		Assignment operator from int8_t
		 *\param[in]	p_cComponent	The component value
		 *\return		Reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation à partir d'un int8_t
		 *\param[in]	p_cComponent	La valeur de la composante
		 *\return		Référence sur cet objet
		 */
		ColourComponent & operator =( int8_t p_i8Component )
		{
			*m_pfComponent = float( uint8_t( p_i8Component ) ) / 255.0f;
			DoClamp();
			return *this;
		}
		/**
		 *\~english
		 *\brief		Assignment operator from uint8_t
		 *\param[in]	p_byComponent	The component value
		 *\return		Reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation à partir d'un uint8_t
		 *\param[in]	p_byComponent	La valeur de la composante
		 *\return		Référence sur cet objet
		 */
		ColourComponent & operator =( uint8_t p_ui8Component )
		{
			*m_pfComponent = float( p_ui8Component ) / 255.0f;
			return *this;
		}
		/**
		 *\~english
		 *\brief		Assignment operator from int16_t
		 *\param[in]	p_iComponent	The component value, must be between 0 and 255
		 *\return		Reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation à partir d'un int16_t
		 *\param[in]	p_iComponent	La valeur de la composante, doit être comprise entre 0 et 255
		 *\return		Référence sur cet objet
		 */
		ColourComponent & operator =( int16_t p_i16Component )
		{
			*m_pfComponent = float( p_i16Component ) / 255.0f;
			DoClamp();
			return *this;
		}
		/**
		 *\~english
		 *\brief		Assignment operator from uint16_t
		 *\param[in]	p_uiComponent	The component value, must be between 0 and 255
		 *\return		Reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation à partir d'un uint16_t
		 *\param[in]	p_uiComponent	La valeur de la composante, doit être comprise entre 0 et 255
		 *\return		Référence sur cet objet
		 */
		ColourComponent & operator =( uint16_t p_ui16Component )
		{
			*m_pfComponent = float( p_ui16Component ) / 255.0f;
			DoClamp();
			return *this;
		}
		/**
		 *\~english
		 *\brief		Assignment operator from int32_t
		 *\param[in]	p_iComponent	The component value, must be between 0 and 255
		 *\return		Reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation à partir d'un int32_t
		 *\param[in]	p_iComponent	La valeur de la composante, doit être comprise entre 0 et 255
		 *\return		Référence sur cet objet
		 */
		ColourComponent & operator =( int32_t p_i32Component )
		{
			*m_pfComponent = float( p_i32Component ) / 255.0f;
			DoClamp();
			return *this;
		}
		/**
		 *\~english
		 *\brief		Assignment operator from uint32_t
		 *\param[in]	p_uiComponent	The component value, must be between 0 and 255
		 *\return		Reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation à partir d'un uint32_t
		 *\param[in]	p_uiComponent	La valeur de la composante, doit être comprise entre 0 et 255
		 *\return		Référence sur cet objet
		 */
		ColourComponent & operator =( uint32_t p_ui32Component )
		{
			*m_pfComponent = float( p_ui32Component ) / 255.0f;
			DoClamp();
			return *this;
		}
		/**
		 *\~english
		 *\brief		Assignment operator from int64_t
		 *\param[in]	p_llComponent	The component value, must be between 0 and 255
		 *\return		Reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation à partir d'un int64_t
		 *\param[in]	p_llComponent	La valeur de la composante, doit être comprise entre 0 et 255
		 *\return		Référence sur cet objet
		 */
		ColourComponent & operator =( int64_t p_i64Component )
		{
			*m_pfComponent = float( p_i64Component ) / 255.0f;
			DoClamp();
			return *this;
		}
		/**
		 *\~english
		 *\brief		Assignment operator from uint64_t
		 *\param[in]	p_llComponent	The component value, must be between 0 and 255
		 *\return		Reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation à partir d'un uint64_t
		 *\param[in]	p_llComponent	La valeur de la composante, doit être comprise entre 0 et 255
		 *\return		Référence sur cet objet
		 */
		ColourComponent & operator =( uint64_t p_ui64Component )
		{
			*m_pfComponent = float( p_ui64Component ) / 255.0f;
			DoClamp();
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
			return p_value = int8_t( value() * 255.0f );
		}
		/**
		 *\~english
		 *\brief		uint8_t conversion operator
		 *\~french
		 *\brief		Opérateur de conversion vers uint8_t
		 */
		inline uint8_t & convert_to( uint8_t & p_value )const
		{
			return p_value = uint8_t( value() * 255.0f );
		}
		/**
		 *\~english
		 *\brief		int16_t conversion operator
		 *\~french
		 *\brief		Opérateur de conversion vers int16_t
		 */
		inline int16_t & convert_to( int16_t & p_value )const
		{
			return p_value = int16_t( value() * 255.0f );
		}
		/**
		 *\~english
		 *\brief		uint16_t conversion operator
		 *\~french
		 *\brief		Opérateur de conversion vers uint16_t
		 */
		inline uint16_t & convert_to( uint16_t & p_value )const
		{
			return p_value = uint16_t( value() * 255.0f );
		}
		/**
		 *\~english
		 *\brief		int32_t conversion operator
		 *\~french
		 *\brief		Opérateur de conversion vers int32_t
		 */
		inline int32_t & convert_to( int32_t & p_value )const
		{
			return p_value = int32_t( value() * 255.0f );
		}
		/**
		 *\~english
		 *\brief		uint32_t conversion operator
		 *\~french
		 *\brief		Opérateur de conversion vers uint32_t
		 */
		inline uint32_t & convert_to( uint32_t & p_value )const
		{
			return p_value = uint32_t( value() * 255.0f );
		}
		/**
		 *\~english
		 *\brief		int64_t conversion operator
		 *\~french
		 *\brief		Opérateur de conversion vers int64_t
		 */
		inline int64_t & convert_to( int64_t & p_value )const
		{
			return p_value = int64_t( value() * 255.0f );
		}
		/**
		 *\~english
		 *\brief		uint64_t conversion operator
		 *\~french
		 *\brief		Opérateur de conversion vers uint64_t
		 */
		inline uint64_t & convert_to( uint64_t & p_value )const
		{
			return p_value = uint64_t( value() * 255.0f );
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
		 *\remark		Clamps the result
		 *\param[in]	p_value	The value
		 *\~french
		 *\brief		Définit la valeur de la composante
		 *\remark		Clampe le résultat
		 *\param[in]	p_value	La valeur
		 */
		void link( float * p_value )
		{
			m_pfComponent = p_value;
			DoClamp();
		}
		/**
		 *\~english
		 *\brief		Addition assignment operator
		 *\remark		Clamps the result
		 *\param[in]	p_component	The value to add
		 *\return		Result of this + p_component
		 *\~french
		 *\brief		Opérateur d'affectation par addition
		 *\remark		Clampe le résultat
		 *\param[in]	p_component	La valeur à ajouter
		 *\return		Resultat de this + p_component
		 */
		ColourComponent & operator +=( ColourComponent && p_component )
		{
			*m_pfComponent += p_component.value();
			DoClamp();
			return *this;
		}
		/**
		 *\~english
		 *\brief		Substraction assignment operator
		 *\remark		Clamps the result
		 *\param[in]	p_component	The value to substract
		 *\return		Result of this - p_component
		 *\~french
		 *\brief		Opérateur d'affectation par soustraction
		 *\remark		Clampe le résultat
		 *\param[in]	p_component	La valeur à soustraire
		 *\return		Resultat de this - p_component
		 */
		ColourComponent & operator -=( ColourComponent && p_component )
		{
			*m_pfComponent -= p_component.value();
			DoClamp();
			return *this;
		}
		/**
		 *\~english
		 *\brief		Multiplication assignment operator
		 *\remark		Clamps the result
		 *\param[in]	p_component	The value to multiply
		 *\return		Result of this * p_component
		 *\~french
		 *\brief		Opérateur d'affectation par multiplication
		 *\remark		Clampe le résultat
		 *\param[in]	p_component	La valeur à multiplier
		 *\return		Resultat de this * p_component
		 */
		ColourComponent & operator *=( ColourComponent && p_component )
		{
			*m_pfComponent *= p_component.value();
			DoClamp();
			return *this;
		}
		/**
		 *\~english
		 *\brief		Division assignment operator
		 *\remark		Clamps the result
		 *\param[in]	p_component	The value to divide
		 *\return		Result of this / p_component
		 *\~french
		 *\brief		Opérateur d'affectation par division
		 *\remark		Clampe le résultat
		 *\param[in]	p_component	La valeur à diviser
		 *\return		Resultat de this / p_component
		 */
		ColourComponent & operator /=( ColourComponent && p_component )
		{
			*m_pfComponent /= p_component.value();
			DoClamp();
			return *this;
		}
		/**
		 *\~english
		 *\brief		Addition assignment operator
		 *\param[in]	p_scalar	The value to add
		 *\return		Result of this + p_scalar
		 *\~french
		 *\brief		Opérateur d'affectation par addition
		 *\param[in]	p_scalar	La valeur à ajouter
		 *\return		Resultat de this + p_scalar
		 */
		template< typename T >
		ColourComponent & operator +=( T && p_scalar )
		{
			float l_fValue = 0;
			ColourComponent l_component( &l_fValue );
			l_component = p_scalar;
			*m_pfComponent += l_component.value();
			DoClamp();
			return *this;
		}
		/**
		 *\~english
		 *\brief		Substraction assignment operator
		 *\param[in]	p_scalar	The value to substract
		 *\return		Result of this - p_scalar
		 *\~french
		 *\brief		Opérateur d'affectation par soustraction
		 *\param[in]	p_scalar	La valeur à soustraire
		 *\return		Resultat de this - p_scalar
		 */
		template< typename T >
		ColourComponent & operator -=( T && p_scalar )
		{
			float l_fValue = 0;
			ColourComponent l_component( &l_fValue );
			l_component = p_scalar;
			*m_pfComponent -= l_component.value();
			DoClamp();
			return *this;
		}
		/**
		 *\~english
		 *\brief		Multiplication assignment operator
		 *\param[in]	p_scalar	The value to multiply
		 *\return		Result of this * p_scalar
		 *\~french
		 *\brief		Opérateur d'affectation par multiplication
		 *\param[in]	p_scalar	La valeur à multiplier
		 *\return		Resultat de this * p_scalar
		 */
		template< typename T >
		ColourComponent & operator *=( T && p_scalar )
		{
			float l_fValue = 0;
			ColourComponent l_component( &l_fValue );
			l_component = p_scalar;
			*m_pfComponent *= l_component.value();
			DoClamp();
			return *this;
		}
		/**
		 *\~english
		 *\brief		Division assignment operator
		 *\param[in]	p_scalar	The value to divide
		 *\return		Result of this / p_scalar
		 *\~french
		 *\brief		Opérateur d'affectation par division
		 *\param[in]	p_scalar	La valeur à diviser
		 *\return		Resultat de this / p_scalar
		 */
		template< typename T >
		ColourComponent & operator /=( T && p_scalar )
		{
			float l_fValue = 0;
			ColourComponent l_component( &l_fValue );
			l_component = p_scalar;
			*m_pfComponent /= l_component.value();
			DoClamp();
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

	private:
		void DoClamp()
		{
			if ( value() < 0 )
			{
				*m_pfComponent = 0;
			}
			else if ( value() > 1 )
			{
				*m_pfComponent = 1;
			}
		}

	private:
		float * m_pfComponent;
	};
	/**
	 *\~english
	 *\brief		Equality operator
	 *\param[in]	p_cpnA,p_clrB	The components to compare
	 *\~french
	 *\brief		Opérateur d'égalité
	 *\param[in]	p_cpnA,p_cpnB	Les composantes à comparer
	 */
	bool operator ==( ColourComponent const & p_cpnA, ColourComponent const & p_cpnB );
	/**
	 *\~english
	 *\brief		Inequality operator
	 *\param[in]	p_cpnA,p_cpnB	The components to compare
	 *\~french
	 *\brief		Opérateur de différence
	 *\param[in]	p_cpnA,p_cpnB	Les composantes à comparer
	 */
	bool operator !=( ColourComponent const & p_cpnA, ColourComponent const & p_cpnB );
	/**
	 *\~english
	 *\brief		Addition assignment operator
	 *\param[in]	p_scalar	The value to add
	 *\return		Result of this + p_scalar
	 *\~french
	 *\brief		Opérateur d'affectation par addition
	 *\param[in]	p_scalar	La valeur à ajouter
	 *\return		Resultat de this + p_scalar
	 */
	template< typename T >
	float operator +( ColourComponent const & p_cpnt, T && p_scalar )
	{
		float l_fValue;
		p_cpnt.convert_to( l_fValue );
		ColourComponent l_cpnt( &l_fValue );
		l_cpnt += p_scalar;
		return l_fValue;
	}
	/**
	 *\~english
	 *\brief		Substraction assignment operator
	 *\param[in]	p_scalar	The value to substract
	 *\return		Result of this - p_scalar
	 *\~french
	 *\brief		Opérateur d'affectation par soustraction
	 *\param[in]	p_scalar	La valeur à soustraire
	 *\return		Resultat de this - p_scalar
	 */
	template< typename T >
	float operator -( ColourComponent const & p_cpnt, T && p_scalar )
	{
		float l_fValue;
		p_cpnt.convert_to( l_fValue );
		ColourComponent l_cpnt( &l_fValue );
		l_cpnt -= p_scalar;
		return l_fValue;
	}
	/**
	 *\~english
	 *\brief		Division assignment operator
	 *\param[in]	p_scalar	The value to divide
	 *\return		Result of this / p_scalar
	 *\~french
	 *\brief		Opérateur d'affectation par division
	 *\param[in]	p_scalar	La valeur à diviser
	 *\return		Resultat de this / p_scalar
	 */
	template< typename T >
	float operator /( ColourComponent const & p_cpnt, T && p_scalar )
	{
		float l_fValue;
		p_cpnt.convert_to( l_fValue );
		ColourComponent l_cpnt( &l_fValue );
		l_cpnt /= p_scalar;
		return l_fValue;
	}
	/**
	 *\~english
	 *\brief		Multiplication assignment operator
	 *\param[in]	p_scalar	The value to multiply
	 *\return		Result of this * p_scalar
	 *\~french
	 *\brief		Opérateur d'affectation par multiplication
	 *\param[in]	p_scalar	La valeur à multiplier
	 *\return		Resultat de this * p_scalar
	 */
	template< typename T >
	float operator *( ColourComponent const & p_cpnt, T && p_scalar )
	{
		float l_fValue;
		p_cpnt.convert_to( l_fValue );
		ColourComponent l_cpnt( &l_fValue );
		l_cpnt *= p_scalar;
		return l_fValue;
	}
	/**
	 *\~english
	 *\brief		Substraction assignment operator
	 *\param[in]	p_scalar	The value to substract
	 *\return		Result of this - p_scalar
	 *\~french
	 *\brief		Opérateur d'affectation par soustraction
	 *\param[in]	p_scalar	La valeur à soustraire
	 *\return		Resultat de this - p_scalar
	 */
	float operator -( ColourComponent const & p_cpnt, ColourComponent const & p_scalar );
	/**
	 *\~english
	 *\brief		Addition assignment operator
	 *\param[in]	p_scalar	The value to add
	 *\return		Result of this + p_scalar
	 *\~french
	 *\brief		Opérateur d'affectation par addition
	 *\param[in]	p_scalar	La valeur à ajouter
	 *\return		Resultat de this + p_scalar
	 */
	float operator +( ColourComponent const & p_cpnt, ColourComponent const & p_scalar );
	/**
	 *\~english
	 *\brief		Multiplication assignment operator
	 *\param[in]	p_scalar	The value to multiply
	 *\return		Result of this * p_scalar
	 *\~french
	 *\brief		Opérateur d'affectation par multiplication
	 *\param[in]	p_scalar	La valeur à multiplier
	 *\return		Resultat de this * p_scalar
	 */
	float operator *( ColourComponent const & p_cpnt, ColourComponent const & p_scalar );
	/**
	 *\~english
	 *\brief		Division assignment operator
	 *\param[in]	p_scalar	The value to divide
	 *\return		Result of this / p_scalar
	 *\~french
	 *\brief		Opérateur d'affectation par division
	 *\param[in]	p_scalar	La valeur à diviser
	 *\return		Resultat de this / p_scalar
	 */
	float operator /( ColourComponent const & p_cpnt, ColourComponent const & p_scalar );
}

#endif
