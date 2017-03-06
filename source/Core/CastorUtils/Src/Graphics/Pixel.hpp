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
#ifndef ___CASTOR_PIXEL_H___
#define ___CASTOR_PIXEL_H___

#include "Math/Point.hpp"
#include "PixelFormat.hpp"

namespace Castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		29/08/2011
	\~english
	\brief		Pixel definition.
	\remark		Takes a PixelFormat as a template argument to determine size anf format.
	\~french
	\brief		Définition d'un pixel.
	\remark		L'argument template PixelFormat sert a déterminer la taille et le format du pixel.
	*/
	template< PixelFormat FT >
	class Pixel
	{
	public:
		//!\~english	Iterator on the pixel's data.
		//!\~french		Itérateur sur les données d'un pixel.
		using iterator = uint8_t *;
		//!\~english	Constant iterator on the pixel's data.
		//!\~french		Itérateur constant sur les données d'un pixel.
		using const_iterator = uint8_t const *;

	protected:
		using component_ptr = uint8_t *;
		using component_const_ptr = uint8_t const *;
		using component_ref = uint8_t &;
		using component_const_ref = uint8_t const &;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_init	Tells if the data are initialised
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_init	Dit si on initialise les données
		 */
		Pixel( bool p_init = false );
		/**
		 *\~english
		 *\brief		Constructor
		 *\remarks		This Pixel doesn't own it's data
		 *\param[in]	p_components	Initialises the data to this one
		 *\~french
		 *\brief		Constructeur
		 *\remarks		Le Pixel ne gère pas ses données
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
		 *\remarks		This Pixel owns it's data
		 *\param[in]	p_components	Initialises the data to this one
		 *\~french
		 *\brief		Constructeur
		 *\remarks		Le Pixel gère ses données
		 *\param[in]	p_components	Initialise les données à celles-ci
		 */
		template< PixelFormat FU >
		Pixel( std::array< uint8_t, pixel_definitions< FU >::Size > const & p_components );
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pxl	The Pixel object to copy
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pxl	L'objet Pixel à copier
		 */
		template< PixelFormat FU >
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
		Pixel & operator=( Pixel && p_pxl );
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
		Pixel & operator=( Pixel const & p_pxl );
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
		template< PixelFormat FU >
		Pixel & operator=( Pixel< FU > const & p_pxl );
		/**
		 *\~english
		 *\brief		Computes the sum of all components
		 *\param[out]	p_result	Receives the sum
		 *\~french
		 *\brief		Calcule la somme de toutes les composantes
		 *\param[out]	p_result	Reçoit la somme
		 */
		template< typename U >
		void sum( U & p_result )const;
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
		template< PixelFormat FU >
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
		template< PixelFormat FU >
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
		 *\remarks		The pixel loses ownership of it's data
		 *\~french
		 *\brief		Lie les données de ce pixel à celles données en paramètre
		 *\remarks		Le pixel perd la responsabilité de ses données
		 */
		void link( uint8_t * p_components );
		/**
		 *\~english
		 *\brief		Unlinks the data pointer from the ones previously given
		 *\remarks		The pixel regains ownership of it's data
		 *\~french
		 *\brief		Délie les données de ce pixel de celles données précédemment
		 *\remarks		Le pixel regagne la responsabilité de ses données
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
		template< PixelFormat FU >
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
		template< PixelFormat FU >
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
		template< PixelFormat FU >
		void set( Pixel< FU > const & p_px );
		/**
		 *\~english
		 *\brief		Retrieves the data at given index
		 *\remarks		No check is made, if you make an index error, expect a crash
		 *\return		A reference on data at wanted index
		 *\~french
		 *\brief		Récupère la donnée à l'index donné
		 *\remarks		Aucun check n'est fait, s'il y a une erreur d'index, attendez-vous à un crash
		 *\return		Une référence sur la donnée à l'index voulu
		 */
		inline component_ref operator[]( uint8_t p_index )
		{
			return m_components.get()[p_index];
		}
		/**
		 *\~english
		 *\brief		Retrieves the data at given index
		 *\remarks		No check is made, if you make an index error, expect a crash
		 *\return		A constant reference on data at wanted index
		 *\~french
		 *\brief		Récupère la donnée à l'index donné
		 *\remarks		Aucun check n'est fait, s'il y a une erreur d'index, attendez-vous à un crash
		 *\return		Une référence constante sur la donnée à l'index voulu
		 */
		inline component_const_ref operator[]( uint8_t p_index )const
		{
			return m_components.get()[p_index];
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
			return ( m_components ? & m_components.get()[0] : nullptr );
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
			return ( m_components ? & m_components.get()[0] : nullptr );
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
			return ( m_components ? & m_components.get()[0] : nullptr );
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
			return ( m_components ? & m_components.get()[0] : nullptr );
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
			return ( m_components ? m_components.get() + pixel_definitions< FT >::Size : nullptr );
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
			return ( m_components ? m_components.get() + pixel_definitions< FT >::Size : nullptr );
		}
		/**
		 *\~english
		 *\brief		Retrieves the pixel format
		 *\return		The pixel format
		 *\~french
		 *\brief		Récupère le format du pixel
		 *\return		Le format du pixel
		 */
		static PixelFormat get_format()
		{
			return PixelFormat( FT );
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
		/**
		 *\~english
		 *\name Arithmetic member operators.
		 *\~french
		 *\name Opérateurs arithmétiques membres.
		 */
		/**@{*/
		template< PixelFormat FU >
		Pixel & operator+=( Pixel< FU > const & p_px );
		template< PixelFormat FU >
		Pixel & operator-=( Pixel< FU > const & p_px );
		template< PixelFormat FU >
		Pixel & operator*=( Pixel< FU > const & p_px );
		template< PixelFormat FU >
		Pixel & operator/=( Pixel< FU > const & p_px );
		template< typename U >
		Pixel & operator+=( U const & p_t );
		template< typename U >
		Pixel & operator-=( U const & p_t );
		template< typename U >
		Pixel & operator*=( U const & p_t );
		template< typename U >
		Pixel & operator/=( U const & p_t );
		/**@}*/

	private:
		std::shared_ptr< uint8_t > m_components;
	};
	/**
	 *\~english
	 *\brief		Equality operator
	 *\param[in]	p_lhs, p_rhs	The pixels to compare
	 *\return		\p true if points have same dimensions and same values
	 *\~french
	 *\brief		Opérateur d'égalité
	 *\param[in]	p_lhs, p_rhs	Les pixels à comparer
	 *\return		\p true si les points ont les mêmes dimensions et les mêmes valeurs
	 */
	template < PixelFormat FT, PixelFormat FU >
	bool operator==( Pixel< FT > const & p_lhs, Pixel< FU > const & p_rhs );
	/**
	 *\~english
	 *\name Arithmetic operators.
	 *\~french
	 *\name Opérateurs arithmétiques.
	 */
	/**@{*/
	template < PixelFormat FT, PixelFormat FU >
	Pixel< FT > operator+( Pixel< FT > const & p_lhs, Pixel< FU > const & p_rhs );
	template < PixelFormat FT, PixelFormat FU >
	Pixel< FT > operator-( Pixel< FT > const & p_lhs, Pixel< FU > const & p_rhs );
	template < PixelFormat FT, PixelFormat FU >
	Pixel< FT > operator/( Pixel< FT > const & p_lhs, Pixel< FU > const & p_rhs );
	template < PixelFormat FT, PixelFormat FU >
	Pixel< FT > operator*( Pixel< FT > const & p_lhs, Pixel< FU > const & p_rhs );
	template < PixelFormat FT, typename U >
	Pixel< FT > operator+( Pixel< FT > const & p_lhs, U const & p_rhs );
	template < PixelFormat FT, typename U >
	Pixel< FT > operator-( Pixel< FT > const & p_lhs, U const & p_rhs );
	template < PixelFormat FT, typename U >
	Pixel< FT > operator/( Pixel< FT > const & p_lhs, U const & p_rhs );
	template < PixelFormat FT, typename U >
	Pixel< FT > operator*( Pixel< FT > const & p_lhs, U const & p_rhs );
	/**@}*/
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Typedef over a pixel in A8R8G8B8 format
	\~french
	\brief		Typedef sur un pixel au format A8R8G8B8
	*/
	typedef Pixel< PixelFormat::eA8R8G8B8 > UbPixel;
}

#include "Pixel.inl"

#endif
