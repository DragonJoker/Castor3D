/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_PIXEL_H___
#define ___CASTOR_PIXEL_H___

#include "CastorUtils/Graphics/PixelFormat.hpp"
#include "CastorUtils/Math/Point.hpp"

namespace castor
{
	/**
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
		 *\param[in]	init	Tells if the data are initialised
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	init	Dit si on initialise les données
		 */
		explicit Pixel( bool init = false );
		/**
		 *\~english
		 *\brief		Constructor
		 *\remarks		This Pixel doesn't own it's data
		 *\param[in]	components	Initialises the data to this one
		 *\~french
		 *\brief		Constructeur
		 *\remarks		Le Pixel ne gère pas ses données
		 *\param[in]	components	Initialise les données à celles-ci
		 */
		explicit Pixel( uint8_t * components );
		/**
		 *\~english
		 *\brief		Copy Constructor
		 *\param[in]	pxl	The Pixel object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	pxl	L'objet Pixel à copier
		 */
		Pixel( Pixel const & pxl );
		/**
		 *\~english
		 *\brief		Move Constructor
		 *\param[in]	pxl	The Pixel object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	pxl	L'objet Pixel à déplacer
		 */
		Pixel( Pixel && pxl );
		/**
		 *\~english
		 *\brief		Constructor
		 *\remarks		This Pixel owns it's data
		 *\param[in]	components	Initialises the data to this one
		 *\~french
		 *\brief		Constructeur
		 *\remarks		Le Pixel gère ses données
		 *\param[in]	components	Initialise les données à celles-ci
		 */
		template< PixelFormat FU >
		explicit Pixel( std::array< uint8_t, PixelDefinitions< FU >::Size > const & components );
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	pxl	The Pixel object to copy
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	pxl	L'objet Pixel à copier
		 */
		template< PixelFormat FU >
		explicit Pixel( Pixel< FU > const & pxl );
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
		 *\param[in]	pxl	The Pixel object to copy
		 *\return		A reference to this Pixel object
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	pxl	L'objet Pixel à copier
		 *\return		Une référence sur cet objet Pixel
		 */
		Pixel & operator=( Pixel && pxl );
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	pxl	The Pixel object to move
		 *\return		A reference to this Pixel object
		 *\~french
		 *\brief		Opérateur d'affectation  par déplacement
		 *\param[in]	pxl	L'objet Pixel à déplacer
		 *\return		Une référence sur cet objet Pixel
		 */
		Pixel & operator=( Pixel const & pxl );
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	pxl	The Pixel object to copy
		 *\return		A reference to this Pixel object
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	pxl	L'objet Pixel à copier
		 *\return		Une référence sur cet objet Pixel
		 */
		template< PixelFormat FU >
		Pixel & operator=( Pixel< FU > const & pxl );
		/**
		 *\~english
		 *\brief		Computes the sum of all components
		 *\param[out]	result	Receives the sum
		 *\~french
		 *\brief		Calcule la somme de toutes les composantes
		 *\param[out]	result	Reçoit la somme
		 */
		template< typename U >
		void sum( U & result )const;
		/**
		 *\~english
		 *\brief		Multiplication function
		 *\param[in]	px	The pixel to multiply
		 *\return		The multiplication result
		 *\~french
		 *\brief		Fonction de multiplication
		 *\param[in]	px	Le pixel à multiplier
		 *\return		Le résultat de la multiplication
		 */
		template< PixelFormat FU >
		Pixel< FU > mul( Pixel< FU > const & px )const;
		/**
		 *\~english
		 *\brief		deallocate components
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
		void link( uint8_t * components );
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
		 *\param[in]	components	Data to convert
		 *\~french
		 *\brief		Convertit les données et les copie dans les composantes de ce pixel
		 *\param[in]	components	Les données à convertir
		 *\return
		 */
		template< PixelFormat FU >
		void set( std::array< uint8_t, PixelDefinitions< FU >::Size > const & components );
		/**
		 *\~english
		 *\brief		Converts given data and copies it into this pixel's components
		 *\param[in]	components	Data to convert
		 *\~french
		 *\brief		Convertit les données et les copie dans les composantes de ce pixel
		 *\param[in]	components	Les données à convertir
		 *\return
		 */
		template< PixelFormat FU >
		void set( uint8_t const * components );
		/**
		 *\~english
		 *\brief		Converts given pixel and copies it into this pixel's components
		 *\param[in]	px	Pixel to convert
		 *\~french
		 *\brief		Convertit le pixel et le copie dans les composantes de ce pixel
		 *\param[in]	px	Le pixel à convertir
		 *\return
		 */
		template< PixelFormat FU >
		void set( Pixel< FU > const & px );
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
		inline component_ref operator[]( uint8_t index )
		{
			return m_components.get()[index];
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
		inline component_const_ref operator[]( uint8_t index )const
		{
			return m_components.get()[index];
		}
		/**
		 *\~english
		 *\brief		Retrieves the pointer on constant datas
		 *\return		The pointer
		 *\~french
		 *\brief		Récupère le pointeur sur les données constantes
		 *\return		Les données
		 */
		inline component_const_ptr constPtr()const
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
			return ( m_components ? m_components.get() + PixelDefinitions< FT >::Size : nullptr );
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
			return ( m_components ? m_components.get() + PixelDefinitions< FT >::Size : nullptr );
		}
		/**
		 *\~english
		 *\brief		Retrieves the pixel format
		 *\return		The pixel format
		 *\~french
		 *\brief		Récupère le format du pixel
		 *\return		Le format du pixel
		 */
		static PixelFormat getFormat()
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
		static uint32_t getSize()
		{
			return PixelDefinitions< FT >::Size;
		}
		/**
		 *\~english
		 *\name Arithmetic member operators.
		 *\~french
		 *\name Opérateurs arithmétiques membres.
		 */
		/**@{*/
		template< PixelFormat FU >
		Pixel & operator+=( Pixel< FU > const & px );
		template< PixelFormat FU >
		Pixel & operator-=( Pixel< FU > const & px );
		template< PixelFormat FU >
		Pixel & operator*=( Pixel< FU > const & px );
		template< PixelFormat FU >
		Pixel & operator/=( Pixel< FU > const & px );
		/**@}*/

	private:
		std::shared_ptr< uint8_t > m_components;
	};
	/**
	 *\~english
	 *\brief		Equality operator
	 *\param[in]	lhs, rhs	The pixels to compare
	 *\return		\p true if points have same dimensions and same values
	 *\~french
	 *\brief		Opérateur d'égalité
	 *\param[in]	lhs, rhs	Les pixels à comparer
	 *\return		\p true si les points ont les mêmes dimensions et les mêmes valeurs
	 */
	template < PixelFormat FT, PixelFormat FU >
	bool operator==( Pixel< FT > const & lhs, Pixel< FU > const & rhs );
	/**
	 *\~english
	 *\name Arithmetic operators.
	 *\~french
	 *\name Opérateurs arithmétiques.
	 */
	/**@{*/
	template < PixelFormat FT, PixelFormat FU >
	Pixel< FT > operator+( Pixel< FT > const & lhs, Pixel< FU > const & rhs );
	template < PixelFormat FT, PixelFormat FU >
	Pixel< FT > operator-( Pixel< FT > const & lhs, Pixel< FU > const & rhs );
	template < PixelFormat FT, PixelFormat FU >
	Pixel< FT > operator/( Pixel< FT > const & lhs, Pixel< FU > const & rhs );
	template < PixelFormat FT, PixelFormat FU >
	Pixel< FT > operator*( Pixel< FT > const & lhs, Pixel< FU > const & rhs );
	/**@}*/
	/**
	\author 	Sylvain DOREMUS
	\~english
	\brief		Typedef over a pixel in A8R8G8B8 format
	\~french
	\brief		Typedef sur un pixel au format A8R8G8B8
	*/
	typedef Pixel< PixelFormat::eR8G8B8A8_UNORM > UbPixel;
}

#include "Pixel.inl"

#endif
