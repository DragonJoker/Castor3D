/*
This source file is part of ProceduralGenerator (https://sourceforge.net/projects/proceduralgene/)

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
#ifndef ___CASTOR_PIXEL_H___
#define ___CASTOR_PIXEL_H___

#include "Point.hpp"
#include "PixelFormat.hpp"

#if defined( _MSC_VER )
#	define TPL_PIXEL_FORMAT	uint32_t
#else
#	define TPL_PIXEL_FORMAT	ePIXEL_FORMAT
#endif

namespace Castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		29/08/2011
	\~english
	\brief		Pixel definition
	\remark		Takes a ePIXEL_FORMAT as a template argument to determine size anf format
	\~french
	\brief		Définition d'un pixel
	\remark		L'argument template ePIXEL_FORMAT sert a déterminer la taille et le format du pixel
	*/
	template< ePIXEL_FORMAT FT >
	class Pixel
	{
	public:
		//!\~english Typedef over a pointer to uint8_t			\~french Typedef d'un pointeur sur un uint8_t
		typedef uint8_t 	*		iterator;
		//!\~english Typedef over a constant pointer to uint8_t	\~french Typedef d'un pointeur constant sur un uint8_t
		typedef uint8_t const 	*	const_iterator;

	protected:
		typedef uint8_t 	*		component_ptr;
		typedef uint8_t const 	*	component_const_ptr;
		typedef uint8_t 	&		component_ref;
		typedef uint8_t const 	&	component_const_ref;

		std::shared_ptr< uint8_t > m_pComponents;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_bInit	Tells if the data are initialised
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_bInit	Dit si on initialise les données
		 */
		Pixel( bool p_bInit = false );
		/**
		 *\~english
		 *\brief		Constructor
		 *\remark		This Pixel doesn't own it's data
		 *\param[in]	p_components	Initialises the data to this one
		 *\~french
		 *\brief		Constructeur
		 *\remark		Le Pixel ne gère pas ses données
		 *\param[in]	p_components	Initialise les données à celles-ci
		 */
		Pixel( uint8_t * p_components );
		/**
		 *\~english
		 *\brief		Copy Constructor
		 *\param[in]	p_pxl	The Pixel object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_pxl	L'objet Pixel à copier
		 */
		Pixel( Pixel const & p_pxl );
		/**
		 *\~english
		 *\brief		Move Constructor
		 *\param[in]	p_pxl	The Pixel object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	p_pxl	L'objet Pixel à déplacer
		 */
		Pixel( Pixel && p_pxl );
		/**
		 *\~english
		 *\brief		Constructor
		 *\remark		This Pixel owns it's data
		 *\param[in]	p_components	Initialises the data to this one
		 *\~french
		 *\brief		Constructeur
		 *\remark		Le Pixel gère ses données
		 *\param[in]	p_components	Initialise les données à celles-ci
		 */
		template< ePIXEL_FORMAT FU >
		Pixel( array< uint8_t, pixel_definitions< FU >::Size > const & p_components );
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pxl	The Pixel object to copy
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pxl	L'objet Pixel à copier
		 */
		template< ePIXEL_FORMAT FU >
		Pixel( Pixel< FU > const & p_pxl );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~Pixel();
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	p_pxl	The Pixel object to copy
		 *\return		A reference to this Pixel object
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	p_pxl	L'objet Pixel à copier
		 *\return		Une référence sur cet objet Pixel
		 */
		Pixel & operator =( Pixel && p_pxl );
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	p_pxl	The Pixel object to move
		 *\return		A reference to this Pixel object
		 *\~french
		 *\brief		Opérateur d'affectation  par déplacement
		 *\param[in]	p_pxl	L'objet Pixel à déplacer
		 *\return		Une référence sur cet objet Pixel
		 */
		Pixel & operator =( Pixel const & p_pxl );
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	p_pxl	The Pixel object to copy
		 *\return		A reference to this Pixel object
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	p_pxl	L'objet Pixel à copier
		 *\return		Une référence sur cet objet Pixel
		 */
		template< ePIXEL_FORMAT FU >
		Pixel & operator =( Pixel< FU > const & p_pxl );
		/**
		 *\~english
		 *\brief		Addition assignment operator
		 *\param[in]	p_px	The Pixel object to add
		 *\return		A reference to this Pixel object
		 *\~french
		 *\brief		Opérateur d'affectation par addition
		 *\param[in]	p_px	L'objet Pixel à additionner
		 *\return		Une référence sur cet objet Pixel
		 */
		template< ePIXEL_FORMAT FU >
		Pixel & operator +=( Pixel< FU > const & p_px );
		/**
		 *\~english
		 *\brief		Addition assignment operator
		 *\param[in]	p_t	The scalar value to add
		 *\return		A reference to this Pixel object
		 *\~french
		 *\brief		Opérateur d'affectation par addition
		 *\param[in]	p_t	La valeur à additionner
		 *\return		Une référence sur cet objet Pixel
		 */
		template< typename U >
		Pixel & operator +=( U const &	p_t );
		/**
		 *\~english
		 *\brief		Substraction assignment operator
		 *\param[in]	p_px	The Pixel object to substract
		 *\return		A reference to this Pixel object
		 *\~french
		 *\brief		Opérateur d'affectation par soustraction
		 *\param[in]	p_px	L'objet Pixel à soustraire
		 *\return		Une référence sur cet objet Pixel
		 */
		template< ePIXEL_FORMAT FU >
		Pixel & operator -=( Pixel< FU > const & p_px );
		/**
		 *\~english
		 *\brief		Substraction assignment operator
		 *\param[in]	p_t	The scalar value to substract
		 *\return		A reference to this Pixel object
		 *\~french
		 *\brief		Opérateur d'affectation par soustraction
		 *\param[in]	p_t	La valeur à soustraire
		 *\return		Une référence sur cet objet Pixel
		 */
		template< typename U >
		Pixel & operator -=( U const & p_t );
		/**
		 *\~english
		 *\brief		Multiplication assignment operator
		 *\param[in]	p_px	The Pixel object to multiply
		 *\return		A reference to this Pixel object
		 *\~french
		 *\brief		Opérateur d'affectation par multiplication
		 *\param[in]	p_px	L'objet Pixel à multiplier
		 *\return		Une référence sur cet objet Pixel
		 */
		template< ePIXEL_FORMAT FU >
		Pixel & operator *=( Pixel< FU > const & p_px );
		/**
		 *\~english
		 *\brief		Multiplication assignment operator
		 *\param[in]	p_t	The scalar value to multiply
		 *\return		A reference to this Pixel object
		 *\~french
		 *\brief		Opérateur d'affectation par multiplication
		 *\param[in]	p_t	La valeur à multiplier
		 *\return		Une référence sur cet objet Pixel
		 */
		template< typename U >
		Pixel & operator *=( U const & p_t );
		/**
		 *\~english
		 *\brief		Division assignment operator
		 *\param[in]	p_px	The Pixel object to divide
		 *\return		A reference to this Pixel object
		 *\~french
		 *\brief		Opérateur d'affectation par division
		 *\param[in]	p_px	L'objet Pixel à diviser
		 *\return		Une référence sur cet objet Pixel
		 */
		template< ePIXEL_FORMAT FU >
		Pixel & operator /=( Pixel< FU > const & p_px );
		/**
		 *\~english
		 *\brief		Division assignment operator
		 *\param[in]	p_t	The scalar value to divide
		 *\return		A reference to this Pixel object
		 *\~french
		 *\brief		Opérateur d'affectation par division
		 *\param[in]	p_t	La valeur à diviser
		 *\return		Une référence sur cet objet Pixel
		 */
		template< typename U >
		Pixel & operator /=( U const & p_t );
		/**
		 *\~english
		 *\brief		Right binary decal operator
		 *\param[in]	p_t	The decal offset
		 *\return		A reference to this pixel
		 *\~french
		 *\brief		Opérateur de décalage binaire à droite
		 *\param[in]	p_t	L'offset de décalage
		 *\return		Une référence sur ce pixel
		 */
		Pixel & operator >>	( uint8_t p_t );
		/**
		 *\~english
		 *\brief		Computes the sum of all components
		 *\param[out]	p_uResult	Receives the sum
		 *\~french
		 *\brief		Calcule la somme de toutes les composantes
		 *\param[out]	p_uResult	Reçoit la somme
		 */
		template< typename U >
		void sum( U & p_uResult )const;
		/**
		 *\~english
		 *\brief		Multiplication function
		 *\param[in]	p_px	The pixel to multiply
		 *\return		The multiplication result
		 *\~french
		 *\brief		Fonction de multiplication
		 *\param[in]	p_px	Le pixel à multiplier
		 *\return		Le résultat de la multiplication
		 */
		template< ePIXEL_FORMAT FU >
		Pixel< FU > mul( Pixel< FU > const & p_px )const;
		/**
		 *\~english
		 *\brief		Multiplication function
		 *\param[in]	p_byVal	The value
		 *\return		The multiplication result
		 *\~french
		 *\brief		Fonction de multiplication
		 *\param[in]	p_byVal	The value
		 *\return		Le résultat de la multiplication
		 */
		template< ePIXEL_FORMAT FU >
		Pixel< FU > mul( uint8_t const & p_byVal )const;
		/**
		 *\~english
		 *\brief		Deallocate components
		 *\~french
		 *\brief		Désalloue les composantes de ce pixel
		 */
		void clear();
		/**
		 *\~english
		 *\brief		Links the data pointer to the one given in parameter
		 *\remark		The pixel loses ownership of it's data
		 *\~french
		 *\brief		Lie les données de ce pixel à celles données en paramètre
		 *\remark		Le pixel perd la maîtrise de ses données
		 */
		void link( uint8_t * p_pComponents );
		/**
		 *\~english
		 *\brief		Unlinks the data pointer from the ones previously given
		 *\remark		The pixel regains ownership of it's data
		 *\~french
		 *\brief		Délie les données de ce pixel de celles données précédemment
		 *\remark		Le pixel regagne la maîtrise de ses données
		 */
		void unlink();
		/**
		 *\~english
		 *\brief		Converts given data and copies it into this pixel's components
		 *\param[in]	p_components	Data to convert
		 *\~french
		 *\brief		Convertit les données et les copie dans les composantes de ce pixel
		 *\param[in]	p_components	Les données à convertir
		 *\return
		 */
		template< ePIXEL_FORMAT FU >
		void set( std::array< uint8_t, pixel_definitions< FU >::Size > const & p_components );
		/**
		 *\~english
		 *\brief		Converts given data and copies it into this pixel's components
		 *\param[in]	p_components	Data to convert
		 *\~french
		 *\brief		Convertit les données et les copie dans les composantes de ce pixel
		 *\param[in]	p_components	Les données à convertir
		 *\return
		 */
		template< ePIXEL_FORMAT FU >
		void set( uint8_t const * p_components );
		/**
		 *\~english
		 *\brief		Converts given pixel and copies it into this pixel's components
		 *\param[in]	p_px	Pixel to convert
		 *\~french
		 *\brief		Convertit le pixel et le copie dans les composantes de ce pixel
		 *\param[in]	p_px	Le pixel à convertir
		 *\return
		 */
		template< ePIXEL_FORMAT FU >
		void set( Pixel< FU > const & p_px );
		/**
		 *\~english
		 *\brief		Retrieves the data at given index
		 *\remark		No check is made, if you make an index error, expect a crash
		 *\return		A reference on data at wanted index
		 *\~french
		 *\brief		Récupère la donnée à l'index donné
		 *\remark		Aucun check n'est fait, s'il y a une erreur d'index, attendez-vous à un crash
		 *\return		Une référence sur la donnée à l'index voulu
		 */
		inline component_ref operator[]( uint8_t p_uiIndex )
		{
			return m_pComponents.get()[p_uiIndex];
		}
		/**
		 *\~english
		 *\brief		Retrieves the data at given index
		 *\remark		No check is made, if you make an index error, expect a crash
		 *\return		A constant reference on data at wanted index
		 *\~french
		 *\brief		Récupère la donnée à l'index donné
		 *\remark		Aucun check n'est fait, s'il y a une erreur d'index, attendez-vous à un crash
		 *\return		Une référence constante sur la donnée à l'index voulu
		 */
		inline component_const_ref operator[]( uint8_t p_uiIndex )const
		{
			return m_pComponents.get()[p_uiIndex];
		}
		/**
		 *\~english
		 *\brief		Retrieves the pointer on constant datas
		 *\return		The pointer
		 *\~french
		 *\brief		Récupère le pointeur sur les données constantes
		 *\return		Les données
		 */
		inline component_const_ptr const_ptr()const
		{
			return ( m_pComponents ? & m_pComponents.get()[0] : NULL );
		}
		/**
		 *\~english
		 *\brief		Retrieves the pointer on datas
		 *\return		The pointer
		 *\~french
		 *\brief		Récupère le pointeur sur les données
		 *\return		Les données
		 */
		inline component_ptr ptr()
		{
			return ( m_pComponents ? & m_pComponents.get()[0] : NULL );
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the first element
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur sur le premier élément
		 *\return		L'itérateur
		 */
		inline iterator begin()
		{
			return ( m_pComponents ? & m_pComponents.get()[0] : NULL );
		}
		/**
		 *\~english
		 *\brief		Retrieves a constant iterator to the first element
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur constant sur le premier élément
		 *\return		L'itérateur
		 */
		inline const_iterator begin()const
		{
			return ( m_pComponents ? & m_pComponents.get()[0] : NULL );
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the last element
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur sur le dernier élément
		 *\return		L'itérateur
		 */
		inline iterator end()
		{
			return ( m_pComponents ? m_pComponents.get() + pixel_definitions< FT >::Size : NULL );
		}
		/**
		 *\~english
		 *\brief		Retrieves a constant iterator to the last element
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur constant sur le dernier élément
		 *\return		L'itérateur
		 */
		inline const_iterator end()const
		{
			return ( m_pComponents ? m_pComponents.get() + pixel_definitions< FT >::Size : NULL );
		}
		/**
		 *\~english
		 *\brief		Retrieves the pixel format
		 *\return		The pixel format
		 *\~french
		 *\brief		Récupère le format du pixel
		 *\return		Le format du pixel
		 */
		static ePIXEL_FORMAT get_format()
		{
			return FT;
		}
		/**
		 *\~english
		 *\brief		Retrieves the pixel size
		 *\return		The size
		 *\~french
		 *\brief		Récupère la taille du pixel
		 *\return		La taille
		 */
		static uint32_t get_size()
		{
			return pixel_definitions< FT >::Size;
		}
	};
	/**
	 *\~english
	 *\brief		Equality operator
	 *\param[in]	p_pxA, p_pxB	The pixels to compare
	 *\return		\p true if points have same dimensions and same values
	 *\~french
	 *\brief		Opérateur d'égalité
	 *\param[in]	p_pxA, p_pxB	Les pixels à comparer
	 *\return		\p true si les points ont les mêmes dimensions et les mêmes valeurs
	 */
	template <ePIXEL_FORMAT FT, ePIXEL_FORMAT FU> bool operator ==( Pixel<FT> const & p_pxA, Pixel< FU > const & p_pxB );
	/**
	 *\~english
	 *\brief		Addition operator
	 *\param[in]	p_pxA, p_pxB	The pixels to add
	 *\return		The addition result
	 *\~french
	 *\brief		Opérateur d'addition
	 *\param[in]	p_pxA, p_pxB	Les pixels à additionner
	 *\return		Le résultat de l'addition
	 */
	template <ePIXEL_FORMAT FT, ePIXEL_FORMAT FU> Pixel<FT> operator +( Pixel<FT> const & p_pxA, Pixel< FU > const & p_pxB );
	/**
	 *\~english
	 *\brief		Addition operator
	 *\param[in]	p_pxA	The pixel
	 *\param[in]	p_tVal	The value to add
	 *\return		The addition result
	 *\~french
	 *\brief		Opérateur d'addition
	 *\param[in]	p_pxA	Le pixel
	 *\param[in]	p_tVal	La valeur à additionner
	 *\return		Le résultat de l'addition
	 */
	template <ePIXEL_FORMAT FT, typename U> Pixel<FT> operator +( Pixel<FT> const &	p_pxA, U const & p_tVal );
	/**
	 *\~english
	 *\brief		Substraction operator
	 *\param[in]	p_pxA, p_pxB	The pixels to substract
	 *\return		The substraction result
	 *\~french
	 *\brief		Opérateur de soustraction
	 *\param[in]	p_pxA, p_pxB	Les pixels à soustraire
	 *\return		Le résultat de la soustraction
	 */
	template <ePIXEL_FORMAT FT, ePIXEL_FORMAT FU> Pixel<FT> operator -( Pixel<FT> const & p_pxA, Pixel< FU > const & p_pxB );
	/**
	 *\~english
	 *\brief		Substraction operator
	 *\param[in]	p_pxA	The pixel
	 *\param[in]	p_tVal	The value to substract
	 *\return		The substraction result
	 *\~french
	 *\brief		Opérateur de soustraction
	 *\param[in]	p_pxA	Le pixel
	 *\param[in]	p_tVal	La valeur à soustraire
	 *\return		Le résultat de la soustraction
	 */
	template <ePIXEL_FORMAT FT, typename U> Pixel<FT> operator -( Pixel<FT> const & p_pxA, U const & p_tVal );
	/**
	 *\~english
	 *\brief		Division operator
	 *\param[in]	p_pxA, p_pxB	The pixels to divide
	 *\return		The division result
	 *\~french
	 *\brief		Opérateur de division
	 *\param[in]	p_pxA, p_pxB	Les pixels à diviser
	 *\return		Le résultat de la division
	 */
	template <ePIXEL_FORMAT FT, ePIXEL_FORMAT FU> Pixel<FT> operator / ( Pixel<FT> const & p_pxA, Pixel< FU > const & p_pxB );
	/**
	 *\~english
	 *\brief		Division operator
	 *\param[in]	p_pxA	The pixel to divide
	 *\param[in]	p_tVal	The value
	 *\return		The division result
	 *\~french
	 *\brief		Opérateur de division
	 *\param[in]	p_pxA	Le pixel à diviser
	 *\param[in]	p_tVal	La valeur
	 *\return		Le résultat de la division
	 */
	template <ePIXEL_FORMAT FT, typename U> Pixel<FT> operator /( Pixel<FT> const & p_pxA, U const & p_tVal );
	/**
	 *\~english
	 *\brief		Multiplication operator
	 *\param[in]	p_pxA, p_pxB	The pixels to multiply
	 *\return		The multiplication result
	 *\~french
	 *\brief		Opérateur de multiplication
	 *\param[in]	p_pxA, p_pxB	Les pixels à multiplier
	 *\return		Le résultat de la multiplication
	 */
	template <ePIXEL_FORMAT FT, ePIXEL_FORMAT FU> Pixel<FT> operator *( Pixel<FT> const & p_pxA, Pixel< FU > const & p_pxB );
	/**
	 *\~english
	 *\brief		Multiplication operator
	 *\param[in]	p_pxA	The pixel to multiply
	 *\param[in]	p_tVal	The value
	 *\return		The multiplication result
	 *\~french
	 *\brief		Opérateur de multiplication
	 *\param[in]	p_pxA	Le pixel à multiplier
	 *\param[in]	p_tVal	La valeur
	 *\return		Le résultat de la multiplication
	 */
	template <ePIXEL_FORMAT FT, typename U> Pixel<FT> operator *( Pixel<FT> const & p_pxA, U const & p_tVal );
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Typedef over a pixel in A8R8G8B8 format
	\~french
	\brief		Typedef sur un pixel au format A8R8G8B8
	*/
	typedef Pixel<ePIXEL_FORMAT_A8R8G8B8> UbPixel;
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		19/10/2011
	\~english
	\brief		UbPixel policy
	\~french
	\brief		Politique pour les UbPixel
	*/
	template <> class Policy<UbPixel>
	{
	private:
		typedef UbPixel value_type;

	public:
		/**
		 *\~english
		 *\return		The neutral value for a Pixel
		 *\~french
		 *\return		La valeur neutre pour un Pixel
		 */
		static value_type zero()
		{
			return value_type();
		}
		/**
		 *\~english
		 *\return		The unit value for a Pixel
		 *\~french
		 *\return		La valeur unitaire pour un Pixel
		 */
		static value_type unit()
		{
			return value_type();
		}
		/**
		 *\~english
		 *\brief		Initialises the given variable to the neutral value
		 *\param[in]	p_a	The variable to initialise
		 *\~french
		 *\brief		Initialise La variable donnée à la valeur neutre
		 *\param[in]	p_a	La variable à initialiser
		 */
		static void init( value_type & p_a )
		{
			p_a = zero();
		}
		/**
		 *\~english
		 *\brief		Tests if the given param is equal to neutral value
		 *\param[in]	p_a	The value to test
		 *\return		The test result
		 *\~french
		 *\brief		Teste si le param donné est égal à la valeur neutre
		 *\param[in]	p_a	La valeur à tester
		 *\return		Le résultat du test
		 */
		static bool is_null( value_type const & p_a )
		{
			return equals( p_a, zero() );
		}
		/**
		 *\~english
		 *\param[in]	p_a	The value to negate
		 *\return		The negated param (opposite of the value, relative to the neutral value)
		 *\~french
		 *\param[in]	p_a	La valeur à négativer
		 *\return		La valeur négativée du param (opposé de la valeur, relativement à la valeur neutre)
		 */
		static value_type negate( value_type const & p_a )
		{
			return p_a;
		}
		/**
		 *\~english
		 *\param[in,out]	p_a	The value to negate, received the negated value
		 *\return			Reference to the param
		 *\~french
		 *\param[in,out]	p_a	La valeur à négativer, reçoit la valeur négativée
		 *\return			Référence sur le paramètre
		 */
		static value_type ass_negate( value_type & p_a )
		{
			return assign( p_a, negate( p_a ) );
		}
		/**
		 *\~english
		 *\brief		If a Pixel is a floating type, rounds it, else doesn't do anything
		 *\param[in]	p_a	The value to stick
		 *\~french
		 *\brief		Si un Pixel est un type flottant, arrondit le paramètre, sinon ne fait rien
		 *\param[in]	p_a	La valeur à arrondir
		 */
		static value_type stick( value_type & p_a )
		{
			return p_a;
		}
		/**
		 *\~english
		 *\brief		Converts a given param of a given type to this class template type
		 *\param[in]	p_value	The value to convert
		 *\return		The converted value (static_cast, essentially)
		 *\~french
		 *\brief		Convertit un param d'un type donné vers un Pixel template de cette classe
		 *\param[in]	p_value	La valeur à convertir
		 *\return		La valeur convertie (static_cast)
		 */
		template <typename Ty> static value_type convert( Ty const & p_value )
		{
			return value_type( p_value );
		}
		/**
		 *\~english
		 *\brief		Tests equality between 2 params of different types. uses std::numeric_limits::epsilon to perform the test
		 *\param[in]	p_a	The first param, whose type is used as the template argument of std::numeric_limits
		 *\param[in]	p_b	The second param, converted into \p p_a 's type before comparing
		 *\return		The compare result
		 *\~french
		 *\brief		Teste l'égalité entre 2 params de type différent, utilise  std::numeric_limits::epsilon pour faire le test
		 *\param[in]	p_a	Le premier param, dont un Pixel est pris comme argument template de std::numeric_limits
		 *\param[in]	p_b	Le second param, converti dans un Pixel de \ p_a avant la comparaison
		 *\return		Le résultat de la comparaison
		 */
		template <typename Ty> static bool equals( value_type const &, Ty const & )
		{
			return false;
		}
		/**
		 *\~english
		 *\brief		Adds two params of different type, gives the result
		 *\param[in]	p_a	The first param
		 *\param[in]	p_b	The second param, converted into \p p_a 's type before adding
		 *\return		The addition result
		 *\~french
		 *\brief		Additionne 2 param de type différent, retourne le résultat
		 *\param[in]	p_a	Le premier param
		 *\param[in]	p_b	Le second param, converti dans un Pixel de \p p_a avant l'addition
		 *\return		Le résultat de l'addition
		 */
		template <typename Ty> static value_type add( value_type const & p_a, Ty const & p_b )
		{
			return p_a + convert<Ty>( p_b );
		}
		/**
		 *\~english
		 *\brief		Substracts two params of different type, gives the result
		 *\param[in]	p_a	The first param
		 *\param[in]	p_b	The second param, converted into \p p_a 's type before substracting
		 *\return		The substraction result
		 *\~french
		 *\brief		Soustrait 2 param de type différent, retourne le résultat
		 *\param[in]	p_a	Le premier param
		 *\param[in]	p_b	Le second param, converti dans un Pixel de \p p_a avant la soustraction
		 *\return		Le résultat de la soustraction
		 */
		template <typename Ty> static value_type substract( value_type const & p_a, Ty const & p_b )
		{
			return p_a - convert<Ty>( p_b );
		}
		/**
		 *\~english
		 *\brief		Multiplies two params of different type, gives the result
		 *\param[in]	p_a	The first param
		 *\param[in]	p_b	The second param, converted into \p p_a 's type before multiplying
		 *\return		The multiplication result
		 *\~french
		 *\brief		Multiplie 2 param de type différent, retourne le résultat
		 *\param[in]	p_a	Le premier param
		 *\param[in]	p_b	Le second param, converti dans un Pixel de \p p_a avant la multiplication
		 *\return		Le résultat de la multiplication
		 */
		template <typename Ty> static value_type multiply( value_type const & p_a, Ty const & p_b )
		{
			return p_a * convert<Ty>( p_b );
		}
		/**
		 *\~english
		 *\brief		Divides two params of different type, gives the result
		 *\param[in]	p_a	The first param
		 *\param[in]	p_b	The second param, converted into \p p_a 's type before dividing
		 *\return	The division result
		 *\~french
		 *\brief		Divise 2 param de type différent, retourne le résultat
		 *\param[in]	p_a	Le premier param
		 *\param[in]	p_b	Le second param, converti dans un Pixel de \p p_a avant la division
		 *\return		Le résultat de la division
		 */
		template <typename Ty> static value_type divide( value_type const & p_a, Ty const & p_b )
		{
			return p_a / convert<Ty>( p_b );
		}
		/**
		 *\~english
		 *\brief			Adds two params of different type, gives the result
		 *\remark			The first param receives the result
		 *\param[in,out]	p_a	The first param, receives the result
		 *\param[in]		p_b	The second param, converted into \p p_a 's type before adding
		 *\return			Reference to the first param
		 *\~french
		 *\brief			Additionne 2 param de type différent, retourne le résultat
		 *\remark			Le premier param reçoit le résultat
		 *\param[in,out]	p_a	Le premier param, reçoit le résultat
		 *\param[in]		p_b	Le second param, converti dans un Pixel de \p p_a avant l'addition
		 *\return			La référence sur le premier param
		 */
		template <typename Ty> static value_type ass_add( value_type & p_a, Ty const & p_b )
		{
			return assign( p_a, add<Ty>( p_a, p_b ) );
		}
		/**
		 *\~english
		 *\brief			Substracts two params of different type, gives the result
		 *\remark			The first param receives the result
		 *\param[in,out]	p_a	The first param, receives the result
		 *\param[in]		p_b	The second param, converted into \p p_a 's type before substracting
		 *\return			Reference to the first param
		 *\~french
		 *\brief			Soustrait 2 param de type différent, retourne le résultat
		 *\remark			Le premier param reçoit le résultat
		 *\param[in,out]	p_a	Le premier param, reçoit le résultat
		 *\param[in]		p_b	Le second param, converti dans un Pixel de \p p_a avant la soustraction
		 *\return			La référence sur le premier param
		 */
		template <typename Ty> static value_type ass_substract( value_type & p_a, Ty const & p_b )
		{
			return assign( p_a, substract<Ty>( p_a, p_b ) );
		}
		/**
		 *\~english
		 *\brief			Multiplies two params of different type, gives the result
		 *\remark			The first param receives the result
		 *\param[in,out]	p_a	The first param, receives the result
		 *\param[in]		p_b	The second param, converted into \p p_a 's type before multiplying
		 *\return			Reference to the first param
		 *\~french
		 *\brief			Multiplie 2 param de type différent, retourne le résultat
		 *\remark			Le premier param reçoit le résultat
		 *\param[in,out]	p_a	Le premier param, reçoit le résultat
		 *\param[in]		p_b	Le second param, converti dans un Pixel de \p p_a avant la multiplication
		 *\return			La référence sur le premier param
		 */
		template <typename Ty> static value_type ass_multiply( value_type & p_a, Ty const & p_b )
		{
			return assign( p_a, multiply<Ty>( p_a, p_b ) );
		}
		/**
		 *\~english
		 *\brief			Divides two params of different type, gives the result
		 *\remark			The first param receives the result
		 *\param[in,out]	p_a	The first param, receives the result
		 *\param[in]		p_b	The second param, converted into \p p_a 's type before dividing
		 *\return			Reference to the first param
		 *\~french
		 *\brief			Divise 2 param de type différent, retourne le résultat
		 *\remark			Le premier param reçoit le résultat
		 *\param[in,out]	p_a	Le premier param, reçoit le résultat
		 *\param[in]		p_b	Le second param, converti dans un Pixel de \p p_a avant la division
		 *\return			La référence sur le premier param
		 */
		template <typename Ty> static value_type ass_divide( value_type & p_a, Ty const & p_b )
		{
			return assign( p_a, divide<Ty>( p_a, p_b ) );
		}
		/**
		 *\~english
		 *\brief			Converts the second param and assigns the result to the first param
		 *\param[in,out]	p_a	The first param, receives the converted second param
		 *\param[in]		p_b	The second param
		 *\return			Reference to the first param
		 *\~french
		 *\brief			Convertit le second param et affecte le résultat au premier
		 *\param[in,out]	p_a	Le premier param, reçoit le second param converti
		 *\param[in]		p_b	Le second param
		 *\return			La référence sur le premier param
		 */
		template <typename Ty> static value_type assign( value_type & p_a, Ty const & p_b )
		{
			return p_a = convert<Ty>( p_b );
		}
	};
}

#include "Pixel.inl"

#endif
