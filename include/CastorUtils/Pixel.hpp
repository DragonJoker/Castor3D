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
#ifndef ___CU_Pixel___
#define ___CU_Pixel___

#include "Point.hpp"
#include <vector>
#include <algorithm>
#include <numeric>

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
	\date		03/01/2011
	\~english
	\brief		Unsupported format exception
	\~french
	\brief		Unsupported format exception
	*/
	class UnsupportedFormatError : public Castor::Exception
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_description	Error description
		 *\param[in]	p_file			Function file
		 *\param[in]	p_function		Function name
		 *\param[in]	p_line			Function line
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_description	Description de l'erreur
		 *\param[in]	p_file			Fichier contenant la fonction
		 *\param[in]	p_function		Nom de la fonction
		 *\param[in]	p_line			Ligne dans la fonction
		 */
		UnsupportedFormatError(	std::string const & p_description, char const * p_file, char const * p_function, uint32_t p_line)
			:	Exception( p_description, p_file, p_function, p_line)
		{
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		english Helper macro to use UnsupportedFormatError
	\~french
	\brief		Macro écrite pour faciliter l'utilisation de UnsupportedFormatError
	*/
#	define UNSUPPORTED_ERROR( p_text) throw UnsupportedFormatError( p_text, __FILE__, __FUNCTION__, __LINE__)
	/**
	 *\~english
	 *\brief		Helper struct that holds pixel size, to_string and converion functions
	 *\~french
	 *\brief		Structure d'aide contenant la taille d'un pixel ainsi que sa fonction to_string et les fonction de conversion vers d'autres formats
	 */
	template< TPL_PIXEL_FORMAT format > struct pixel_definitions;
	namespace PF
	{
		/**
		 *\~english
		 *\brief		Function to retrieve Pixel size without templates
		 *\param[in]	p_pfFormat	The pixel format
		 *\~french
		 *\brief		Fonction de récuperation de la taille d'un pixel sans templates
		 *\param[in]	p_pfFormat	Le format de pixels
		 */
		uint8_t GetBytesPerPixel( ePIXEL_FORMAT p_pfFormat );
		/**
		 *\~english
		 *\brief		Function to retrieve pixel format from a name
		 *\param[in]	p_strFormat	The pixel format name
		 *\~french
		 *\brief		Fonction de récuperation d'un format de pixel par son nom
		 *\param[in]	p_strFormat	Le nom du format de pixels
		 */
		ePIXEL_FORMAT GetFormatByName( String const & p_strFormat );
		/**
		 *\~english
		 *\brief		Function to perform convertion without templates
		 *\param[in]		p_eSrcFmt	The source format
		 *\param[in/out]	p_pSrc		The source pixel
		 *\param[in]		p_eDestFmt	The destination format
		 *\param[in/out]	p_pDest		The destination pixel
		 *\~french
		 *\brief		Fonction de conversion sans templates
		 *\param[in]		p_eSrcFmt	Le format de la source
		 *\param[in/out]	p_pSrc		Le pixel source
		 *\param[in]		p_eDestFmt	Le format de la destination
		 *\param[in/out]	p_pDest		Le pixel destination
		 */
		void ConvertPixel( ePIXEL_FORMAT p_eSrcFmt, uint8_t const *& p_pSrc, ePIXEL_FORMAT p_eDestFmt, uint8_t *& p_pDest );
		/**
		 *\~english
		 *\brief		Function to perform convertion without templates
		 *\param[in]	p_eSrcFormat	The source format
		 *\param[in]	p_pSrcBuffer	The source buffer
		 *\param[in]	p_uiSrcSize		The source size
		 *\param[in]	p_eDstFormat	The destination format
		 *\param[in]	p_pDstBuffer	The destination buffer
		 *\param[in]	p_uiDstSize		The destination size
		 *\~french
		 *\brief		Fonction de conversion sans templates
		 *\param[in]	p_eSrcFormat	Le format de la source
		 *\param[in]	p_pSrcBuffer	Le buffer source
		 *\param[in]	p_uiSrcSize		La taille de la source
		 *\param[in]	p_eDstFormat	Le format de la destination
		 *\param[in]	p_pDstBuffer	Le buffer destination
		 *\param[in]	p_uiDstSize		La taille de la destination
		 */
		void ConvertBuffer( ePIXEL_FORMAT p_eSrcFormat, uint8_t const * p_pSrcBuffer, uint32_t p_uiSrcSize, ePIXEL_FORMAT p_eDstFormat, uint8_t * p_pDstBuffer, uint32_t p_uiDstSize );
		/**
		 *\~english
		 *\brief		Extracts alpha values from a source buffer holding alpha and puts it in a destination buffer
		 *\remark		Destination buffer will be a L8 pixel format buffer and alpha channel from source buffer will be removed
		 *\param[in]	p_pSrc	The source buffer
		 *\return		The destination buffer, \p nullptr if source didn't have alpha
		 *\~french
		 *\brief		Extrait les valeurs alpha d'un buffer source pour les mettre dans un buffer à part
		 *\remark		Le buffer contenant les valeurs alpha sera au format L8 et le canal alpha du buffer source sera supprimé
		 *\param[in]	p_pSrc	Le buffer source
		 *\return		Le buffer alpha, \p nullptr si la source n'avait pas d'alpha
		 */
		PxBufferBaseSPtr ExtractAlpha( PxBufferBaseSPtr & p_pSrc );
		/**
		 *\see			ePIXEL_FORMAT
		 *\~english
		 *\brief		Checks the alpha component support for given pixel format
		 *\return		\p false if format is depth, stencil or one without alpha
		 *\~french
		 *\brief		Vérifie si le format donné possède une composante alpha
		 *\return		\p false si le format est depth, stencil ou un format sans alpha
		 */
		bool HasAlpha( ePIXEL_FORMAT p_ePf );
		/**
		 *\see			ePIXEL_FORMAT
		 *\~english
		 *\brief		Checks if the given pixel format is a compressed one
		 *\return		The value
		 *\~french
		 *\brief		Vérifie si le format donné est un format compressé
		 *\return		La valeur
		 */
		 bool IsCompressed( ePIXEL_FORMAT p_ePf );
		/**
		 *\~english
		 *\brief		Retrieves the pixel format without alpha that is near from the one given
		 *\param[in]	p_ePixelFmt	The pixel format
		 *\return		The given pixel format if none found
		 *\~french
		 *\brief		Récupère le format de pixel sans alpha le plus proche de celui donné
		 *\param[in]	p_ePixelFmt	Le format de pixel
		 *\return		Le format de pixels donné si non trouvé
		 */
		ePIXEL_FORMAT GetPFWithoutAlpha( ePIXEL_FORMAT p_ePixelFmt );
		/**
		 *\~english
		 *\brief			Reduces an image to it's alpha channel, stored in a L8 format buffer
		 *\param[in,out]	p_pSrc	The buffer to reduce
		 *\~english
		 *\brief			Réduit une image à son canal alpha stocké dans un buffer au format L8
		 *\param[in,out]	p_pSrc	Le buffer à réduire
		 */
		void ReduceToAlpha( PxBufferBaseSPtr & p_pSrc );
	}
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
		//!\~english	Typedef over a pointer to uint8_t			\~french	Typedef d'un pointeur sur un uint8_t
		typedef uint8_t *			iterator;
		//!\~english	Typedef over a constant pointer to uint8_t	\~french	Typedef d'un pointeur constant sur un uint8_t
		typedef uint8_t const *		const_iterator;

	protected:
		typedef uint8_t *			component_ptr;
		typedef uint8_t const *		component_const_ptr;
		typedef uint8_t &			component_ref;
		typedef uint8_t const &		component_const_ref;

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
		Pixel( bool p_bInit=false );
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
		Pixel( uint8_t * p_components);
		/**
		 *\~english
		 *\brief		Copy Constructor
		 *\param[in]	p_pxl	The Pixel object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_pxl	L'objet Pixel à copier
		 */
		Pixel( Pixel const & p_pxl);
		/**
		 *\~english
		 *\brief		Move Constructor
		 *\param[in]	p_pxl	The Pixel object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	p_pxl	L'objet Pixel à déplacer
		 */
		Pixel( Pixel && p_pxl);
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
		Pixel( array< uint8_t, pixel_definitions< FU >::size > const & p_components );
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
		Pixel & operator =( Pixel&&	p_pxl );
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
		void set( std::array< uint8_t, pixel_definitions< FU >::size > const & p_components );
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
		inline component_ref operator[]( uint8_t p_uiIndex ) { return m_pComponents.get()[p_uiIndex]; }
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
		inline component_const_ref operator[]( uint8_t p_uiIndex )const { return m_pComponents.get()[p_uiIndex]; }
		/**
		 *\~english
		 *\brief		Retrieves the pointer on constant datas
		 *\return		The pointer
		 *\~french
		 *\brief		Récupère le pointeur sur les données constantes
		 *\return		Les données
		 */
		inline component_const_ptr const_ptr()const { return (m_pComponents ? & m_pComponents.get()[0] : NULL); }
		/**
		 *\~english
		 *\brief		Retrieves the pointer on datas
		 *\return		The pointer
		 *\~french
		 *\brief		Récupère le pointeur sur les données
		 *\return		Les données
		 */
		inline component_ptr ptr() { return (m_pComponents ? & m_pComponents.get()[0] : NULL); }
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the first element
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur sur le premier élément
		 *\return		L'itérateur
		 */
		inline iterator begin() { return (m_pComponents ? & m_pComponents.get()[0] : NULL); }
		/**
		 *\~english
		 *\brief		Retrieves a constant iterator to the first element
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur constant sur le premier élément
		 *\return		L'itérateur
		 */
		inline const_iterator begin()const { return (m_pComponents ? & m_pComponents.get()[0] : NULL); }
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the last element
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur sur le dernier élément
		 *\return		L'itérateur
		 */
		inline iterator end(){ return (m_pComponents ? m_pComponents.get() + pixel_definitions< FT >::size : NULL ); }
		/**
		 *\~english
		 *\brief		Retrieves a constant iterator to the last element
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur constant sur le dernier élément
		 *\return		L'itérateur
		 */
		inline const_iterator end()const { return (m_pComponents ? m_pComponents.get() + pixel_definitions< FT >::size : NULL ); }
		/**
		 *\~english
		 *\brief		Retrieves the pixel format
		 *\return		The pixel format
		 *\~french
		 *\brief		Récupère le format du pixel
		 *\return		Le format du pixel
		 */
		static ePIXEL_FORMAT get_format() { return FT; }
		/**
		 *\~english
		 *\brief		Retrieves the pixel size
		 *\return		The size
		 *\~french
		 *\brief		Récupère la taille du pixel
		 *\return		La taille
		 */
		static uint32_t get_size() { return pixel_definitions< FT >::size; }
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
	template <ePIXEL_FORMAT FT, ePIXEL_FORMAT FU> bool operator ==( Pixel<FT> const & p_pxA, Pixel< FU > const & p_pxB);
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
	template <ePIXEL_FORMAT FT, ePIXEL_FORMAT FU> Pixel<FT> operator +( Pixel<FT> const & p_pxA, Pixel< FU > const & p_pxB);
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
	template <ePIXEL_FORMAT FT, typename U> Pixel<FT> operator +( Pixel<FT> const &	p_pxA, U const & p_tVal);
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
	template <ePIXEL_FORMAT FT, ePIXEL_FORMAT FU> Pixel<FT> operator -( Pixel<FT> const & p_pxA, Pixel< FU > const & p_pxB);
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
	template <ePIXEL_FORMAT FT, typename U> Pixel<FT> operator -( Pixel<FT> const & p_pxA, U const & p_tVal);
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
	template <ePIXEL_FORMAT FT, ePIXEL_FORMAT FU> Pixel<FT> operator / ( Pixel<FT> const & p_pxA, Pixel< FU > const & p_pxB);
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
	template <ePIXEL_FORMAT FT, typename U> Pixel<FT> operator /( Pixel<FT> const & p_pxA, U const & p_tVal);
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
	template <ePIXEL_FORMAT FT, ePIXEL_FORMAT FU> Pixel<FT> operator *( Pixel<FT> const & p_pxA, Pixel< FU > const & p_pxB);
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
	template <ePIXEL_FORMAT FT, typename U> Pixel<FT> operator *( Pixel<FT> const & p_pxA, U const & p_tVal);
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
		static value_type zero() { return value_type(); }
		/**
		 *\~english
		 *\return		The unit value for a Pixel
		 *\~french
		 *\return		La valeur unitaire pour un Pixel
		 */
		static value_type unit() { return value_type(); }
		/**
		 *\~english
		 *\brief		Initialises the given variable to the neutral value
		 *\param[in]	p_a	The variable to initialise
		 *\~french
		 *\brief		Initialise La variable donnée à la valeur neutre
		 *\param[in]	p_a	La variable à initialiser
		 */
		static void init( value_type & p_a) { p_a = zero(); }
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
		static bool is_null( value_type const & p_a ) { return equals( p_a, zero()); }
		/**
		 *\~english
		 *\param[in]	p_a	The value to negate
		 *\return		The negated param (opposite of the value, relative to the neutral value)
		 *\~french
		 *\param[in]	p_a	La valeur à négativer
		 *\return		La valeur négativée du param (opposé de la valeur, relativement à la valeur neutre)
		 */
		static value_type negate( value_type const & p_a) { return p_a; }
		/**
		 *\~english
		 *\param[in,out]	p_a	The value to negate, received the negated value
		 *\return			Reference to the param
		 *\~french
		 *\param[in,out]	p_a	La valeur à négativer, reçoit la valeur négativée
		 *\return			Référence sur le paramètre
		 */
		static value_type ass_negate( value_type & p_a) { return assign( p_a, negate( p_a)); }
		/**
		 *\~english
		 *\brief		If a Pixel is a floating type, rounds it, else doesn't do anything
		 *\param[in]	p_a	The value to stick
		 *\~french
		 *\brief		Si un Pixel est un type flottant, arrondit le paramètre, sinon ne fait rien
		 *\param[in]	p_a	La valeur à arrondir
		 */
		static value_type stick( value_type & p_a) { return p_a; }
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
		template <typename Ty> static value_type convert( Ty const & p_value) { return value_type( p_value); }
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
		template <typename Ty> static bool equals( value_type const & , Ty const & ) { return false; }
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
		template <typename Ty> static value_type add( value_type const & p_a, Ty const & p_b) { return p_a + convert<Ty>( p_b); }
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
		template <typename Ty> static value_type substract( value_type const & p_a, Ty const & p_b) { return p_a - convert<Ty>( p_b); }
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
		template <typename Ty> static value_type multiply( value_type const & p_a, Ty const & p_b) { return p_a * convert<Ty>( p_b); }
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
		template <typename Ty> static value_type divide( value_type const & p_a, Ty const & p_b) { return p_a / convert<Ty>( p_b); }
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
		template <typename Ty> static value_type ass_add( value_type & p_a, Ty const & p_b) { return assign( p_a, add<Ty>( p_a, p_b)); }
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
		template <typename Ty> static value_type ass_substract( value_type & p_a, Ty const & p_b) { return assign( p_a, substract<Ty>( p_a, p_b)); }
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
		template <typename Ty> static value_type ass_multiply( value_type & p_a, Ty const & p_b) { return assign( p_a, multiply<Ty>( p_a, p_b)); }
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
		template <typename Ty> static value_type ass_divide( value_type & p_a, Ty const & p_b) { return assign( p_a, divide<Ty>( p_a, p_b)); }
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
		template <typename Ty> static value_type assign( value_type & p_a, Ty const & p_b) { return p_a = convert<Ty>( p_b); }
	};
	template <ePIXEL_FORMAT PF> class PxBuffer;
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		29/08/2011
	\~english
	\brief		Pixel buffer base definition
	\remark		It has 2 dimensions
	\~french
	\brief		Définition de la classe de base d'un buffer de Pixel
	\remark		Il a 2 dimensions
	*/
	class PxBufferBase
	{
	public:
		//!\~english	Typedef over a pointer to uint8_t			\~french	Typedef d'un pointeur sur uint8_t
		typedef uint8_t *			iterator;
		//!\~english	Typedef over a constant pointer to uint8_t	\~french	Typedef d'un pointeur constant sur uint8_t
		typedef uint8_t const *	const_iterator;

	private:
		ePIXEL_FORMAT	m_ePixelFormat;

	protected:
		//!\~english Buffer dimensions	\~french Dimensions du buffer
		Size	m_size;
		//!\~english Buffer data		\~french Données du buffer
		uint8_t *	m_pBuffer;

	public:
		/**
		 *\~english
		 *\brief		Constructor from another buffer
		 *\param[in]	p_size			Buffer dimensions
		 *\param[in]	p_ePixelFormat	Pixels format
		 *\param[in]	p_pBuffer		Data buffer
		 *\param[in]	p_eBufferFormat	Data buffer's pixels format
		 *\~french
		 *\brief		Constructeur à partir d'un autre buffer
		 *\param[in]	p_size			Dimensions du buffer
		 *\param[in]	p_ePixelFormat	Format des pixels du buffer
		 *\param[in]	p_pBuffer		Buffer de données
		 *\param[in]	p_eBufferFormat	Format des pixels du buffer de données
		 */
		PxBufferBase( Size const & p_size, ePIXEL_FORMAT p_ePixelFormat, uint8_t const * p_pBuffer = NULL, ePIXEL_FORMAT p_eBufferFormat = ePIXEL_FORMAT_A8R8G8B8);
		/**
		 *\~english
		 *\brief		Copy Constructor
		 *\param[in]	p_pixelBuffer	The PxBufferBase object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_pixelBuffer	L'objet PxBufferBase à copier
		 */
		PxBufferBase( PxBufferBase const & p_pixelBuffer);
		/**
		 *\~english
		 *\brief		Move Constructor
		 *\param[in]	p_pixelBuffer	The PxBufferBase object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	p_pixelBuffer	L'objet PxBufferBase à déplacer
		 */
		PxBufferBase( PxBufferBase && p_pixelBuffer);
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~PxBufferBase();
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	p_pixelBuffer	The PxBufferBase object to copy
		 *\return		A reference to this PxBufferBase object
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	p_pixelBuffer	L'objet PxBufferBase à copier
		 *\return		Une référence sur cet objet PxBufferBase
		 */
		PxBufferBase & operator =( PxBufferBase const & p_pixelBuffer);
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	p_pixelBuffer	The PxBufferBase object to move
		 *\return		A reference to this PxBufferBase object
		 *\~french
		 *\brief		Opérateur d'affectation  par déplacement
		 *\param[in]	p_pixelBuffer	L'objet PxBufferBase à déplacer
		 *\return		Une référence sur cet objet PxBufferBase
		 */
		PxBufferBase & operator =( PxBufferBase && p_pixelBuffer);
		/**
		 *\~english
		 *\brief		Deletes the data buffer
		 *\~french
		 *\brief		Détruit le buffer de données
		 */
		virtual void clear();
		/**
		 *\~english
		 *\brief		Initialises the data buffer to the given one
		 *\remark		Conversions are made if needed
		 *\param[in]	p_pBuffer		Data buffer
		 *\param[in]	p_eBufferFormat	Data buffer's pixels format
		 *\~french
		 *\brief		Initialise le buffer de données à celui donné
		 *\remark		Des conversions sont faites si besoin est
		 *\param[in]	p_pBuffer		Buffer de données
		 *\param[in]	p_eBufferFormat	Format des pixels du buffer de données
		 */
		virtual void init( uint8_t const * p_pBuffer, ePIXEL_FORMAT p_eBufferFormat);
		/**
		 *\~english
		 *\brief		Initialises the data buffer at the given size
		 *\remark		Conversions are made if needed
		 *\param[in]	p_size		Buffer dimensions
		 *\~french
		 *\brief		Initialise le buffer de données à la taille donnée
		 *\remark		Des conversions sont faites si besoin est
		 *\param[in]	p_size		Les dimensions du buffer
		 */
		virtual void init( Size const & p_size);
		/**
		 *\~english
		 *\brief		Swaps this buffer's data with the given one's
		 *\param[in]	p_pixelBuffer	The buffer to swap
		 *\~french
		 *\brief		Echange les données de ce buffer avec celles du buffer donné
		 *\param[in]	p_pixelBuffer	Le buffer à échanger
		 */
		virtual void swap( PxBufferBase & p_pixelBuffer);
		/**
		 *\~english
		 *\brief		Converts and assigns a data buffer to this buffer
		 *\param[in]	p_pBuffer		Data buffer
		 *\param[in]	p_eBufferFormat	Data buffer's pixels format
		 *\return
		 *\~french
		 *\brief		Convertit et assigne les données du buffer donné à ce buffer
		 *\param[in]	p_pBuffer		Buffer de données
		 *\param[in]	p_eBufferFormat	Format des pixels du buffer de données
		 *\return
		 */
		virtual void assign( std::vector< uint8_t > const & p_pBuffer, ePIXEL_FORMAT p_eBufferFormat)=0;
		/**
		 *\~english
		 *\brief		Retrieves the pointer on constant datas
		 *\return		The pointer
		 *\~french
		 *\brief		Récupère le pointeur sur les données constantes
		 *\return		Les données
		 */
		virtual uint8_t const * const_ptr()const=0;
		/**
		 *\~english
		 *\brief		Retrieves the pointer on datas
		 *\return		The pointer
		 *\~french
		 *\brief		Récupère le pointeur sur les données
		 *\return		Les données
		 */
		virtual uint8_t * ptr()=0;
		/**
		 *\~english
		 *\brief		Retrieves the total size of the buffer
		 *\return		count() * (size of a pixel)
		 *\~french
		 *\brief		Récupère la taille totale du buffer
		 *\return		count() * (size of a pixel)
		 */
		virtual uint32_t size()const=0;
		/**
		 *\~english
		 *\brief		Creates a new buffer with same values as this one
		 *\return		The created buffer
		 *\~french
		 *\brief		Crée un nouveau buffer avec les mêmes valeurs
		 *\return		Le buffer créé
		 */
		virtual std::shared_ptr<PxBufferBase> clone()const=0;
		/**
		 *\~english
		 *\brief		Retrieves the pixel data at given position
		 *\param[in]	x, y	The pixel position
		 *\return		The pixel data
		 *\~french
		 *\brief		Récupère les données du pixel à la position donnée
		 *\param[in]	x, y	The pixel position
		 *\return		Les données du pixel
		 */
		virtual uint8_t * get_at( uint32_t x, uint32_t y )=0;
		/**
		 *\~english
		 *\brief		Retrieves the pixel data at given position
		 *\param[in]	x, y	The pixel position
		 *\return		The constant pixel data
		 *\~french
		 *\brief		Récupère les données du pixel à la position donnée
		 *\param[in]	x, y	The pixel position
		 *\return		Les données constantes du pixel
		 */
		virtual uint8_t const * get_at( uint32_t x, uint32_t y )const=0;
		/**
		 *\~english
		 *\brief		Makes a vertical swap of pixels
		 *\~french
		 *\brief		Effectue un échange vertical des pixels
		 */
		virtual void flip()=0;
		/**
		 *\~english
		 *\brief		Makes a horizontal swap of pixels
		 *\~french
		 *\brief		Effectue un échange horizontal des pixels
		 */
		virtual void mirror()=0;
		/**
		 *\~english
		 *\brief		Retrieves the pixels format
		 *\return		The pixels format
		 *\~french
		 *\brief		Récupère le format ds pixels
		 *\return		Le format des pixels
		 */
		inline ePIXEL_FORMAT format()const { return m_ePixelFormat; }
		/**
		 *\~english
		 *\brief		Retrieves the buffer width
		 *\return		The buffer width
		 *\~french
		 *\brief		Récupère la largeur du buffer
		 *\return		La largeur du buffer
		 */
		inline uint32_t width()const { return m_size.width(); }
		/**
		 *\~english
		 *\brief		Retrieves the buffer height
		 *\return		The buffer height
		 *\~french
		 *\brief		Récupère la hauteur du buffer
		 *\return		La hauteur du buffer
		 */
		inline uint32_t height()const { return m_size.height(); }
		/**
		 *\~english
		 *\brief		Retrieves the buffer's dimensions
		 *\return		The buffer's dimensions
		 *\~french
		 *\brief		Récupère les dimensions du buffer
		 *\return		Les dimensions du buffer
		 */
		inline Size const & dimensions()const { return m_size; }
		/**
		 *\~english
		 *\brief		Retrieves the pixels count
		 *\return		width * height
		 *\~french
		 *\brief		Récupère le compte des pixels
		 *\return		largeur * hauteur
		 */
		inline uint32_t count()const { return width() * height(); }
		/**
		 *\~english
		 *\brief		Retrieves the pixel data at given position
		 *\param[in]	p_position	The pixel position
		 *\return		The pixel data
		 *\~french
		 *\brief		Récupère les données du pixel à la position donnée
		 *\param[in]	p_position	The pixel position
		 *\return		Les données du pixel
		 */
		inline uint8_t * get_at( Position const & p_position) { return get_at( p_position.x(), p_position.y() ); }
		/**
		 *\~english
		 *\brief		Retrieves the pixel data at given position
		 *\param[in]	p_position	The pixel position
		 *\return		The pixel constant data
		 *\~french
		 *\brief		Récupère les données du pixel à la position donnée
		 *\param[in]	p_position	The pixel position
		 *\return		Les données constantes du pixel
		 */
		inline uint8_t const * get_at( Position const & p_position)const { return get_at( p_position.x(), p_position.y() ); }
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the first element
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur sur le premier élément
		 *\return		L'itérateur
		 */
		inline iterator begin() { return & m_pBuffer[0]; }
		/**
		 *\~english
		 *\brief		Retrieves a constant iterator to the first element
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur constant sur le premier élément
		 *\return		L'itérateur
		 */
		inline const_iterator begin()const { return & m_pBuffer[0]; }
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the last element
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur sur le dernier élément
		 *\return		L'itérateur
		 */
		inline iterator end() { return m_pBuffer + size(); }
		/**
		 *\~english
		 *\brief		Retrieves a constant iterator to the last element
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur constant sur le dernier élément
		 *\return		L'itérateur
		 */
		inline const_iterator end()const { return m_pBuffer + size(); }
		/**
		 *\~english
		 *\brief		Creates a buffer with the given data
		 *\param[in]	p_size			Buffer dimensions
		 *\param[in]	p_eWantedFormat	Pixels format
		 *\param[in]	p_pBuffer		Data buffer
		 *\param[in]	p_eBufferFormat	Data buffer's pixels format
		 *\return		The created buffer
		 *\~french
		 *\brief		Crée un buffer avec les données voulues
		 *\param[in]	p_size			Dimensions du buffer
		 *\param[in]	p_eWantedFormat	Format des pixels du buffer
		 *\param[in]	p_pBuffer		Buffer de données
		 *\param[in]	p_eBufferFormat	Format des pixels du buffer de données
		 *\return		Le buffer créé
		 */
		static PxBufferBaseSPtr create( Size const & p_size, ePIXEL_FORMAT p_eWantedFormat, uint8_t const * p_pBuffer = NULL, ePIXEL_FORMAT p_eBufferFormat = ePIXEL_FORMAT_A8R8G8B8 );

	protected:
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		19/10/2011
	\~english
	\brief		Pixel buffer class, with pixel format as a template param
	\~french
	\brief		Buffer de pixels avec le format de pixel en argument template
	*/
	template< ePIXEL_FORMAT PF >
	class PxBuffer : public PxBufferBase
	{
	public:
		//!\~english	Typedef on a Pixel				\~french	Typedef sur un Pixel
		typedef Pixel<PF>				pixel;
		//!\~english	Typedef on a vector of pixel	\~french	Typedef sur un vector de pixel
		typedef std::vector< pixel >	column;

	private:
		pixel	m_pixel;
		column	m_column;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_ptSize		Buffer wanted dimensions
		 *\param[in]	p_pBuffer		Data buffer
		 *\param[in]	p_eBufferFormat	Data buffer's pixels format
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_ptSize		Les dimensions voulues pour le buffer
		 *\param[in]	p_pBuffer		Buffer de données
		 *\param[in]	p_eBufferFormat	Format des pixels du buffer de données
		 */
		PxBuffer( Size const & p_ptSize, uint8_t const * p_pBuffer = NULL, ePIXEL_FORMAT p_eBufferFormat = ePIXEL_FORMAT_A8R8G8B8 );
		/**
		 *\~english
		 *\brief		Copy Constructor
		 *\param[in]	p_pixelBuffer	The PxBuffer object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_pixelBuffer	L'objet PxBuffer à copier
		 */
		PxBuffer( PxBuffer const & p_pixelBuffer);
		/**
		 *\~english
		 *\brief		Move Constructor
		 *\param[in]	p_pixelBuffer	The PxBuffer object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	p_pixelBuffer	L'objet PxBuffer à déplacer
		 */
		PxBuffer( PxBuffer && p_pixelBuffer);
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~PxBuffer();
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	p_pixelBuffer	The PxBuffer object to copy
		 *\return		A reference to this PxBuffer object
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	p_pixelBuffer	L'objet PxBuffer à copier
		 *\return		Une référence sur cet objet PxBuffer
		 */
		PxBuffer & operator =( PxBuffer const & p_pixelBuffer);
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	p_pixelBuffer	The PxBuffer object to move
		 *\return		A reference to this PxBuffer object
		 *\~french
		 *\brief		Opérateur d'affectation  par déplacement
		 *\param[in]	p_pixelBuffer	L'objet PxBuffer à déplacer
		 *\return		Une référence sur cet objet PxBuffer
		 */
		PxBuffer & operator =( PxBuffer && p_pixelBuffer);
		/**
		 *\~english
		 *\brief		Retrieves the column at given index
		 *\remark		No check is made, if you make an index error, expect a crash
		 *\param[in]	p_uiIndex	The wanted index
		 *\return		A constant reference on column at wanted index
		 *\~french
		 *\brief		Récupère la colonne à l'index donné
		 *\remark		Aucun check n'est fait, s'il y a une erreur d'index, attendez-vous à un crash
		 *\param[in]	p_uiIndex	L'index
		 *\return		Une référence constante sur la colonne à l'index voulu
		 */
		inline	column const & operator []( uint32_t p_uiIndex)const { do_init_column( p_uiIndex );return m_column; }
		/**
		 *\~english
		 *\brief		Retrieves the column at given index
		 *\remark		No check is made, if you make an index error, expect a crash
		 *\param[in]	p_uiIndex	The wanted index
		 *\return		A reference on column at wanted index
		 *\~french
		 *\brief		Récupère la colonne à l'index donné
		 *\remark		Aucun check n'est fait, s'il y a une erreur d'index, attendez-vous à un crash
		 *\param[in]	p_uiIndex	L'index
		 *\return		Une référence sur la colonne à l'index voulu
		 */
		inline	column & operator []( uint32_t p_uiIndex) { do_init_column( p_uiIndex );return m_column; }
		/**
		 *\~english
		 *\brief		Swaps this buffer's data with the given one's
		 *\param[in]	p_pixelBuffer	The buffer to swap
		 *\~french
		 *\brief		Echange les données de ce buffer avec celles du buffer donné
		 *\param[in]	p_pixelBuffer	Le buffer à échanger
		 */
		virtual void swap( PxBuffer & p_pixelBuffer);
		/**
		 *\~english
		 *\brief		Converts and assigns a data buffer to this buffer
		 *\param[in]	p_pBuffer		Data buffer
		 *\param[in]	p_eBufferFormat	Data buffer's pixels format
		 *\return
		 *\~french
		 *\brief		Convertit et assigne les données du buffer donné à ce buffer
		 *\param[in]	p_pBuffer		Buffer de données
		 *\param[in]	p_eBufferFormat	Format des pixels du buffer de données
		 *\return
		 */
		virtual void assign( std::vector< uint8_t > const & p_pBuffer, ePIXEL_FORMAT p_eBufferFormat );
		/**
		 *\~english
		 *\brief		Retrieves the pointer on constant datas
		 *\return		The pointer
		 *\~french
		 *\brief		Récupère le pointeur sur les données constantes
		 *\return		Les données
		 */
		virtual uint8_t const * const_ptr()const { return m_pBuffer; }
		/**
		 *\~english
		 *\brief		Retrieves the pointer on datas
		 *\return		The pointer
		 *\~french
		 *\brief		Récupère le pointeur sur les données
		 *\return		Les données
		 */
		virtual uint8_t * ptr() { return m_pBuffer; }
		/**
		 *\~english
		 *\brief		Retrieves the total size of the buffer
		 *\return		count() * pixel_definitions< PF >::size
		 *\~french
		 *\brief		Récupère la taille totale du buffer
		 *\return		count() * pixel_definitions< PF >::size
		 */
		virtual uint32_t size()const { return count() * pixel_definitions< PF >::size; }
		/**
		 *\~english
		 *\brief		Creates a new buffer with same values as this one
		 *\return		The created buffer
		 *\~french
		 *\brief		Crée un nouveau buffer avec les mêmes valeurs
		 *\return		Le buffer créé
		 */
		virtual std::shared_ptr< PxBufferBase >	clone()const;
		/**
		 *\~english
		 *\brief		Retrieves the pixel data at given position
		 *\param[in]	x, y	The pixel position
		 *\return		The pixel data
		 *\~french
		 *\brief		Récupère les données du pixel à la position donnée
		 *\param[in]	x, y	The pixel position
		 *\return		Les données du pixel
		 */
		virtual uint8_t * get_at( uint32_t x, uint32_t y);
		/**
		 *\~english
		 *\brief		Retrieves the pixel data at given position
		 *\param[in]	x, y	The pixel position
		 *\return		The pixel constant data
		 *\~french
		 *\brief		Récupère les données du pixel à la position donnée
		 *\param[in]	x, y	The pixel position
		 *\return		Les données constantes du pixel
		 */
		virtual uint8_t const * get_at( uint32_t x, uint32_t y)const;
		/**
		 *\~english
		 *\brief		Makes a vertical swap of pixels
		 *\~french
		 *\brief		Effectue un échange vertical des pixels
		 */
		virtual void flip();
		/**
		 *\~english
		 *\brief		Makes a horizontal swap of pixels
		 *\~french
		 *\brief		Effectue un échange horizontal des pixels
		 */
		virtual void mirror();

	private:
		virtual void do_init_column	( uint32_t p_uiColumn );
	};
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Typedef over a buffer of pixels in format A8R8G8B8
	\~french
	\brief		Typedef sur un buffer de pixels au format A8R8G8B8
	*/
	typedef PxBuffer< ePIXEL_FORMAT_A8R8G8B8 > PixelBuffer;
}

#include "Pixel.inl"

#endif
